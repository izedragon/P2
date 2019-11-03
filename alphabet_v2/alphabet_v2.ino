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

// Hardcoded node ID in hexadecimal.

const uint16_t ID = 0x4E32;
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
  IRBroadcastReceiveEW,
  IRReceiveDataEW,
  IRReceiveDataE,
  IRReceiveDataW,
  IRTransmitDataE,
  IRTransmitDataEW,
  IRTransmitDataW,
};
enum States state = IRBroadcastReceiveEW;

// Variables related to the directions of sending and receiving.

String directionsStrEW[2] = {"WEST", "EAST"};

int directionsRecEW[2] = {0x01, 0x04};  // WEST, EAST
int directionsSendEW[2] = {0x10, 0x40}; // WEST, EAST
int amountOfDirectionsEW = 2;           // 2
// Variables related to the neighbors that the node has and it's role.

int neighbours[3] = {0, 0, 0};
int sizeOfNeighbours = 3;
String role = "None";

// Messages received at east and west.

uint8_t lettersReceived[3] = {0, 0, 0};
int lettersReceivedLength = 3;
int fillLettersReceived = 0;

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
String displayLetter = "D";
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
  invertDisplay(false, display);
  dispLetter(displayLetter, display);

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

  /**
   * In this state the node will Broadcasts it's presence at the same time that it's listening.
   * This is accomplished by turning on the receiver corresponding to the direction of the broadcast
   * and waiting for the interrupt to occur (500 milliseconds). If the interrupt occurs the node listens at the direction
   * in which it had received an interrupt. 
   * 
   * Additional functionality:
   * 1) Sends at east/west alternatingly for 2 seconds 1/4th of while loop executions.
   * 2) Breaks out of a listening-loop if the 'correct' message is received. 
   * 3) Sends once at the opposite direction of the received message direction. 
   * 
   * Written by: Daniel Abella with help from Michael Ong based on original code for sending an receiving created by: Daniel Abella, Fong To and Ahmed Abdulkader (and the author of the library IRremoteESP8266).
   **/

  case IRBroadcastReceiveEW:
  {
    Serial.println("Starting Broadcast");
    resetRole();

    int receivedAt = -1;
    uint32_t received = -1;
    int startTimeBroadcast = millis();
    int endTimeBroadcast = startTimeBroadcast;

    uint8_t west = 0x3C; // '3C' is the same as '<'
    uint8_t east = 0x3E; // '3E' is the same as '>'

    uint32_t messageToSendEast = (ID << 8) | (east);
    uint32_t messageToSendWest = (ID << 8) | (west);

    int randomNum = random(0, 4);

    // Run the code within this state (BroadcastReceiveEW) for 1 minute so that the nodes have a high probability of establishing their neighbours.

    while ((endTimeBroadcast - startTimeBroadcast) <= 60000)
    {
      randomNum = random(0, 4);

      for (int i = 0; i < amountOfDirectionsEW; i++)
      {
        // Turn on receiver.
        delay(10);
        turnOnReceiver(directionsRecEW[i]);
        delay(10);
        int startTimeDelay = millis();
        int endTimeDelay = startTimeDelay;

        // Delay (500 milliseconds) so that the receiver has a chance to receive a message and trigger the interrupt-routine.
        while ((endTimeDelay - startTimeDelay) <= 500)
        {
          endTimeDelay = millis();
        }

        // Send at the direction defined in the for loop (i = 0 -> West, i = 1 -> East).
        Serial.print("Sending at: ");
        Serial.println(directionsStrEW[i]);
        if (i == 0)
        {
          delay(10);
          turnOnSender(directionsSendEW[i]);
          delay(10);
          irsend.sendNEC(messageToSendWest, kNECBits, 1);
          delay(10);
          turnOffSender(directionsSendEW[i]);
          delay(10);
        }
        else if (i == 1)
        {
          delay(10);
          turnOnSender(directionsSendEW[i]);
          delay(10);
          irsend.sendNEC(messageToSendEast, kNECBits, 1);
          delay(10);
          turnOffSender(directionsSendEW[i]);
          delay(10);
        }

        // Check to see if message is received at the same direction.
        if (irrecv.decode(&results))
        {
          Serial.print("Interrupt received: ");
          serialPrintUint64(results.value, HEX);

          int startTimeRec = millis();
          int endTimeRec = startTimeRec;

          receivedAt = i;
          while ((endTimeRec - startTimeRec) <= 2500) // Listen in the direction defined by the for loop (west->east, repeat inside encapsulating while loop).
          {
            received = results.value;
            if (receivedCorrect(received, directionsStrEW[i]))
            {
              establishRole(received, directionsStrEW[receivedAt]); // Eastablishes the role of the node based on the received message. 
              break;                                                // This is done continually in the 1 minute loop so that the node has time to receive the correct
            }                                                       // message at both directions(West/East). 
            delay(10);
            received = recIR(irrecv, results, receivedAt);
            delay(10);
            establishRole(received, directionsStrEW[receivedAt]);

            // Send IR same direction.
            if (i == 0)
            {
              delay(10);
              turnOnSender(directionsSendEW[i]);
              delay(10);
              irsend.sendNEC(messageToSendWest, kNECBits, 1);
              delay(10);
              turnOffSender(directionsSendEW[i]);
              delay(10);
            }
            else if (i == 1)
            {
              delay(10);
              turnOnSender(directionsSendEW[i]);
              delay(10);
              irsend.sendNEC(messageToSendEast, kNECBits, 1);
              delay(10);
              turnOffSender(directionsSendEW[i]);
              delay(10);
            }
            //Send IR opposite direction.
            delay(10);
            turnOffSender(directionsSendEW[i]);
            delay(10);
            if (i == 0)
            { // If just received at west, send east.
              delay(10);
              turnOnSender(directionsSendEW[1]);
              delay(10);
              irsend.sendNEC(messageToSendEast, kNECBits, 1);
              delay(10);
              turnOffSender(directionsSendEW[1]);
              delay(10);
            }
            else if (i == 1)
            { // If just received at east, send west.
              delay(10);
              turnOnSender(directionsSendEW[0]);
              delay(10);
              irsend.sendNEC(messageToSendWest, kNECBits, 1);
              delay(10);
              turnOffSender(directionsSendEW[0]);
              delay(10);
            }
            // If the correct message was received, stop listening.
            if (receivedCorrect(received, directionsStrEW[i]))
            {
              break;
            }

            endTimeRec = millis();
          }
          delay(10);
          turnOffReceiver(directionsRecEW[i]);
          delay(10);

          // Listen at the other direction (If listened at West previously -> listen at East and vice versa) for 2.5 seconds.
          int listenDir = -1;
          if (receivedAt == 0)
          {
            listenDir = 1;
          }
          else if (receivedAt == 1)
          {
            listenDir = 0;
          }
          delay(10);
          turnOnReceiver(directionsRecEW[listenDir]);
          delay(10);
          int startListenOtherDir = millis();
          int endListenOtherDir = startListenOtherDir;

          while ((endListenOtherDir - startListenOtherDir) <= 2500)
          {
            delay(10);
            received = recIR(irrecv, results, listenDir);
            receivedAt = listenDir;
            delay(10);
            establishRole(received, directionsStrEW[listenDir]);
            endListenOtherDir = millis();
          }
          delay(10);
          turnOffReceiver(directionsRecEW[listenDir]);
          delay(10);
        }
        delay(10);
        turnOffReceiver(directionsRecEW[i]);
        delay(10);
      }
      // If the random number generated is equal to 2 (1/4th probability) the node sends messages alternating between east and west for 2 seconds.
      if (randomNum == 2)
      {
        int startTimeSend = millis();
        int endTimeSend = startTimeSend;

        int randomSender = random(0, 2);
        delay(10);
        turnOnSender(directionsSendEW[randomSender]);
        delay(10);
        while ((endTimeSend - startTimeSend) <= 2000)
        {
          if (randomSender == 0)
          {
            delay(10);
            irsend.sendNEC(messageToSendWest, kNECBits, 1);
            delay(10);
          }
          else if (randomSender == 1)
          {
            delay(10);
            irsend.sendNEC(messageToSendEast, kNECBits, 1);
            delay(10);
          }
          delay(10);
          randomSender = (++randomSender) % 2;
          endTimeSend = millis();
        }
        delay(10);
        turnOffSender(directionsSendEW[randomSender]);
        delay(10);
      }

      delay(10);
      endTimeBroadcast = millis();
    }
    Serial.println(role);
    establishRole(received, directionsStrEW[receivedAt]);
    Serial.println(role);

    // Determines which state the node should transition to depending on its role.

    if (role == "Master")
    {
      state = IRTransmitDataE;
    }
    else if (role == "Relay")
    {
      state = IRReceiveDataW;
    }
    else if (role == "End")
    {
      state = IRReceiveDataW;
    }
    else if (role == "None")
    {
      state = IRBroadcastReceiveEW;
    }
    break;
  }
  case IRReceiveDataEW:
  {
    Serial.println("IRReceiveDataEW");
    break;
  }

  case IRReceiveDataW:
  {
    Serial.println("IRREceiveDataW");
    break;
  }
  case IRReceiveDataE:
  {
    Serial.println("IRReceiveDataE");
    break;
  }
  case IRTransmitDataE:
  {
    Serial.println("IRTransmitDataE");
    state = IRTransmitDataE;
    break;
  }

  case IRTransmitDataW:
  {
    Serial.println("IRTransmiteDataW");
    break;
  }
  }
}

