package org.beanvortex.androidclient;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import org.beanvortex.androidclient.websocket.WebsocketListener;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.WebSocket;

public class MainActivity extends AppCompatActivity {
    private OkHttpClient client;
    private WebSocket webSocket;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        client = new OkHttpClient();
        Request request = new Request.Builder().url("ws://192.168.1.105:8080/chat").build();
        WebsocketListener listener = new WebsocketListener();
        webSocket = client.newWebSocket(request, listener);

    }
}