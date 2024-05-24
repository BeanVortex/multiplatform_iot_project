#ifndef AESUTIL_H
#define AESUTIL_H

#include <Arduino.h>

String encryptAES(const char* msg);
String decryptAES(const char* msg);

#endif
