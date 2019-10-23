String intToStr(int received) {
  String returnStr = "";

  if (received == 0x00) {
    returnStr = "WEST";
  } else if (received == 0x01) {
    returnStr = "SOUTH";
  } else if (received == 0x02) {
    returnStr = "EAST";
  } else if (received == 0x03) {
    returnStr = "NORTH";
  } else {
    returnStr = "0";
  }

  return returnStr;
}

String splitString(String data, char separator, int partIndex)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= partIndex; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > partIndex ? data.substring(strIndex[0], strIndex[1]) : "";
}

boolean validLetter(String letter) {
  const char* letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ";
  for (int i = 0; i < strlen(letters); i++) {
    if (letters[i] == letter[0]) {
      return true;
    }
  }
  return false;
}
