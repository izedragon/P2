#include <ESP8266WiFi.h>

//SSID of your network
char ssid[] = "slynet"; //SSID of your Wi-Fi router
char pass[] = "password"; //Password of your Wi-Fi router
WiFiServer server(1337);
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.begin();

  printWiFiStatus();
}

void loop () {
  // Check if module is still connected to WiFi.
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    // Print the new IP to Serial.
    printWiFiStatus();
  }
  
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected.");
    while (client.connected()) {
      if (client.available()) {
        char command = client.read();
        Serial.print("My letter is: ");
        Serial.println(command);
      }
    }
    Serial.println("Client disconnected.");
    client.stop();
  }
}

void printWiFiStatus() {
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
