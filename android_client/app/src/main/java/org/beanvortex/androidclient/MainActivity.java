package org.beanvortex.androidclient;

import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.textfield.TextInputEditText;

import org.beanvortex.androidclient.utils.AESUtil;
import org.beanvortex.androidclient.utils.Message;
import org.beanvortex.androidclient.utils.MessageAdapter;
import org.beanvortex.androidclient.websocket.WebsocketListener;

import java.util.ArrayList;
import java.util.List;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.WebSocket;

public class MainActivity extends AppCompatActivity {
    private OkHttpClient client;
    private WebSocket webSocket;

    private TextInputEditText promptEditText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        promptEditText = findViewById(R.id.promptEditText);
        MaterialButton sendButton = findViewById(R.id.sendButton);
        MaterialButton voiceButton = findViewById(R.id.voiceButton);
        RecyclerView messagesRecyclerView = findViewById(R.id.messagesRecyclerView);

        List<Message> messageList = new ArrayList<>();
        MessageAdapter messageAdapter = new MessageAdapter(messageList);
        messagesRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        messagesRecyclerView.setAdapter(messageAdapter);

        client = new OkHttpClient();
        Request request = new Request.Builder().url("ws://192.168.1.105:8080/chat").build();
        WebsocketListener listener = new WebsocketListener(messageAdapter);
        webSocket = client.newWebSocket(request, listener);
        client.dispatcher().executorService().shutdown();

        sendButton.setOnClickListener(v -> {
            String messageText = promptEditText.getText().toString().trim();
            if (!messageText.isEmpty()) {
                try {
                    webSocket.send(AESUtil.encryptMsg(messageText));
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }
                promptEditText.setText("");
            }
        });

        voiceButton.setOnClickListener(v -> {
            // Implement voice input functionality here
        });
    }
}