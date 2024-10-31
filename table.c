#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "table.h"

TABLE inittable(int totalplayer, int startpoints)
{
	TABLE table;
	table.player = (PLAYER *)malloc(sizeof(PLAYER) * totalplayer);
	// allocating memory for player struct from player[0] to player[totalplayer -1]
	table.deck = CreateDeck();
	table.totalPlayers = totalplayer;
	table.activePlayers = totalplayer;
	// table.gamestage = 0;
	table.pot = 0;
	table.dealerpos = 0;
	table.currentbet = 0;

	// set cards to blanks 
	for (int i = 0; i < 5; i++) {
		table.communitycards[i].rank = blankrank;
		table.communitycards[i].suit = blanksuit;
	}

	/* setting player information */
	table.player[table.dealerpos].role = DEALER;
	table.player[GetNextPos(table, table.dealerpos)].role = SMALLBLIND;
	table.player[GetNextPos(table, GetNextPos(table, table.dealerpos))].role = BIGBLIND;
	for (int i = 3; i < totalplayer; i++) {
		table.player[i].zotbucks = startpoints;
		table.player[i].playerCards[0].rank = blankrank;
		table.player[i].playerCards[0].suit = blanksuit;
		table.player[i].playerCards[1].rank = blankrank;
		table.player[i].playerCards[1].suit = blanksuit;
		table.player[i].action = NOMOVE;
		table.player[i].zotbucks = 0;
		table.player[i].position = i;
	}
	return table;	
}	

TABLE deleteTable(TABLE table) {
	free(table.player);
	table.player = NULL;
	table.totalPlayers = 0;
	table.activePlayers = 0;
	return table;
}


/************************ HELPER FUNCTIONS ************************/
/* moving clockwise */
int GetNextPos(TABLE table, int currentPos) {
    if (currentPos + 1 <= table.totalPlayers - 1) {
        return currentPos + 1;
    }
    else {
        return 0;
    }
}

/* moving counterclockwise */
int GetPrevPos (TABLE table, int currentPos) {
	if (currentPos - 1 >= 0) {
        return currentPos - 1;
    }
    else {
        return table.totalPlayers - 1; 
    }
}

/************************ HANDCHECK FUNCTIONS ************************/
int checkOnePair(CARD *cards) {
    int rankForPair;
    for (int i = 0; i < 6; i++) {
        rankForPair = cards[i].rank;
        for (int j = i + 1; j < 7; j++) {
            if (cards[j].rank == rankForPair) {
                return 1;
            }
        }
    }
    return 0;
}

int checkTwoPair(CARD *cards) {
    /* first make sure one pair exists */
    if (checkOnePair(cards)) {
        int rankForPair;
        int pairs = 0;

        for (int i = 0; i < 6; i++) {
            rankForPair = cards[i].rank;
            for (int j = i + 1; j < 7; j++) {
                pairs = (cards[j].rank == rankForPair) ? (pairs + 1) : pairs;
            }
        }
        /* note: we could have 2 or 3 pairs here bc if 3 pairs that means 2 pairs exist*/
        if (pairs >= 2) {
			return 1;
		}
        else {
			return 0;
		}
    }
    else {
		return 0;
	}
}

int checkThreeOfAKind(CARD *cards) {
    int cardrankToCheck, rankInstances;
    if (checkOnePair(cards)) {
        for (int i = 0; i < 5; i++) {
            cardrankToCheck = cards[i].rank;
            rankInstances = 1;
            for (int j = i + 1; j < 7; j++) {
                rankInstances = (cardrankToCheck == cards[j].rank) ? (rankInstances + 1) : rankInstances;
            }
            if (rankInstances >= 3) {
                return 1;
            }
        }
    }
    return 0;
}

int checkStraight(CARD *cards) {
    int startCard, startCardPlus1, startCardPlus2, startCardPlus3, startCardPlus4;
    int sc_p1, sc_p2, sc_p3, sc_p4;
    for (int i = 0; i < 7; i ++) {
        startCard = cards[i].rank;
        startCardPlus1 = 0;
        startCardPlus2 = 0;
        startCardPlus3 = 0;
        startCardPlus4 = 0;
        for (int j = 0; j < 7; j++) {
            sc_p1 = (startCard + 1 > 14) ? startCard - 14 + 2 : startCard + 1;
            sc_p2 = (startCard + 2 > 14) ? startCard - 14 + 3 : startCard + 2;
            sc_p3 = (startCard + 3 > 14) ? startCard - 14 + 4 : startCard + 3;
            sc_p4 = (startCard + 4 > 14) ? startCard - 14 + 5 : startCard + 4;
            if (sc_p1 == cards[j].rank) {
                startCardPlus1 = 1;
            }
            else if (sc_p2 == cards[j].rank) {
                startCardPlus2 = 1;
            }
            else if (sc_p3 == cards[j].rank) {
                startCardPlus3 = 1;
            }
            else if (sc_p4 == cards[j].rank) {
                startCardPlus4 = 1;
            }
        }
        if (startCardPlus1 && startCardPlus2 && startCardPlus3 && startCardPlus4) {
            return 1;
        }
    }
    return 0;
}

