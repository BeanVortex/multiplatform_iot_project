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
#include <WebSocketsClient.h>
#include <string>
#include <iostream>
#include <cstdint>
#include <Keypad.h>
#include "AESUtil.h"

const char *serverAddress = "192.168.1.105";
const char *serverUrl = "/chat";
const int serverPort = 8080;

const char *ssid = "TP-LINK_DE179F";
const char *password = "12345678";

WiFiMulti WiFi_multi;
WebSocketsClient webSocket;

#define USE_SERIAL Serial

void displayDigit(const int *pins);
void displayNumbers(int, int, int, int);
void enableAnswerMode();
void enableCountDownMode();
void enableLoginMode();
void modeA(char);
void modeC(char);
void modeD(char);
void disableModes();
void count_down();
void errorBlink();
void successBlink();

const int error_led = 15;
const int success_led = 16;
const int digits[] = {3, 9, 10, 6};

const int numbers[10][7] = {
    {21, 13, 47, 7, 5, 48},     // 0
    {13, 47},                   // 1
    {21, 13, 14, 7, 5},         // 2
    {21, 13, 47, 14, 7},        // 3
    {48, 13, 47, 14},           // 4
    {21, 48, 14, 47, 7},        // 5
    {48, 5, 14, 47, 7},         // 6
    {21, 13, 47},               // 7
    {21, 13, 47, 7, 5, 48, 14}, // 8
    {21, 13, 47, 48, 14}        // 9
};
//                      a   b   c   d   e  f   g
const int segments[] = {21, 13, 47, 7, 5, 48, 14}; // Array of segment pins
// 0 a b c d e f      21 13 47 7 5 48
// 1 b c              13 47
// 2 a b g d e        21 13 14 7 5
// 3 a b c g d        21 13 47 14 7
// 4 f b c g          48 13 47 14
// 5 a f g c d        21 48 14 47 7
// 6 f e g c d        48 5 14 47 7
// 7 a b c            21 13 47
// 8 a b c d e f g    21 13 47 7 5 48 14
// 9 a b c f g        21 13 47 48 14
int count_down_value = 0;

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

int clock_beat = 0;
int count = 0;

bool count_down_mode = false;
String count_down_value_str = "";

bool answer_mode = false;
String answer_value_str = "";

bool login_mode = false;
bool login_status = false;

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16)
{
  const uint8_t *src = (const uint8_t *)mem;
  USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
  for (uint32_t i = 0; i < len; i++)
  {
    if (i % cols == 0)
    {
      USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
    }
    USE_SERIAL.printf("%02X ", *src);
    src++;
  }
  USE_SERIAL.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  String message = (char *)payload;
  if (message.startsWith("{e}"))
    message = decryptAES(message.substring(3).c_str());

  switch (type)
  {
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[WSc] Disconnected!\n");
    errorBlink();
    login_status = false;
    break;
  case WStype_CONNECTED:
    USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
    webSocket.sendTXT("subscribe");
    break;
  case WStype_TEXT:
    USE_SERIAL.print("[WSc] get text: ");
    USE_SERIAL.println(message);
    if (message.equals("Successfully subscribed"))
      successBlink();

    if (login_status)
    {
      if (message.startsWith("count_down="))
      {
        USE_SERIAL.printf("got %s\n", message.substring(11, length));
        count_down_value = message.substring(11, length).toInt();
        if (count_down_value > 6000)
          count_down_value = 5999;
      }
      if (message.startsWith("question="))
      {
        answer_mode = true;
        enableAnswerMode();
      }
    }

    if (message.equals("login success"))
    {
      login_status = true;
      successBlink();
    }

    if (message.equals("Correct answer"))
      successBlink();

    if (message.equals("login failed") || message.equals("Wrong answer"))
      errorBlink();

    break;
  case WStype_BIN:
    USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
    hexdump(payload, length);
    break;
  case WStype_ERROR:
    USE_SERIAL.printf("[WSc] Error!\n");
    errorBlink();
    break;
  default:
    USE_SERIAL.printf("[WSc] Unknown event type: %u\n", type);
  }
}

void setup()
{
  pinMode(error_led, OUTPUT);
  pinMode(success_led, OUTPUT);
  for (int pin : digits)
    pinMode(pin, OUTPUT);

  for (int pin : segments)
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  displayDigit(numbers[0]);
  for (int pin : digits)
    digitalWrite(pin, HIGH);
  displayDigit(numbers[0]);

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

  webSocket.begin(serverAddress, serverPort, serverUrl);

  webSocket.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(3000);
}

void loop()
{
  webSocket.loop();

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

void disableModes()
{
  answer_mode = false;
  count_down_mode = false;
  login_mode = false;
  answer_value_str = "";
  count_down_value = 0;
  count_down_value_str = "";
}

void enableCountDownMode()
{
  if (!login_status)
  {
    errorBlink();
    return;
  }
  disableModes();
  count_down_mode = true;
  successBlink();
}

void enableAnswerMode()
{
  if (!login_status)
  {
    errorBlink();
    return;
  }
  if (!answer_mode)
  {
    String value = "new_question";
    String encrypted = "{e}" + encryptAES(value.c_str());
    webSocket.sendTXT(encrypted);
  }

  disableModes();
  answer_mode = true;
  successBlink();
}

void enableLoginMode()
{
  if (login_status)
  {
    errorBlink();
    return;
  }
  disableModes();
  login_mode = true;
  successBlink();
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
      String value = "answer=" + answer_value_str;
      String payload = "{e}" + encryptAES(value.c_str());

      webSocket.sendTXT(payload);
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
      String value = "login_pass=" + answer_value_str;
      webSocket.sendTXT("{e}" + encryptAES(value.c_str()));
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

void displayDigit(const int *pins)
{
  for (auto i = 0; i < 7; i++)
    digitalWrite(segments[i], HIGH);

  for (auto i = 0; i < 7; i++)
  {
    if (pins[i] == 0)
      break;
    digitalWrite(pins[i], LOW);
  }
}

void displayNumbers(int thousands, int hundreds, int tens, int ones)
{
  digitalWrite(digits[0], LOW);
  digitalWrite(digits[3], HIGH);
  displayDigit(numbers[ones]);
  delay(1);
  digitalWrite(digits[3], LOW);
  digitalWrite(digits[2], HIGH);
  displayDigit(numbers[tens]);
  delay(1);
  digitalWrite(digits[2], LOW);
  digitalWrite(digits[1], HIGH);
  displayDigit(numbers[hundreds]);
  delay(1);
  digitalWrite(digits[1], LOW);
  digitalWrite(digits[0], HIGH);
  displayDigit(numbers[thousands]);
  delay(1);
}

void errorBlink()
{
  for (size_t i = 0; i < 3; i++)
  {
    digitalWrite(error_led, HIGH);
    delay(200);
    digitalWrite(error_led, LOW);
    delay(200);
  }
}

void successBlink()
{
  digitalWrite(success_led, HIGH);
  delay(1000);
  digitalWrite(success_led, LOW);
}