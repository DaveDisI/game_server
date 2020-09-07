#include "web_socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>



s32 main(int argc, char** argv){
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

    s32 clientSocket;
    socklen_t addressLength = 0;
    err = clientSocket = accept(serverSocket, (sockaddr*)&address, &addressLength);
    if (err < 0) { 
        printf("Error accepting client socket.\naddress length: %i\nerrno: %i\n", addressLength, errno); 
        return 1; 
    }

    struct sockaddr_in* pV4Addr = (sockaddr_in*)&address;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
    printf("%s\n", str); 

    const u32 BUFFER_SIZE = 1024;
    s8 buffer[BUFFER_SIZE];
    s32 readAmt = read(clientSocket, buffer, BUFFER_SIZE); 
    while(readAmt >= 0){
        printf("%s\n", buffer); 
        s32 sendAmt = send(clientSocket, "YO!", 3 , 0);
        if(sendAmt < 0){ 
            printf("Error sending data to client.");
            return 1;
        }
        readAmt = read(clientSocket, buffer, BUFFER_SIZE); 
    }
     
    return 0;
}