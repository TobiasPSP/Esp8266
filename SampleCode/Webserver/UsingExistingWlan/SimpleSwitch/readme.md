# Simple Webserver

This is a very simple webserver that connects to an existing WiFi and displays a webpage that you can use to turn on and off some GPIO ports (or the internal LED).

This has two consequences:

* **Connection Details**: Since the ESP8266 must connect to *your* WIFI, you need to hard-code your **SSID** and **Password** to the sketch or use more sophisticated ways of storing and submitting these pieces of information.
* **IP-Address**: Since *you* want to access a web page provided by the ESP8266 webserver, you need to know its **IP Address**. In this sketch, the IP address is dynamically assigned to the ESP8266 by your router and will be printed to the serial monitor. Without a serial monitor connected, and thus without knowing the assigned IP address, you will not be able to connect to the webserver.

```c++
#include <Arduino.h>

// wifi library is included in platform.io, no need to add dependency
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
// timeout in milliseconds for clients to respond before connection is dropped
const long timeoutTime = 2000;

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputLedState = "off";
String output4State = "off";

// Assign output variables to GPIO pins
const int outputLed = LED_BUILTIN;  // switch the internal LED on board (note that it lights up in OFF state)
const int output4 = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 


void setup() {
  Serial.begin(115200);
  // define output pins
  pinMode(outputLed, OUTPUT);
  pinMode(output4, OUTPUT);
  // define output default states
  digitalWrite(outputLed, LOW);
  digitalWrite(output4, LOW);

  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // report IP address assigned:
  Serial.println(WiFi.localIP());

  // start webserver
  server.begin();
}

void loop(){
  // wait for incoming connections
  WiFiClient client = server.accept();   

  // when a client connects...
  if (client) { 
    Serial.println("Client connected");   

    // receive client info in currentLine and monitor connection time to
    // detect timeout     
    String currentLine = ""; 

    // initialize with current time       
    currentTime = millis();
    previousTime = currentTime;

    // wait for client data until connection persists and timeout has not been reached
    while (client.connected() && currentTime - previousTime <= timeoutTime) 
    { 
      currentTime = millis();         
      if (client.available()) {    
        char c = client.read();   
        // emit read byte both to serial monitor and store it in header
        Serial.write(c);   
        header += c;
        if (c == '\n') 
        {        
          // assume that the client sent a valid url
          bool isOk = true;

          // two newline characters in a row ends the request
          // at this time, the FULL client information has been stored in header
          // and can now be evaluated to find out what the client wants from us
          if (currentLine.length() == 0) {
            // write the http header to return to the client
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // parse the requested url for the requested action
            // note the blank space at the end which makes sure we can detect invalid urls
            // that start with the expected value but then continue with something else
            if (header.indexOf("GET /led/on ") >= 0) {
              Serial.println("LED on");
              outputLedState = "on";
              digitalWrite(outputLed, LOW);  // led is ON when LOW
            } else if (header.indexOf("GET /led/off ") >= 0) {
              Serial.println("LED off");
              outputLedState = "off";
              digitalWrite(outputLed, HIGH);  // led is OFF when HIGH
            } else if (header.indexOf("GET /4/on ") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off ") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            else
            {
              // invalid url submitted by client
              Serial.print("Client submitted illegal url: ");
              Serial.println(header);
              isOk = false;
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #44FF44; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".warning { background-color: #FFAAAA; border: black; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 16px; margin: 2px;}");
            
            client.println(".button2 {background-color: #FF4444;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 WebControl</h1>");
            
            if (!isOk)
            {
              // client sent an invalid url
              uint pos1 = header.indexOf(" ");
              uint pos2 = header.indexOf(" ", pos1+1);
              
              client.print("<p class=\"warning\">The url you submitted does not exist: ");
              client.print(header.substring(pos1+1, pos2));
              client.println(". Try again.</p>");

            }

            // display html button for led 
            client.println("<p>LED State " + outputLedState + "</p>");
            // If the LED is off, it displays the ON button       
            if (outputLedState=="off") {
              client.println("<p><a href=\"/led/on\"><button class=\"button\">TURN ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">TURN OFF</button></a></p>");
            } 
               
            // display html button for gpio4 
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">TURN ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">TURN OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // HTTP response ends with blank line
            client.println();
            
            break;
          } else { 
            currentLine = "";
          }
        } 
        else if (c != '\r') 
        {  
          currentLine += c;     
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
```

> The library used in this example to run the webserver is a default library both for *Arduino IDE* and *platform.io*  so you don't need to install anything in addition or add any manual dependencies.

Once you added your WIFI connection details to the sketch and uploaded it, your ESP8266 tries to connect to your WIFI, and on success displays its assigned **IP address**.

> If you cannot connect to your Wifi, keep in mind that most ESP8266 dev boards have no real antenna and instead use a weak PCB antenna. Make sure your ESP8266 is located close enough to one of your Wifi access points. If in doubt, use a simple Wifi strength test sketch (readily available) and test the signal strength of the desired wifi network you are trying to reach. Make sure the signal strength received by your ESP8266 for this particular wifi network is high, yet keep in mind that this is just a rough indicator. *Your* ESP8266 acts as an emitter in this case, so even if your WIFI router produces a strong signal due to its high power and good antennas, your ESP8266 with its own low power and missing antenna might not get back its own signal to your router.

Next, enter this IP address into your web browser to see the web page generated by the ESP8266. It displays two buttons to control (a) the built-in LED and (b) GPIO4.

## Notes

When you click one of the buttons you see that you actually built a webservice. Each button click emits a URL in the format `http://192.168.68.125/led/on`. Of course you could also call these Urls directly to directly turn on and off hardware connected to your ESP8266.

When you enter an illegal url (one for which no action exists in the sketch), an error message will be added to the web page.





