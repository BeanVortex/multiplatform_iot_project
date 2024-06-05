package org.beanvortex.androidclient.websocket;

import org.beanvortex.androidclient.utils.AESUtil;

import okhttp3.Response;
import okhttp3.WebSocket;
import okhttp3.WebSocketListener;

public class WebsocketListener extends WebSocketListener {
    @Override
    public void onOpen(WebSocket webSocket, Response response) {
        super.onOpen(webSocket, response);
        System.out.println("WebSocket Connected");
        webSocket.send("subscribe");
    }

    @Override
    public void onMessage(WebSocket webSocket, String text) {
        super.onMessage(webSocket, text);
        try {
            String msg = AESUtil.decrypt(text.substring(3));
            System.out.println("Received message: " + msg);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public void onClosing(WebSocket webSocket, int code, String reason) {
        super.onClosing(webSocket, code, reason);
        System.out.println("WebSocket Closing: " + code + " / " + reason);
    }

    @Override
    public void onClosed(WebSocket webSocket, int code, String reason) {
        super.onClosed(webSocket, code, reason);
        System.out.println("WebSocket Closed: " + code + " / " + reason);
    }

    @Override
    public void onFailure(WebSocket webSocket, Throwable t, Response response) {
        super.onFailure(webSocket, t, response);
        System.out.println("WebSocket Failure: " + t.getMessage());
    }
}