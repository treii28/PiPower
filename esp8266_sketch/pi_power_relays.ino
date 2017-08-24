
#include <Bounce2.h>
#include <Relay.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid     = "NETGEAR60";
const char* password = "************";

ESP8266WebServer server ( 80 );
const int led = 16;

int bPin[]  = {5,0,4,2};
Bounce bounce[] = { Bounce(), Bounce(), Bounce(), Bounce() };
Relay relay[] = {
  Relay(14, LOW),
  Relay(12, LOW),
  Relay(13, LOW),
  Relay(15, LOW),  
};

void setupButtons() {
  Serial.println('initializing buttons...');

  // set up the buttons with internal pull-up
  pinMode(bPin[0], INPUT_PULLUP);
  pinMode(bPin[1], INPUT_PULLUP);
  pinMode(bPin[2], INPUT_PULLUP);
  pinMode(bPin[3], INPUT_PULLUP);
  Serial.println('attaching buttons to debouncers...');
  bounce[0].attach(bPin[0]);
  bounce[1].attach(bPin[1]);
  bounce[2].attach(bPin[2]);
  bounce[3].attach(bPin[3]);  
}
void setupRelays() {
  Serial.println('initializing relays...');
  relay[0].off();
  relay[1].off();
  relay[2].off();
  relay[3].off();
}
void wifiConnect() {
    Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupServer() {
    if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }

  server.on ( "/", handleRoot );
  /*
  server.on ( "/test.svg", drawGraph );
  server.on ( "/inline", []() { 
    server.send ( 200, "text/plain", "this works as well" );
  } );
  */
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void setup() {
  Serial.begin(115200);
  delay(2500);

  setupButtons();
  delay(500);

  setupRelays();
  delay(500);

  wifiConnect();
  delay(500);

  setupServer();
  delay(500);

  Serial.println('Ready!');
}

void loop() {
  readButtons();
  delay(10);
}

/**
 * read serial incoming data for input
 */
void readSerial() {
    if(Serial.available() > 0) {
    int byte = (int)Serial.read() - 49;
    if((byte >= 0) && (byte <= 4)) {
      toggle(byte);
    }
  }
}
/**
 * scan buttons for presses
 */
void readButtons() {
  // update and read button Bounce instances
  for(int i = 0; i<4; i++) {
    if(bounce[i].update()) {
      if(bounce[i].read() == LOW) {
        toggle(i);
      }
    }
  }  
}
/**
 * toggle relay state
 */
void toggle(int r) {
        Serial.print("Button ");
        Serial.print(r+1);
        Serial.print(" state ");
        Serial.print(relay[r].state());
        relay[r].toggle();
        Serial.print(" switch to ");
        Serial.print(relay[r].state());
        Serial.print(" (");
        if (relay[r].isRelayOn()) Serial.print("on"); 
        if (relay[r].isRelayOff()) Serial.print("off");
        Serial.println(")");  
}

void handleRoot() {
  digitalWrite ( led, 1 );
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

    hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
  digitalWrite ( led, 0 );
}

void handleNotFound() {
  digitalWrite ( led, 1 );
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
  digitalWrite ( led, 0 );
}
