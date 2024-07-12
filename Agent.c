/* 
 * File:   Field.c
 * Author: Ryan
 *
 * Created on May 30, 2024, 2:35 PM
 */
// bitch I am back. (Soham Jain)

# include "Field.h"
#include "FieldOled.h"
#include "Oled.h"
#include "OledDriver.h"
#include "Ascii.h"
#include <stdint.h>
#include "Agent.h"
#include <stdio.h>

// <editor-fold defaultstate="collapsed" desc="macros for trace mode">
#ifndef TRACE_MODE
#define debug_printf(...)
#else
#define debug_printf printf
#endif
// </editor-fold>

static AgentState agentState;
static int turnCounter;
uint16_t secret;
Field Own_Field;
Field Enemy_Field;
GuessData sending;
GuessData recieving;

void AgentInit(void){
    agentState = AGENT_STATE_START;
    turnCounter = 0;
    secret = rand() % 0xFFFF;
    // Reset Everything Below Here TODO.
}




/**
 * This function registers an attack at the gData coordinates on the provided field. This means that
 * 'f' is updated with a FIELD_SQUARE_HIT or FIELD_SQUARE_MISS depending on what was at the
 * coordinates indicated in 'gData'. 'gData' is also updated with the proper HitStatus value
 * depending on what happened AND the value of that field position BEFORE it was attacked. Finally
 * this function also reduces the lives for any boat that was hit from this attack.
 * @param f The field to check against and update.
 * @param gData The coordinates that were guessed. The result is stored in gData->result as an
 *               output.  The result can be a RESULT_HIT, RESULT_MISS, or RESULT_***_SUNK.
 * @return The data that was stored at the field position indicated by gData before this attack.
 */

// How do I use the heads and tails to decide who is attacking or defending. 
Message AgentRun(BB_Event event){
    Message message;
    switch (agentState) {
        case AGENT_STATE_START:
            if (event.type == BB_EVENT_START_BUTTON) {
                agentState = AGENT_STATE_CHALLENGING;
                message.type = MESSAGE_CHA;
                message.param0 = NegotiationHash(secret);
                FieldInit(&Own_Field, &Enemy_Field);
                OledClear(OLED_COLOR_BLACK);
                char buff[60];
                sprintf(buff, "Challenge Sent: \nLobby %d\n\0", message.param0);
                OledDrawString(buff);
                //FieldOledDrawScreen(&Own_Field, &Enemy_Field, FIELD_OLED_TURN_NONE, turnCounter);
                OledUpdate();
                FieldAIPlaceAllBoats(&Own_Field);
                return message;
                //FieldPrint_UART(&Own_Field, &Enemy_Field);
            }else if (event.type == BB_EVENT_CHA_RECEIVED) {
                agentState = AGENT_STATE_ACCEPTING;
                message.type = MESSAGE_ACC;
                message.param0 = secret;
                FieldInit(&Own_Field, &Enemy_Field);
                FieldAIPlaceAllBoats(&Own_Field);
                return message;
            }else if (event.type == BB_EVENT_RESET_BUTTON){
                AgentInit();
            } 
            
        case AGENT_STATE_CHALLENGING:
            if (event.type == BB_EVENT_ACC_RECEIVED){  
                if (NegotiateCoinFlip(message.param0, NegotiationHash(secret)) == 0){
                    agentState = AGENT_STATE_WAITING_TO_SEND;
                }else{
                    agentState = AGENT_STATE_DEFENDING;
                }
                message.type = MESSAGE_REV;
                message.param0 = secret;
                return message;
            }else if (event.type == BB_EVENT_RESET_BUTTON){
                AgentInit();
            }
           
        case AGENT_STATE_ACCEPTING:
            if (event.type == BB_EVENT_REV_RECEIVED) {
                if (NegotiateCoinFlip(message.param0, NegotiationHash(secret)) == 0){
                    agentState = AGENT_STATE_DEFENDING;
                    message.type = BB_EVENT_MESSAGE_SENT;
                }else{
                    agentState = AGENT_STATE_ATTACKING;
                    // Send Sho
                    sending = FieldAIDecideGuess(&Enemy_Field);
                    message.param0 = sending.row;
                    message.param1 = sending.col;
                }
                if (!NegotiationVerify(secret, NegotiationHash(message.param0))){
                    agentState = AGENT_STATE_END_SCREEN;
                }
            }else if (event.type == BB_EVENT_RESET_BUTTON){
                AgentInit();
            } 
            
        case AGENT_STATE_WAITING_TO_SEND:
            if (event.type == BB_EVENT_MESSAGE_SENT) {
                agentState = AGENT_STATE_ATTACKING;
                message.type = MESSAGE_SHO;
                sending = FieldAIDecideGuess(&Enemy_Field);
                message.param0 = sending.row;
                message.param1 = sending.col;
                turnCounter++;
                return message;
            }else if (event.type == BB_EVENT_RESET_BUTTON){
                AgentInit();
            }           
            
        case AGENT_STATE_ATTACKING:
            if (event.type == BB_EVENT_RES_RECEIVED) {
                agentState = AGENT_STATE_WAITING_TO_SEND;
                message.type = MESSAGE_SHO;
                recieving.row = event.param0;
                recieving.col = event.param1;
                recieving.result = event.param2;
                FieldUpdateKnowledge(&Enemy_Field, &recieving);
                if ((Enemy_Field.hugeBoatLives == 0) && (Enemy_Field.largeBoatLives == 0) && (Enemy_Field.mediumBoatLives == 0) && (Enemy_Field.smallBoatLives == 0)){
                    agentState = AGENT_STATE_END_SCREEN;
                }
                return message;
            }else if (event.type == BB_EVENT_RESET_BUTTON){
                AgentInit();
            } 
            
        case AGENT_STATE_DEFENDING:
            if (event.type == BB_EVENT_SHO_RECEIVED) {
                agentState = AGENT_STATE_WAITING_TO_SEND;
                message.type = MESSAGE_RES;
                recieving.row = event.param0;
                recieving.col = event.param1;
                FieldRegisterEnemyAttack(&Own_Field, &recieving);
                if ((Own_Field.hugeBoatLives == 0) && (Own_Field.largeBoatLives == 0) && (Own_Field.mediumBoatLives == 0) && (Own_Field.smallBoatLives == 0)){
                    agentState = AGENT_STATE_END_SCREEN;
                }
                return message;
            }else if (event.type == BB_EVENT_RESET_BUTTON){
                AgentInit();
            } 
    }
//    if (event.type == BB_EVENT_RESET_BUTTON){
//        AgentInit();
//    }
}
  

AgentState AgentGetState(void){
    return agentState;
}

void AgentSetState(AgentState newState){
    agentState = newState;
}

// Check for the Reset
// Check Error.


