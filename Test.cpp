#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <bitset>
#define	BUFFSIZE	8192

using namespace std;

unsigned short computeChecksum(unsigned short* addr, int count){
    unsigned int sum = 0;
    return((unsigned short) sum);
}

int main(int argc, char const *argv[])
{
    srand (time(NULL));
    char buffer[BUFFSIZE];
    memset(buffer, 0, sizeof(buffer));
    char host[] = "mathcs05";
    //int overflow = 65535; // 2^16 - 1
    unsigned char versionTX =  0b01100000;
    unsigned char length = 0b00000001;
    uint16_t id = 0x4567;
    uint16_t checksum = 0x0;
    buffer[0] = versionTX;
    buffer[1] = length;
    buffer[2] = id >> 8;
    buffer[3] = id;
    for (size_t i = 0; i < strlen(host); i++)
    {
        buffer[6+i] = host[i];
    }

    cout<<bitset<8>(buffer[0])<<bitset<8>(buffer[1])<<endl;
    cout<<bitset<8>(buffer[2])<<bitset<8>(buffer[3])<<endl;
    cout<<bitset<8>(buffer[4])<<bitset<8>(buffer[5])<<endl;
    cout<<bitset<8>(buffer[6])<<bitset<8>(buffer[7])<<endl;
    cout<<bitset<8>(buffer[8])<<bitset<8>(buffer[9])<<endl;
    cout<<bitset<8>(buffer[10])<<bitset<8>(buffer[11])<<endl;
    cout<<bitset<8>(buffer[12])<<bitset<8>(buffer[13])<<endl;
    cout<<bitset<8>(buffer[14])<<bitset<8>(buffer[15])<<endl;
    cout<<bitset<8>(buffer[16])<<bitset<8>(buffer[17])<<endl;

    cout<<"+++++++++++++++++++++++++"<<endl;
    unsigned int sum;
    //unsigned int hold;
    for(int i = 0; i < 14 ; i+=2){
        uint16_t mask = 0x00ff;
        uint16_t upper = mask & buffer[i];  //0000000011111111 & 00000000xxxxxxxx
        upper = upper << 8;                 //xxxxxxxx00000000
        uint16_t lower = mask & buffer[i+1];//0000000011111111 & 00000000yyyyyyyy
        int fin = upper | lower;            //0000000000000000 xxxxxxxxyyyyyyyy (fin is 32 bit)
        sum += fin;                         //00000000000000000000000000000000 + ^ 
    }
    while((sum >> 16) != 0){
            unsigned int temp = sum >> 16;
            sum = (sum & 0xFFFF);
            sum += temp;
    }
    sum = (~sum);
    checksum = (uint16_t)sum;
    cout<<"Checksum new: "<<checksum<<endl;
    
    buffer[4] = 0x00ff & (checksum>>8);
    buffer[5] = 0x00ff & checksum;
    cout<<"+++++++++++++++++++++++++"<<endl;
    sum = 0;
    checksum = 0;
    // loop over buffer array
    for(int i = 0; i < 14; i+=2){
        uint16_t mask = 0x00ff;
        uint16_t upper = mask & buffer[i];
        upper = upper << 8;
        uint16_t lower = mask & buffer[i+1];
        int fin = upper | lower;
        sum += fin;
    //end of loop
    }
    while((sum >>16) != 0){
            unsigned int temp = sum >> 16;
            sum = (sum & 0xFFFF);
            sum += temp;
    }
    checksum = sum;
     cout<<"Checksum after 2nd iteration: "<<checksum<<endl;
     checksum = (~checksum);
     cout<<"Checksum after 2nd XOR: "<<checksum<<endl;
    
    return 0;
}
