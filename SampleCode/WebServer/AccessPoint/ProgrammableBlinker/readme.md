# Programmable Blinker

This project illustrates how you can add remote access to your *ESP8266* projects to control and adjust parameters via WiFi.

The sketch produces a high frequency blink pattern simulating a police emergency light. In the background, the sketch establishes a Wifi Access Point so you can use any Internet-enabled device like a smartphone or laptop to connect to it, and then asynchronously in the background listens for requests via WiFi.

Your primary code - in this example the LED blinker - works untouched in the foreground inside the *loop()* function.

## WiFi Connection Details

By default, the sketch uses these details:

* **SSID**: internal
* **Password**: testtest
* **IP Address**: 192.168.1.1

The internal *ESP8266* LED will start a blinking pattern simulating a *Hänsch Movia* German police emergency light.

When you connect to this access point and then navigate to **http;//192.168.1.1**, you see a static webpage.

## Updating the blinking pattern

To update and change the blinking pattern, use this webservice:

http://192.168.1.1/update?timing=50,50

This will change the light pattern to 50ms ON, then 50ms OFF.

To restore the original more complex pattern, run this:

http://192.168.1.1/update?timing=154,45,16,25,15,25,16,188

Likewise, you can now change the pattern in any way you wish.

> Note how the webserver handling the requests works transparently in the background and does not interfere with the light pattern generation. So even when you request an update, the pattern appears unchanged with no delays or hangs.
> Adding this combination of Access Point and asynchronous web server to your projects provides you with enourmous flexibility, yet the solution is so light-weight that it won't slow down operations even on a simple ESP8266.

```c++
#include <Arduino.h>

// wifi library is included in platform.io, no need to add dependency
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// control these hardware GPIOs:
const int outputLed = LED_BUILTIN;  // switch the internal LED on board (note that it lights up in OFF state)
#define PINMOSFET1        D7
#define PINMOSFET2        D6
#define ON               LOW
#define OFF              HIGH


#define SPTR_SIZE   100
char   *sPtr [SPTR_SIZE];

// Replace with your network credentials
// password MUST be 8-32 characters. Else you get unprotected default SSID "FaryLink.."
const char* ssid     = "internal";
const char* password = "testtest";

const char* PARAM_INPUT_1 = "timing";

// pattern Movia D (Haensch)
int pattern1[100] = { 154,45,16,25,15,25,16,188 };
int pattern1Size = 8;
long pattern1Timer = 0;        // millis() when pattern part was activated
int pattern1Count = 0;         // index into array pointing at next pattern part
bool pattern1State = true;    // start with lamp turned on
int brightness = 255;


// define IP address for your access point
// if you do not do this, your AP uses default IP 192.168.4.1
IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,2);
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

void ResetPattern1()
{
  pattern1Timer = millis(); // millis() when pattern part was activated
  pattern1State = true;    // start with lamp turned on
}

int separate (
    String str,
    char   **p,
    int    size,
    int (& arr) [100] )
{
    int  n;
    char s [100];

    strcpy (s, str.c_str ());

    *p++ = strtok (s, ",");
    for (n = 1; NULL != (*p++ = strtok (NULL, ",")); n++)
        if (size == n)
            break;

    for (int i = 0; i < n; i++)
    {
              
              Serial.println (sPtr [i]);
              arr[i] = atoi((char *)sPtr[i]);;
    }

    return n;
}


void SwitchLampPattern1(bool state)
{
    
  if (state)
  {
    //digitalWrite(PINMOSFET, HIGH);
    // set led brightness via PWM (must be connected to a PWM pin!)
    // Uno, Nano, Mini 3, 5, 6, 9, 10, 11 490 Hz (pins 5 and 6: 980 Hz)
    // Mega 2 - 13, 44 - 46 490 Hz (pins 4 and 13: 980 Hz)
    // Uno WiFi Rev2, Nano Every 3, 5, 6, 9, 10 976 Hz
    analogWrite(PINMOSFET1, brightness);
    digitalWrite(PINMOSFET2, LOW);
    
    
    digitalWrite(LED_BUILTIN, ON); 
    
  }
  else
  {
    digitalWrite(PINMOSFET1, LOW);
    analogWrite(PINMOSFET2,  brightness);
    
    digitalWrite(LED_BUILTIN, OFF); 
  }
}


void setup() {
  Serial.begin(115200);

  // define output pins
  pinMode(outputLed, OUTPUT);
  // define output pins:
  pinMode(PINMOSFET1, OUTPUT);
  pinMode(PINMOSFET2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  ResetPattern1(); 
  // turn lamp on for first part of pattern
  pattern1State = !pattern1State;
  SwitchLampPattern1(pattern1State);

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

  // action "update" can have a parameter
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String param1;
    
    // GET input1 value on <ESP_IP>/update?timing=1,2,3,4,5...
    if (request->hasParam(PARAM_INPUT_1)) {
      param1 = request->getParam(PARAM_INPUT_1)->value();
      pattern1Size = separate (param1, sPtr, SPTR_SIZE, pattern1);
      pattern1Timer = 0;        // millis() when pattern part was activated
      pattern1Count = 0;         // index into array pointing at next pattern part
      
      pattern1State = true;
      
      request->send(200, "text/plain", "OK");
    }
    else {
      request->send(200, "text/plain", "Invalid parameter. Use comma-separated list of integers.");
    }
    
  });

  // Start server
  server.begin();
}

void loop(){
  long curTimer = millis();
  
  if (curTimer > pattern1[pattern1Count]+pattern1Timer)
  {
    pattern1Timer = curTimer;
    pattern1Count++;
    bool endOfCycle=pattern1Count>pattern1Size-1;

    if (endOfCycle)
    {
      pattern1State = true;
      pattern1Count = 0;
    }
    else
    {
      pattern1State = !pattern1State;
    }
    SwitchLampPattern1(pattern1State);
  }
}
```
