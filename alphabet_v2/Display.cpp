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

/**
 * Draws a circle indicating a neighbour has been found at the direction defined.
 * Uses the screen-size and screen-width to determine which coordinates indicate the directions East and West. 
 */
void showNeighbourCircle(Adafruit_SSD1306& display, String direction) {
  int screenWidth = 64;
  int screenHeight = 48;

  int x = 0;
  int y = 0;
  int r = 5;

  if(direction == "EAST") {
    x = (screenWidth*9)/10;
    y = (screenHeight/2);
  } else if(direction == "WEST") {
    x = (screenWidth)/10;
    y = (screenHeight/2);
  }

  display.fillCircle(x,y,r, WHITE);
  display.display();
}