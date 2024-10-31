#ifndef TABLE_H
#define TABLE_H


#include "card.h"

typedef struct {
    char username[256];
    int points;
    /*card1 & card2*/
    int bet;
    int raise;
    CARD card[2];
    int action;
    int isOccupied;
    int isPlayer;
    int isReady;
    int fold;
    CARD combination[7];
    int mineCheck;
    int bot;

} PLAYER;

#define MAX_PLAYERS 4
PLAYER players[MAX_PLAYERS];

typedef enum {
    MENU,
    WAIT,
    READY,
    PREFLOP,
    FLOP,
    TURN,
    RIVER,
    SHOWDOWN
} ROUND;

typedef struct {
    
    int totalPlayers;
    ROUND currRound;
    int roundCounter;
    int roundBegin;
    int actioncounter;
} GAMESTATE;



GAMESTATE game;


typedef struct {
    int remaincards;
    CARD cards[52];
} DECK;

typedef struct{
    
    DECK deck;
    int pot;
    int minBet;
    int prevminBet;
    int currentBet;
    int totalActivePlayers; /*players who haven't folded*/
    CARD communitycards[5];

}TABLE;



/************************ HANDCHECK FUNCTIONS ************************/
int checkOnePair(CARD cards[7], int number);
int checkTwoPair(CARD cards[7], int number);
int checkThreeOfAKind(CARD cards[7], int number);
int checkStraight(CARD cards[7], int number);
int checkFlush(CARD cards[7], int number);
int checkFullHouse(CARD cards[7], int number);
int checkFourOfAKind(CARD cards[7], int number);
int checkStraightFlush(CARD cards[7], int number);
int checkRoyalFlush(CARD cards[7], int number);
int highestrank(int number);
int tiecondition(int firstcase, int secondcase);

DECK CreateDeck();
CARD DrawCard(DECK *deck);
DECK ShuffleDeck(DECK *deck);
TABLE initTable();
TABLE deleteTable(TABLE table);

#endif
