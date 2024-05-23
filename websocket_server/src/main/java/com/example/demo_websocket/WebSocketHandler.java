package com.example.demo_websocket;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;
import java.util.Base64;
import java.util.HashSet;
import java.util.Random;
import java.util.Set;

@Component
public class WebSocketHandler extends TextWebSocketHandler {

    private final Set<WebSocketSession> subscribedSessions = new HashSet<>();
    private static final String ALGORITHM = "AES";
    private static final String KEY = "%HELLO_KEY%&1556";
    private static final String LOGIN_PASS = "1234";
    private int questionResult;

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

    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
        var payload = message.getPayload();
        if (payload.startsWith("{e}"))
            payload = decrypt(payload.substring(3));

        if (payload.equalsIgnoreCase("subscribe")) {
            if (subscribedSessions.contains(session)) {
                session.sendMessage(new TextMessage("You have already subscribed to channel"));
                return;
            }
            subscribedSessions.add(session);
            session.sendMessage(new TextMessage("Successfully subscribed"));
        } else if (payload.equalsIgnoreCase("unsubscribe")) {
            subscribedSessions.remove(session);
        } else if (payload.startsWith("login_pass")) {
            if (payload.substring(payload.indexOf('=') + 1).equals(LOGIN_PASS))
                broadcastMessage("login success");
            else
                broadcastMessage("login failed");
        } else if (payload.startsWith("new_question")) {
            var question = generateArithmeticQuestion();
            broadcastMessage("{e}" + encrypt("question=" + question));
        } else if (payload.startsWith("answer")) {
            if (questionResult == Integer.parseInt(payload.substring(payload.indexOf("=") + 1)))
                broadcastMessage("Correct answer");
            else
                broadcastMessage("Wrong answer");
        } else {
            broadcastMessage("{e}" + encrypt(payload));
        }
    }


    private String generateArithmeticQuestion() {
        var random = new Random();
        var num1 = random.nextInt(100) + 1;
        var num2 = random.nextInt(100) + 1;
        var operator = getRandomOperator(random);
        switch (operator) {
            case '+' -> questionResult = num1 + num2;
            case '-' -> questionResult = num1 > num2 ? num1 - num2 : num2 - num1;
            case '*' -> questionResult = num1 * num2;
            case '/' -> questionResult = num1 > num2 ? num1 / num2 : num2 / num1;
        }
        return num1 + " " + operator + " " + num2;
    }

    private char getRandomOperator(Random random) {
        var operators = new char[]{'+', '-', '*', '/'};
        var index = random.nextInt(operators.length);
        return operators[index];
    }

    private void broadcastMessage(String message) throws Exception {
        for (var subscriber : subscribedSessions)
            if (subscriber.isOpen())
                subscriber.sendMessage(new TextMessage(message));
    }

    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) {
        subscribedSessions.remove(session);
    }
}
