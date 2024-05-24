// gnd          gnd
// pow          13      /ok
// 3   /ok      14      /ok
// 9   /ok      21      /ok
// 10  /ok      47      /ok
// 11  /ok      48      /ok
// 12  /ok      35      /ok
// 4   /ok      36      /ok
// 5   /ok      37      /ok
// 6   /ok      38      /ok
// 7   /ok      39      /ok
// 15  /ok      40      /ok
// 16  /ok      41      /ok
// 17  /ok      42      /ok
// 18  /ok      2       /not ok / wire
// 8   /ok      1       /not ok / wire
// 19  /ok      43 tx   /ok
// 20  /ok      44 rx   /ok

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <string>
#include <iostream>
#include <cstdint>
#include <Keypad.h>
#include "./utils/DisplayUtil.h"
#include "./utils/WebsocketUtil.h"
#include "./utils/BlinkUtil.h"

const char *ssid = "TP-LINK_DE179F";
const char *password = "12345678";

WiFiMulti WiFi_multi;

#define USE_SERIAL Serial

extern int count_down_value;
extern bool count_down_mode;
extern String count_down_value_str;
extern bool answer_mode;
extern String answer_value_str;
extern bool login_mode;
extern bool login_status;

void modeA(char);
void modeC(char);
void modeD(char);
void disableModes();
void count_down();

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte rowPins[ROWS] = {35, 36, 37, 38};
byte colPins[COLS] = {39, 40, 41, 42};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup()
{
  initLED();
  initDisplayPins();
  disableModes();

  USE_SERIAL.begin(115200);
  USE_SERIAL.println("\n\n");
  USE_SERIAL.println("connecting to wifi");

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(200);
  }

  WiFi_multi.addAP(ssid, password);

  while (WiFi_multi.run() != WL_CONNECTED)
    delay(100);
  USE_SERIAL.println("wifi connected");

  initWebsocket();
}

void loop()
{
  websocketLoop();
  char key = customKeypad.getKey();

  if (key == 'B')
    disableModes();

  if (key == 'A')
    enableCountDownMode();

  if (key == 'C')
    enableAnswerMode();

  if (key == 'D')
    enableLoginMode();

  modeA(key);

  modeC(key);

  modeD(key);

  count_down();
}

void modeA(char key)
{
  if (count_down_mode)
  {
    if (key == '#')
    {
      if (count_down_value_str.isEmpty())
        count_down_value = 0;
      else
      {
        int number = count_down_value_str.toInt();
        if (number >= 9960)
          number = 9959;
        int minutes = number / 100;
        int seconds = number % 100;
        count_down_value = minutes * 60 + seconds;
        count_down_mode = false;
      }
    }
    // ascii for numbers
    else if (key >= 48 && key <= 57 && count_down_value_str.length() < 4)
      count_down_value_str += key;
    int number = count_down_value_str.toInt();
    int thousands = number / 1000;
    int hundreds = (number % 1000) / 100;
    int tens = (number % 100) / 10;
    int ones = number % 10;
    displayNumbers(thousands, hundreds, tens, ones);
  }
}

void modeC(char key)
{
  if (answer_mode)
  {
    if (key == '#')
    {
      websocketSendTXT("answer=" + answer_value_str);
      answer_value_str = "";
      answer_mode = false;
    }
    // ascii for numbers
    else if (key >= 48 && key <= 57 && count_down_value_str.length() < 4)
      answer_value_str += key;
    if (answer_value_str.length() <= 4)
    {
      int number = answer_value_str.toInt();
      int thousands = number / 1000;
      int hundreds = (number % 1000) / 100;
      int tens = (number % 100) / 10;
      int ones = number % 10;
      displayNumbers(thousands, hundreds, tens, ones);
    }
  }
}

void modeD(char key)
{
  if (login_mode)
  {
    if (key == '#')
    {
      websocketSendTXT("login_pass=" + answer_value_str);
      answer_value_str = "";
      login_mode = false;
    }
    // ascii for numbers
    else if (key >= 48 && key <= 57 && answer_value_str.length() < 4)
      answer_value_str += key;
    if (answer_value_str.length() <= 4)
    {
      int number = answer_value_str.toInt();
      int thousands = number / 1000;
      int hundreds = (number % 1000) / 100;
      int tens = (number % 100) / 10;
      int ones = number % 10;
      displayNumbers(thousands, hundreds, tens, ones);
    }
  }
}

void count_down()
{
  if (count_down_value != 0)
  {
    static unsigned long previousMillis = 0;
    const unsigned long interval = 1000;
    unsigned long currentMillis = millis();

    int minutes = count_down_value / 60;
    int seconds = count_down_value % 60;
    int minuteTens = minutes / 10;
    int minuteOnes = minutes % 10;
    // Separate the second digits
    int secondTens = seconds / 10;
    int secondOnes = seconds % 10;

    displayNumbers(minuteTens, minuteOnes, secondTens, secondOnes);

    if (currentMillis - previousMillis >= interval)
    {
      count_down_value--;
      previousMillis = currentMillis;
    }
  }
  else if (!count_down_mode && !answer_mode && !login_mode)
    displayNumbers(0, 0, 0, 0);
}
