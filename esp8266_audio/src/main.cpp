// gnd          gnd
// pow in       pow out(3.3)
// adc          tx
// 16           rx
// 14           5
// 12           4
// 13           2
// 11           13
// 7            14
// 9            10

#include <Arduino.h>

#define LED_BUILTIN 2

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000); // Wait for a second
  // Turn the LED off by making the voltage HIGH
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000); // Wait for a second
}
