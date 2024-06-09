#include "DisplayUtil.h"

const int digits[] = {3, 9, 10, 6};

const int numbers[10][7] = {
    {21, 48, 14, 7, 5, 13},     // 0
    {48, 14},                   // 1
    {21, 48, 47, 7, 5},         // 2
    {21, 48, 14, 47, 7},        // 3
    {13, 48, 14, 47},           // 4
    {21, 13, 47, 14, 7},        // 5
    {13, 5, 47, 14, 7},         // 6
    {21, 48, 14},               // 7
    {21, 48, 14, 7, 5, 13, 47}, // 8
    {21, 48, 14, 13, 47}        // 9
};
//                      a   b   c   d   e  f   g
const int segments[] = {21, 48, 14, 7, 5, 13, 47}; // Array of segment pins
// 0 a b c d e f      21 48 14 7 5 13
// 1 b c              48 14
// 2 a b g d e        21 48 47 7 5
// 3 a b c g d        21 48 14 47 7
// 4 f b c g          13 48 14 47
// 5 a f g c d        21 13 47 14 7
// 6 f e g c d        13 5 47 14 7
// 7 a b c            21 48 14
// 8 a b c d e f g    21 48 14 7 5 13 47
// 9 a b c f g        21 48 14 13 47

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