int checkFlush(CARD *cards) {
    int numSuit;
    for (int i = 0; i < 4; i++) {
        numSuit = 0;
        for (int j = 0; j < 7; j++) {
            numSuit = (cards[j].suit == i) ? (numSuit + 1) : numSuit;
        }
        if (numSuit >= 5) {
            return 1;
        }
    }
    return 0;
}

int checkFullHouse(CARD *cards) {
    if (checkThreeOfAKind(cards) && checkTwoPair(cards)) {
        return 1;
    }
    else {
		return 0;
	}
}

int checkFourOfAKind(CARD *cards) {
    int cardRankToCheck, rankInstances;

    if (checkThreeOfAKind(cards)) {
        for (int i = 0; i < 4; i++) {
            cardRankToCheck = cards[i].rank;
            rankInstances = 1;
            for (int j = i + 1; j < 7; j++) {
                rankInstances = (cardRankToCheck == cards[j].rank) ? (rankInstances + 1) : rankInstances;
            }
            if (rankInstances >= 4) {
                return 1;
            }
        }
    }
    return 0;
}

int checkStraightFlush(CARD *cards) {
	int numSuit;
    int startCard, startCardPlus1, startCardPlus2, startCardPlus3, startCardPlus4;
    int sc_p1, sc_p2, sc_p3, sc_p4;
    if (checkFlush(cards) && checkStraight(cards)) {
		for (int i = 0; i < 4; i++) {
			for (int k = 0; k < 7; k++) {
                numSuit = 0;
                startCardPlus1 = 0;
                startCardPlus2 = 0;
                startCardPlus3 = 0;
                startCardPlus4 = 0;
				startCard = cards[k].rank;
				for (int j = 0; j < 7; j++) {
					if (cards[j].suit == i) {
                        numSuit++;
                        sc_p1 = (startCard + 1 > 14) ? startCard - 14 + 2 : startCard + 1;
                        sc_p2 = (startCard + 2 > 14) ? startCard - 14 + 3 : startCard + 2;
                        sc_p3 = (startCard + 3 > 14) ? startCard - 14 + 4 : startCard + 3;
                        sc_p4 = (startCard + 4 > 14) ? startCard - 14 + 5 : startCard + 4;
                        if (sc_p1 == cards[j].rank) {
                            startCardPlus1 = 1;
						}
						else if (sc_p2 == cards[j].rank) {
                            startCardPlus2 = 1;
						}
						else if (sc_p3 == cards[j].rank) {
                            startCardPlus3 = 1;
						}
						else if (sc_p4 == cards[j].rank) {
                            startCardPlus4 = 1;
						}
					}
				}
				if (startCardPlus1 && startCardPlus2 && startCardPlus3 && startCardPlus4 && numSuit >= 5) {
					return 1;
				}
			}
		}
	}
}

int checkRoyalFlush(CARD *cards) {
    int suitColor, i;

    for (suitColor = 0; suitColor < 4; suitColor++) {
        int color10 = 0;
        int colorJ  = 0;
        int colorQ  = 0;
        int colorK  = 0;
        int colorA  = 0;

        for (i = 0; i < 7; i++) {
            if (cards[i].suit == suitColor && cards[i].rank == 10) {
                color10 = 1;
            }    
            else if (cards[i].suit == suitColor && cards[i].rank == 11) {
                colorJ = 1;
            }
            else if (cards[i].suit == suitColor && cards[i].rank == 12) {
                colorQ = 1;
            }
            else if (cards[i].suit == suitColor && cards[i].rank == 13) {
                colorK = 1;
            }
            else if (cards[i].suit == suitColor && cards[i].rank == 14) {
                colorA = 1;
            }
        }
        if (color10 && colorJ && colorQ && colorK && colorA) {
            return 1;
        }
    }
    return 0;
}