
void dispText(String disptext) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(disptext);
  display.display();
}

void dispLetter(String letter) {
  display.clearDisplay();
  display.setTextSize(6);
  display.setTextColor(WHITE);
  display.setCursor(18, 3);
  display.println(letter);
  display.display();
}

void invertDisplay(boolean b) {
  display.invertDisplay(b);
  display.display();
}
