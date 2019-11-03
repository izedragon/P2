#include "IRSender.h"
#include "IRRec.h"
#include <Arduino.h>

void sendIR(uint32_t hex, IRsend &irsend)
{
  irsend.sendNEC(hex);
  delay(10);
}
