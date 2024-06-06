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
        } else if (payload.equalsIgnoreCase("unsubscribe")) {
            subscribedSessions.remove(session);
        } else if (payload.startsWith("login_pass")) {
            if (payload.substring(payload.indexOf('=') + 1).equals(LOGIN_PASS))
                broadcastMessage("login success");
            else
                broadcastMessage("login failed");
        } else if (payload.startsWith("new_question")) {
            var question = commandService.generateArithmeticQuestion();
            broadcastMessage("question=" + question);
        } else if (payload.startsWith("answer")) {
            if (commandService.questionResult == Integer.parseInt(payload.substring(payload.indexOf("=") + 1)))
                broadcastMessage("answer_result=Correct answer");
            else
                broadcastMessage("answer_result=Wrong answer");
        } else if (payload.startsWith("play_num")) {
            broadcastMessage(payload);
        } else if (payload.startsWith("Fire")) {
            broadcastMessage(payload);
        } else {
            var result = commandService.processCommand(payload);
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
