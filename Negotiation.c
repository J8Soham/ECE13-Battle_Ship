/* 
 * File:   Negotiation.c
 * Author: Ryan
 *
 * Created on May 30, 2024, 2:34 PM
 */

#include <stdio.h>
#include <math.h>
#include "Negotiation.h"
#include "Agent.h"
#include "BOARD.h"
/*
 * 
 */

int perfectSquare(int number) {
    for (int possibleroot = 0; possibleroot <= number; possibleroot++) {
        if (number == (possibleroot * possibleroot)) {
            return possibleroot;
        } 
    }
    return FALSE;
}
NegotiationData NegotiationHash(NegotiationData secret){
    return (secret*secret)%((NegotiationData)PUBLIC_KEY);//Returns a hashed version of Secret by using the public key
}

int NegotiationVerify(NegotiationData secret, NegotiationData commitment){
    return NegotiationHash(secret)==commitment;//Checks if the commitment(Hashed secret) is actually equal to the hashed secret
}

NegotiationOutcome NegotiateCoinFlip(NegotiationData A, NegotiationData B){
    NegotiationData aXORB=A ^ B;
    int coin=0;
    for(int bitshift=0;bitshift<16;bitshift++){
            coin+=(aXORB>>bitshift)&1;
    }
    return coin%2==0?HEADS:TAILS;
    
}

NegotiationData NegotiateGenerateBGivenHash(NegotiationData hash_a){//For accepting(tails)
    NegotiationData unhashedA = hash_a;
    int square=perfectSquare(unhashedA);
    while(square==FALSE){//If we know the hash, we can check until we know what a is
        unhashedA+=(int)PUBLIC_KEY;
        square=perfectSquare(unhashedA);
    }
    unhashedA=square;
    NegotiationData newb=1;
    while(NegotiateCoinFlip(unhashedA, newb)!=TAILS){
        newb++;
    }
    return newb;
    
}

NegotiationData NegotiateGenerateAGivenB(NegotiationData B){//for challenging(heads))
    NegotiationData secret = 1;
    while(NegotiateCoinFlip(NegotiationHash(secret), B)!=HEADS){
        secret++;
    }
    return NegotiationHash(secret);
}