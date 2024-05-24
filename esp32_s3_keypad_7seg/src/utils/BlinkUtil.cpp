#include "BlinkUtil.h"

const int error_led = 15;
const int success_led = 16;

void initLED()
{
    pinMode(error_led, OUTPUT);
    pinMode(success_led, OUTPUT);
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