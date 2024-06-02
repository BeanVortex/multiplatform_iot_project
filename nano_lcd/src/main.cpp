#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

SoftwareSerial espSerial(10, 11); // RX, TX

const int rs = 9, en = 8, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int dispLen = 16;

const int firePin = 3;
const unsigned long debounceDelay = 300;
volatile unsigned long lastInterruptTime = 0;

void handleFireWarning()
{
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceDelay)
  {
    espSerial.println("Fire");
    lcd.clear();
    lcd.print("Fire detected");
    lastInterruptTime = currentTime;
  }
}

void setup()
{
  Serial.begin(115200);
  espSerial.begin(9600);
  pinMode(firePin, INPUT);
  lcd.begin(16, 2);
  lcd.print("No messages here");
  attachInterrupt(digitalPinToInterrupt(firePin), handleFireWarning, FALLING);
}

void loop()
{
  if (espSerial.available() > 0)
  {
    String receivedText = espSerial.readStringUntil('\n');
    Serial.println(receivedText);
    if (receivedText.startsWith("question=") || receivedText.startsWith("message=")
     || receivedText.startsWith("answer_result=") || receivedText.startsWith("login")
     || receivedText.startsWith("Connecting") || receivedText.startsWith("Wifi")
     || receivedText.startsWith("Sub"))
    {
      String msg = receivedText.substring(receivedText.indexOf('=') + 1, receivedText.length() - 1);
      int msgLength = msg.length();
      lcd.clear();

      if (msgLength > dispLen)
      {
        for (int i = 0; i < msgLength - 15; i++)
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(msg.substring(i, i + 16));
          if (i + 16 < msgLength)
          {
            lcd.setCursor(0, 1);
            lcd.print(msg.substring(i + 16, i + 32));
          }
          else
          {
            lcd.setCursor(0, 1);
            lcd.print(msg.substring(0, 16 - (msgLength - i)));
          }
          delay(800);
        }
      }
      else
        lcd.print(msg);
    }
  }
}
