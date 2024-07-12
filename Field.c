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
#include <stdio.h>
#include "BOARD.h"
#include "BattleBoats.h"


static uint16_t last_boat_states;
static uint8_t target[4];
static uint8_t last_shot[2];
# define FIRST_HIT 12
# define type_shot 4



/**
 * This function is optional, but recommended.   It prints a representation of both
 * fields, similar to the OLED display.
 * @param f The field to initialize.
 * @param p The data to initialize the entire field to, should be a member of enum
 *                     SquareStatus.
void FieldPrint_UART(Field *own_field, Field * opp_field);
 */

/**
 * FieldInit() will initialize two passed field structs for the beginning of play.
 * Each field's grid should be filled with the appropriate SquareStatus (
 * FIELD_SQUARE_EMPTY for your own field, FIELD_SQUARE_UNKNOWN for opponent's).
 * Additionally, your opponent's field's boatLives parameters should be filled
 *  (your own field's boatLives will be filled when boats are added)
 * 
 * FieldAI_PlaceAllBoats() should NOT be called in this function.
 * 
 * @param own_field     //A field representing the agents own ships
 * @param opp_field     //A field representing the opponent's ships
 */
void FieldInit(Field *own_field, Field * opp_field){
    for (int i = 0; i < (FIELD_ROWS); i++){
        for (int k = 0; k < (FIELD_COLS); k++){
            own_field->grid[i][k] = FIELD_SQUARE_EMPTY;
            opp_field->grid[i][k] = FIELD_SQUARE_UNKNOWN;
            //printf("At %d,%d have Mine: %d, Opps: %d", k, i own_field->grid[k][i], opp_field->grid[k][i]);
        }
    }
}


/**
 * Retrieves the value at the specified field position.
 * @param f     //The Field being referenced
 * @param row   //The row-component of the location to retrieve
 * @param col   //The column-component of the location to retrieve
 * @return  FIELD_SQUARE_INVALID if row and col are not valid field locations
 *          Otherwise, return the status of the referenced square 
 */
SquareStatus FieldGetSquareStatus(const Field *f, uint8_t row, uint8_t col){
    if ((row < FIELD_ROWS) && (col < FIELD_COLS)){
        return (f->grid[row][col]);
    } else {
        return FIELD_SQUARE_INVALID;
    }
}


/**
 * This function provides an interface for setting individual locations within a Field struct. This
 * is useful when FieldAddBoat() doesn't do exactly what you need. For example, if you'd like to use
 * FIELD_SQUARE_CURSOR, this is the function to use.
 * 
 * @param f The Field to modify.
 * @param row The row-component of the location to modify
 * @param col The column-component of the location to modify
 * @param p The new value of the field location
 * @return The old value at that field location
 */
SquareStatus FieldSetSquareStatus(Field *f, uint8_t row, uint8_t col, SquareStatus p){
    if ((row < FIELD_ROWS) && (col < FIELD_COLS)){
        SquareStatus temp = f->grid[row][col];
        f->grid[row][col] = p;
        return (temp);
    } else {
        return FIELD_SQUARE_INVALID;
    }
}



/**
 * FieldAddBoat() places a single ship on the player's field based on arguments 2-5. Arguments 2, 3
 * represent the x, y coordinates of the pivot point of the ship.  Argument 4 represents the
 * direction of the ship, and argument 5 is the length of the ship being placed. 
 * 
 * All spaces that
 * the boat would occupy are checked to be clear before the field is modified so that if the boat
 * can fit in the desired position, the field is modified as SUCCESS is returned. Otherwise the
 * field is unmodified and STANDARD_ERROR is returned. There is no hard-coded limit to how many
 * times a boat can be added to a field within this function.
 * 
 * In addition, this function should update the appropriate boatLives parameter of the field.
 *
 * So this is valid test code:
 * {
 *   Field myField;
 *   FieldInit(&myField,FIELD_SQUARE_EMPTY);
 *   FieldAddBoat(&myField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_TYPE_SMALL);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_TYPE_MEDIUM);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_TYPE_HUGE);
 *   FieldAddBoat(&myField, 0, 6, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_TYPE_SMALL);
 * }
 *
 * should result in a field like:
 *      0 1 2 3 4 5 6 7 8 9
 *     ---------------------
 *  0 [ 3 3 3 . . . 3 . . . ]
 *  1 [ 4 4 4 4 . . 3 . . . ]
 *  2 [ . . . . . . 3 . . . ]
 *  3 [ . . . . . . . . . . ]
 *  4 [ . . . . . . . . . . ]
 *  5 [ . . . . . . . . . . ]
 *     
 * @param f The field to grab data from.
 * @param row The row that the boat will start from, valid range is from 0 and to FIELD_ROWS - 1.
 * @param col The column that the boat will start from, valid range is from 0 and to FIELD_COLS - 1.
 * @param dir The direction that the boat will face once places, from the BoatDirection enum.
 * @param boatType The type of boat to place. Relies on the FIELD_SQUARE_*_BOAT values from the
 * SquareStatus enum.
 * @return SUCCESS for success, STANDARD_ERROR for failure
 */