/**
 * Sets the neighbours array which is defined as; {0,0,0} -> {neighbourWest, Itself, neighbourEast}
 * A check is performed where the node has to receive the correct message at either listening direction as follows;
 * 
 * -- If it listens at West it needs to receive a message sent from East (contains > in the message).
 * -- If it listens at East it needs to receive a message sent from West (contains < in the message).
 * 
 */

void establishRole(uint32_t message, String direction)
{
  for (int i = 0; i < amountOfDirectionsEW; i++)
  {
    if ((message & 0x000000FF) == 0x3E && direction == directionsStrEW[0]) // Listening at West, received from East.
    {
      neighbours[0] = 1;
    }
    else if ((message & 0x000000FF) == 0x3C && direction == directionsStrEW[1]) // Listening at East, received from West. 
    {
      neighbours[2] = 1;
    }
    else
    { // Nothing.
    }
  }
  makeRole();
}

/**
 * Sets the neighbours array to its original state -> {0,0,0}
 */
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

/**
 * Sets the role of the node depending on what the neigbours array looks like;
 * -- Example, {1,0,0} means a neighbour exists to the left only and as such the node is at the farthest right in the formation, a role called "End".
 * 
 * Also displays a circle only at West, only at East or at both directions depending on where the node has neigbours.
 */

