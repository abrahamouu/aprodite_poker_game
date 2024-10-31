#ifndef CARD_H
#define CARD_H

typedef enum
{
    	club = 0,
	spade = 1,
	diamond = 2,
	heart = 3,
	othersuit = 4, /*other players suit*/
	blanksuit = 5 /*table cards that aren't shown*/

}SUIT;

/* Defining the rank of a card */
typedef enum
{

	two = 2,
	three = 3, 
	four = 4,
	five = 5, 
	six = 6,
	seven = 7, 
	eight = 8, 
	nine = 9,
	ten = 10, 
	jack = 11,
	queen = 12, 
	king = 13, 
	ace = 14,
	otherrank = 15,
	blankrank = 16

} RANK;


/* Defining the card */
typedef struct
{
	SUIT suit;
	RANK rank;

} CARD;

CARD p1cmp[7];
CARD p2cmp[7];
CARD p3cmp[7];
CARD p4cmp[7];

#endif
