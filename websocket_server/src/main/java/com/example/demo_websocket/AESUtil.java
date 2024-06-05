package com.example.demo_websocket;

import org.springframework.web.socket.TextMessage;
import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import java.util.Base64;

public class AESUtil {
    private static final String ALGORITHM = "AES";
    private static final String KEY = "%HELLO_KEY%&1556";

    public static String encrypt(String data) throws Exception {
        var secretKey = new SecretKeySpec(KEY.getBytes(), ALGORITHM);
        var cipher = Cipher.getInstance(ALGORITHM);
        cipher.init(Cipher.ENCRYPT_MODE, secretKey);
        var encrypted = cipher.doFinal(data.getBytes());
        return Base64.getEncoder().encodeToString(encrypted);
    }

    public static String decrypt(String encryptedData) throws Exception {
        var secretKey = new SecretKeySpec(KEY.getBytes(), ALGORITHM);
        var cipher = Cipher.getInstance(ALGORITHM);
        cipher.init(Cipher.DECRYPT_MODE, secretKey);
        var decodedBytes = Base64.getDecoder().decode(encryptedData);
        var decrypted = cipher.doFinal(decodedBytes);
        return new String(decrypted);
    }

    public static TextMessage encryptedTextMessage(String payload) throws Exception {
        return new TextMessage("{e}" + encrypt(payload));
    }
}
