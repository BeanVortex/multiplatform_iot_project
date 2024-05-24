#ifndef DISPLAYUTIL_H
#define DISPLAYUTIL_H

#include <Arduino.h>

void initDisplayPins();
void displayNumbers(int thousands, int hundreds, int tens, int ones);
void displayDigit(const int *pins);

#endif
