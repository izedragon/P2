#include "stdint.h"
#include "IRremoteESP8266.h"
#include <IRrecv.h>
uint32_t recIR(IRrecv& irrecv, decode_results results, int direction);
void delayForRec(int milliseconds);
String concatHEXBroadcastRec(uint32_t hex, int messageLength);