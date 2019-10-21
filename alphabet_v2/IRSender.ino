

void sendIR(int hex) {
  
  irsend.sendNEC(hex, 32, 3);
  //irsend.sendRaw(north, 1, 38);
  delay(10);
}