void makeRole()
{
  if (neighbours[0] == 1 && neighbours[2] == 0) // Neighbour only at West. 
  {
    showNeighbourCircle(display, directionsStrEW[0]);
    role = "End";
  }
  else if (neighbours[0] == 1 && neighbours[2] == 1) // Neigbbour at both East and West direction.
  {
    showNeighbourCircle(display, directionsStrEW[0]);
    showNeighbourCircle(display, directionsStrEW[1]);
    role = "Relay";
  }
  else if (neighbours[0] == 0 && neighbours[2] == 1) // Neighbour only at East.
  {
    showNeighbourCircle(display, directionsStrEW[1]);
    role = "Master";
  }
  else if (neighbours[0] == 0 && neighbours[2] == 0) // No neighbours found.
  {
    dispLetter(displayLetter, display);
    role = "None";
  }
}

void printRole()
{
  Serial.print("Role: ");
  Serial.print(role);
  Serial.println(" established.");
}

void turnOnReceiver(int receiver)
{
  delay(10);
  Wire.beginTransmission(slaveAddress);
  delay(10);
  Wire.write(0x09);
  delay(10);
  Wire.write(receiver);
  delay(10);
  Wire.endTransmission();
  delay(10);
}

void turnOffReceiver(int receiver)
{
  delay(10);
  Wire.beginTransmission(slaveAddress);
  delay(10);
  Wire.write(0x09);
  delay(10);
  Wire.write(receiver);
  delay(10);
  Wire.endTransmission();
  delay(10);
}

void turnOnSender(int sender)
{
  delay(10);
  Wire.beginTransmission(slaveAddress);
  delay(10);
  Wire.write(0x09);
  delay(10);
  Wire.write(sender);
  delay(10);
  Wire.endTransmission();
  delay(10);
}

void turnOffSender(int sender)
{
  delay(10);
  Wire.beginTransmission(slaveAddress);
  delay(10);
  Wire.write(0x09);
  delay(10);
  Wire.write(sender);
  delay(10);
  Wire.endTransmission();
  delay(10);
}

// Used in previous versions where the role of the node was displayed on the screen instead of dots indicating at what direction neigbours have been found.

void displayRole()
{
  makeRole();
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
}

/**
 * Checks if the message at a certain direction is the one expected if the node is attempting to establish neighbours.
 * 
 * Example -- If the message is received at direction West then it should contain an indicator that is was sent from another nodes East (in this case '>')
 * 
 */
boolean receivedCorrect(uint32_t message, String direction)
{
  boolean correct = false;
  if ((message & 0x000000FF) == 0x3E && direction == directionsStrEW[0]) // // Listening at West, received from East.
  {
    correct = true;
  }
  else if ((message & 0x000000FF) == 0x3C && direction == directionsStrEW[1]) // // Listening at Eest, received from West.
  {
    correct = true;
  }
  else
  {
    // Do nothing.
  }
  return false;
}

