// WiFi
#include <ESP8266WiFi.h>

void WiFiConnect(const char* ssid, const char* pass);
void WiFiStatus(const char* ssid);
void ServerConnect(const char* host, const uint16_t port);
void ServerSend(String message, WiFiClient client);
