const KEY = '%HELLO_KEY%&1556';

async function encrypt(data) {
    const algorithm = 'AES-GCM';
    const key = await crypto.subtle.importKey(
        'raw',
        new TextEncoder().encode(KEY),
        { name: algorithm },
        false,
        ['encrypt']
    );

    const iv = crypto.getRandomValues(new Uint8Array(12));

    const encryptedData = await crypto.subtle.encrypt(
        { name: algorithm, iv },
        key,
        new TextEncoder().encode(data)
    );

    return `${iv}${new Uint8Array(encryptedData)}`;
}

async function decrypt(encryptedData) {
    const ALGORITHM = 'AES';
    const decodedBytes = Uint8Array.from(atob(encryptedData), c => c.charCodeAt(0));
    const key = await crypto.subtle.importKey(
        'raw',
        new TextEncoder().encode(KEY),
        { name: ALGORITHM },
        false,
        ['decrypt']
    );
    const cipher = await crypto.subtle.createCipher(
        ALGORITHM,
        key
    );
    const decryptedData = await crypto.subtle.decrypt(
        ALGORITHM,
        key,
        decodedBytes
    );
    return new TextDecoder().decode(decryptedData);
}

async function encryptMsg(msg) {
    try {
        const encryptedData = await encrypt(msg);
        return `{e}${btoa(String.fromCharCode(...new Uint8Array(encryptedData)))}`;
    } catch (error) {
        throw new Error(error);
    }
}

async function decryptMsg(msg) {
    try {
        const encodedData = msg.substring(3);
        const decodedData = customAtob(encodedData);
        const encryptedData = new Uint8Array(decodedData.length);
        for (let i = 0; i < decodedData.length; ++i)
            encryptedData[i] = decodedData.charCodeAt(i);
        return await decrypt(encryptedData);
    } catch (error) {
        throw new Error(error);
    }
}
function customAtob(encodedData) {
    const normalizedData = normalizeBase64(encodedData);

    try {
        return atob(normalizedData);
    } catch (error) {
        throw new Error('Invalid Base64 encoding');
    }
}

function normalizeBase64(encodedData) {
    const paddingLength = encodedData.length % 4;
    if (paddingLength > 0) {
        encodedData += '='.repeat(4 - paddingLength);
    }
    return encodedData.replace(/[^A-Za-z0-9+/]/g, '');
}