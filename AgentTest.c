/* 
 * File:   AgentTest.c
 * Author: Ryan
 *
 * Created on May 30, 2024, 2:33 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "Agent.h"
#include "BOARD.h"
#define TESTSAMOUNT 4

/*
 * 
 */


int main(int argc, char** argv) {

    BOARD_Init();
    printf("\n###### Beginning 2014570's Agent test harness: ####\n\n");
    const char TESTEDFUNCTIONS[TESTSAMOUNT][22] = {"AgentInit", "AgentGetState", "AgentSetState", "AgentRun"};
    const int TESTSNUM[TESTSAMOUNT] = {1, 1, 2, 2};
    int testsPassed[TESTSAMOUNT] = {0, 0, 0, 0};
    int testIndex = 0;
    int error = 0;

    /*Testing AgentInit*/
    /*Test 1: Check if agent is being initialized correctly*/
    AgentInit();
    if (AgentGetState() == AGENT_STATE_START) {//Basically tests both agentGetState, because it can't be accessed since it's a static var
        testsPassed[testIndex]++;
    }

    /*Testing AgentGetState*/
    testIndex++;
    if (AgentGetState() == AGENT_STATE_START) {//Testing again to make sure that it didn't change state earlier. SHould be the same test
        testsPassed[testIndex]++;
    }

    /*Testing AgentSetState*/
    testIndex++;
    /*Testing if AgentSetState sets to AGENT_STATE_ATTACKING*/
    AgentSetState(AGENT_STATE_ATTACKING);
    if (AgentGetState() == AGENT_STATE_ATTACKING) {
        testsPassed[testIndex]++;
    }
    /*Testing if AgentSetState sets to AGENT_STATE_WAITING_TO_SEND*/
    AgentSetState(AGENT_STATE_WAITING_TO_SEND);
    if (AgentGetState() == AGENT_STATE_WAITING_TO_SEND) {
        testsPassed[testIndex]++;
    }


    /*Testing AgentRun*/
    testIndex++;
    AgentSetState(AGENT_STATE_DEFENDING);
    BB_Event inputEvent1 = {BB_EVENT_SHO_RECEIVED, 2, 9};
    Message outputMessage0 = AgentRun(inputEvent1);
    if (outputMessage0.type==MESSAGE_RES && AgentGetState() == AGENT_STATE_WAITING_TO_SEND) {
        testsPassed[testIndex]++;
    }

    AgentSetState(AGENT_STATE_ATTACKING);
    BB_Event inputEvent2 = {BB_EVENT_RES_RECEIVED, 4, 9};
    Message outputMessage1 = AgentRun(inputEvent2);
    if (outputMessage1.type==MESSAGE_SHO && AgentGetState() ==AGENT_STATE_WAITING_TO_SEND) {
        testsPassed[testIndex]++;
    }    





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
    return 0;
}
