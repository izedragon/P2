#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// IR and I2C
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <Wire.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Adafruit_MCP23008.h>

// WiFi
#include <ESP8266WiFi.h>

// MCP variables.
Adafruit_MCP23008 mcp;

// Variables related to the D1 Mini OLED Display.
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define irNorth 7
#define irEast 6
#define irSouth 5
#define irWest 4

#define recvNorth 3
#define recvEast 2
#define recvSouth 1
#define recvWest 0

// Variables related to the communication with the MCP23008 I/O Expander.
int IODIR = 0x00;
int IPOL = 0x01;
int GPINTEN = 0x02;
int DEFVAL = 0x03;
int INTCON = 0x04;
int IOCON = 0x05;
int GPPU = 0x06;
int INTF = 0x07;
int INTCAP = 0x08;
int GPIO = 0x09;
int OLAT = 0x0A;
int SCLU = 5;
int SDLU = 4;

// States
enum States { ConnectWiFi, ConnectServer, SendServer, RegisterServer, ResetState, LetterServer,
              CheckServer, ReceiveServer, IRBroadcastEW, IRListenEW, IRTransmitDataE,
              IRReceiveDataE, IRReceiveDataWE, IRTransmitDataW, IRReceiveDataW
            };
enum States state = ConnectWiFi;

// Variables related to the directions of sending and receiving.
int directionsRec [4] = {0, 1, 2, 3};
String directionsStr [4] = {"WEST", "SOUTH", "EAST", "NORTH"};
int directionsSend [4] = {4, 5, 6, 7};
int amountOfDirections = 4;
int whatToSend[4] = {0x00, 0x01, 0x02, 0x03};

// Variables related to the library which enables us to send and receive.
int slaveAddress = 0x20;
int IRReceived = 0;

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU board).
const uint16_t kIrLed = 14;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
const uint16_t kRecvPin = 13;
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.
IRrecv irrecv(kRecvPin);
decode_results results;

// Router credentials
const char* ssid = "MAU";
const char* pass = "12345678";

// Server credentials
const char* host = "192.168.0.4";
const uint16_t port = 1337;

// WiFi client & server
WiFiClient client;
WiFiServer server(port);

// Strings used for messages
String displayLetter = "?";
String checkWord = "S";           // <<<------ "A" temp. set for testing, should be empty
String serverMessage = "";

// State machine booleans
boolean registered = false;
boolean letterSet = false;
boolean serverMessageReceived = false;
boolean isMaster = false;
boolean winState = false;

// Timeout variables (ms)
#define SERVER_RECEIVE_TIMEOUT 1000
unsigned long timeout;

/*
  boolean isRelay = false;
  boolean isEnd = false;
  boolean eastOnly;
  boolean westOnly;
  boolean eastAndWest;
  boolean dirWest;
  boolean dirEast;
*/

void setup() {
  Wire.begin(SDLU, SCLU);
  irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266
  irrecv.enableIRIn();  // Start the receiver
  mcp.begin();
  for (int i = 0; i < 8; i++) {
    mcp.pinMode(i, OUTPUT);
  }
  server.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  invertDisplay(false);
  dispLetter(displayLetter);
}

void loop() {
  statemachine();
}

//<------------------------------------------WiFi integration with state machine code starts here
void statemachine() {
  switch (state) {
    case ConnectWiFi: {
        if (WiFi.status() != WL_CONNECTED) {
          WiFiConnect();
          state = ConnectServer;
        } else {
          state = ConnectServer;
        }
        ServerConnect();
        break;
      }
    case ConnectServer: {
        if (!client.connect(host, port)) {
          Serial.print(".");
          delay(200);
          state = ConnectServer;
        } else {
          Serial.println("");
          Serial.println("Connected to server.");
          state = SendServer;
        }
        break;
      }
    case SendServer: {
        if (!registered) {
          state = RegisterServer;
        } else if (winState) {
          state = ResetState;
        } else if (registered && !letterSet) {
          state = LetterServer;
        } else if (registered && letterSet && isMaster) {
          state = CheckServer;
        } else {
          Serial.println("Nothing to do!");
          state = ReceiveServer;
        }
        break;
      }
    case RegisterServer: {
        ServerSend("0");
        registered = true;
        state = ReceiveServer;
        break;
      }
    case ResetState: {
        isMaster = false;
        letterSet = false;
        displayLetter = "?";
        winState = false;
        state = ReceiveServer;
        break;
      }
    case LetterServer: {
        if (validLetter(displayLetter)) {
          ServerSend("2:1");
          letterSet = true;
          invertDisplay(false);
          dispLetter(displayLetter);
        } else {
          ServerSend("2:0");
          letterSet = false;
        }
        state = ReceiveServer;
        break;
      }
    case CheckServer: {
        String message = "3:" + checkWord;
        ServerSend(message);
        state = ReceiveServer;
        break;
      }
    case ReceiveServer: {
        timeout = millis() + SERVER_RECEIVE_TIMEOUT;
        while (millis() < timeout) {
          client = server.available();
          if (client) {
            Serial.print("");
            while (client.connected()) {
              if (client.available()) {
                serverMessage = client.readStringUntil('\r');
                Serial.println("");
                Serial.print("Message received from server: ");
                Serial.println(serverMessage);
                serverMessageReceived = true;
              }
            }
          }
        }
        if (serverMessageReceived) {
          String command = splitString(serverMessage, ':', 0);
          String parameter = splitString(serverMessage, ':', 1);
          if (command == "1") {
            displayLetter = parameter;
            if (validLetter(parameter)) {
              isMaster = true;            // <<<------ temp. for testing without IR!
              state = ConnectServer;      // <<<------ temp. for testing without IR!
              //state = IRBroadcastEW;    // <<<------ actual path for code
            } else {
              state = ConnectServer;
            }
          } else if (command == "4") {
            winState = parameter == "1";
            if (winState) {
              invertDisplay(true);
            }
            state = ConnectServer;        // <<<------ temp. for testing without IR!
            //state = IRBroadcastEW;      // <<<------ actual path for code
          }
          serverMessageReceived = false;
        } else {
          state = ConnectServer;
        }
        break;
      }
    //<------------------------------------------WiFi integration with state machine code ends here
    //<------------------------------------------IR integration with state machine code starts here
    case IRBroadcastEW: {

        break;
      }
    case IRListenEW: {

        break;
      }
    case IRTransmitDataE: {

        break;
      }
    case IRReceiveDataE: {

        break;
      }
    case IRReceiveDataWE: {

        break;
      }
    case IRTransmitDataW: {

        break;
      }
    case IRReceiveDataW: {

        break;
      }
  }
}
//<------------------------------------------IR integration with state machine code ends here
