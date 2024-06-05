package org.beanvortex.androidclient.utils;


import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import android.util.Base64;

public class AESUtil {
    private static final String ALGORITHM = "AES";
    private static final String KEY = "%HELLO_KEY%&1556";

    public static String encrypt(String data) throws Exception {
        SecretKeySpec secretKey = new SecretKeySpec(KEY.getBytes(), ALGORITHM);
        Cipher cipher = Cipher.getInstance(ALGORITHM);
        cipher.init(Cipher.ENCRYPT_MODE, secretKey);
        byte[] encrypted = cipher.doFinal(data.getBytes());
        return Base64.encodeToString(encrypted, Base64.DEFAULT);
    }

    public static String decrypt(String encryptedData) throws Exception {
        SecretKeySpec secretKey = new SecretKeySpec(KEY.getBytes(), ALGORITHM);
        Cipher cipher = Cipher.getInstance(ALGORITHM);
        cipher.init(Cipher.DECRYPT_MODE, secretKey);
        byte[] decodedBytes = Base64.decode(encryptedData, Base64.DEFAULT);
        byte[] decrypted = cipher.doFinal(decodedBytes);
        return new String(decrypted);
    }

    public static String encryptedTextMessage(String payload) throws Exception {
        return "{e}" + encrypt(payload);
    }
}