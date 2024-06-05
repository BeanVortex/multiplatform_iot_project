const connectBtn = document.getElementById("connect");
const disconnectBtn = document.getElementById("disconnect");
const sendBtn = document.getElementById("send");
const conversationArea = document.getElementById("conversation");
const messagesBody = document.getElementById("messages");
const messageInput = document.getElementById("message");
const forms = document.getElementsByTagName("form");

for (let formElement of forms)
    formElement.onsubmit = e => e.preventDefault();

const setConnected = (connected) => {
    connectBtn.disabled = connected;
    disconnectBtn.disabled = !connected;
    conversationArea.style.display = connected ? "block" : "none";
}

let websocket = null;
const connect = () => {
    websocket = new WebSocket("ws://localhost:8080/chat");
    websocket.onopen = () => {
        setConnected(true);
        websocket.send("subscribe");
    };
    websocket.onclose = () => disconnect(false);
    websocket.onmessage = event => showMessage(event.data)
}

const disconnect = () => {
    setConnected(false);
    messagesBody.innerHTML = "";
    websocket.send("unsubscribe");
    console.log("Disconnected");
}

const sendMessage = () => websocket.send(encrypt(messageInput.value));

const showMessage = message => {
    const tr = document.createElement('tr');
    tr.innerHTML = `<td>${decrypt(message)}</td>`
    messagesBody.appendChild(tr);
}
connectBtn.onclick = connect;
disconnectBtn.onclick = disconnect;
sendBtn.onclick = sendMessage;