/* 
 * File:   FieldTest.c
 * Author: Ryan
 *
 * Created on May 30, 2024, 2:34 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "BOARD.h"
#include "Field.h"
#define TESTSAMOUNT 9

/*
 * 
 */
void assignGuess(GuessData *insert, uint8_t row, uint8_t col, uint8_t shotResult) {//helper function
    insert->row = row;
    insert->col = col;
    insert->result = shotResult;
}

int main(int argc, char** argv) {

    BOARD_Init();
    printf("\n###### Beginning 2014570's Field test harness: ####\n\n");
    const char TESTEDFUNCTIONS[TESTSAMOUNT][25] = {"FieldInit", "FieldGetSquareStatus",
        "FieldSetSquareStatus", "FieldAddBoat", "FieldRegisterEnemyAttack", "FieldUpdateKnowledge", "FieldGetBoatStates",
        "FieldAIPlaceAllBoats", "FieldAIDecideGuess"};
    const int TESTSNUM[TESTSAMOUNT] = {1, 2, 1, 2, 3, 2, 2, 2, 2};
    int testsPassed[TESTSAMOUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    int testIndex = 0;
    int error = 0;

    /*Testing FieldInit*/
    /*Test 1: Testing if initialized correctly*/
    error = SUCCESS; //It is not invalid if it has not been checked
    Field *myField = malloc(sizeof (Field));
    Field *emptyOpponentField = malloc(sizeof (Field));
    FieldInit(myField, emptyOpponentField);
    for (int row = 0; row < FIELD_ROWS; row++) {
        for (int col = 0; col < FIELD_COLS; col++) {
            if (myField->grid[row][col] != FIELD_SQUARE_EMPTY) {
                error = STANDARD_ERROR; //True as in there is an error/an invalid
            }
            if (emptyOpponentField->grid[row][col] != FIELD_SQUARE_UNKNOWN) {
                error = STANDARD_ERROR; //True as in there is an error/an invalid
            }
        }
    }
    if (myField->smallBoatLives != 0 || myField->mediumBoatLives != 0 || myField->largeBoatLives != 0 || myField->hugeBoatLives != 0) {
        error = STANDARD_ERROR;
    }
    if (error == SUCCESS) {
        testsPassed[testIndex]++;
    }
    free(myField);





    /*Testing FieldGetSquareStatus*/
    testIndex++;
    /*Test 1: Checking if it can get the correct square status of a given matrix*/
    error = SUCCESS; //It is not invalid if it has not been checked
    Field *test21Field = malloc(sizeof (Field));
    FieldInit(test21Field, emptyOpponentField);
    for (int row = 0; row < FIELD_ROWS; row++) {
        for (int col = 0; col < FIELD_COLS; col++) {
            test21Field->grid[row][col] = (SquareStatus) row; //This row variable corresponds to some value of the SquareStatus Enum
        }
    }
    for (int row = 0; row < FIELD_ROWS; row++) {
        if (FieldGetSquareStatus(test21Field, row, 0) != (SquareStatus) row) {
            error = STANDARD_ERROR;
        }
    }
    if (error == SUCCESS) {
        testsPassed[testIndex]++;
    }

    /*Test 2: Checking if it gives invalid for a wrong row&col*/
    if (FieldGetSquareStatus(test21Field, FIELD_ROWS + 5, FIELD_COLS + 2) == FIELD_SQUARE_INVALID) {
        error = SUCCESS;
    } else {
        error = STANDARD_ERROR;
    }
    if (error == SUCCESS) {
        testsPassed[testIndex]++;
    }
    free(test21Field);




    /*Testing FieldSetSquareStatus*/
    testIndex++;
    /*Test 1: Checking if it can set the correct square status. Same as FieldGetSquareStatus but 'flipped'*/
    error = SUCCESS; //It is not invalid if it has not been checked
    Field *test31Field = malloc(sizeof (Field)); //Will also be used for test 2; gives all 0s
    FieldInit(test31Field, emptyOpponentField);
    for (int row = 0; row < FIELD_ROWS; row++) {
        for (int col = 0; col < FIELD_COLS; col++) {
            int prevStatus = FieldSetSquareStatus(test31Field, row, col, (SquareStatus) row); //This row variable corresponds to some value of the SquareStatus Enum
            if (prevStatus != FIELD_SQUARE_EMPTY) {
                error = STANDARD_ERROR;
            }
        }
    }
    for (int row = 0; row < FIELD_ROWS; row++) {
        if (test31Field->grid[row][0] != (SquareStatus) row) {
            error = STANDARD_ERROR;
        }
    }
    if (error == SUCCESS) {
        testsPassed[testIndex]++;
    }
    free(test31Field);



    /*Testing FieldAddBoat*/
    testIndex++;
    /*Test 1: Checking if it adds a boat correctly*/
    error = SUCCESS; //It is not invalid if it has not been checked
    Field *test41Field = malloc(sizeof (Field));
    FieldInit(test41Field, emptyOpponentField);
    error = FieldAddBoat(test41Field, 2, 4, FIELD_DIR_SOUTH, FIELD_BOAT_TYPE_MEDIUM);
    if (error == SUCCESS) {
        for (int row = 2; row < 2+FIELD_BOAT_SIZE_MEDIUM; row++) {
            if (test31Field->grid[row][4] != FIELD_SQUARE_MEDIUM_BOAT) {
                error = STANDARD_ERROR;
            }
        }
    }
    if (error == SUCCESS) {
        testsPassed[testIndex]++;
    }
    free(test41Field);


    /*Test 2: Checking that it fails to add a boat*/
    error = SUCCESS; //It is not invalid if it has not been checked
    Field *test42Field = malloc(sizeof (Field));
    FieldInit(test42Field, emptyOpponentField);
    error = FieldAddBoat(test42Field, 3, FIELD_COLS, FIELD_DIR_EAST, FIELD_BOAT_TYPE_HUGE);
    if (error == STANDARD_ERROR) {
        testsPassed[testIndex]++;
    }
    free(test42Field);




    /*Testing FieldRegisterEnemyAttack*/
    testIndex++;
    /*Setup*/
    Field *test5Field = malloc(sizeof (Field));
    SquareStatus enemyAtk5;
    FieldInit(test5Field, emptyOpponentField);
    FieldAddBoat(test5Field, 3, 3, FIELD_DIR_EAST, FIELD_BOAT_TYPE_SMALL);
    /*Setup End*/

    /*Test 1: Checking if it works when it HITS a boat*/
    error = SUCCESS; //It is not invalid if it has not been checked
    GuessData guess5 = {3, 4, 0};
    enemyAtk5 = FieldRegisterEnemyAttack(test5Field, &guess5);
    if (enemyAtk5 == FIELD_SQUARE_SMALL_BOAT && guess5.result == RESULT_HIT) {//Will hit, but should not update the field
        testsPassed[testIndex]++;
    }


    /*Test 2: Checking if it works when it DOESN'T HIT a boat*/
    error = SUCCESS; //It is not invalid if it has not been checked
    assignGuess(&guess5, 1, 1, 0);
    enemyAtk5 = FieldRegisterEnemyAttack(test5Field, &guess5);
    if (enemyAtk5 == FIELD_SQUARE_EMPTY && guess5.result == RESULT_MISS) {
        testsPassed[testIndex]++;
    }
    free(test5Field);


    /*Test 3: Checking if it works when it SINKS a boat*/
    assignGuess(&guess5, 3, 4, 0);
    FieldRegisterEnemyAttack(test5Field, &guess5);
    assignGuess(&guess5, 3, 5, 0);
    enemyAtk5 = FieldRegisterEnemyAttack(test5Field, &guess5);
    if (enemyAtk5 == FIELD_SQUARE_SMALL_BOAT && guess5.result == RESULT_SMALL_BOAT_SUNK) {
        testsPassed[testIndex]++;
    }
    free(test5Field);



    /*Testing FieldUpdateKnowledge*/
    testIndex++;
    /*Setup*/
    Field *test6Field = malloc(sizeof (Field));
    SquareStatus enemyAtk6;
    FieldInit(test6Field, emptyOpponentField);
    FieldAddBoat(test6Field, 2, 2, FIELD_DIR_SOUTH, FIELD_BOAT_TYPE_MEDIUM);
    /*Setup End*/

    /*Test 1: Checking if it works when it HITS a boat*/
    error = SUCCESS; //It is not invalid if it has not been checked
    GuessData guess6 = {3, 2, RESULT_HIT};
    enemyAtk6 = FieldUpdateKnowledge(test6Field, &guess6);
    if (enemyAtk6 == FIELD_SQUARE_MEDIUM_BOAT && test6Field->grid[3][2] == FIELD_SQUARE_HIT) {
        testsPassed[testIndex]++;
    }


    /*Test 2: Checking if it works when it DOESN'T HIT a boat*/
    error = SUCCESS; //It is not invalid if it has not been checked
    assignGuess(&guess6, 3, 3, 0); //second guess
    enemyAtk6 = FieldUpdateKnowledge(test6Field, &guess6);
    if (enemyAtk6 == FIELD_SQUARE_EMPTY && test6Field->grid[3][2] == FIELD_SQUARE_MISS) {
        testsPassed[testIndex]++;
    }
    free(test6Field);


    /*Testing FieldGetBoatStates*/
    testIndex++;
    /*Setup*/
    Field *test7Field = malloc(sizeof (Field));
    FieldInit(test7Field, emptyOpponentField);
    uint8_t states;
    /*Setup End*/

    /*Test 1: Testing on a 0-ship(newly initialized)*/
    states = FieldGetBoatStates(test7Field);
    if (states == 0) {
        testsPassed[testIndex]++;
    }

    /*Test 2: Testing on a 2-ship*/
    FieldAddBoat(test7Field, 2, 2, FIELD_DIR_SOUTH, FIELD_BOAT_TYPE_MEDIUM);
    FieldAddBoat(test7Field, 2, 3, FIELD_DIR_EAST, FIELD_BOAT_TYPE_HUGE);
    states = FieldGetBoatStates(test7Field);
    if (states == 10) {
        testsPassed[testIndex]++;
    }



//    /*Testing FieldAIPlaceAllBoats*/
//    testIndex++;
//    /*Test 1: Testing if it successfully adds boats*/
//    error = SUCCESS; //It is not invalid if it has not been checked
//    Field *test81Field = malloc(sizeof (Field));
//    FieldInit(test81Field, emptyOpponentField);
//    error = FieldAIPlaceAllBoats(test81Field);
//    if (error == SUCCESS && FieldGetBoatStates(test81Field) == 15) {
//        testsPassed[testIndex]++;
//    }
//    free(test81Field);
//
//    /*Test 2: Testing if it stops adding boats if */
//    Field *test82Field = malloc(sizeof (Field));
//    FieldInit(test82Field, emptyOpponentField);
//    FieldAddBoat(test82Field, 2, 3, FIELD_DIR_EAST, FIELD_BOAT_TYPE_HUGE);
//    error = FieldAIPlaceAllBoats(test82Field);
//    if (error == STANDARD_ERROR) {
//        testsPassed[testIndex]++;
//    }
//    free(test82Field);


    /*Testing FieldAIDecideGuess*/
    testIndex++;
    /*Test 1: Testing if it successfully guesses*/
    error = SUCCESS; //It is not invalid if it has not been checked
    Field *test9Field = malloc(sizeof (Field));
    FieldInit(test9Field, emptyOpponentField);
    for (int row = 0; row < FIELD_ROWS; row++) {
        for (int col = 0; col < FIELD_COLS; col++) {
            if (row % 2 == 0 && col % 2 == 0) {
                FieldSetSquareStatus(test9Field, row, col, FIELD_SQUARE_HIT);
            }
        }
    }
    for (int guesses = 0; guesses < 5; guesses++) {//Testing to make sure it's not lucky
        GuessData aiGuess = FieldAIDecideGuess(test9Field);
        if (aiGuess.row % 2 != 0 && aiGuess.col % 2 != 0) {
            FieldSetSquareStatus(test9Field, aiGuess.row, aiGuess.col, FIELD_SQUARE_MISS);
        } else {
            error = STANDARD_ERROR;
        }
    }
    if (error == SUCCESS) {
        testsPassed[testIndex]++;
    }
    free(test9Field);




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

