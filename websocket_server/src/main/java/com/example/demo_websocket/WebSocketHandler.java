package com.example.demo_websocket;

import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.stereotype.Component;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.util.HashSet;
import java.util.Set;

@Component
public class WebSocketHandler extends TextWebSocketHandler {

    private final Set<WebSocketSession> subscribedSessions = new HashSet<>();

    @Override
    protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
        var payload = message.getPayload();
        if (payload.equalsIgnoreCase("subscribe")) {
            if (subscribedSessions.contains(session)) {
                session.sendMessage(new TextMessage("You have already subscribed to channel"));
                return;
            }
            subscribedSessions.add(session);
            session.sendMessage(new TextMessage("Successfully subscribed"));
        } else if (payload.equalsIgnoreCase("unsubscribe")) {
            subscribedSessions.remove(session);
        } else {
            broadcastMessage(payload);
        }
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
