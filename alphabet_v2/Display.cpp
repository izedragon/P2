#include "Display.h"

void dispText(String disptext, Adafruit_SSD1306& display) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(disptext);
  display.display();
}

void dispLetter(String letter, Adafruit_SSD1306& display) {
  display.clearDisplay();
  display.setTextSize(6);
  display.setTextColor(WHITE);
  display.setCursor(18, 3);
  if (letter == "Å") {
   display.write(143);
 } else if (letter == "Ä") {
   display.write(142);
 } else if (letter == "Ö") {
   display.write(153);
 } else {
   display.println(letter);
 }
  display.display();
}

void invertDisplay(boolean b, Adafruit_SSD1306& display) {
  display.invertDisplay(b);
  display.display();
}
