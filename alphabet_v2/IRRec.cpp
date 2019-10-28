#include "IRRec.h"
#include <IRutils.h>

String directionsStrEW1[2] = {"WEST", "EAST"};

uint32_t recIR(IRrecv& irrecv, decode_results results, int direction) {
 
  uint32_t received = 0;
  //Receive and decode the message from the IR receiver and return the value (received). 
  if (irrecv.decode(&results)) {
    Serial.print("DirRec: ");
    Serial.print(directionsStrEW1[direction]);
    Serial.print(" WhatRec: ");
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    received = results.value;
    results.value = 0;
    irrecv.resume();  // Receive the next value
    delay(10);
  }
  return received;
}
