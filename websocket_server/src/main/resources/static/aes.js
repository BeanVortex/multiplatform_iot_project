const KEY = '%HELLO_KEY%&1556';
function encrypt(data) {
    const keyUtf8 = CryptoJS.enc.Utf8.parse(KEY);
    const encrypted = CryptoJS.AES.encrypt(data, keyUtf8, {
        mode: CryptoJS.mode.ECB,
        padding: CryptoJS.pad.Pkcs7
    });
    return "{e}" + CryptoJS.enc.Base64.stringify(encrypted.ciphertext);
}

function decrypt(encryptedData) {
    encryptedData = encryptedData.substring(3);
    const keyUtf8 = CryptoJS.enc.Utf8.parse(KEY);
    const encryptedHexStr = CryptoJS.enc.Base64.parse(encryptedData);
    const decrypted = CryptoJS.AES.decrypt({
        ciphertext: encryptedHexStr
    }, keyUtf8, {
        mode: CryptoJS.mode.ECB,
        padding: CryptoJS.pad.Pkcs7
    });
    return CryptoJS.enc.Utf8.stringify(decrypted);
}
