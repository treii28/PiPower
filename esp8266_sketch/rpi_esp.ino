#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <TM1637Display.h>
#include <Bounce2.h>
#include <RpiPower.h>

const char* ssid     = "NETGEAR60";
const char* password = "smilingcheese464";

MDNSResponder mdns;
ESP8266WebServer server ( 80 );

#define NUMDEVICES 6
RpiPower devices[] = {
  RpiPower(D2, 0b00000100, 0b01011110, "idaeus"),    // 0 - id
  RpiPower(D3, 0b01011110, 0b01010000, "drupelet"),  // 1 - dr
  RpiPower(D5, 0b01010000, 0b00011100, "rubori"),    // 2 - ru
  RpiPower(D6, 0b01011000, 0b01110100, "chambord"),   // 3 - ch
  RpiPower(D7, 0b01111100, 0b01110011, "chiquita"),  // 4 - bP
  RpiPower(D8, 0b01111100, 0b01110100, "raspette")   // 5 - bh
};

// Module connection pins (Digital Pins)
#define DIO D0
#define CLK D1

// button pin
#define BTNPIN   D4
// number of button-down loops to consider a 'long' press
#define LONGPRESS   25

// define button with Bounce2
Bounce btn;
// set true while button is pressed
boolean btnOn   = false;
// prior loop state of button for any given loop to detect press-state changes
boolean btnLast = false;
// button down counter for determining long-press situations
int btn_cnt = 0;

// The amount of time (in milliseconds) to wait each main loop
#define LOOP_DELAY   50
// number of non-press loops before escalating the wait/idle state
#define STATE_SPAN   200
// how many loops to wait while rotating before changing display
#define ROT_SPAN  100
// device that was selected before rotation/idle state(s) started
uint8_t prerot_dev = 0;
// counter for measuring in-rotation cycles
int rot_cnt = 0;

// useful state definitions
#define STATE_CMD    0
// device rotation will occur in escalations between CMD and IDLE
#define STATE_IDLE   5

// initial display state - all blank
uint8_t data[] = { 0, 0, 0, 0 };

// current active device
uint8_t curDev = 0;
// current wait state
uint8_t curState  = STATE_IDLE;
// prior wait state
uint8_t lastState = STATE_IDLE;
int cnt = 0;
bool relayToggled = false;

TM1637Display display(CLK, DIO);

