/* IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend.
 *
 * Version 1.1 January, 2019
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to the ESP8266 on a pin
 * as specified by kIrLed below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <Wire.h>

#include <IRrecv.h>
#include <IRutils.h>

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

int slaveAddress = 0x20;

const uint16_t kIrLed = 14;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

// An IR detector/demodulator is connected to GPIO pin 14(D5 on a NodeMCU
// board).
const uint16_t kRecvPin = 13;

IRrecv irrecv(kRecvPin);

decode_results results;



void setup() {
  Wire.begin(SDLU, SCLU);
  irsend.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266

irrecv.enableIRIn();  // Start the receiver

Wire.beginTransmission(slaveAddress);
Wire.write(0x00);
Wire.write(0x00);
Wire.endTransmission();

}

void loop() {
  
  int hex = 0;

  
// Send through all LEDs.
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(0x08);
  Wire.endTransmission();
  delay(200);
  hex = 0x01;
  sendAndRecIR(hex);
  
  /*
  // Enable first.

  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(0x10);
  Wire.endTransmission();
  hex = 0x01;
  sendAndRecIR(hex);
  
  /*
  // Enable first.
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(0x10);
  Wire.endTransmission();
  hex = 0x01;
  sendAndRecIR(hex);

/*
  // Enable second.
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(0x20);

  
  // Enable second.
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(0x22);

  
  // Enable second.
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);
  Wire.write(0x22);
  Wire.endTransmission();
  hex = 0x02;
  sendAndRecIR(hex);


  // Enable third.
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);

  Wire.write(0x40);

  Wire.write(0x44);

  Wire.write(0x44);
  Wire.endTransmission();
  hex = 0x03;
  sendAndRecIR(hex);


  // Enable fourth.
  Wire.beginTransmission(slaveAddress);
  Wire.write(0x09);

  Wire.write(0x80);
  Wire.endTransmission();
  hex = 0x04;
  sendAndRecIR(hex);
*/
  
}


void sendAndRecIR(int hex) {
  //Serial.println("NEC");
  
  //irsend.sendNEC(hex);
  //delay(500);
  
  if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    results.value = 0;
    irrecv.resume();  // Receive the next value
  }
  
  delay(100);
}
