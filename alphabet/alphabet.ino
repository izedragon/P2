enum StateM_enum {ConnectWiFi, ConnectServer, SendServer, RegisterServer, LetterServer, CheckServer,
                   ReciveServer, IRBroadcastEW, IRListenEW, IRTransmitDataE, IRReceiveDataE, IRReciveDataWE,
                   IRTransmitDataW, IRReceiveDataW
                  };

void statemachine();
int condition;
enum StateM_enum state = ConnectWiFi;
String t2 = String(22);
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
  Serial.begin(115200);
  delay(10);
}

void loop() {
  
    statemachine();
    delay(10);
  
}
/*
String read_IR() {
  condition=Serial.read();
  return condition;
}
*/
void statemachine()
{
 if (Serial.available() > 0) {
  Serial.println(state);
  condition = Serial.parseInt();

  switch (state)
  {
    case ConnectWiFi:
      if (condition==1) {
        Serial.println("ConnectWiFi");
        state = ConnectWiFi;
      } else {
        Serial.println("ConnectWiFi");
        state = ConnectServer;
      }
      break;

    case ConnectServer:
      if (condition==2) {
        Serial.println("ConnectServer");
        state = SendServer;
        //state = ConnectServer;
      } else {
        Serial.println("ConnectServer");
        //state = SendServer;
        state = ConnectServer;
      }
      break;

    case SendServer:
      if (condition==1) {
        Serial.println("SendServer");
        state = RegisterServer;
      } else if (condition==2) {
        Serial.println("SendServer");
        state = LetterServer;
      } else if (condition==3) {
        Serial.println("SendServer");
        state = CheckServer;
      }
      break;

    case RegisterServer:
      Serial.println("RegisterServer");
      state = ReciveServer;
      break;

    case LetterServer:
      Serial.println("LetterServer");
      state = ReciveServer;
      break;

    case CheckServer:
      Serial.println("CheckServer");
      state = ReciveServer;
      break;

    case ReciveServer:
      if (condition==24) {
        Serial.println("ReciveServer");
        state = SendServer;
      } else if (condition==25) {
        Serial.println("ReciveServer");
        state = IRBroadcastEW;
      }
      break;

    case IRBroadcastEW:
      if (condition==26) {
        Serial.println("IRBroadcastEW");
        state = IRBroadcastEW;
      } else {
        Serial.println("IRBroadcastEW");
        state = IRListenEW;
      }
      break;

    case IRListenEW:
      if (condition==5) {
        Serial.println("IRListenEW");
        state = IRListenEW;
      } else if (condition==6) {
        Serial.println("IRListenEW");
        state = IRBroadcastEW;
      } else if (condition==7) {
        Serial.println("IRListenEW");
        state = IRTransmitDataE;
      } else if (condition==8) {
        Serial.println("IRListenEW");
        state = IRTransmitDataW;
      } else if (condition==9) {
        Serial.println("IRListenEW");
        state = IRReciveDataWE;
      }
      break;

    case IRTransmitDataE:
      if (condition==10) {
        Serial.println("IRTransmitDataE");
        state = ReciveServer;
      } else if (condition==11) {
        Serial.println("IRTransmitDataE");
        state = IRBroadcastEW;
      } else if (condition==12) {
        Serial.println("IRTransmitDataE");
        state = IRReceiveDataE;
      }
      break;

    case IRReceiveDataE:
      if (condition==false) {
        Serial.println("IRReceiveDataE");
        state = SendServer;
      } else{
        Serial.println("IRReceiveDataE");
        state = IRReceiveDataE;
      }
      break;

    case IRReciveDataWE:
      if (condition==13) {
        Serial.println("IRReciveDataWE");
        state = IRReciveDataWE;
      } else if (condition==14) {
        Serial.println("IRReciveDataWE");
        state = IRTransmitDataE;
      } else if (condition==15) {
        Serial.println("IRReciveDataWE");
        state = IRTransmitDataW;
      } else if (condition==16) {
        Serial.println("IRReciveDataWE");
        state = IRBroadcastEW;
      }
      break;

    case IRTransmitDataW:
      if (condition==true) {
        Serial.println("IRTransmitDataW");
        state = IRBroadcastEW;
      } else {
        Serial.println("IRTransmitDataW");
        state = IRReceiveDataW;
      }
      break;

    case IRReceiveDataW:
      if (condition==17) {
        Serial.println("IRReceiveDataW");
        state = IRReceiveDataW;
      } else if (condition==19) {
        Serial.println("IRReceiveDataW");
        state = ReciveServer;
      } else if (condition==20) {
        Serial.println("IRReceiveDataW");
        state = IRBroadcastEW;
      } else if (condition==21) {
        Serial.println("IRReceiveDataW");
        state = IRBroadcastEW;
      }
      break;
  }
 }
}
