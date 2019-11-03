#include "stdint.h"
#include <IRremoteESP8266.h>
#include <irSend.h>
#include <IRrecv.h>
#include <IRRec.h>
#include <IRutils.h>
void sendIR(int hex, IRsend &irsend);
void delayForSend(int milliseconds);

