#include "utilities.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

union ServerData{
    struct sockaddr socketAddress;
    struct Data {
        sa_family_t sa_family;
        u16 portNumber;
        u64 ipAddress;
        u32 zero;
    } data;
};

s32 main(int argc, char** argv){
    s32 serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(!serverSocket){
        printf("Error creating serverSocket!\n");
        return 1;
    }

    s32 options;
    s32 err = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | 
                         SO_REUSEPORT, &options, sizeof(int));

    if(err){
        printf("Error setting socket options.\n");
        return 1;
    }

    struct sockaddr_in address; 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = 0; 
    address.sin_port = reverseByteOrder16(8080); 

    err = bind(serverSocket, (struct sockaddr *)&address, sizeof(address));
    if(err < 0){
        printf("Error address to server socket.\n");
        return 1;
    }

    err = listen(serverSocket, 3);
    if(err < 0) { 
        printf("Error listening for connections."); 
        return 1; 
    } 

    s32 clientSocket;
    u32 addressLength;
    err = clientSocket = accept(serverSocket, (struct sockaddr *)&address,  (socklen_t*)&addressLength);
    if (err < 0) { 
        printf("Error accepting client socket."); 
        return 1; 
    }

    struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
    struct in_addr ipAddr = pV4Addr->sin_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
    printf("%s\n", str); 

    const u32 BUFFER_SIZE = 1024;
    s8 buffer[BUFFER_SIZE];
    s32 valread = read(clientSocket, buffer, BUFFER_SIZE); 
    printf("%s\n", buffer); 
    // send(clientSocket, "YO!", strlen("YO!") , 0); 
    // printf("Hello message sent\n"); 

    return 0;
}