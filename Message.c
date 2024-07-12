/* 
 * File:   Message.c
 * Author: Ryan
 *
 * Created on May 30, 2024, 2:34 PM
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BOARD.h"
#include "Message.h"
#include "BattleBoats.h"
// <editor-fold defaultstate="collapsed" desc="macros for trace mode">
#ifndef TRACE_MODE
#define debug_printf(...)
#else
#define debug_printf printf
#endif
// </editor-fold>

/*
 * 
 */
uint8_t Message_CalculateChecksum(const char* payload) {
    uint8_t checkSum = 0;//Set checksum to 0; will add to this and will be the final return value
    for (int characters = 0; characters < strlen(payload); characters++) {//For every character
        checkSum += (int) payload[characters];//Add the amount that it is
    }
    return checkSum;
}

int Message_ParseMessage(const char* payload,
        const char* checksum_string, BB_Event * message_event) {
    if (Message_CalculateChecksum(payload) != atoi(checksum_string) || strlen(checksum_string) != 2) {//If the checksum != to the given checksum 
        //OR the checksum is not the required length(2)
        return STANDARD_ERROR;//THrow error
    }
    char messageType[3];//Variable used to see the message type that it is
    char number[6];
    sprintf(messageType, payload);
    if (strcmp(messageType, "CHA")) {//If it is a challenge...
        message_event->type = BB_EVENT_CHA_RECEIVED;//Then change the type of the message to be a challenge
        sprintf( number, &payload[4] );
        message_event->param0 = atoi(&number[0]);//And get the one parameter which should be at the 4th index
        return SUCCESS;//And then done
    } else if (strcmp(messageType, "ACC")) {//If it is an acceptance...
        message_event->type = BB_EVENT_ACC_RECEIVED;//Then change the type of the message to be an accepting
        sprintf( number, &payload[4] );
        message_event->param0 = atoi(&number[0]);//And the one parameter must be at the 4th index
        return SUCCESS;//And then done
    } else if (strcmp(messageType, "REV")) {
        message_event->type = BB_EVENT_REV_RECEIVED;//Then change the type of the message to be an reveal
        sprintf( number, &payload[4] );
        message_event->param0 = atoi(&number[0]);//And the one parameter must be at the 4th index
        return SUCCESS;//And then done
    } else if (strcmp(messageType, "SHO")) {//If it is a shot...
        message_event->type = BB_EVENT_SHO_RECEIVED;//Then change the type of the message to be a shot
        memcpy( number, &payload[4], 1 );
        message_event->param0 = atoi(&number[0]);//And the first parameter must be at the 4th index
        memcpy( number, &payload[6], 1 );
        message_event->param1 = atoi(&number[0]);//And the second parameter must be at the 6th index
        return SUCCESS;
    } else if (strcmp(messageType, "RES")) {//If it is a result...
        message_event->type = BB_EVENT_RES_RECEIVED;//Then change the type of the message to be a result
        memcpy( number, &payload[4], 1 );
        message_event->param0 = atoi(&number[0]);//And the first parameter must be at the 4th index
        memcpy( number, &payload[6], 1 );
        message_event->param1 = atoi(&number[0]);//And the second parameter must be at the 6th index
        memcpy( number, &payload[8], 1 );
        message_event->param2 = atoi(&number[0]);//And the Third parameter must be at the 8th index
        return SUCCESS;
    }
    return STANDARD_ERROR;//If it wasnt in any of these then there is an error
}

int Message_Encode(char *message_string, Message message_to_encode) {
    char payload[9];
    switch (message_to_encode.type) {//Depending on what type of message...
        case MESSAGE_CHA:
            sprintf(payload, PAYLOAD_TEMPLATE_CHA, message_to_encode.param0);//Add parameter 0 only(the only parameter) to the format
            break;
        case MESSAGE_ACC:
            sprintf(payload, PAYLOAD_TEMPLATE_ACC, message_to_encode.param0);//Add parameter 0 only(the only parameter) to the format
            break;
        case MESSAGE_REV:
            sprintf(payload, PAYLOAD_TEMPLATE_REV, message_to_encode.param0);//Add parameter 0 only(the only parameter) to the format
            break;
        case MESSAGE_SHO:
            sprintf(payload, PAYLOAD_TEMPLATE_SHO, message_to_encode.param0, message_to_encode.param1);//Add parameter 0 and 1 to the format
            break;
        case MESSAGE_RES:
            sprintf(payload, PAYLOAD_TEMPLATE_RES, message_to_encode.param0, message_to_encode.param1, message_to_encode.param2);//Add all parameters to the format
            break;
    }
    sprintf(message_string, MESSAGE_TEMPLATE, payload, Message_CalculateChecksum(payload));
    return strlen(message_string);//Returns length of string
}
//$SHO,2,9*5F\n
int Message_Decode(unsigned char char_in, BB_Event * decoded_message_event) {
    static char payloadString[9];
    static char checkSumString[3];
    static int place=0;
    static int stringUsed=0;//0 will be payload, checksum will be 1

    if(char_in=='$'&&stringUsed==0&&place==0){//IF it's at the start of the string...
        return SUCCESS;
    }
    else if(char_in=='*'){//If the character is a *, then we are starting the checksum
        payloadString[place]='\0';
        stringUsed=1;
        place=0;
        return SUCCESS;
    }
    else if(char_in=='\n'){//This is the end of the message. Check if it can be parsed, and if so, make it the decoded_message_event
        int error= Message_ParseMessage(payloadString,checkSumString, decoded_message_event);
        stringUsed=0;//Set the string that will be used to the payload
        place=0;
        return error;
    }
    if(!stringUsed){//If the string used is the payload...
//        if(char_in =='1'){
//            return SUCCESS;
//        }
        payloadString[place++]=char_in;//Place the character into the payload string. Also increment by 1
        return SUCCESS;
    }
    else if(stringUsed){//If the string used is the checksum...
        checkSumString[place++]=char_in;//Place the character into the payload string. Also increment by 1
        if(place==3){
            checkSumString[2]='\0';
            int error= Message_ParseMessage(payloadString,checkSumString, decoded_message_event);
            stringUsed=0;//Set the string that will be used to the payload
            place=0;
            return error;
        }
        return SUCCESS;
    }
    return STANDARD_ERROR;
}