#include "IRSender.h"
#include "IRRec.h"
#include <Arduino.h>

void sendIR(int hex, IRsend &irsend)
{
  irsend.sendNEC(hex);
  delay(10);
}

String intToStr(int received)
{
  String returnStr = "";

  if (received == 0x01)
  {
    returnStr = "WEST";
  }
  else if (received == 0x02)
  {
    returnStr = "SOUTH";
  }
  else if (received == 0x03)
  {
    returnStr = "EAST";
  }
  else if (received == 0x04)
  {
    returnStr = "NORTH";
  }
  else
  {
    returnStr = "0";
  }

  return returnStr;
}
