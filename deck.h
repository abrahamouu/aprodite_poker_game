#ifndef DECK_H
#define DECK_H

#include "card.h"
#include <stdlib.h>

typedef struct {
    int remaincards;
    CARD cards[52];
} DECK;

DECK CreateDeck();

DECK ShuffleDeck(DECK deck);

#endif
