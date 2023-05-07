# Simple AccessPoint Acting As WebService

Goal is to operate a ESP8266 stand-alone and without the need for an existing WiFi to control its hardware.

So in the code below, some adjustments are recommended:

* **SSID**: Since the ESP8266 is setting up its own access point WiFi, you should define your own *SSID*. If you do not, the access point shows up with a default name starting with *FaryLink*...
* **Password**: To protect your access point, set a password. If you omit setting a password, the access point is unprotected and can be used by anyone.

> The password needs to be 8-32 characters. If you specify an invalid password (i.e. too short), *both* your *SSID* and *password* are ignored, and you end up with an unprotected access point using a default *SSID*.

## Sample Code

The sample code shows how you can turn your ESP8266 into a simple access-point based webserver that controls your internal ESP8266 LED so you can turn the LED on and off via URLs from any external device.

The code uses two dependencies: *ESPAsyncTCP* and *ESPAsyncWebServer*. You need to add these two libraries to your project via library manager in *Arduino IDE* or *platform.io*.

```c++
#include <Arduino.h>

// wifi library is included in platform.io, no need to add dependency
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// control these hardware GPIOs:
const int outputLed = LED_BUILTIN;  // switch the internal LED on board (note that it lights up in OFF state)


// Replace with your network credentials
// password MUST be 8-32 characters. Else you get unprotected default SSID "FaryLink.."
const char* ssid     = "internal";
const char* password = "testtest";

// define IP address for your access point
// if you do not do this, your AP uses default IP 192.168.4.1
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

// Set web server port number to 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 1.0rem; }
    p { font-size: 0.5rem; }
  </style>
</head>
<body>
  <h2>ESP8266 WebService</h2>
  <p>
    Use these urls:

    http://[IP-Adress]/on  to switch LED ON
    http://[IP-Adress]/off to switch LED OFF
    
  </p>
  
</body>
</html>)rawliteral";


void setup() {
  Serial.begin(115200);

  // define output pins
  pinMode(outputLed, OUTPUT);

  // Connect to WiFi
  Serial.println("Setting AP (Access Point)");
  Serial.print("Setting IP address ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  // Remove the password parameter, if you want the AP (Access Point) to be open
  Serial.print("Enabling AP... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // define actions for various urls:

  // show static html for base url:
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // define the webservice urls
  // in this example, each webservice url returns a text status message
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    // in here, you can execute any code you want
    // it is invoked when a caller calls the web address specified above,
    // i.e. in this example: 
    // http://192.168.4.22/on
    digitalWrite(outputLed, LOW);  // led is ON when LOW
    request->send(200, "text/plain", String("LED is ON now.").c_str());
  });

  // same for the "off" action:
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(outputLed, HIGH);  // led is ON when LOW
    request->send_P(200, "text/plain", String("LED is OFF now.").c_str());
  });

  // Start server
  server.begin();
}

void loop(){
  // since the webserver works asynchronously, your loop can do whatever it needs to do
  // this makes it simple to add webservice control logic to any project you may have
  // the webservice could for example change timings or turn functionality on or off
}
```

## Test Drive

Once you have uploaded the code, there is a new access point available in your WiFi list called **internal**. You need to connect to it.

Once you connect to **internal**, you are asked to enter a password. Enter *testtest*.

Now your computer is connected directly to your *ESP8266*. Since it also acts as a webserver, open a browser and navigate to its IP address:

*http://192.168.4.22*

The browser shows a default static html page listing the webservice urls available. To turn the internal *ESP8266* LED off, in your browser enter:

*http://192.168.4.22/off*

To turn the LED back on, enter:

*http://192.168.4.22/on*