void setup()
{
  Serial.begin(115200);
  delay(2500);

  WiFi.begin ( ssid, password );
  Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );
  delay(500);

  if (mdns.begin("EsPiPower", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
  delay(500);

  server.on ( "/", handleRoot );
  server.on ( "/toggle", handleToggle );
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  delay(500);

  Serial.println("Setting up button");
  pinMode(BTNPIN, INPUT_PULLUP);
  btn.attach(BTNPIN);
  delay(500);

  Serial.println("Setting up LED display");
  display.setBrightness(0x08);
  // All segments on
  display.setSegments(data);
  delay(500);

  Serial.println('Ready!');
}

void loop()
{
  checkButton();
  server.handleClient();
  delay(LOOP_DELAY);
}

void tickCounter() {
  cnt++;
  if (cnt > STATE_SPAN) {
    // escalate state
    cnt = 0;
    if (curState == STATE_CMD)
      prerot_dev = curDev;
    lastState = curState;
    curState++;
    // max at idle value
  }
}
void handleIdle() {
  tickCounter();
  if (curState >= STATE_IDLE) {
    // idle state
    if (lastState != STATE_IDLE) {
      // just entering idle state, blank out display
      Serial.println("idle timeout reached");
      blankDisplay();
      // set lastState to prevent looping
      lastState = STATE_IDLE;
    }
    // make sure state is not greater than idle (max)
    curState  = STATE_IDLE;
  } else if (curState > STATE_CMD) { // curState should always be > 0 here
    // perform a rotation of connected device states
    rot_cnt++;
    if (rot_cnt > ROT_SPAN) {
      Serial.println("Idle display rotation");
      curDev++;
      showDev();
      rot_cnt = 0;
    }
  }
}

void setActive() {
  cnt = 0; // reset standard counter
  relayToggled = false;
  // set device to pre-rotation device if we are coming out of a sleep state
  if (lastState > STATE_CMD)
    curDev = prerot_dev;
  // change state to active command mode
  lastState = curState;
  curState  = STATE_CMD;
  showDev();
}

void checkButton() {
  btn.update();
  if (btn.read() == LOW) {
    btnOn = true;
    btn_cnt++;
  } else {
    btnOn = false;
  }

  if (btnOn != btnLast) {
    // button state changed
    if (btnOn == true) {
      // button just turned on
      Serial.println("--- button down ---");
      btn_cnt = 0; // reset button counter
      setActive();
    } else {
      Serial.print("--- button up ---   btn_cnt = ");
      Serial.println(btn_cnt);
      if ((btn_cnt < LONGPRESS) && (lastState == STATE_CMD)) {
        // button just turned off - actually do stuff based on length of press
        Serial.println("(was a short press, rotating device)");
        curDev++;
        showDev();
      }
    }
  } else if (btnOn == false)
    // button is idle
    handleIdle(); // modify the counter and possibly the state

  // allow toggling the relay while button still pressed
  if (btnOn == true) {
    if ((btn_cnt > LONGPRESS) && (relayToggled == false)) {
      Serial.println("Longpress condition met!");
      toggleRelay(curDev);
      relayToggled = true;
    }
  }

  btnLast = btnOn;
}

/**
   toggle a specific device relay
   Note this can be called from any interface, and update relevant displays

   @param uint8_t dNum  device to toggle
*/
void toggleRelay(uint8_t dNum) {
  Serial.print("toggling relay state for device: ");
  Serial.print(dNum);
  Serial.print(" state = ");
  Serial.println(devices[dNum].getRelayState());
  devices[dNum].toggleRelay();
  Serial.print("New relay state = ");
  Serial.println(devices[dNum].getRelayState());

  // if it's not on, turn on the LED screen and update it
  setActive();
}

void showDev() {
  // the increment doesn't check for over max
  if (curDev >= NUMDEVICES)
    curDev = 0;

  Serial.print("updating LED for device: ");
  Serial.println(curDev);

  // set first digit of display to array index + 1
  data[0] = display.encodeDigit(curDev + 1);
  // set 2nd and 3rd digits to device id characters
  data[1] = devices[curDev].getIdc1();
  data[2] = devices[curDev].getIdc2();
  // set 4th digit to on/off state of device relay
  if (devices[curDev].getRelayState() == true)
    data[3] = SEG_DIPON;
  else
    data[3] = SEG_DIPOFF;
  display.setSegments(data);
}

void blankDisplay() {
  Serial.println("Blanking display");
  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  data[3] = 0;
  display.setSegments(data);
}

void handleRoot() {
  String html =
    "<html>\
 <head>\
  <meta http-equiv='refresh' content=\"5;URL='/'\"/>\
  <title>ESP8266 Pi Power Control</title>\
 </head>\
 <body>\
  <h1>Status:</h1>\
  <table>";

  for (uint8_t i = 0; i < NUMDEVICES; i++) {
    html += getRelayHTML(i);
  }

  html +=
    "  <table>\
 </body>\
</html>";
  server.send ( 200, "text/html", html );
}

void handleToggle() {
  int r = -1;
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    if (server.argName(i) == "r") {
      r = server.arg(i).toInt();
    }
  }
  toggleRelay(r);
  /* server.send ( 200, "text/html", "<html><head><meta http-equiv=\"refresh\" content=\"0;URL='/'\" /></head></html>"  ); */
  handleRoot();
}

String getRelayHTML(int r) {
  char* dName = "........";
  if((r >= 0) && (r < NUMDEVICES))
    devices[r].copyName(dName);
  bool isOn = devices[r].getRelayState();

  String row = "<tr><th>Pi ";
  row += r;
  row += "</th><td>";
  row += dName;
  row += "</td><td><a href=\"/toggle?r=";
  row += r;
  row += "\">";
  row += "<svg width=\"15px\" height=\"25px\" viewBox=\"0 0 10 17\" xmlns=\"http://www.w3.org/2000/svg\">";
  row += "<path fill=\"none\" stroke=\"#000\" stroke-width=\"2\" d=\"M9,12c0,2.209-1.791,4-4,4l0,0c-2.209,0-4-1.791-4-4 V5c0-2.209,1.791-4,4-4l0,0c2.209,0,4,1.791,4,4V12z\"/>";
  row += "<circle fill=\"";
  row += (isOn == true) ? "#0A4" : "#E12";
  row += "\" cx=\"5\" cy=\"";
  row += (isOn == true) ? "5" : "11";
  row += ".5\" r=\"2.5\"/></svg> ";
  row += (isOn == true) ? "On" : "Off";
  row += "</a></td></tr>\n";
  return row;
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}
