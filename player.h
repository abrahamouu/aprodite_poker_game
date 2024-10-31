#ifndef PLAYER_H
#define PLAYER_H
#include <stdbool.h>
#include <stdlib.h>
#include "card.h"

typedef enum{
	DEALER,
	SMALLBLIND,
	BIGBLIND,
	NONE
}ROLE;

typedef enum
{
	NOMOVE = 0,
	FOLD = 1,
	CHECK = 2,
	CALL = 3,
	RAISE = 4,
	ALLIN = 5
}ACTION;

typedef struct{
	ACTION choice;
	int raiseAmount;
}MOVE;

/* Structure for player */
typedef struct 
{
	ROLE role;
	int points;
	char username[256];
	int position;
	CARD playerCards[2];
	int zotbucks; // current betting amt
	ACTION action;
}PLAYER;

#endif