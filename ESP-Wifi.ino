#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

const int ledPin = 5;
const char *rootResponse =
"<!DOCTYPE HTML>"
"<html>"
"<head>"
"<title>ESP8266 Webserver</title>"
"</head>"
"<body>"
"<h1>It Works!</h1>"
"</body>"
"</html>";

WiFiServer server(80);

void setup() {
  pinMode(ledPin, OUTPUT);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP8266WIFI", "minecraft");
  server.begin();
  Serial.begin(9600);
  Serial.println(WiFi.softAPIP());
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    // get everything we need
    String request(client.readString());

    Serial.println("Request:");
    Serial.println(request);
    Serial.println("----------------");

    if (request.length() > 1) {
      int requestEnd = request.indexOf('\n');
      String requestStr = request.substring(0, requestEnd);
      int protocolEnd = requestStr.indexOf(' ');
      String protocol = requestStr.substring(0, protocolEnd);
      int httpVersionBegin = requestStr.lastIndexOf(' ');
      String httpVersion = requestStr.substring(httpVersionBegin + 1);
      String path = requestStr.substring(protocolEnd + 1, httpVersionBegin);
  
      if (path == "/control" && protocol == "POST") {
        int dataBegin = request.indexOf("\r\n\r\n");
        String data = request.substring(dataBegin + 4);
        
        Serial.print("Got a request to: ");
        Serial.println(path);
        Serial.print("With a protocol: ");
        Serial.println(protocol);
        Serial.println("And data:");
        Serial.println(data);
  
        if (data.length() < 50) {
          StaticJsonBuffer<100> buffer;
          JsonObject& root = buffer.parseObject(data);
  
          boolean value = root["value"];
          
          Serial.print("Setting LED value to: ");
          Serial.println(value);
  
          digitalWrite(ledPin, value);
  
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println();
          client.println(data);
          client.println();
        }
      } else {
        Serial.print("Got a request to: ");
        Serial.println(path);
        Serial.print("With a protocol: ");
        Serial.println(protocol);
  
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println(rootResponse);
        client.println();
      }
    } else {
      Serial.println("Request has no content");
    }
    Serial.println("=====================");
  }
}
