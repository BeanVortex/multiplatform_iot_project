#include "DisplayUtil.h"

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

void initDisplayPins()
{
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