uint8_t FieldAddBoat(Field *own_field, uint8_t row, uint8_t col, BoatDirection dir, BoatType boat_type){
    if (((int)row < FIELD_ROWS) && ((int)col < FIELD_COLS)){
        if (dir == FIELD_DIR_EAST){
            switch(boat_type){
                case FIELD_BOAT_TYPE_SMALL:
                    for (int i = col; i < (col+FIELD_BOAT_SIZE_SMALL); i++){
                        if (FieldGetSquareStatus(own_field, row, i) != FIELD_SQUARE_EMPTY){
                            return STANDARD_ERROR;
                        }
                    }
                    for (int i = col; i < (col+FIELD_BOAT_SIZE_SMALL); i++){
                        FieldSetSquareStatus(own_field, row, i, FIELD_SQUARE_SMALL_BOAT);
                    }
                    own_field->smallBoatLives += 3;
                    //printf("First Small: %d", SUCCESS);
                    return SUCCESS;
                case FIELD_BOAT_TYPE_MEDIUM: 
                    for (int i = col; i < (col+FIELD_BOAT_SIZE_MEDIUM); i++){
                        if (FieldGetSquareStatus(own_field, row, i) != FIELD_SQUARE_EMPTY){
                            return STANDARD_ERROR;
                        }
                    }
                    for (int i = col; i < (col+FIELD_BOAT_SIZE_MEDIUM); i++){
                        FieldSetSquareStatus(own_field, row, i, FIELD_SQUARE_MEDIUM_BOAT);
                    }
                    own_field->mediumBoatLives += 4;
                    //printf("First Med: %d", SUCCESS);
                    return SUCCESS;
                case FIELD_BOAT_TYPE_LARGE:
                    for (int i = col; i < (col+FIELD_BOAT_SIZE_LARGE); i++){
                        if (FieldGetSquareStatus(own_field, row, i) != FIELD_SQUARE_EMPTY){
                            return STANDARD_ERROR;
                        }
                    }
                    for (int i = col; i < (col+FIELD_BOAT_SIZE_LARGE); i++){
                        FieldSetSquareStatus(own_field, row, i, FIELD_SQUARE_LARGE_BOAT);
                    }
                    own_field->largeBoatLives += 5;
                    //printf("First Large: %d", SUCCESS);
                    return SUCCESS;
                case FIELD_BOAT_TYPE_HUGE:
                    for (int i = col; i < (col+FIELD_BOAT_SIZE_HUGE); i++){
                        if (FieldGetSquareStatus(own_field, row, i) != FIELD_SQUARE_EMPTY){
                            return STANDARD_ERROR;
                        }
                    }
                    for (int i = col; i < (col+FIELD_BOAT_SIZE_HUGE); i++){
                        FieldSetSquareStatus(own_field, row, i, FIELD_SQUARE_HUGE_BOAT);
                    }
                    own_field->hugeBoatLives += 6;
                    //printf("First Huge: %d", SUCCESS);
                    return SUCCESS;
            } 
        } else {
            switch(boat_type){
                case FIELD_BOAT_TYPE_SMALL:
                    for (int i = row; i < (row+FIELD_BOAT_SIZE_SMALL); i++){
                        if (FieldGetSquareStatus(own_field, i, col) != FIELD_SQUARE_EMPTY){
                            return STANDARD_ERROR;
                        }
                    }
                    for (int i = row; i < (row+FIELD_BOAT_SIZE_SMALL); i++){
                        FieldSetSquareStatus(own_field, i, col, FIELD_SQUARE_SMALL_BOAT);
                    }
                    own_field->smallBoatLives += 3;
                    return SUCCESS;
                case FIELD_BOAT_TYPE_MEDIUM: 
                    for (int i = row; i < (row+FIELD_BOAT_SIZE_MEDIUM); i++){
                        if (FieldGetSquareStatus(own_field, i, col) != FIELD_SQUARE_EMPTY){
                            return STANDARD_ERROR;
                        }
                    }
                    for (int i = row; i < (row+FIELD_BOAT_SIZE_MEDIUM); i++){
                        FieldSetSquareStatus(own_field, i, col, FIELD_SQUARE_MEDIUM_BOAT);
                    }
                    own_field->mediumBoatLives += 4;
                    return SUCCESS;
                case FIELD_BOAT_TYPE_LARGE:
                    for (int i = row; i < (row+FIELD_BOAT_SIZE_LARGE); i++){
                        if (FieldGetSquareStatus(own_field, i, col) != FIELD_SQUARE_EMPTY){
                            return STANDARD_ERROR;
                        }
                    }
                    for (int i = row; i < (row+FIELD_BOAT_SIZE_LARGE); i++){
                        FieldSetSquareStatus(own_field, i, col, FIELD_SQUARE_LARGE_BOAT);
                    }
                    own_field->largeBoatLives += 5;
                    return SUCCESS;
                case FIELD_BOAT_TYPE_HUGE:
                    for (int i = row; i < (row+FIELD_BOAT_SIZE_HUGE); i++){
                        if (FieldGetSquareStatus(own_field, i, col) != FIELD_SQUARE_EMPTY){
                            return STANDARD_ERROR;
                        }
                    }
                    for (int i = row; i < (row+FIELD_BOAT_SIZE_HUGE); i++){
                        FieldSetSquareStatus(own_field, i, col, FIELD_SQUARE_HUGE_BOAT);
                    }
                    own_field->hugeBoatLives += 6;
                    return SUCCESS;
            } 
        }      
    } else {
        return STANDARD_ERROR;
    }
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
SquareStatus FieldRegisterEnemyAttack(Field *own_field, GuessData *opp_guess){
    SquareStatus original_status = own_field->grid[opp_guess->row][opp_guess->col];
    
    if(original_status == FIELD_SQUARE_EMPTY){
        FieldSetSquareStatus(own_field, opp_guess->row, opp_guess->col, FIELD_SQUARE_MISS);
        opp_guess->result = RESULT_MISS;
    } else {
        FieldSetSquareStatus(own_field, opp_guess->row, opp_guess->col, FIELD_SQUARE_HIT);
        opp_guess->result = RESULT_HIT;
        
        switch(original_status){
            case FIELD_SQUARE_SMALL_BOAT:
                own_field->smallBoatLives -= 1;
                if(own_field->smallBoatLives == 0){
                    opp_guess->result = RESULT_SMALL_BOAT_SUNK;
                }
                break;
            case FIELD_SQUARE_MEDIUM_BOAT:
                own_field->mediumBoatLives--;
                if(own_field->mediumBoatLives == 0){
                    opp_guess->result = RESULT_MEDIUM_BOAT_SUNK;
                }
                break;
            case FIELD_SQUARE_LARGE_BOAT:
                own_field->largeBoatLives--;
                if(own_field->largeBoatLives == 0){
                    opp_guess->result = RESULT_LARGE_BOAT_SUNK;
                }
                break;
            case FIELD_SQUARE_HUGE_BOAT:
                own_field->hugeBoatLives--;
                if(own_field->hugeBoatLives == 0){
                    opp_guess->result = RESULT_HUGE_BOAT_SUNK;
                }
                break;
        }
    }
    return original_status;
}

/**
 * This function updates the FieldState representing the opponent's game board with whether the
 * guess indicated within gData was a hit or not. If it was a hit, then the field is updated with a
 * FIELD_SQUARE_HIT at that position. If it was a miss, display a FIELD_SQUARE_EMPTY instead, as
 * it is now known that there was no boat there. The FieldState struct also contains data on how
 * many lives each ship has. Each hit only reports if it was a hit on any boat or if a specific boat
 * was sunk, this function also clears a boats lives if it detects that the hit was a
 * RESULT_*_BOAT_SUNK.
 * @param f The field to grab data from.
 * @param gData The coordinates that were guessed along with their HitStatus.
 * @return The previous value of that coordinate position in the field before the hit/miss was
 * registered.
 */
SquareStatus FieldUpdateKnowledge(Field *opp_field, const GuessData *own_guess){
    SquareStatus original_status = FieldGetSquareStatus(opp_field, own_guess->row, own_guess->col);
    if (own_guess->result == RESULT_HIT) {
        FieldSetSquareStatus(opp_field, own_guess->row, own_guess->col, FIELD_SQUARE_HIT);
    } else if (own_guess->result == RESULT_MISS) {
        FieldSetSquareStatus(opp_field, own_guess->row, own_guess->col, FIELD_SQUARE_EMPTY);
    } else if (own_guess->result == RESULT_SMALL_BOAT_SUNK || own_guess->result == RESULT_MEDIUM_BOAT_SUNK || own_guess->result == RESULT_LARGE_BOAT_SUNK || own_guess->result == RESULT_HUGE_BOAT_SUNK) {
        switch (own_guess->result) {
            case RESULT_SMALL_BOAT_SUNK:
                opp_field->smallBoatLives = 0;
                break;
            case RESULT_MEDIUM_BOAT_SUNK:
                opp_field->mediumBoatLives = 0;
                break;
            case RESULT_LARGE_BOAT_SUNK:
                opp_field->largeBoatLives = 0;
                break;
            case RESULT_HUGE_BOAT_SUNK:
                opp_field->hugeBoatLives = 0;
                break;
        }
    }
    return original_status;
}

/**
 * This function returns the alive states of all 4 boats as a 4-bit bitfield (stored as a uint8).
 * The boats are ordered from smallest to largest starting at the least-significant bit. So that:
 * 0b00001010 indicates that the samll boat and large boat are sunk, while the medium and huge boat
 * are still alive. See the BoatStatus enum for the bit arrangement.
 * @param f The field to grab data from.
 * @return A 4-bit value with each bit corresponding to whether each ship is alive or not.
 */
uint8_t FieldGetBoatStates(const Field *f){
        return ((f->smallBoatLives > 0 ? FIELD_BOAT_STATUS_SMALL : 0) | (f->mediumBoatLives > 0 ? FIELD_BOAT_STATUS_MEDIUM : 0) | (f->largeBoatLives > 0 ? FIELD_BOAT_STATUS_LARGE : 0) | (f->hugeBoatLives > 0 ? FIELD_BOAT_STATUS_HUGE : 0));
}

/**
 * This function is responsible for placing all four of the boats on a field.
 * 
 * @param f         //agent's own field, to be modified in place.
 * @return SUCCESS if all boats could be placed, STANDARD_ERROR otherwise.
 * 
 * This function should never fail when passed a properly initialized field!
 */
uint8_t FieldAIPlaceAllBoats(Field *own_field){
    BoatSize boat_sizes[] = {FIELD_BOAT_SIZE_HUGE, FIELD_BOAT_SIZE_LARGE, FIELD_BOAT_SIZE_MEDIUM, FIELD_BOAT_SIZE_SMALL};
    BoatType boat_type[] = {FIELD_BOAT_TYPE_HUGE, FIELD_BOAT_TYPE_LARGE, FIELD_BOAT_TYPE_MEDIUM, FIELD_BOAT_TYPE_SMALL};
    uint8_t i = 0;
    uint8_t result;
    
    for (int i = 0; i < FIELD_NUM_BOATS; i++) {
        BoatSize boat_size = boat_sizes[i];
        result = STANDARD_ERROR;
        uint8_t row = 0;
        uint8_t col = 0;
        while (result == STANDARD_ERROR){
            BoatDirection dir = ((rand() & 1) == 0) ? FIELD_DIR_EAST : FIELD_DIR_SOUTH;
            if (dir == FIELD_DIR_EAST) {
                row = rand() % 6;
                col = rand() % (10 - boat_size);
            } else {
                if (boat_size == 6){
                    row = 0;
                }else{
                    row = rand() % (6 - boat_size);   
                }
                col = rand() % 10;
            }
            result = FieldAddBoat(own_field, row, col, dir, boat_type[i]);
        }
    }
    if (i == FIELD_NUM_BOATS){
        return SUCCESS;
    }
    return STANDARD_ERROR;
}
  



// Implementation of Parity.
// Give it static tuple of the last guess it makes. Set it before.
// Give it unit16 variable of the last condition of the opps boats using FieldGetBoatStates intialized.
// Give it a target array of that has a number from 0-10 for 4 variables.
GuessData FieldAIDecideGuess(const Field *opp_field){
   GuessData guess;
    // Check if the last shot was a hit or a miss. 
    // If hit then check if there has been a new ship sunk from the last FieldGetBoatStates, varaible. If so go to choose a new point and also update the shipsunked last variabele, plus set all taget variables to 10.
    // Else If there hasn't been a new ship sunk then check first if all target array values are all 10, if so then sent the first to its row plus 1, second to its row minus 1, third to its col plus 1, fourth to its col minus 1 skip the next one. Else if any target array of them were hit. For the first two variables represent rows the other represent columns. 
    // If there is a target that has been hit then if it is the first or third then add one to the row or column, and send a new guess data. For second and fourth subtract subtract one and send the new guess data.
    // If there hasn't been a hit then for the one that has not been a miss choose that and send it for guess data. (row, col). Go in array order. Use FieldGetSquareStatus to see if it has been a hit. 
    // Else, if there hasn't been a hit at all. Then for the smallest boat alive using FieldGetBoatStates. Get a row and column using parity. 
    // Create basically a switch case. if 3 size is alive then create a rand 0 and 1. and rand mod 10 for columns, check if not gueesed and then send.
    // if lowest size is 4,5,6 then create spots 4 spots apart starting from 0,0 inside the grid that has not been hit and then send that. 
    // Update the last guess for each before the return.
    if (FieldGetSquareStatus(opp_field, last_shot[0], last_shot[1]) == FIELD_SQUARE_HIT) { //  ROW, COLOUMN
        if (last_boat_states != FieldGetBoatStates(opp_field)) {
            last_boat_states = FieldGetBoatStates(opp_field);
            target[0] == FIRST_HIT;
        }else{
            if (target[0] == FIRST_HIT) {
                target[0] = last_shot[0] + 1;
                target[1] = last_shot[0] - 1;
                target[2] = last_shot[1] + 1;
                target[3] = last_shot[1] - 1;
            }
            if (FieldGetSquareStatus(opp_field, target[0], last_shot[1]) == FIELD_SQUARE_UNKNOWN){
                guess.row = target[0];
                guess.col = last_shot[1];
                if (target[0] < FIELD_ROWS){
                    target[0] += 1;
                }
                return guess;
            }
            if (FieldGetSquareStatus(opp_field, target[1], last_shot[1]) == FIELD_SQUARE_UNKNOWN){
                guess.row = target[1];
                guess.col = last_shot[1];
                if (target[0] > 0){
                    target[0] -= 1;
                }
                return guess;
            }
            if (FieldGetSquareStatus(opp_field, last_shot[0], target[0]) == FIELD_SQUARE_UNKNOWN){
                guess.col = target[0];
                guess.row = last_shot[0];
                if (target[1] < FIELD_COLS){
                    target[1] += 1;
                }
                return guess;
            }
            if (FieldGetSquareStatus(opp_field, last_shot[0], target[1]) == FIELD_SQUARE_UNKNOWN){
                guess.col = target[1];
                guess.row = last_shot[0];
                if (target[1] > 0){
                    target[1] -= 1;
                }
                return guess;
            }
        }
    }
    if (FieldGetBoatStates(opp_field) & FIELD_BOAT_STATUS_SMALL) {
        int index = FIELD_BOAT_SIZE_SMALL * (rand() % (FIRST_HIT / FIELD_BOAT_SIZE_SMALL));
        int temp_row = rand() % ((index+1) < FIELD_ROWS ? (index+1) : FIELD_ROWS);
        while ((index - temp_row) >= FIELD_COLS){
            temp_row = rand() % ((index+1) < FIELD_ROWS ? (index+1) : FIELD_ROWS);
        }
        guess.col = (index - temp_row); 
        guess.row = temp_row;
        last_shot[0] = guess.row;
        last_shot[1] = guess.col;
        return guess;
        
    } else if (FieldGetBoatStates(opp_field) & FIELD_BOAT_STATUS_MEDIUM) {
        int index = FIELD_BOAT_SIZE_MEDIUM * (rand() % (FIRST_HIT / FIELD_BOAT_SIZE_MEDIUM));
        int temp_row = rand() % ((index+1) < FIELD_ROWS ? (index+1) : FIELD_ROWS);
        while ((index - temp_row) >= FIELD_COLS){
            temp_row = rand() % ((index+1) < FIELD_ROWS ? (index+1) : FIELD_ROWS);
        }
        guess.col = (index - temp_row); 
        guess.row = temp_row;
        last_shot[0] = guess.row;
        last_shot[1] = guess.col;
        return guess;

    } else if (FieldGetBoatStates(opp_field) & FIELD_BOAT_STATUS_LARGE) {
        int index = FIELD_BOAT_SIZE_LARGE * (rand() % (FIRST_HIT / FIELD_BOAT_SIZE_LARGE));
        int temp_row = rand() % ((index+1) < FIELD_ROWS ? (index+1) : FIELD_ROWS);
        while ((index - temp_row) >= FIELD_COLS){
            temp_row = rand() % ((index+1) < FIELD_ROWS ? (index+1) : FIELD_ROWS);
        }
        guess.col = (index - temp_row); 
        guess.row = temp_row;
        last_shot[0] = guess.row;
        last_shot[1] = guess.col;
        return guess;

    } else if (FieldGetBoatStates(opp_field) & FIELD_BOAT_STATUS_HUGE) {
        int index = FIELD_BOAT_SIZE_HUGE * (rand() % (FIRST_HIT / FIELD_BOAT_SIZE_HUGE));
        int temp_row = rand() % ((index+1) < FIELD_ROWS ? (index+1) : FIELD_ROWS);
        while ((index - temp_row) >= FIELD_COLS){
            temp_row = rand() % ((index+1) < FIELD_ROWS ? (index+1) : FIELD_ROWS);
        }
        guess.col = (index - temp_row); 
        guess.row = temp_row;
        last_shot[0] = guess.row;
        last_shot[1] = guess.col;
        return guess;
    }
}


void FieldPrint_UART(Field *own_field, Field *opp_field) {
    // Print own field
    printf("Own Field:\n");
    for (int i = 0; i < FIELD_ROWS; i++) {
        for (int j = 0; j < FIELD_COLS; j++) {
            switch (own_field->grid[i][j]) {
                case FIELD_SQUARE_EMPTY:
                    printf("-");
                    break;
                case FIELD_SQUARE_SMALL_BOAT:
                    printf("3");
                    break;
                case FIELD_SQUARE_MEDIUM_BOAT:
                    printf("4");
                    break;
                case FIELD_SQUARE_LARGE_BOAT:
                    printf("5");
                    break;
                case FIELD_SQUARE_HUGE_BOAT:
                    printf("6");
                    break;
                case FIELD_SQUARE_HIT:
                    printf("H");
                    break;
                case FIELD_SQUARE_MISS:
                    printf("M");
                    break;
            }
            printf(" ");
        }
        printf("\n");
    }
    if (opp_field != NULL){
        printf("Opponent's Field:\n");
        for (int i = 0; i < FIELD_ROWS; i++) {
            for (int j = 0; j < FIELD_COLS; j++) {
                switch (opp_field->grid[i][j]) {
                    case FIELD_SQUARE_UNKNOWN:
                        printf("-");
                        break;
                    case FIELD_SQUARE_HIT:
                        printf("H");
                        break;
                }
                printf(" ");
            }
            printf("\n");
        }
    }
}