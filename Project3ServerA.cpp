#include "NetworkHeader.h"
#include <iostream>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include "Database.c"

using namespace std;

void DieWithError(string errorMessage){
  cout<<errorMessage<<endl;
}

u_short computeChecksum(char* buffer, uint16_t checksum){
    // Initiate masks for bit operations and variable placeholders
    uint32_t maskop32 = 0x0000ffff;
    unsigned int sum;

    // Compute checksum of packet
    for(int i = 0; i < 14; i+=2){
        uint16_t mask = 0x00ff;
        uint16_t upper = mask & buffer[i];
        upper = upper << 8;
        uint16_t lower = mask & buffer[i+1];
        int fin = upper | lower;
        sum += fin;
        
    }
    while((sum >> 16) != 0){
            unsigned int temp = sum >> 16;
            sum = (sum & maskop32);
            sum += temp;
    }
    sum = (~sum);
    checksum = (uint16_t) sum;
    return checksum;
}

void HandleUDPClient(char *buffer, char *filename){
    unsigned char temp = buffer[0] >> 1;
    if(temp == 0b0110000){
        //unsigned short tempID1 = (0x00ff & buffer[2]) << 8;
        if(computeChecksum(buffer, 0) == 0){
            char temp[8];
            for (size_t i = 0; i < 8; i++){
                temp[i] = buffer[6+i];
            }

            open_database(filename);
            char **list = lookup_user_names(temp, 0);
        }
    }
}



int main(int argc, char *argv[])
{
    char* serverPort;
    char* filename;
    char c;

    if (!(argc == 5))
    {
        cout<<"Usage Project3ServerA -p <port> -d <data-base-filename>"<<endl;
        exit(1);
    }

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            c = argv[i][1];

            /* found an option, so look at next
            * argument to get the value of
            * the option */
            switch (c) {
                case 'p':
                    serverPort = argv[i+1];
                    break;
                case 'd':
                    filename = argv[i+1];
                    break;
            }
        }
    }

    // Construct the server address stucture 
    struct addrinfo addrCriteria; // Criteria for address
    memset(&addrCriteria, 0 , sizeof(addrCriteria)); // Zero-out structure
    addrCriteria.ai_family = AF_UNSPEC; // Any address family
    addrCriteria.ai_flags = AI_PASSIVE; // Accept on any address/port
    addrCriteria.ai_socktype = SOCK_DGRAM; // Only datagram socket
    addrCriteria.ai_protocol = IPPROTO_UDP; // Only UDP sockets

    struct addrinfo *servAddr; // List of server addresses
    int rtnVal = getaddrinfo(NULL, serverPort, &addrCriteria, &servAddr);
    if(rtnVal != 0)
        DieWithError("getaddriinfo() failed");
    
    // Create socket for incoming connections
    int sock = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol);
    if(sock < 0)
        DieWithError("socket() failed");
    
    // Bind local address
    if(bind(sock, servAddr->ai_addr, servAddr->ai_addrlen) < 0)
        DieWithError("bind() failed");

    // Free address list allocated by getaddrinfo()
    freeaddrinfo(servAddr);

    for(;;) // Run forever
    {
        struct sockaddr_storage clntAddr; // Client address
        socklen_t clntAddrLen = sizeof(clntAddr);
        char buffer[BUFFSIZE];
        ssize_t numBytesRcvd = recvfrom(sock, buffer, BUFFSIZE, 0, (struct sockaddr *) &clntAddr, &clntAddrLen);
        if(numBytesRcvd < 0)
            DieWithError("recvfrom() failed");

        HandleUDPClient(buffer, filename);
        // SEND THE USERS LOGGED IN
    }
    return 0;
}
