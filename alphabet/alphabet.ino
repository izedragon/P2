enum StateM_enum {ConnectWiFi, ConnectServer, SendServer, RegisterServer, LetterServer, CheckServer,
                  ReciveServer, IRBroadcastEW, IRListenEW, IRTransmitDataE, IRReceiveDataE, IRReciveDataWE,
                  IRTransmitDataW, IRReceiveDataW
                 };

void statemachine();
int condition=0;
enum StateM_enum state = ConnectWiFi;

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

void statemachine()
{
  if (Serial.available() > 0) {
    //Serial.println(state);
    condition = 0;
    condition = Serial.parseInt();

    switch (state)
    {
      case ConnectWiFi:
        Serial.println("ConnectWiFi");
        if (condition == 1) {
          Serial.println("ConnectWiFi-ConnectWiFi");
          state = ConnectWiFi;
        } else if (condition == 2) {
          Serial.println("ConnectWiFi-ConnectServer");
          state = ConnectServer;
        }
        break;

      case ConnectServer:
        Serial.println("ConnectServer");
        if (condition == 3) {
          Serial.println("ConnectServer-SendServer");
          state = SendServer;
        } else if (condition == 4) {
          Serial.println("ConnectServer-ConnectServer");
          state = ConnectServer;
        }
        break;

      case SendServer:
        Serial.println("SendServer");
        if (condition == 5) {
          Serial.println("SendServer-RegisterServer");
          state = RegisterServer;
        } else if (condition == 6) {
          Serial.println("SendServer-LetterServer");
          state = LetterServer;
        } else if (condition == 7) {
          Serial.println("SendServer-CheckServer");
          state = CheckServer;
        }
        break;

      case RegisterServer:
        Serial.println("RegisterServer");
        Serial.println("RegisterServer-ReciveServer");
        state = ReciveServer;
        break;

      case LetterServer:
        Serial.println("LetterServer");
        Serial.println("LetterServer-ReciveServer");
        state = ReciveServer;
        break;

      case CheckServer:
        Serial.println("CheckServer");
        Serial.println("CheckServer-ReciveServer");
        state = ReciveServer;
        break;

      case ReciveServer:
        Serial.println("ReciveServer");
        if (condition == 8) {
          Serial.println("ReciveServer-SendServer");
          state = SendServer;
        } else if (condition == 9) {
          Serial.println("ReciveServer-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 32) {
          Serial.println("ReciveServer-ReciveServer");
          state = ReciveServer;
        } else if (condition == 34) {
          Serial.println("ReciveServer-IRBroadcastEW");
          state = IRBroadcastEW;
        }
        break;

      case IRBroadcastEW:
        Serial.println("IRBroadcastEW");
        if (condition == 10) {
          Serial.println("IRBroadcastEW-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 11) {
          Serial.println("IRBroadcastEW-IRListenEW");
          state = IRListenEW;
        }
        break;

      case IRListenEW:
        Serial.println("IRListenEW");
        if (condition == 12) {
          Serial.println("IRListenEW-IRListenEW");
          state = IRListenEW;
        } else if (condition == 13) {
          Serial.println("IRListenEW-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 14) {
          Serial.println("IRListenEW-IRTransmitDataE");
          state = IRTransmitDataE;
        } else if (condition == 15) {
          Serial.println("IRListenEW-IRTransmitDataW");
          state = IRTransmitDataW;
        } else if (condition == 16) {
          Serial.println("IRListenEW-IRReciveDataWE");
          state = IRReciveDataWE;
        }
        break;

      case IRTransmitDataE:
        Serial.println("IRTransmitDataE");
        if (condition == 17) {
          Serial.println("IRTransmitDataE-ReciveServer");
          state = ReciveServer;
        } else if (condition == 18) {
          Serial.println("IRTransmitDataE-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 19) {
          Serial.println("IRTransmitDataE-IRReceiveDataE");
          state = IRReceiveDataE;
        }
        break;

      case IRReceiveDataE:
        Serial.println("IRReceiveDataE");
        if (condition == 20) {
          Serial.println("IRReceiveDataE-SendServer");
          state = SendServer;
        } else if (condition == 21) {
          Serial.println("IRReceiveDataE-IRReceiveDataE");
          state = IRReceiveDataE;
        } else if (condition == 33) {
          Serial.println("IRReceiveDataE-IRBroadcastEW");
          state = IRBroadcastEW;
        }
        break;

      case IRReciveDataWE:
        Serial.println("IRReciveDataWE");
        if (condition == 22) {
          Serial.println("IRReciveDataWE-IRReciveDataWE");
          state = IRReciveDataWE;
        } else if (condition == 23) {
          Serial.println("IRReciveDataWE-IRTransmitDataE");
          state = IRTransmitDataE;
        } else if (condition == 24) {
          Serial.println("IRReciveDataWE-IRTransmitDataW");
          state = IRTransmitDataW;
        } else if (condition == 25) {
          Serial.println("IRReciveDataWE-IRBroadcastEW");
          state = IRBroadcastEW;
        }
        break;

      case IRTransmitDataW:
        Serial.println("IRTransmitDataW");
        if (condition == 26) {
          Serial.println("IRTransmitDataW-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 27) {
          Serial.println("IRTransmitDataW-IRReceiveDataW");
          state = IRReceiveDataW;
        }
        break;

      case IRReceiveDataW:
        Serial.println("IRReceiveDataW");
        if (condition == 28) {
          Serial.println("IRReceiveDataW-IRReceiveDataW");
          state = IRReceiveDataW;
        } else if (condition == 29) {
          Serial.println("IRReceiveDataW-ReciveServer");
          state = ReciveServer;
        } else if (condition == 30) {
          Serial.println("IRReceiveDataW-IRBroadcastEW");
          state = IRBroadcastEW;
        } else if (condition == 31) {
          Serial.println("IRReceiveDataW-IRBroadcastEW");
          state = IRBroadcastEW;
        }
        break;
    }
  }
}
