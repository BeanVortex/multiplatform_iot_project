package org.beanvortex.androidclient.websocket;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.Looper;

import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import org.beanvortex.androidclient.MainActivity;
import org.beanvortex.androidclient.R;
import org.beanvortex.androidclient.utils.AESUtil;
import org.beanvortex.androidclient.utils.Message;
import org.beanvortex.androidclient.utils.MessageAdapter;

import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import okhttp3.WebSocket;
import okhttp3.WebSocketListener;
import okio.ByteString;

public class WebsocketClient {

    private static final String WEBSOCKET_URL = "ws://192.168.1.105:8080/chat";
    private final OkHttpClient client;
    private WebSocket webSocket;
    private final long RECONNECT_INTERVAL = 1000;
    private final MessageAdapter messageAdapter;
    private Handler mainHandler;
    private final Context context;

    public WebsocketClient(MessageAdapter messageAdapter, Context context) {
        mainHandler = new Handler(Looper.getMainLooper());
        this.messageAdapter = messageAdapter;
        this.context = context;
        client = new OkHttpClient();
    }

    public void connect() {
        connectInternal();
    }

    private void connectInternal() {
        Request request = new Request.Builder().url(WEBSOCKET_URL).build();
        webSocket = client.newWebSocket(request, new MyWebSocketListener());
    }

    public void disconnect() {
        if (webSocket != null) {
            webSocket.close(1000, "Normal Closure");
            webSocket = null;
        }
    }

    public void sendMsg(String msg) {
        webSocket.send(AESUtil.encryptMsg(msg));
    }

    private class MyWebSocketListener extends WebSocketListener {

        @Override
        public void onOpen(WebSocket webSocket, Response response) {
            WebsocketClient.this.webSocket = webSocket;
            System.out.println("WebSocket connection opened!");
            webSocket.send(AESUtil.encryptMsg("subscribe"));
        }

        @Override
        public void onClosed(WebSocket webSocket, int code, String reason) {
            WebsocketClient.this.webSocket = null;
            System.out.println("WebSocket connection closed: " + reason);
            if (code != 1000) {
                try {
                    Thread.sleep(RECONNECT_INTERVAL);
                    connectInternal();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        }

        @Override
        public void onMessage(WebSocket webSocket, String text) {
            System.out.println("Received message: " + text);
            try {
                mainHandler.post(() -> {
                    String msg = AESUtil.decryptMsg(text);
                    messageAdapter.addMessage(new Message(msg));
                    if (msg.startsWith("Fire"))
                        showFireAlertNotification();
                });
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }

        @Override
        public void onMessage(WebSocket webSocket, ByteString bytes) {
            System.out.println("Received binary message: " + bytes.hex());
        }

        @Override
        public void onFailure(WebSocket webSocket, Throwable t, Response response) {
            System.out.println("WebSocket connection failed: " + t.getMessage());
            try {
                Thread.sleep(RECONNECT_INTERVAL);
                connectInternal();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        private static final int FIRE_ALERT_NOTIFICATION_ID = 1;

        private void showFireAlertNotification() {
            NotificationCompat.Builder builder = new NotificationCompat.Builder(context, MainActivity.CHANNEL_ID) // Replace with your channel ID
                    .setSmallIcon(R.drawable.ic_firealert)
                    .setContentTitle("Fire Emergency")
                    .setContentText("Evacuate the building immediately!")
                    .setStyle(new NotificationCompat.BigTextStyle()
                            .bigText("Fire alarm activated. Please evacuate the building immediately and follow evacuation routes to the nearest exit. Call emergency services if needed."))
                    .setPriority(NotificationCompat.PRIORITY_HIGH)
                    .setAutoCancel(true);

            NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);
            if (ActivityCompat.checkSelfPermission(context, android.Manifest.permission.POST_NOTIFICATIONS) != PackageManager.PERMISSION_GRANTED)
                return;
            notificationManager.notify(FIRE_ALERT_NOTIFICATION_ID, builder.build());
        }

    }
}
