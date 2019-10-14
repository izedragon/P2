int words;

void dispWifi(){
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done
 
  display.display();
  delay(2000);
 
  // Clear the buffer.
  display.clearDisplay();
 
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("connect Wifi");
  display.display();
  delay(2000);
  display.clearDisplay();
  }

  void dispServer(){
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done
 
  display.display();
  delay(2000);
 
  // Clear the buffer.
  display.clearDisplay();
 
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("connect Server");
  display.display();
  delay(2000);
  display.clearDisplay();
  }

    void dispL(){
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done
 
  display.display();
  delay(2000);
 
  // Clear the buffer.
  display.clearDisplay();
 
  // text display tests
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("B");
  display.display();
  delay(2000);
  display.clearDisplay();
  }

    void dispH(){
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
  // init done
 
  display.display();
  delay(2000);
 
  // Clear the buffer.
  display.clearDisplay();
 
  // text display tests
  display.setTextSize(5);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("A");
  display.display();
  delay(2000);
  display.clearDisplay();
  }
