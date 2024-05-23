#include "AESUtil.h"
#include <Crypto.h>
#include <AES.h>
#include <base64.hpp>

const char *key = "%HELLO_KEY%&1556";

String encryptAES(const char *msg)
{
    byte keyBytes[16];
    memcpy(keyBytes, key, 16);

    AES128 aesEncryptor;
    aesEncryptor.setKey(keyBytes, aesEncryptor.keySize());

    byte plainText[16];
    memset(plainText, 0, 16);
    strncpy((char *)plainText, msg, 16);

    byte cipherText[16];
    aesEncryptor.encryptBlock(cipherText, plainText);

    // Base64 encoding
    unsigned char *out;
    encode_base64(cipherText, 16, out);
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

    byte plainText[16];
    aesDecryptor.decryptBlock(plainText, decodedBytes);

    return String((char *)plainText);
}