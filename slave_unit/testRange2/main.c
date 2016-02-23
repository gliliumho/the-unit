

#include <Nordic\reg9e5.h>
#include "util.h"
#include "uart.h"
#include "radio.h"

#define SLAVE 0

#if SLAVE //If slave

unsigned char CheckPack(unsigned char pack[PACKET_SIZE]){
    unsigned char i;

    for(i=0; i<PACKET_SIZE; i++){
        if(pack[i] != (0xF0 + i))
            return 0;
    }

    return 1;
}

void ReplyFeedback(){
    unsigned char pack[PACKET_SIZE]={0};
    unsigned char i;

    for(i=0; i<PACKET_SIZE; i++)
        pack[i] = (0xFF-i);

    TransmitPacket(&pack);
}


void main(){
    unsigned char packet[PACKET_SIZE]={0};

    InitUART();
    InitRF();
    PutString("\r\n");
    PutString("Slave started for range test.\r\n");

    ReceivePacket(&packet, 0);
    PutString("Received pack..\r\n");
    if(CheckPack(&packet)){
        ReplyFeedback();
        PutString("Replied!!!\r\n");
    }

}

#else //If master

void RequestFeedback(){
    unsigned char pack[PACKET_SIZE]={0};
    unsigned char i;

    for(i=0; i<PACKET_SIZE; i++)
        pack[i] = (0xF0 + i);

    TransmitPacket(&pack);
}

unsigned char WaitFeedback(){
    unsigned char pack[PACKET_SIZE]={0};
    unsigned char i;

    if(ReceivePacket(&pack, 55)){
        for(i=0; i<PACKET_SIZE; i++){
            if(pack[i] != (0xFF-i))
                return 0;
        }
        return 1;
    }
    return 0;
}

void main(){

    unsigned char input, counter;

    InitUART();
    InitRF();

    PutString("\r\n");
    PutString("---The Unit Range Test---\r\n");
    PutString("Please press S to request slave for feedback.");
    PutString("\r\n");

    while(1){
        PutString("Test no ");
        PrintChar(counter);
        //GetChar(&input);
        //PutChar(0x08);
        input = 'f';
        PutString("\r\n");
        if(input >= 'a' && input <= 'z'){
            RequestFeedback();
            PutString("Request sent\r\n");
            if(WaitFeedback())
                PutString("FEEDBACK RECEIVED. SLAVE ALIVE!\r\n");
            else
                PutString("No feedback.........=( \r\n");
        }
        counter++;
        PutString("\r\n");
        Delay5ms(1);
    }
}

#endif
