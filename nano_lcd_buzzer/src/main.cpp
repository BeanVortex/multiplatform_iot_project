#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 11); // RX, TX

int buzz = 12;

void setup()
{
  Serial.begin(115200);
  espSerial.begin(9600);
  pinMode(buzz, OUTPUT);
  tone(buzz, 440);
  delay(200);
  tone(buzz, 480);
  delay(200);
  tone(buzz, 520);
  delay(200);
  tone(buzz, 560);
  delay(200);
  tone(buzz, 600);
  delay(200);
  tone(buzz, 640);
  delay(200);
  noTone(buzz);
}

void loop()
{
    espSerial.println("Hello from nano");
    delay(1000);
}
