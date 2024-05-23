#include "AESUtil.h"
#include <Crypto.h>
#include <AES.h>
#include <base64.hpp>

const char *key = "%HELLO_KEY%&1556";

const unsigned char *stringToUnsignedChar(const String &str)
{
    return reinterpret_cast<const unsigned char *>(str.c_str());
}

void padBuffer(byte *buffer, size_t length, size_t blockSize)
{
    size_t padLength = blockSize - (length % blockSize);
    for (size_t i = length; i < length + padLength; i++)
    {
        buffer[i] = padLength;
    }
}

void unpadBuffer(byte *buffer, size_t &length, size_t blockSize)
{
    size_t padLength = buffer[length - 1];
    if (padLength > 0 && padLength <= blockSize)
    {
        length -= padLength;
    }
}

String encryptAES(const char *msg)
{
    byte keyBytes[16];
    memcpy(keyBytes, key, 16);

    AES128 aesEncryptor;
    aesEncryptor.setKey(keyBytes, aesEncryptor.keySize());

    size_t msgLen = strlen(msg);
    size_t paddedLen = ((msgLen / 16) + 1) * 16;
    byte plainText[paddedLen];
    memset(plainText, 0, paddedLen);
    strncpy((char *)plainText, msg, msgLen);

    padBuffer(plainText, msgLen, 16);

    byte cipherText[paddedLen];
    for (size_t i = 0; i < paddedLen; i += 16)
        aesEncryptor.encryptBlock(cipherText + i, plainText + i);

    // Base64 encoding
    unsigned char out[encode_base64_length(paddedLen)];
    encode_base64(cipherText, paddedLen, out);
    String encoded = (char *)out;
    return encoded;
}

String decryptAES(const char *msg)
{
    byte keyBytes[16];
    memcpy(keyBytes, key, 16);

    AES128 aesDecryptor;
    aesDecryptor.setKey(keyBytes, aesDecryptor.keySize());

    const unsigned char *message = (unsigned char *)msg;
    // Base64 decoding
    unsigned int decodedLen = decode_base64_length(message);
    byte decodedBytes[decodedLen];
    decode_base64(message, decodedBytes);

    byte plainText[decodedLen];
    for (size_t i = 0; i < decodedLen; i += 16)
        aesDecryptor.decryptBlock(plainText + i, decodedBytes + i);

    size_t unpaddedLen = decodedLen;
    unpadBuffer(plainText, unpaddedLen, 16);

    return String((char*)plainText).substring(0, unpaddedLen);
}