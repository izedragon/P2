#include "IRRec.h"
#include <IRutils.h>

String directionsStrEW1[2] = {"WEST", "EAST"};
int broadcastMessageLength = 3;


uint32_t recIR(IRrecv& irrecv, decode_results results, int direction) {
 
  uint32_t received = 0;
  //Receive and decode the message from the IR receiver and return the value (received). 
  if (irrecv.decode(&results)) { // Checks whether the interrupt routine has received a message. 
    Serial.print("DirRec: ");
    Serial.print(directionsStrEW1[direction]);
    Serial.print(" WhatRec: ");
    //concatHEXBroadcastRec(results.value, broadcastMessageLength);
    //Serial.print(concatHEXBroadcastRec(results.value));
    //serialPrintUint64(results.value, HEX);
    Serial.println("");
    received = results.value;
    results.value = 0;
    irrecv.resume();  // Empties the buffer in preparation for the next value received.
  }
  delay(10);
  return received;
}

/**
 * Decodes the received message, from a hex value to 4 separate characters(bytes), assuming it's 32 bits in size (as it is a NEC-protocoll message that is received).
 * Also prints the received message.
 * Written by: Daniel Abella with the help of Szilveszter Deszi.
 */ 
String concatHEXBroadcastRec(uint32_t hex, int messageLength) {
  String message = "";
  
  byte messageArray[4] = {0,0,0,0};
  
  messageArray[0] = (hex & 0x000000FF);
  messageArray[1] = (hex & 0x0000FF00) >> 8;
  messageArray[2] = (hex & 0x00FF0000) >> 16;
  messageArray[3] = (hex & 0xFF000000) >> 24;
  
  for(int i = 0; i < messageLength; i++) {
    Serial.print((char)messageArray[i]);
    message += messageArray[i];
  }

  return message;
}

