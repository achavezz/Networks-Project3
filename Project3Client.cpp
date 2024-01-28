#include <iostream>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include "NetworkHeader.h"
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <bitset>

using namespace std;
unsigned int tries = 0;

void CatchAlarm(int ignored);

void DieWithError(string errorMessage){
  cout<<errorMessage<<endl;
}

void computeChecksum(char* buffer, uint16_t checksum){
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
    buffer[4] = 0x00ff & (checksum >> 8);
    buffer[5] = 0x00ff & checksum;
}

int main(int argc, char *argv[]){
    // For getting a new seed
    srand (time(NULL));
    
    // Initial variables we're going to use
    char* serverIP = (char *)SERVER_HOST;
    char* serverPort = (char*)SERVER_PORT;
    int timeout;
    int max_retries;
    char* hostname; // mathcs0x'\0'
    char c;
    char buffer[BUFFSIZE];
    memset(buffer, 0, sizeof(buffer));

    // Error message for how to initiate Client
    if (argc < 7 || argc > 11){
        cout<<"Usage Project3ClienA [-h <serverIP>] [-p <port>] -t <timeout> -i <max-retries> -d <hostname>"<<endl;
        exit(1);
    }

    // Parse parameter information to their respective variables
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            c = argv[i][1];

            /* found an option, so look at next
            * argument to get the value of
            * the option */
            switch (c) {
                case 't':
                    timeout = stoi(argv[i+1]);
                    break;
                case 'i':
                    max_retries = stoi(argv[i+1]);
                    break;
                case 'd':
                    hostname = argv[i+1];
                    break;
                case 'h':
                    serverIP = argv[i+1];
                    break;
                case 'p':
                    serverPort = argv[i+1];
                    break;
            }
        }
    }
    // char is 8 bits, checksum is 16 bits, version is 4 bits, type is 3 bits. x is 1 bit, length is 1 bit
    // Initiate packet information
    unsigned char versionTX= 0b01100000;
    unsigned char length = 0b1;
    int16_t queryID = 0x4567;
    uint16_t checksum = 0x0;
   
    // Add header information to packet
    buffer[0] = versionTX;
    buffer[1] = length;
    buffer[2] = (unsigned char)(queryID >> 8);
    buffer[3] = (unsigned char)queryID;

    //append the string the the message leaving 16 bits open for checksum
    for (size_t i = 0; i < strlen(hostname); i++)
    {
        buffer[6+i] = hostname[i];
    }
    
    computeChecksum(buffer, checksum);
    // Testing if checksum is correct 
    //computeChecksum(buffer, checksum);
    
    
    struct addrinfo addrCriteria; // Criteria for address match
    memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
    addrCriteria.ai_family = AF_UNSPEC; // Any address family
    addrCriteria.ai_socktype = SOCK_DGRAM; // Only datagram sockets
    addrCriteria.ai_protocol = IPPROTO_UDP; // Only UDP protocol

    //Get address(es)
    struct addrinfo *servAddr; // List of server addresses
    int rtnVal = getaddrinfo(serverIP, serverPort, &addrCriteria, &servAddr);
    if(rtnVal != 0){
        DieWithError("getaddrinfo() failed");
        gai_strerror(rtnVal);
    }

    //Create a datagram/UDP socket
    int sock = socket(servAddr->ai_family, servAddr->ai_socktype, servAddr->ai_protocol); // Socket descriptor for client
    if(sock < 0)
        DieWithError("socket() failed");

    //Set signal handler for alarm signal
    struct sigaction handler; // Signal handler
    handler.sa_handler = CatchAlarm;
    if(sigfillset(&handler.sa_mask) < 0)
        DieWithError("sigfillset() failed");

    handler.sa_flags = 0;
    if(sigaction(SIGALRM, &handler, 0) < 0)
        DieWithError("sigaction() failed for SIGALRM");

    
    int byteSize = sizeof(hostname) + 6;
    if(byteSize % 2 != 0 )
        byteSize+=1;
    //Send buffer to server
    ssize_t numBytes = sendto(sock, buffer, byteSize, 0, servAddr->ai_addr, servAddr->ai_addrlen);
    cout<<"Sent packet."<<endl;
    if(numBytes < 0)
        DieWithError("sendto() failed");
    else if (numBytes != byteSize)
        DieWithError("sendto() error, sent unexpected number of bytes");
    
    // START TIMER AFTER SENDING THE PACKET

    // catch alarm goes off but priting the packet
    // if time runs out but still processing the packet
    // if bad packet and time runs out
    // if time runs out and no packet
    // RECEIVE THE RESPONSE BACK FROM SERVER TO PRINT USERS LOG IN
    char buffer2[BUFFSIZE];

    struct sockaddr_storage fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);
    
    alarm(timeout);
    /*
    while((numBytes = recvfrom(sock, buffer2, BUFFSIZE, 0, (struct sockaddr *) &fromAddr, &fromAddrLen)) < 0){
        if(errno == EINTR){ // Alarm went off
            if(tries < max_retries){ // Incremented by signal handler
                cout<<"Sending packet again."<<endl;
                numBytes = sendto(sock, buffer, 14, 0, (struct sockaddr *) servAddr->ai_addr, servAddr->ai_addrlen);

                if(numBytes < 0)
                    DieWithError("sendto() failed");
                else if(numBytes != 14)
                    DieWithError("sendto() error, sent unexpected number of bytes");
            }else
                DieWithError("No response, unable to communicate");
        }else
            DieWithError("recv failed()");
    }*/
    
    // Testing for receive a packet 
    char buffer3[BUFFSIZE];
    for(int i = 0; i < 4; i++){
    cout<<i<<endl;
    numBytes = recvfrom(sock, buffer2, BUFFSIZE, 0, (struct sockaddr *) &fromAddr, &fromAddrLen);
    strcat(buffer3, buffer2);
    cout<<"Received packet."<<endl;
        if(numBytes < 0)
            DieWithError("sendto() failed");
        else if(numBytes != BUFFSIZE)
            DieWithError("sendto() error, sent unexpected number of bytes");
    }
    //buffer2[14] = '\0';
    cout<<"Client received a packet."<<endl;
    alarm(0);
    for(int i = 0; i < BUFFSIZE; i++){
        cout<<buffer3[i]<<endl;
    }
    close(sock);
    exit(0);
}

void CatchAlarm(int ignored){
    cout<<"Alarm went off"<<endl;
    tries +=1;
}
