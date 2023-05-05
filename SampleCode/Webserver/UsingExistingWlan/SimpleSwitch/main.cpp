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
