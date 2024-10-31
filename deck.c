#include <stdlib.h>
#include <time.h>
#include "card.h"
#include "deck.h"

DECK CreateDeck()
{
	DECK deck;
	deck.remaincards = 52;
	int i = 0; // i refers to the order of cards in a deck i of 0 means first card of the deck

	for(SUIT s = 0; s < 4; s++)
	{
		for(RANK r = 0; r < 15; r++)
		{
			deck.cards[i].suit = s;
			deck.cards[i].rank = r;
			i++;
		}
	}

	deck.remaincards = 52;
	return deck;
} 

DECK ShuffleDeck(DECK deck) {
    srand(time(NULL));
    CARD temp_card;
    int rand_index;

    // shuffling
    for (int i = 51; i > 0; i--) {
        rand_index = rand() % (i + 1);
        temp_card = deck.cards[i];
        deck.cards[i] = deck.cards[rand_index];
        deck.cards[rand_index] = temp_card;
    }
    return deck;
}

CARD DrawCard(DECK *deck) {
	CARD topcard;
	
	int remainingcards = 52 - deck -> remaincards;
	topcard = deck -> cards[remainingcards];

	deck -> remaincards--;

	return topcard;
}
