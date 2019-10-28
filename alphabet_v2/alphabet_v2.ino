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

//Utilities variables.
#include "utilities.h"

// Display variables.

#include "Display.h"

// WiFi variables.

#include "WiFi.h"

// Display variables.

#include "Display.h"

// IRSender variables.

#include "IRSender.h"

// IRRec variables.

#include "IRRec.h"

// SCL GPIO5
// SDA GPIO4

// Variables related to the D1 Mini OLED Display.
#define OLED_RESET 0 // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

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

enum States
{
  ConnectWiFi,
  ConnectServer,
  SendServer,
  RegisterServer,
  ResetState,
  LetterServer,
  CheckServer,
  ReceiveServer,
  IRBroadcastEW,
  IRTransmitAndReceiveDataE,
  IRTransmitAndReceiveDataWE,
  IRTransmitAndReceiveDataW,
};
enum States state = IRBroadcastEW;

// Variables related to the directions of sending and receiving.

String directionsStrEW[2] = {"WEST", "EAST"};

int directionsRecEW[2] = {0x01, 0x04};  // WEST, EAST
int directionsSendEW[2] = {0x10, 0x40}; // WEST, EAST
int amountOfDirectionsEW = 2;           // 2
// Variables related to the neighbors that the node has and it's role.

int neighbours[3] = {0, 0, 0};
int sizeOfNeighbours = 3;
String role = "None";

// Variables related to the library which enables us to send and receive.

int slaveAddress = 0x20;
int IRReceived = 0;
const uint16_t kIrLed = 14; // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed); // Set the GPIO to be used to sending the message.

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
const uint16_t kRecvPin = 13;

IRrecv irrecv(kRecvPin);

decode_results results;

// Router credentials
const char *ssid = "MAU";
const char *pass = "12345678";

// Server credentials
const char *host = "192.168.0.2";
const uint16_t port = 1337;

// WiFi client & server
WiFiClient client;
WiFiServer server(port);

// Strings used for messages
String displayLetter = "?";
String checkWord = "S"; // <<<------ "A" temp. set for testing, should be empty
String serverMessage = "";

// State machine booleans related to roles.
boolean registered = false;
boolean letterSet = false;
boolean serverMessageReceived = false;
boolean isMaster = false;
boolean isRelay = false;
boolean isEnd = false;
boolean winState = false;

// Timeout variables (ms)
#define SERVER_RECEIVE_TIMEOUT 1000
unsigned long timeout;

void setup()
{
  Wire.begin(SDLU, SCLU);
  irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif // ESP8266

  irrecv.enableIRIn(); // Start the receiver

  // Setup the MCP.

  Wire.beginTransmission(slaveAddress);
  Wire.write(0x00);
  Wire.write(0x00);
  Wire.endTransmission();

  // Server setup.
 // server.begin();

  // Display setup.
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 64x48)
  /*
  invertDisplay(false, display);
  dispLetter(displayLetter, display);
  */
  // Initiate random generator.

  randomSeed(analogRead(0));
}

void loop()
{
  statemachine();
}

