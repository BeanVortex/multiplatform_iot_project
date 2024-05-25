// gnd          gnd
// pow in       pow out(3.3)
// adc          1   tx  u
// 16   u       3   rx  u
// 14   u       5   u
// 12   u       4   u
// 13   u       2   u   (led_builtin_active_low)
// 11   nu      8   nu
// 6    nu      9   nu
// 7    nu      10  u

#include <Arduino.h>

#define RXD1 8
#define TXD1 2

void setup()
{
  Serial.begin(9600);
  Serial1.begin(115200);
}

void loop()
{
  if (Serial.available() > 0) {
    // Read the incoming string until newline and print it
    String receivedText = Serial.readStringUntil('\n');
    Serial1.print("Received: ");
    Serial1.println(receivedText);
    delay(1000);
  }
}
