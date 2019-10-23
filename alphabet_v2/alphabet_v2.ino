#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// String
#include <string>
// IR and I2C.

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <Wire.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <Adafruit_MCP23008.h>

// MCP variables.

Adafruit_MCP23008 mcp;

// SCL GPIO5
// SDA GPIO4

// Variables related to the D1 Mini OLED Display.
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

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

enum StateM_enum {ConnectWiFi, ConnectServer, SendServer, RegisterServer, LetterServer, CheckServer,
                  ReciveServer, IRBroadcastEW, IRListenEW, IRTransmitDataE, IRReceiveDataE, IRReciveDataWE,
                  IRTransmitDataW, IRReceiveDataW
                 };
int condition = 0;
enum StateM_enum state = IRListenEW;
extern String disptext;
void statemachine();

// Variables related to the directions of sending and receiving.

int directionsRec [4] = {0, 1, 2, 3};
String directionsStr [4] = {"WEST", "SOUTH", "EAST", "NORTH"};
int directionsSend [4] = {4, 5, 6, 7};
int amountOfDirections = 4;
int whatToSend[4] = {0x00, 0x01, 0x02, 0x03};



// Variables related to the library which enables us to send and receive.

int slaveAddress = 0x20;
int IRReceived = 0;
const uint16_t kIrLed = 14;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
const uint16_t kRecvPin = 13;

IRrecv irrecv(kRecvPin);

decode_results results;


