#include "TinySHA1.h"
#include "utilities.h"

#include <stdio.h>

char base64Chars[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

char convertHexCharToByte(char hex){
    if(hex > '9'){
        return hex - 'a' + 10;
    }
    return hex - '0';
}

void convertHexStringToByteArray(char* str, unsigned char* buffer){
    int bctr = 0;
    for(int i = 0; i < 40; i += 2){
        char b1 = convertHexCharToByte(str[i]);
        char b2 = convertHexCharToByte(str[i + 1]);
        buffer[bctr++] = b1 * 16 + b2;
    }
}

void convertByteArrayToBase64String(unsigned char* byteArray, char* b64Str){
    int bctr = 0;
    for(int i = 0; i < 18; i += 3){
        char b1 = byteArray[i] >> 2;
        char b2 = ((byteArray[i] & 3) << 4) | (byteArray[i + 1] >> 4);
        char b3 = ((byteArray[i + 1] & 15) << 2) | (byteArray[i + 2] >> 6);
        char b4 = byteArray[i + 2] & 63;
        b64Str[bctr++] = base64Chars[b1];
        b64Str[bctr++] = base64Chars[b2];
        b64Str[bctr++] = base64Chars[b3];
        b64Str[bctr++] = base64Chars[b4];
    }
    char b1 = byteArray[18] >> 2;
    char b2 = ((byteArray[18] & 3) << 4) | (byteArray[19] >> 4);
    char b3 = (byteArray[19] & 15) << 2;
    b64Str[bctr++] = base64Chars[b1];
    b64Str[bctr++] = base64Chars[b2];
    b64Str[bctr++] = base64Chars[b3];
    b64Str[bctr++] = '=';
    b64Str[bctr] = '\0';
}

void convertWebSocketKey(char* webSocketKey, char* webSocketAccept){
    char magicString[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char wskWithMagicString[128];
    int cCtr = 0;
    char* c = webSocketKey;
    while(*c != '\0'){
        wskWithMagicString[cCtr++] = *c;
        c++;
    }
    c = magicString;
    while(*c != '\0'){
        wskWithMagicString[cCtr++] = *c;
        c++;
    }
    wskWithMagicString[cCtr] = '\0';

    sha1::SHA1 s;
    s.processBytes(wskWithMagicString, cCtr);
    uint32_t digest[5];
    s.getDigest(digest);	
    char sha1Encoding[41];
    snprintf(sha1Encoding, 41, "%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
    printf("Calculated : (\"%s\") = %s\n", wskWithMagicString, sha1Encoding);
    unsigned char byteArray[20];
    convertHexStringToByteArray(sha1Encoding, byteArray);

    convertByteArrayToBase64String(byteArray, webSocketAccept);
    printf("b64: %s\n", webSocketAccept);
}

void getWebSocketKeyFromString(char* str, char* wskBuffer){
    int kbctr = 0;
    char* c = str;
    while(*c != '\0'){
        if(*c != 'S'){
            while(*c != '\n') c++;
        }else{
            c += 17;
            if(*c == ':'){
                c += 2;
                while(*c != '\r'){
                    wskBuffer[kbctr++] = *c;
                    c++;
                }
                c += 2;
            }
        }
        c++;
    }
    wskBuffer[kbctr] = '\0';
}