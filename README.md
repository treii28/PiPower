# PiPower
remote control for ESP8266 with relays for raspberry pi power connections

I uploaded the ESP8266 code, but it's a base version that just uses my custom library to define the raspberry pis
connected to the relay and control the switching and telemetry for the current switch state.
I didn't get deep into creating the RESTful code in there yet because it was easier to test and support the
RESTful responses using one of the pi's with an apache server and a short PHP script until I fully settled
on what exactly I was going to be sending and receiving.
So I also threw the rest.php file in there I'm using with the actual code that talks via Rest.

As of this point, I am stuck at two places.
One is knowing why it's crashing when I'm trying to send the toggle ajax and receive the response.
The second is I wasn't able to figure out how to dynamically create the list items inside of java.
(you'll note that I just manually create two of them in  PiPower/app/src/main/java/org/wijg/swood/pipower/MainActivity.java )

My current plans for the Ajax includes the following functions
(my test script supports GET/POST/PUSH
- I basically use a standard wrapper to check all possible incoming data streams/formats
for such test code as all my different stuff behaves in different ways)

Specify function with GET or POST parameter 'func'

func=info
returns the array of information about the devices configured on the relay/power module itself
(this is what I want to use to generate the actual list of what is displayed on the screen)

   example uri: http://thewild.dyndns.org:44080/rest.php?func=info
   example response: {"func":"info","relays":[{"pin":1,"name":"Rasp1","id":"p1","status":1,"address":"192.168.1.101"},{"pin":2,"name":"Rasp2","id":"p2","status":0,"address":"192.168.1.102"}]}

NOTE: as of this point, it returns basic information.
      Eventually I am considering implementing the ability to also pass a pin=# that would
      return more detailed information about a specific device connected to a given relay pin.
      I'm still debating on this (but you will see unfinished 'case' for this in the php test code)
      as it may be easier to retrieve the information directly from the given raspberry pi itself
      rather than trying to collect, monitor and store such information on these little embedded devices.

func=toggle  parameter: pin={number}
using the info above, you can toggle the current state (off/on represented by 0/1) by sending this and the pin

  example uri: http://thewild.dyndns.org:44080/rest.php?func=toggle&pin=1
  example response: {"func":"toggle","pin":1,"state":0}

NOTE: I haven't yet implemented a persistent state in the short PHP script as I was still working on just
      getting the response at all. If needed, I can drop some kind of cookie or flat file to 'remember'
      the last state for testing if needed. Currently, pin=1 always returns '0' (off) for status,
      pin=2 always returns '1' (on) for status.


