/*
 Server-Sent Events / EventSource DEMO
 forked from Claudius Coenen repository
 based on Web Server example by David A. Mellis and Tom Igoe
 Adapted to the new ESP8266 SDK 2.4.2 by Marco Campinoti

 Circuit:
 * Analog input attached to pins A0 (optional)
 * Digital input attached to pins 5 or 6 (optional)

 This is free software. Use, modify and tinker with it however you like!
 LICENSED UNDER CC-BY-4.0 http://creativecommons.org/licenses/by/4.0/
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//declare the webserver
ESP8266WebServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(F("Connecting"));
        
    WiFi.persistent(false);       // WiFi config isn't saved in flash
    WiFi.mode(WIFI_STA);          // use WIFI_AP_STA if you want an AP
    WiFi.hostname("ESP8266");    // must be called before wifi.begin()
    WiFi.begin("YourSSID", "YourPassword");
   
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
  }
     
  Serial.println();
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());  
  
  //start the webserver
  server.begin();
  //Server Sent Events will be handled from this URI
  server.on("/ssedata", handleSSEdata);
}

void loop() {
  // listen for incoming clients
  server.handleClient();
}

void handleSSEdata(){
  WiFiClient client = server.client();
  
  if (client) {
    Serial.println("new client");
    serverSentEventHeader(client);
    while (client.connected()) {
      serverSentEvent(client);
      delay(16); // round about 60 messages per second
    }

    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void serverSentEventHeader(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/event-stream;charset=UTF-8");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println("Access-Control-Allow-Origin: *");  // allow any connection. We don't want Arduino to host all of the website ;-)
  client.println("Cache-Control: no-cache");  // refresh the page automatically every 5 sec
  client.println();
  client.flush();
}

void serverSentEvent(WiFiClient client) {
  client.println("event: esp8266"); // this name could be anything, really.
  client.print("data: {");
  client.print("\"A0\": ");
  client.print(1.0 * analogRead(0) / 1024.0);
  client.print(", \"in5\": ");
  client.print(digitalRead(5));
  client.print(", \"in6\": ");
  client.print(digitalRead(6));
  client.print(", \"text\": ESP8266");    // added just to show how you can add your own parameters
  client.println("}");
  client.println();
  client.flush();
}
