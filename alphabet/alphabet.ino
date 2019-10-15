enum Connect_enum {ConnectWiFi, ConnectServer, SendServer, RegisterServer, LetterServer, CheckServer,
ReciveServer, };

enum Sensors_enum {IRBroadcastEW, IRListenEW, IRTransmitDataE, IRReceiveDataE, IRReciveDataWE,
IRTransmitDataW, IRReceiveDataW};
 
void statemachine(uint16_t condition);

//uint8_t state = STOP;
 
void setup(){
}
 
void loop(){
  statemachine();
  delay(10);
}
 
void statemachine(uint16_t condition) 
{
  switch(state)
  {
    case ConnectWiFi:
        state = ConnectWiFi;
        state = ConnectServer;
      }
      
    case ConnectServer:
        state = ConnectServer;
        state = SendServer;
      }
      
    case SendServe:
        state = RegisterServer;
        state = LetterServer;
        state = CheckServer;

    case RegisterServer:
        state = ReciveServer;


    case LetterServer:
        state = ReciveServer;
        
    case CheckServer:
        state = ReciveServer;

    case ReciveServer:
        state = SendServe;
        state = IRBroadcastEW;

    case IRBroadcastEW:
        state = IRBroadcastEW;
        state = IRListenEW;

    case IRListenEW:
        state = IRListenEW;
        state = IRBroadcastEW;
        state = IRTransmitDataE;
        state = IRTransmitDataW;
        state = IRReciveDataWE;

    case IRTransmitDataE:
        state = ReciveServer;
        state = IRBroadcastEW;
        state = IRReceiveDataE;

    case IRReceiveDataE:
        state = IRReceiveDataE;
        state = SendServe;

    case IRReciveDataWE:
        state = IRReciveDataWE;
        state = IRTransmitDataE;
        state = IRTransmitDataW;
        state = IRBroadcastEW;
        
    case IRTransmitDataW:
        state = IRReceiveDataW;
        state = IRBroadcastEW;

    case IRReceiveDataW:
        state = IRReceiveDataW;
        state = ReciveServer;
        state = IRBroadcastEW;
        state = IRBroadcastEW;             
}
