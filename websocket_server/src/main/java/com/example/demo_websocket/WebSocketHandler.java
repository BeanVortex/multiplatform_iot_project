package com.example.demo_websocket;

import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.util.HashSet;
import java.util.Set;

import static com.example.demo_websocket.AESUtil.decrypt;
import static com.example.demo_websocket.AESUtil.encryptedTextMessage;

@Component
public class WebSocketHandler extends TextWebSocketHandler {

    private final Set<WebSocketSession> subscribedSessions = new HashSet<>();
    private static final String LOGIN_PASS = "1234";
    private final CommandService commandService;

    public WebSocketHandler(CommandService commandService) {
        this.commandService = commandService;
    }

    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
        var payload = message.getPayload();
        if (payload.startsWith("{e}"))
            payload = decrypt(payload.substring(3));


        if (payload.equalsIgnoreCase("subscribe")) {
            if (subscribedSessions.contains(session)) {
                session.sendMessage(encryptedTextMessage("You have already subscribed to channel"));
                return;
            }
            subscribedSessions.add(session);
            session.sendMessage(encryptedTextMessage("Successfully subscribed"));
            session.sendMessage(encryptedTextMessage("play_num=1"));
        } else if (payload.equalsIgnoreCase("unsubscribe")) {
            subscribedSessions.remove(session);
        } else if (payload.startsWith("login_pass")) {
            if (payload.substring(payload.indexOf('=') + 1).equals(LOGIN_PASS)) {
                broadcastMessage("login success");
                broadcastMessage("play_num=2");
            } else {
                broadcastMessage("login failed");
                broadcastMessage("play_num=3");
            }
        } else if (payload.startsWith("count_status=")) {
            if (payload.contains("started"))
                broadcastMessage("play_num=4");
            else
                broadcastMessage("play_num=5");
        } else if (payload.startsWith("new_question")) {
            var question = commandService.generateArithmeticQuestion();
            broadcastMessage("question=" + question);
            broadcastMessage("play_num=6");
        } else if (payload.startsWith("answer")) {
            if (commandService.questionResult == Integer.parseInt(payload.substring(payload.indexOf("=") + 1))) {
                broadcastMessage("answer_result=Correct answer");
                broadcastMessage("play_num=7");
            } else {
                broadcastMessage("answer_result=Wrong answer");
                broadcastMessage("play_num=8");
            }
        } else if (payload.startsWith("play_num")) {
            broadcastMessage(payload);
        } else if (payload.startsWith("Fire")) {
            broadcastMessage("play_num=9");
        } else {
            var result = commandService.processCommand(payload);
            if (result.contains("question"))
                broadcastMessage("play_num=6");
            broadcastMessage(result);
        }
    }


    private void broadcastMessage(String message) throws Exception {
        for (var subscriber : subscribedSessions)
            if (subscriber.isOpen())
                subscriber.sendMessage(encryptedTextMessage(message));
    }

    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) {
        subscribedSessions.remove(session);
    }
}