void statemachine()
{
  switch (state)
  {
  case ConnectWiFi:
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      WiFiConnect(ssid, pass);
      state = ConnectServer;
    }
    else
    {
      state = ConnectServer;
    }
    ServerConnect(host, port);
    break;
  }
  case ConnectServer:
  {
    if (!client.connect(host, port))
    {
      Serial.print(".");
      delay(200);
      state = ConnectServer;
    }
    else
    {
      Serial.println("");
      Serial.println("Connected to server.");
      state = SendServer;
    }
    break;
  }
  case SendServer:
  {
    if (!registered)
    {
      state = RegisterServer;
    }
    else if (winState)
    {
      state = ResetState;
    }
    else if (registered && !letterSet)
    {
      state = LetterServer;
    }
    else if (registered && letterSet && isMaster)
    {
      state = CheckServer;
    }
    else
    {
      Serial.println("Nothing to do!");
      state = ReceiveServer;
    }
    break;
  }
  case RegisterServer:
  {
    ServerSend("0", client);
    registered = true;
    state = ReceiveServer;
    break;
  }
  case ResetState:
  {
    isMaster = false;
    letterSet = false;
    displayLetter = "?";
    winState = false;
    state = ReceiveServer;
    break;
  }
  case LetterServer:
  {
    if (validLetter(displayLetter))
    {
      ServerSend("2:1", client);
      letterSet = true;
      invertDisplay(false, display);
      dispLetter(displayLetter, display);
    }
    else
    {
      ServerSend("2:0", client);
      letterSet = false;
    }
    state = ReceiveServer;
    break;
  }
  case CheckServer:
  {
    String message = "3:" + checkWord;
    ServerSend(message, client);
    state = ReceiveServer;
    break;
  }
  case ReceiveServer:
  {
    timeout = millis() + SERVER_RECEIVE_TIMEOUT;
    while (millis() < timeout)
    {
      client = server.available();
      if (client)
      {
        Serial.print("");
        while (client.connected())
        {
          if (client.available())
          {
            serverMessage = client.readStringUntil('\r');
            Serial.println("");
            Serial.print("Message received from server: ");
            Serial.println(serverMessage);
            serverMessageReceived = true;
          }
        }
      }
    }
    if (serverMessageReceived)
    {
      String command = splitString(serverMessage, ':', 0);
      String parameter = splitString(serverMessage, ':', 1);
      if (command == "1")
      {
        displayLetter = parameter;
        if (validLetter(parameter))
        {
          isMaster = true;       // <<<------ temp. for testing without IR!
          state = ConnectServer; // <<<------ temp. for testing without IR!
          //state = IRBroadcastEW;    // <<<------ actual path for code
        }
        else
        {
          state = ConnectServer;
        }
      }
      else if (command == "4")
      {
        winState = parameter == "1";
        if (winState)
        {
          invertDisplay(true, display);
        }
        state = ConnectServer; // <<<------ temp. for testing without IR!
        //state = IRBroadcastEW;      // <<<------ actual path for code
      }
      serverMessageReceived = false;
    }
    else
    {
      state = ConnectServer;
    }
    break;
  }
  case IRBroadcastEW:
  {
    Serial.println("Starting Broadcast");
    resetRole();

    //int hex = 0x01;
    int receivedAt = -1;
    int received = -1;
    int startTimeBroadcast = millis();
    int endTimeBroadcast = startTimeBroadcast;

    int randomNum = random(0, 4);

    while ((endTimeBroadcast - startTimeBroadcast) <= 15000)
    {
      randomNum = random(0, 4);

      for (int i = 0; i < amountOfDirectionsEW; i++)
      {
        // Turn on receiver.
        turnOnReceiver(directionsRecEW[i]);

        int startTimeDelay = millis();
        int endTimeDelay = startTimeDelay;
        while ((endTimeDelay - startTimeDelay) <= 500)
        {
          endTimeDelay = millis();
        }

        // Send at the direction.
        Serial.print("Sending at: ");
        Serial.println(directionsStrEW[i]);
        if (i == 0)
        {
          turnOnSender(directionsSendEW[i]);
          irsend.sendNEC(0xA, kNECBits, 1);
          turnOffSender(directionsSendEW[i]);
        }
        else if (i == 1)
        {
          turnOnSender(directionsSendEW[i]);
          irsend.sendNEC(0xB, kNECBits, 1);
          turnOffSender(directionsSendEW[i]);
        }

        // Check to see if message is received at the same direction.
        if (irrecv.decode(&results))
        {
          int startTimeRec = millis();
          int endTimeRec = startTimeRec;

          receivedAt = i;
          while ((endTimeRec - startTimeRec) <= 1000)
          {
            received = recIR(irrecv, results, receivedAt);
            delay(10);
            establishRole(received, directionsStrEW[receivedAt]);

            // Send IR same direction.
            if (i == 0)
            {
              turnOnSender(directionsSendEW[i]);
              irsend.sendNEC(0xA, kNECBits, 1);
              turnOffSender(directionsSendEW[i]);
            }
            else if (i == 1)
            {
              turnOnSender(directionsSendEW[i]);
              irsend.sendNEC(0xB, kNECBits, 1);
              turnOffSender(directionsSendEW[i]);
            }
            //Send IR opposite direction.
            turnOffSender(directionsSendEW[i]);
            if (i == 0)
            { // If just received at west, send east.
              turnOnSender(directionsSendEW[1]);
              sendIR(0xB, irsend);
              turnOffSender(directionsSendEW[1]);
            }
            else if (i == 1)
            { // If just received at east, send west.
              turnOnSender(directionsSendEW[0]);
              sendIR(0xA, irsend);
              turnOffSender(directionsSendEW[0]);
            }

            if (received == 0xB || received == 0xA)
            {
              break;
            }

            endTimeRec = millis();
          }
        }
        turnOffReceiver(directionsRecEW[i]);
      }

      if (randomNum == 2)
      {
        int startTimeSend = millis();
        int endTimeSend = startTimeSend;

        int randomSender = random(0, 2);
        turnOnSender(directionsSendEW[randomSender]);
        while ((endTimeSend - startTimeSend) <= 1000)
        {
          if (randomSender == 0)
          {
            irsend.sendNEC(0xA, kNECBits, 1);
          }
          else if (randomSender == 1)
          {
            irsend.sendNEC(0xB, kNECBits, 1);
          }
          delay(10);
          endTimeSend = millis();
        }
        turnOffSender(directionsSendEW[randomSender]);
      }

      delay(10);
      endTimeBroadcast = millis();
    }
    printRole();
    
    if (role == "Master")
    {
      displayLetter = "M";
      dispLetter(displayLetter, display);
      //state = IRTransmitAndReceiveDataE;
    }
    else if (role == "End")
    {
      displayLetter = "E";
      dispLetter(displayLetter, display);
      //state = IRTransmitAndReceiveDataW;
    }
    else if (role == "Relay")
    {
      displayLetter = "R";
      dispLetter(displayLetter, display);
      //state = IRTransmitAndReceiveDataWE;
    }
    else if (role = "None")
    {
      displayLetter = "N";
      dispLetter(displayLetter, display);
      //state = IRBroadcastEW;
    }
    

    state = IRBroadcastEW;
    break;
  }

  case IRTransmitAndReceiveDataE:
  {
    Serial.println("IRTransmitAndReceiveDataE");
    char letter = displayLetter.charAt(0);
    int letterAsHex = (int)letter;
    Serial.print("My letter: ");
    Serial.println(letter);
    int receivedAt = -1;
    int received = -1;
    int startTimeBroadcast = millis();
    int endTimeBroadcast = startTimeBroadcast;

    int randomNum = random(0, 4);

    while ((endTimeBroadcast - startTimeBroadcast) <= 15000)
    {
      randomNum = random(0, 4);

      // Turn on receiver.
      turnOnReceiver(directionsRecEW[1]);

      int startTimeDelay = millis();
      int endTimeDelay = startTimeDelay;
      while ((endTimeDelay - startTimeDelay) <= 500)
      {
        endTimeDelay = millis();
      }

      // Send at the direction.
      Serial.print("Sending at: ");
      Serial.println(directionsStrEW[1]);

      turnOnSender(directionsSendEW[1]);
      irsend.sendNEC(letterAsHex, kNECBits, 1);
      turnOffSender(directionsSendEW[1]);

      // Check to see if message is received at the same direction.
      if (irrecv.decode(&results))
      {
        int startTimeRec = millis();
        int endTimeRec = startTimeRec;

        receivedAt = 1;
        while ((endTimeRec - startTimeRec) <= 1000)
        {
          received = recIR(irrecv, results, receivedAt);
          delay(10);
          endTimeRec = millis();
        }
      }
      turnOffReceiver(directionsRecEW[1]);

      if (randomNum == 2)
      {
        int startTimeSend = millis();
        int endTimeSend = startTimeSend;

        turnOnSender(directionsSendEW[1]);
        while ((endTimeSend - startTimeSend) <= 1000)
        {
          irsend.sendNEC(letterAsHex, kNECBits, 1);
          delay(10);
          endTimeSend = millis();
        }
        turnOffSender(directionsSendEW[1]);
      }
      Serial.print("Char received: ");
      Serial.println(char(received));
      delay(10);
      endTimeBroadcast = millis();
    }
    state = IRBroadcastEW;
    break;
  }

  case IRTransmitAndReceiveDataW:
  {
    Serial.println("IRTransmitAndReceiveDataW");
    char letter = displayLetter.charAt(0);
    int letterAsHex = (int)letter;
    Serial.print("My letter: ");
    Serial.println(letter);
    int receivedAt = -1;
    int received = -1;
    int startTimeBroadcast = millis();
    int endTimeBroadcast = startTimeBroadcast;

    int randomNum = random(0, 4);

    while ((endTimeBroadcast - startTimeBroadcast) <= 15000)
    {
      randomNum = random(0, 4);

      // Turn on receiver.
      turnOnReceiver(directionsRecEW[0]);

      int startTimeDelay = millis();
      int endTimeDelay = startTimeDelay;
      while ((endTimeDelay - startTimeDelay) <= 500)
      {
        endTimeDelay = millis();
      }

      // Send at the direction.
      Serial.print("Sending at: ");
      Serial.println(directionsStrEW[0]);

      turnOnSender(directionsSendEW[0]);
      irsend.sendNEC(letterAsHex, kNECBits, 1);
      turnOffSender(directionsSendEW[0]);

      // Check to see if message is received at the same direction.
      if (irrecv.decode(&results))
      {
        int startTimeRec = millis();
        int endTimeRec = startTimeRec;

        receivedAt = 0;
        while ((endTimeRec - startTimeRec) <= 1000)
        {
          received = recIR(irrecv, results, receivedAt);
          delay(10);
          endTimeRec = millis();
        }
      }
      turnOffReceiver(directionsRecEW[0]);

      if (randomNum == 2)
      {
        int startTimeSend = millis();
        int endTimeSend = startTimeSend;

        turnOnSender(directionsSendEW[0]);
        while ((endTimeSend - startTimeSend) <= 1000)
        {
          irsend.sendNEC(letterAsHex, kNECBits, 1);
          delay(10);
          endTimeSend = millis();
        }
        turnOffSender(directionsSendEW[0]);
      }
      Serial.print("Char received: ");
      Serial.println(char(received));
      delay(10);
      endTimeBroadcast = millis();
    }
    state = IRBroadcastEW;
    break;
  }
  case IRTransmitAndReceiveDataWE:
  {
    Serial.println("IRTransmitAndReceiveDataWE");

    char letter = displayLetter.charAt(0);
    int letterAsHex = (int)letter;
    Serial.print("My letter: ");
    Serial.println(letter);
    Serial.print("My letter as int: ");
    Serial.println(letterAsHex);
    Serial.print("My letter again: ");
    Serial.println(char(letterAsHex));
    int receivedAt = -1;
    int received = -1;
    int startTimeBroadcast = millis();
    int endTimeBroadcast = startTimeBroadcast;

    int randomNum = random(0, 4);

    while ((endTimeBroadcast - startTimeBroadcast) <= 15000)
    {
      randomNum = random(0, 4);

      for (int i = 0; i < amountOfDirectionsEW; i++)
      {
        // Turn on receiver.
        turnOnReceiver(directionsRecEW[i]);

        int startTimeDelay = millis();
        int endTimeDelay = startTimeDelay;
        while ((endTimeDelay - startTimeDelay) <= 500)
        {
          endTimeDelay = millis();
        }

        // Send at the direction.
        Serial.print("Sending at: ");
        Serial.println(directionsStrEW[i]);

        turnOnSender(directionsSendEW[i]);
        irsend.sendNEC(letterAsHex, kNECBits, 1);
        turnOffSender(directionsSendEW[i]);

        // Check to see if message is received at the same direction.
        if (irrecv.decode(&results))
        {
          int startTimeRec = millis();
          int endTimeRec = startTimeRec;

          receivedAt = i;
          while ((endTimeRec - startTimeRec) <= 1000)
          {
            received = recIR(irrecv, results, receivedAt);
            if (isLetter(received))
            {
              break;
            }
            delay(10);

            if (i == 0)
            { // If just received at west, send east.
              turnOnSender(directionsSendEW[1]);
              sendIR(letterAsHex, irsend);
              turnOffSender(directionsSendEW[1]);
            }
            else if (i == 1)
            { // If just received at east, send west.
              turnOnSender(directionsSendEW[0]);
              sendIR(letterAsHex, irsend);
              turnOffSender(directionsSendEW[0]);
            }

            endTimeRec = millis();
          }
        }
        turnOffReceiver(directionsRecEW[i]);
      }

      if (randomNum == 2)
      {
        int startTimeSend = millis();
        int endTimeSend = startTimeSend;

        int randomSender = random(0, 2);
        turnOnSender(directionsSendEW[randomSender]);
        while ((endTimeSend - startTimeSend) <= 1000)
        {
          irsend.sendNEC(letterAsHex, kNECBits, 1);
          delay(10);
          endTimeSend = millis();
        }
        turnOffSender(directionsSendEW[randomSender]);
      }
      Serial.print("Int received: ");
      Serial.println(received);
      Serial.print("Char received: ");
      Serial.println(char(received));
      delay(10);
      endTimeBroadcast = millis();
    }
    state = IRBroadcastEW;
    break;
  }
  }
}

