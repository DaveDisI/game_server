#include "web_socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

struct ClientData{
    s32 socket;
    s32 x;
    s32 y;
    pthread_t thread_id;
    bool varified;
};

void* handleClient(void* data){
    ClientData* cdat = (ClientData*)data;

    const u32 BUFFER_SIZE = 1024;
    s8 buffer[BUFFER_SIZE];
    s32 readAmt = read(cdat->socket, buffer, BUFFER_SIZE); 

    if(readAmt >= 0){

        char resp[] = "\
HTTP/1.1 101 Switching Protocols\r\n\
Upgrade: websocket\r\n\
Connection: Upgrade\r\n\
Sec-WebSocket-Accept: \0";
        
        int resplen = 0;
        char* c = resp;
        char responseBuffer[BUFFER_SIZE];
        int rbctr = 0;
        while(*c != '\0'){
            resplen++;
            responseBuffer[rbctr++] = *c;
            c++;
        }

        char recvbuf[BUFFER_SIZE];
        
        char keyBuffer[128];
        char base64Str[29];
        int iSendResult = 0;
        int recvbuflen = BUFFER_SIZE;
        s32 err = 1;

        bool keyExchanged = false;
        
        getWebSocketKeyFromString(buffer, keyBuffer);
        convertWebSocketKey(keyBuffer, base64Str);
        
        for(int i = 0; i < 28; i++){
            responseBuffer[rbctr++] = base64Str[i];
        }
        responseBuffer[rbctr++] = '\r';
        responseBuffer[rbctr++] = '\n';
        responseBuffer[rbctr++] = '\r';
        responseBuffer[rbctr++] = '\n';
        responseBuffer[rbctr++] = '\0';

        printf("%s\n", responseBuffer);

        s32 sendAmt = send(cdat->socket, responseBuffer, rbctr - 1, 0);
        if(sendAmt < 0){ 
            printf("Error sending data to client.");
            return 0;
        }
        keyExchanged = true;
    }

    while(readAmt >= 0){
        printf("%s\n", buffer); 
        // s32 sendAmt = send(clientSocket, "YO!", 3 , 0);
        // if(sendAmt < 0){ 
        //     printf("Error sending data to client.");
        //     return 1;
        // }
        readAmt = read(cdat->socket, buffer, BUFFER_SIZE); 
    }

    return 0;
}

s32 main(s32 argc, char** argv){
    s32 serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(!serverSocket){
        printf("Error creating serverSocket!\n");
        return 1;
    }

    s32 options = 0;
    s32 err = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | 
                         SO_REUSEPORT, &options, sizeof(int));

    if(err){
        printf("Error setting socket options.\nerrno: %i\n", errno);
        return 1;
    }

    sockaddr_in address = {}; 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = reverseByteOrder64(INADDR_ANY); 
    address.sin_port = reverseByteOrder16(8080); 

    err = bind(serverSocket, (sockaddr*)&address, sizeof(address));
    if(err < 0){
        printf("Error address to server socket.\nerrno: %i\n", errno);
        return 1;
    }

    err = listen(serverSocket, 3);
    if(err < 0) { 
        printf("Error listening for connections.\nerrno: %i\n", errno);
        return 1; 
    } 

    ClientData clients[5] = {};
    u32 totalClients = 0;
    socklen_t addressLength = 0;
    bool running = true;
    while(running){
        err = clients[totalClients].socket = accept(serverSocket, (sockaddr*)&address, &addressLength);
        if (err < 0) { 
            printf("Error accepting client socket.\naddress length: %i\nerrno: %i\n", addressLength, errno); 
            return 1; 
        }
        struct sockaddr_in* pV4Addr = (sockaddr_in*)&address;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
        printf("client connected from: %s\n", str); 
        pthread_create(&clients[totalClients].thread_id, NULL, &handleClient, (void*)&clients[totalClients]); 
        totalClients++;
    }
     
    return 0;
}