/* 
 * File:   MessageTest.c
 * Author: Ryan
 *
 * Created on May 30, 2024, 2:33 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BOARD.h"
#include "Message.h"
#include "BattleBoats.h"


#define TESTSAMOUNT 5
/*
 * 
 */
int main(int argc, char** argv) {
    BOARD_Init();
    printf("\n###### Beginning 2032150's Field test harness: #####\n\n");
    const char TESTEDFUNCTIONS[TESTSAMOUNT][25] = {"Message_CalculateChecksum", "Message_ParseMessage",
        "Message_Encode", "Message_Decode"};
    const int TESTSNUM[TESTSAMOUNT] = {2, 4, 2, 2};
    int testsPassed[TESTSAMOUNT] = {0, 0, 0, 0};
    char buffer[MESSAGE_MAX_LEN];
    int test_index = 0;    
    // Message_CalculateChecksum
    strcpy(buffer, "CHA,5022");
    uint8_t check_sum1 = 193;
    uint8_t check1 = Message_CalculateChecksum(buffer);
    if (check1 == check_sum1){
        testsPassed[test_index]++;
    }
    
    strcpy(buffer,"RES,1,2,0");
    uint8_t check_sum2 = 91;
    uint8_t check2 = Message_CalculateChecksum(buffer);
    if (check2 == check_sum2){
        testsPassed[test_index]++;
    }        
            

    // Message_ParseMessage
    test_index++;
    strcpy(buffer,"CHA,5022");
    char payload[3] = "C1";
    BB_Event event;
    int check3 = Message_ParseMessage(buffer, payload, &event);
    if ((check3 == SUCCESS) && (event.type == BB_EVENT_CHA_RECEIVED)){
        testsPassed[test_index]++;
    }
    
    strcpy(buffer, "SHO,4,8");
    char payload1[3] = "58";
    int check4 = Message_ParseMessage(buffer, payload1, &event);
    if ((check4 == SUCCESS) && (event.type == BB_EVENT_RES_RECEIVED)){
        testsPassed[test_index]++;
    }
    
    strcpy(buffer,"HEL,4,8");
    char payload2[3] = "95";
    int check5 = Message_ParseMessage(buffer, payload2, &event);
    if ((check5 == STANDARD_ERROR)){
        testsPassed[test_index]++;
    }
    
    strcpy(buffer, "RES,4,9,0");
    char payload3[3] = "55";
    int check6 = Message_ParseMessage(buffer, payload3, &event);
    if ((check6 == SUCCESS) && (event.type == BB_EVENT_RES_RECEIVED) && (event.param0 == 4) && (event.param1 == 9) && (event.param2 == 0)){
        testsPassed[test_index]++;
    }
    
    // Message_Encode
    test_index++;
//    strcpy(buffer, "$RES,2,5,0*5F\n");
    Message eve={MESSAGE_RES, 2, 5, 0};
    int check7 = Message_Encode(buffer, eve);
    int correct = 15;
    if (check7==strlen(buffer)&&strcmp("$RES,2,5,0*5F\n", buffer)==0){
        testsPassed[test_index]++;
    }
    
//    strcpy(buffer, "$SHO,1,6*53\n");
    Message eve1={MESSAGE_SHO, 1, 6};
    int check8 = Message_Encode(buffer, eve1);
    int correct1 = 13;
    if (check8 == strlen(buffer)&&strcmp("$SHO,1,6*53\n", buffer)==0){
        testsPassed[test_index]++;
    }
    
    // Message_Decode
    
    strcpy(buffer, "$SHO,1,6*53\n");
    int curr_index = 0;
    BB_Event new_event; 
    while (buffer[curr_index] != '\0'){
        Message_Decode(buffer[curr_index], &new_event);
        curr_index++;
    }
    if ((new_event.type == BB_EVENT_SHO_RECEIVED)){
        testsPassed[test_index]++;
    }
    
    
    strcpy(buffer, "$RES,3,1,0*5A\n");
    int curr_index1 = 0;
    BB_Event new_event1; 
    while (buffer[curr_index1] != '\0'){
        Message_Decode(buffer[curr_index1], &new_event1);
        curr_index1++;
    }
    if ((new_event1.type == BB_EVENT_RES_RECEIVED)){
        testsPassed[test_index]++;
    }
    
     
    // Ending
    printf("All Tests finished: Results:\n");
    int totalTestsPassed = 0;
    for (int x = 0; x < TESTSAMOUNT; x++) {
        printf("%s():%d/%d\n", TESTEDFUNCTIONS[x], testsPassed[x], TESTSNUM[x]);
        if (testsPassed[x] == TESTSNUM[x]) {
            totalTestsPassed++;
        }
    }
    printf("%d/%d Tests Passed\n", totalTestsPassed, TESTSAMOUNT);

    BOARD_End();
    return (EXIT_SUCCESS);
}

