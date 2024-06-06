package org.beanvortex.androidclient;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Intent;
import android.os.Bundle;
import android.speech.RecognizerIntent;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.button.MaterialButton;
import com.google.android.material.textfield.TextInputEditText;

import org.beanvortex.androidclient.utils.AESUtil;
import org.beanvortex.androidclient.utils.Message;
import org.beanvortex.androidclient.utils.MessageAdapter;
import org.beanvortex.androidclient.websocket.WebsocketClient;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.WebSocket;

public class MainActivity extends AppCompatActivity {

    private static final int REQUEST_CODE_SPEECH_INPUT = 1;

    private TextInputEditText promptEditText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        createNotificationChannel();
        promptEditText = findViewById(R.id.promptEditText);
        MaterialButton sendButton = findViewById(R.id.sendButton);
        MaterialButton voiceButton = findViewById(R.id.voiceButton);
        RecyclerView messagesRecyclerView = findViewById(R.id.messagesRecyclerView);

        List<Message> messageList = new ArrayList<>();
        MessageAdapter messageAdapter = new MessageAdapter(messageList);
        messagesRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        messagesRecyclerView.setAdapter(messageAdapter);

        WebsocketClient wc = new WebsocketClient(messageAdapter, this);
        wc.connect();

        sendButton.setOnClickListener(v -> {
            String messageText = promptEditText.getText().toString().trim();
            if (!messageText.isEmpty()) {
                try {
                    wc.sendMsg(messageText);
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }
                promptEditText.setText("");
            }
        });

        voiceButton.setOnClickListener(v -> startSpeechToText());
    }

    private void startSpeechToText() {
        Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale.getDefault());
        intent.putExtra(RecognizerIntent.EXTRA_PROMPT, "Hi, say something...");

        try {
            startActivityForResult(intent, REQUEST_CODE_SPEECH_INPUT);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == REQUEST_CODE_SPEECH_INPUT) {
            if (resultCode == RESULT_OK && data != null) {
                ArrayList<String> result = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);
                if (result != null && !result.isEmpty())
                    promptEditText.setText(result.get(0));
            }
        }
    }


    public static final String CHANNEL_ID = "fire_alert_channel";
    private void createNotificationChannel() {
        CharSequence name = "android client fire channel ";
        String description = "fire channel description";
        int importance = NotificationManager.IMPORTANCE_HIGH;
        NotificationChannel channel = new NotificationChannel(CHANNEL_ID, name, importance);
        channel.setDescription(description);
        NotificationManager notificationManager = getSystemService(NotificationManager.class);
        notificationManager.createNotificationChannel(channel);
    }
}