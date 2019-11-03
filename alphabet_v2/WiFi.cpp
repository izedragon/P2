  
 #include "WiFi.h" 

void WiFiConnect(const char* ssid, const char* pass) {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WiFiStatus(ssid);
}

void WiFiStatus(const char* ssid) {
  Serial.println("");
  Serial.print("Connected to ");
  Serial.print(ssid);
  Serial.println(".");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void ServerConnect(const char* host, const uint16_t port) {
  Serial.print("Connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.print(port);
}

void ServerSend(String message, WiFiClient client) {
  if (client.connected()) {
    client.println(message);
    Serial.print("Message sent to server: ");
    Serial.println(message);
    client.stop();
    Serial.println("Disconnected from server.");
  }
}