/*
  boolean registered;
  boolean letterSet;
  boolean isMaster;
  boolean received;
  boolean messageType;
  boolean setLetter;
  boolean letterSett;
  boolean wordResult;
  boolean timeOut;
  boolean eastOnly;
  boolean westOnly;
  boolean eastAndWest;
  boolean winState;
  boolean isRelay;
  boolean isEnd;
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
}

void loop() {
  statemachine();
}

void statemachine()
{
  switch (state)
  {
    case ConnectWiFi:
      {
        disptext = "ConnectWiFi";
        disp();
        Serial.println("ConnectWiFi");
        if (condition == 1) {
          //disptext = "ConnectWiFi-ConnectWiFi";
          //disp();
          //Serial.println("ConnectWiFi-ConnectWiFi");
          state = ConnectWiFi;
        } else if (condition == 2) {
          //disptext = "ConnectWiFi-ConnectServer";
          //disp();
          //Serial.println("ConnectWiFi-ConnectServer");
          state = ConnectServer;
        }
        break;
      }
    case ConnectServer:
      {
        disptext = "ConnectServer";
        disp();
        Serial.println("ConnectServer");
        if (condition == 3) {
          //disptext = "ConnectServer-SendServer";
          //disp();
          //Serial.println("ConnectServer-SendServer");
          state = SendServer;
        } else if (condition == 4) {
          //disptext = "ConnectServer-ConnectServer";
          //disp();
          //Serial.println("ConnectServer-ConnectServer");
          state = ConnectServer;
        }
        break;
      }
    case SendServer:
      {
        disptext = "SendServer";
        disp();
        Serial.println("SendServer");
        if (condition == 5) {
          //disptext = "SendServer-RegisterServer";
          //disp();
          //Serial.println("SendServer-RegisterServer");
          state = RegisterServer;
        } else if (condition == 6) {
          //disptext = "SendServer-LetterServer";
          //disp();
          //Serial.println("SendServer-LetterServer");
          state = LetterServer;
        } else if (condition == 7) {
          //disptext = "SendServer-CheckServer";
          //disp();
          //Serial.println("SendServer-CheckServer");
          state = CheckServer;
        }
        break;
      }
    case RegisterServer:
      {
        disptext = "RegisterServer";
        disp();
        Serial.println("RegisterServer");
        //Serial.println("RegisterServer-ReciveServer");
        state = ReciveServer;
        break;
      }
    case LetterServer:
      {
        disptext = "LetterServer";
        disp();
        Serial.println("LetterServer");
        //Serial.println("LetterServer-ReciveServer");
        state = ReciveServer;
        break;
      }
    case CheckServer:
      {
        disptext = "CheckServer";
        disp();
        Serial.println("CheckServer");
        //Serial.println("CheckServer-ReciveServer");
        state = ReciveServer;
        break;
      }
    case ReciveServer:
      {
        disptext = "ReciveServer";
        disp();
        Serial.println("ReciveServer");
        if (condition == 8) {
          //disptext = "ReciveServer-SendServer";
          //disp();
          //Serial.println("ReciveServer-SendServer");
          state = SendServer;
        } else if (condition == 9) {
          //disptext = "ReciveServer-IRBroadcastEW";
          //disp();
          //Serial.println("ReciveServer-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 32) {
          //disptext = "ReciveServer-ReciveServer";
          //disp();
          //Serial.println("ReciveServer-ReciveServer");
          state = ReciveServer;
        } else if (condition == 34) {
          //disptext = "ReciveServer-IRBroadcastEW";
          //disp();
          //Serial.println("ReciveServer-IRBroadcastEW");
          state = IRBroadcastEW;
        }
        break;
      }
    case IRBroadcastEW:
      {

        Serial.println("IRBroadcastEW");
        for (int i = 0; i < amountOfDirections; i++) {
          mcp.digitalWrite(directionsSend[i], HIGH);
          delay(500);
          sendIR(whatToSend[i]);
          mcp.digitalWrite(directionsSend[i], LOW);
        }

        break;
      }
    case IRListenEW:
      {
        int received = 0;
        Serial.println("IRListenEW");


        for (int i = 0; i < amountOfDirections; i++) {
          mcp.digitalWrite(directionsRec[i], HIGH);
          delay(500);
          received = recIR();
          mcp.digitalWrite(directionsRec[i], LOW);
          if (received != 0) {
            Serial.print("DirRec " + directionsStr[i] + " WhatRec: ");
            Serial.println(intToStr(received));
          }
        }
        /*
          if (condition == 12) {
          //disptext = "IRListenEW-IRListenEW";
          //disp();
          //Serial.println("IRListenEW-IRListenEW");
          state = IRListenEW;
          } else if (condition == 13) {
          //disptext = "IRListenEW-IRBroadcastEW";
          //disp();
          //Serial.println("IRListenEW-IRBroadcastEW");
          state = IRBroadcastEW;
          } else if (condition == 14) {
          //disptext = "IRListenEW-IRTransmitDataE";
          //disp();
          //Serial.println("IRListenEW-IRTransmitDataE");
          state = IRTransmitDataE;
          } else if (condition == 15) {
          //disptext = "IRListenEW-IRTransmitDataW";
          //disp();
          //Serial.println("IRListenEW-IRTransmitDataW");
          state = IRTransmitDataW;
          } else if (condition == 16) {
          //disptext = "IRListenEW-IRReciveDataWE";
          //disp();
          //Serial.println("IRListenEW-IRReciveDataWE");
          state = IRReciveDataWE;
          }
        */
        break;
      }
    case IRTransmitDataE:
      {
        disptext = "IRTransmitDataE";
        disp();
        Serial.println("IRTransmitDataE");
        if (condition == 17) {
          //disptext = "IRTransmitDataE-ReciveServer";
          //disp();
          //Serial.println("IRTransmitDataE-ReciveServer");
          state = ReciveServer;
        } else if (condition == 18) {
          //disptext = "IRTransmitDataE-IRBroadcastEW";
          //disp();
          //Serial.println("IRTransmitDataE-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 19) {
          //disptext = "IRTransmitDataE-IRReceiveDataE";
          //disp();
          //Serial.println("IRTransmitDataE-IRReceiveDataE");
          state = IRReceiveDataE;
        }
        break;
      }
    case IRReceiveDataE:
      {
        disptext = "IRReceiveDataE";
        disp();
        Serial.println("IRReceiveDataE");
        if (condition == 20) {
          //disptext = "IRReceiveDataE-SendServer";
          //disp();
          //Serial.println("IRReceiveDataE-SendServer");
          state = SendServer;
        } else if (condition == 21) {
          //disptext = "IRReceiveDataE-IRReceiveDataE";
          //disp();
          //Serial.println("IRReceiveDataE-IRReceiveDataE");
          state = IRReceiveDataE;
        } else if (condition == 33) {
          //disptext = "IRReceiveDataE-IRBroadcastEW";
          //disp();
          //Serial.println("IRReceiveDataE-IRBroadcastEW");
          state = IRBroadcastEW;
        }
        break;
      }
    case IRReciveDataWE:
      {
        disptext = "IRReciveDataWE";
        disp();
        Serial.println("IRReciveDataWE");
        if (condition == 22) {
          //disptext = "IRReciveDataWE-IRReciveDataWE";
          //disp();
          //Serial.println("IRReciveDataWE-IRReciveDataWE");
          state = IRReciveDataWE;
        } else if (condition == 23) {
          //disptext = "IRReciveDataWE-IRTransmitDataE";
          //disp();
          //Serial.println("IRReciveDataWE-IRTransmitDataE");
          state = IRTransmitDataE;
        } else if (condition == 24) {
          //disptext = "IRReciveDataWE-IRTransmitDataW";
          //disp();
          //Serial.println("IRReciveDataWE-IRTransmitDataW");
          state = IRTransmitDataW;
        } else if (condition == 25) {
          //disptext = "IRReciveDataWE-IRBroadcastEW";
          //disp();
          //Serial.println("IRReciveDataWE-IRBroadcastEW");
          state = IRBroadcastEW;
        }
        break;
      }
    case IRTransmitDataW:
      {
        disptext = "IRTransmitDataW";
        disp();
        Serial.println("IRTransmitDataW");
        if (condition == 26) {
          //disptext = "IRTransmitDataW-IRBroadcastEW";
          //disp();
          //Serial.println("IRTransmitDataW-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 27) {
          //disptext = "IRTransmitDataW-IRReceiveDataW";
          //disp();
          //Serial.println("IRTransmitDataW-IRReceiveDataW");
          state = IRReceiveDataW;
        }
        break;
      }
    case IRReceiveDataW:
      {
        disptext = "IRReceiveDataW";
        disp();
        Serial.println("IRReceiveDataW");
        if (condition == 28) {
          //disptext = "IRReceiveDataW-IRReceiveDataW";
          //disp();
          //Serial.println("IRReceiveDataW-IRReceiveDataW");
          state = IRReceiveDataW;
        } else if (condition == 29) {
          //disptext = "IRReceiveDataW-ReciveServer";
          //disp();
          //Serial.println("IRReceiveDataW-ReciveServer");
          state = ReciveServer;
        } else if (condition == 30) {
          //disptext = "IRReceiveDataW-IRBroadcastEW";
          //disp();
          //Serial.println("IRReceiveDataW-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 31) {
          //disptext = "IRReceiveDataW-IRBroadcastEW";
          //disp();
          //Serial.println("IRReceiveDataW-IRBroadcastEW");
          state = IRBroadcastEW;
        }
        break;
      }
  }

}

String intToStr(int received) {
  String returnStr = "";

  if(received == 0x00) {
    returnStr = "WEST";
  } else if(received == 0x01) {
    returnStr = "SOUTH";
  } else if(received == 0x02) {
    returnStr = "EAST";
  } else if(received == 0x03) {
    returnStr = "NORTH";
  } else {
    returnStr = "0";
  }

  return returnStr;
}