void establishRole(uint32_t message, String direction)
{
  for (int i = 0; i < amountOfDirectionsEW; i++)
  {
    if (message == 0xB && direction == directionsStrEW[0]) // WEST.
    {
      neighbours[0] = 1;
    }
    else if (message == 0xA && direction == directionsStrEW[1]) // EAST
    {
      neighbours[2] = 1;
    }
    else
    { // Nothing.
    }
  }
  makeRole();
}

void resetRole()
{
  for (int i = 0; i < sizeOfNeighbours; i++)
  {
    neighbours[i] = 0;
  }
  isMaster = 0;
  isRelay = 0;
  isEnd = 0;
}

void makeRole()
{
  if (neighbours[0] == 1 && neighbours[2] == 0)
  {
    role = "End";
  }
  else if (neighbours[0] == 1 && neighbours[2] == 1)
  {
    role = "Relay";
  }
  else if (neighbours[0] == 0 && neighbours[2] == 1)
  {
    role = "Master";
  }
  else if (neighbours[0] == 0 && neighbours[2] == 0)
  {
    role = "None";
  }
}

void printRole()
{
  Serial.print("Role: ");
  Serial.print(role);
  Serial.println(" established.");
}

/*
void checkMessage(uint32_t message, String direction)
{
  Serial.println(message, HEX);
  if (message == 0xFFE01F)
  {
    establishRole(message, direction);
  }
  
  else if (isalpha(message))
  {
    Serial.println(char(message));
  }

}
*/

void turnOnReceiver(int receiver)
{
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(receiver);
  Wire.endTransmission();
}

void turnOffReceiver(int receiver)
{
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(receiver);
  Wire.endTransmission();
}

void turnOnSender(int sender)
{
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(sender);
  Wire.endTransmission();
}

void turnOffSender(int sender)
{
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(sender);
  Wire.endTransmission();
}

boolean isLetter(int received)
{
  char letter = (char)received;

  if (isalpha(letter))
  {
    Serial.println("Is letter!");
    return true;
  }
  else
  {
    Serial.println("Not letter!");
    return false;
  }
}