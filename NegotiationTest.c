/* 
 * File:   NegotiationTest.c
 * Author: Ryan
 *
 * Created on May 30, 2024, 2:34 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "BOARD.h"
#include <math.h>
#include "Negotiation.h"

#define TESTSAMOUNT 3

/*
 * 
 */
int main(int argc, char** argv) {
    
    BOARD_Init();
    printf("\n###### Beginning 2032150's Field test harness: #####\n\n");
    const char TESTEDFUNCTIONS[TESTSAMOUNT][25] = {"NegotiationHash", "NegotiationVerify",
        "NegotiateCoinFlip"};
    const int TESTSNUM[TESTSAMOUNT] = {2, 4, 3};
    int testsPassed[TESTSAMOUNT] = {0, 0, 0};
    int test_index = 0;
    NegotiationData secret; // Customizable
    NegotiationData secret1 = 3;
    NegotiationData data1 = 9;
    NegotiationData secret2 = 12345;
    NegotiationData data2 = 43182;
    
    
    // NegotiationHash 
    if (NegotiationHash(secret1) == data1){
        testsPassed[test_index]++;
    }
    if (NegotiationHash(secret2) == data2){
        testsPassed[test_index]++;
    }
    
    // NegotiationVerify
    test_index++;
    // Test case 1: Valid commitment
    secret = 0x1234;
    NegotiationData commitment = 0x340c;
    if (NegotiationVerify(secret, commitment) == SUCCESS){
        testsPassed[test_index]++;
    }
    // Test case 2: Invalid commitment
    secret = 0x5678;
    if (NegotiationVerify(secret, commitment) == STANDARD_ERROR){
        testsPassed[test_index]++;
    }
    if (NegotiationVerify(secret1, data1) == SUCCESS){
        testsPassed[test_index]++;
    }
    if (NegotiationVerify(secret2, data2) == SUCCESS){
        testsPassed[test_index]++;
    }
    
    // NegotiateCoinFlip
    test_index++;
    secret = 0x1234;
    NegotiationData B = 0x5678;
    if (NegotiateCoinFlip(secret, B) == TAILS){
        testsPassed[test_index]++;
    } 
    if (NegotiateCoinFlip(secret, secret) == HEADS){
        testsPassed[test_index]++;
    }
    B = 10;
    if (NegotiateCoinFlip(data1, B) == HEADS){
        testsPassed[test_index]++;
    }
    
    // NegotiateGenerateBGivenHash
    // NegotiateGenerateAGivenB
    // Didn't test cause didn't implement.
    
    // All finished
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
    return (EXIT_SUCCESS);
}

