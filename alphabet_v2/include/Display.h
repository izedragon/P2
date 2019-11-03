#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include <Arduino.h>
void dispText(String disptext, Adafruit_SSD1306& display);
void dispLetter(String letter, Adafruit_SSD1306& display);
void invertDisplay(boolean b, Adafruit_SSD1306& display);
void showNeighbourCircle(Adafruit_SSD1306& display, String direction);