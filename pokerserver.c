#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <assert.h>

#include "card.h"
#include "table.h"

/*** global variables ****************************************************/

const char *Program = NULL;
int Shutdown = 0;
char ClockBuffer[26] = "";

/*** global functions ****************************************************/

DECK CreateDeck()
{
	DECK deck;
	deck.remaincards = 52;
	
    int suit = 0;
    int rank = 2;

    for(int i = 0; i < 52; i++){
        if(rank > 14){
            rank = 2;
            suit++;
        }
        deck.cards[i].rank = rank;
        deck.cards[i].suit =suit;
        

        rank++;
        
    }
    

	return deck;
} 

DECK ShuffleDeck(DECK *deck) {
    srand(time(NULL));
    
    CARD temp_card;
    int rand1, rand2;

    for(int i = 0; i < 500; i++){
        rand1 = rand()%52;
        rand2 = rand()%52;
        temp_card = deck->cards[rand1];
        deck->cards[rand1] = deck->cards[rand2];
        deck->cards[rand2] = temp_card;
        
    }

    // for(int i = 0; i < 52; i++){
    //     printf("rank %d, suit %d\n", deck->cards[i].rank, deck->cards[i].suit);
        
    //     }
    
    return *deck;
}

CARD DrawCard(DECK *deck) {
        CARD topcard;

        int remainingcards = 52 - deck -> remaincards;
        topcard = deck -> cards[remainingcards];

        deck -> remaincards--;

        return topcard;
}



TABLE inittable()
{

        TABLE table;
        table.deck = CreateDeck();
        table.deck = ShuffleDeck(&(table.deck));
        int i = 0;
        while(i < 4)
        {
                players[0].card[i] = DrawCard(&(table.deck));
                players[1].card[i] = DrawCard(&(table.deck));
                players[2].card[i] = DrawCard(&(table.deck));
                players[3].card[i] = DrawCard(&(table.deck));
                i++;
        }
        i = 0;
        while (i < 5)
        {
                table.communitycards[i] = DrawCard(&(table.deck));
                i++;
        }

        table.pot = 0;

        return table;
}



char convertRank(int rank) {
    switch(rank){
        case two:
            return '2';
        case three:
            return '3';
        case four:
            return '4';
        case five:
            return '5';
        case six:
            return '6';
        case seven:
            return '7';
        case eight:
            return '8';
        case nine:
            return '9';
        case ten:
            return 'T';
        case 11:
            return 'J';
        case 12:
            return 'Q';
        case 13:
            return 'K';
        case 14:
            return 'A';
        
        default:
            return '?'; // Invalid rank
    }
}

char convertSuit(int suit) {
    switch(suit){
        case club:
            return 'C';
        case spade:
            return 'S';
        case diamond:
            return 'D';
        case heart:
            return 'H';
        case othersuit:
            return 'O'; // Other player's suit
        case blanksuit:
            return 'B'; // Table cards that aren't shown
        default:
            return '?'; // Invalid suit
    }
}


int checkOnePair(CARD cards[7], int number) {
    int rankForPair;
    for (int i = 0; i < 6; i++) {
        rankForPair = cards[i].rank;
        for (int j = i + 1; j < 7; j++) {
            if (cards[j].rank == rankForPair) {
		players[number].combination[0] = cards[i];
		players[number].combination[1] = cards[j];
                return 1;
            }
        }
    }
    return 0;
}

int checkTwoPair(CARD cards[7], int number) {
    /* first make sure one pair exists */
    if (checkOnePair(cards, number)) {
        int rankForPair;
        int pairs = 0;

        for (int i = 0; i < 6; i++) {
            rankForPair = cards[i].rank;
            for (int j = i + 1; j < 7; j++) {
                pairs = (cards[j].rank == rankForPair) ? (pairs + 1) : pairs;
		if(cards[j].rank == rankForPair)
		{
			players[number].combination[2] = cards[i];
			players[number].combination[3] = cards[j];
		}
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

int checkThreeOfAKind(CARD cards[7], int number) {
    int cardrankToCheck, rankInstances;
    if (checkOnePair(cards, number)) {
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

int checkStraight(CARD cards[7], int number) {
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
                startCardPlus2 = cards[j].rank;
            }
            else if (sc_p2 == cards[j].rank) {
                startCardPlus2 = cards[j].rank;
            }
            else if (sc_p3 == cards[j].rank) {
                startCardPlus3 = cards[j].rank;
            }
            else if (sc_p4 == cards[j].rank) {
                startCardPlus4 = cards[j].rank;
            }
        }
        if (startCardPlus1 && startCardPlus2 && startCardPlus3 && startCardPlus4) {
	    players[number].combination[0].rank = startCard;
	    players[number].combination[1].rank = startCardPlus1;
	    players[number].combination[2].rank = startCardPlus2;
	    players[number].combination[3].rank = startCardPlus3;
	    players[number].combination[4].rank = startCardPlus4;
            return 1;
        }
    }
    return 0;
}

int checkFlush(CARD cards[7], int number) {
    int numSuit;
    int flushrank;
    flushrank  = 0;

    for (int i = 0; i < 4; i++) {
        numSuit = 0;
        for (int j = 0; j < 7; j++) {
            numSuit = (cards[j].suit == i) ? (numSuit + 1) : numSuit;
	    if(cards[j].suit == i && flushrank < cards[j].rank)
	    {	
		flushrank = cards[j].rank;
	    }   
        }
        if (numSuit >= 5) {
	    players[number].combination[0].rank = flushrank;
            return 1;
        }
    }
    return 0;
}

int checkFullHouse(CARD cards[7], int number) {
    if (checkThreeOfAKind(cards, number) && checkTwoPair(cards, number)) {
        return 1;
    }
    else {
		return 0;
	}
}

int checkFourOfAKind(CARD cards[7], int number) {
    int cardRankToCheck, rankInstances;

    if (checkThreeOfAKind(cards, number)) {
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

int checkStraightFlush(CARD cards[7], int number) {
	int numSuit;
    int startCard, startCardPlus1, startCardPlus2, startCardPlus3, startCardPlus4;
    int sc_p1, sc_p2, sc_p3, sc_p4;
    if (checkFlush(cards, number) && checkStraight(cards, number)) {
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
    return 0;
}

int checkRoyalFlush(CARD cards[7], int number) {
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

int bestHand(CARD cards[7], int number){
    if (checkRoyalFlush(cards, number)) {
        return 10; // Royal Flush
    } 
    else if (checkStraightFlush(cards, number)) {
        return 9; // Straight Flush
    } 
    else if (checkFourOfAKind(cards, number)) {
        return 8; // Four of a Kind
    } 
    else if (checkFullHouse(cards, number)) {
        return 7; // Full House
    } 
    else if (checkFlush(cards, number)) {
        return 6; // Flush
    } 
    else if (checkStraight(cards, number)) {
        return 5; // Straight
    } 
    else if (checkThreeOfAKind(cards, number)) {
        return 4; // Three of a Kind
    } 
    else if (checkTwoPair(cards, number)) {
        return 3; // Two Pair
    } 
    else if (checkOnePair(cards, number)) {
        return 2; // One Pair
    } 
    else {
        return 1; // High Card
    }
    
}

int highestrank(int number)
{
	int highestrank = players[number].combination[0].rank;

	for(int i = 1; i < 7; i++)
	{
		if(players[number].combination[i].rank > highestrank)
		{
			highestrank = players[number].combination[i].rank;
		}
	}
	
	return highestrank;
}
	

int tiecondition(int firstcase, int secondcase)
{
	int highestrankfirst;
	int highestranksecond;

	highestrankfirst = highestrank(firstcase);
	highestranksecond = highestrank(secondcase);

	if(highestrankfirst > highestranksecond)
	{
		return firstcase;
	}
	else
	{
		return secondcase;
	}

	return 0;
}
void FatalError(const char *ErrorMsg) {
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
}

int MakeServerSocket(uint16_t PortNo) {
    int ServSocketFD;
    struct sockaddr_in ServSocketName;

    ServSocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (ServSocketFD < 0) {
        FatalError("service socket creation failed");
    }

    ServSocketName.sin_family = AF_INET;
    ServSocketName.sin_port = htons(PortNo);
    ServSocketName.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(ServSocketFD, (struct sockaddr*)&ServSocketName, sizeof(ServSocketName)) < 0) {
        FatalError("binding the server to a socket failed");
    }

    if (listen(ServSocketFD, 5) < 0) {
        FatalError("listening on socket failed");
    }

    return ServSocketFD;
}

void PrintCurrentTime(void) {
    time_t CurrentTime;
    char *TimeString;
    char Wheel, *WheelChars = "|/-\\";
    static int WheelIndex = 0;

    CurrentTime = time(NULL);
    TimeString = ctime(&CurrentTime);
    strncpy(ClockBuffer, TimeString, 25);
    ClockBuffer[24] = 0;
    WheelIndex = (WheelIndex + 1) % 4;
    Wheel = WheelChars[WheelIndex];
    printf("\rClock: %s %c", ClockBuffer, Wheel);
    fflush(stdout);
}

void ProcessRequest(int DataSocketFD, TABLE *table) {
    int l, n;
    char RecvBuf[256];
    char SendBuf[256];

    printf("actioncounter:%d\n", game.actioncounter); 
    printf("total players %d\n", game.totalPlayers);
    game.roundCounter = 1;
    n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf) - 1);
    if (n < 0) {
        FatalError("reading from data socket failed");
    }
    RecvBuf[n] = 0;
           
    while(game.roundCounter < 11){

    
    if (strncmp(RecvBuf, "SEAT ", 5) == 0) 
    {
        int seat = RecvBuf[5] - '0';
        if (seat >= 1 && seat <= MAX_PLAYERS) 
        {
            players[seat - 1].isOccupied = 1;
            snprintf(SendBuf, sizeof(SendBuf), "YOU ARE NOW SEAT %d", seat);
            game.totalPlayers += 1;
            printf("total players %d", game.totalPlayers);
            game.currRound = WAIT;
        } else 
        {
            snprintf(SendBuf, sizeof(SendBuf), "ERROR invalid seat number: %d", seat);
        }
    } 
    else if ((game.currRound == WAIT) && (strncmp(RecvBuf, "NAME ", 5) == 0)) 
    {
        char *username = RecvBuf + 5;
        int seat;
        for (seat = 0; seat < MAX_PLAYERS; seat++) 
        {
            if (players[seat].isOccupied && players[seat].username[0] == '\0') {
                strncpy(players[seat].username, username, sizeof(players[seat].username) - 1);
                players[seat].username[sizeof(players[seat].username) - 1] = '\0';
                snprintf(SendBuf, sizeof(SendBuf), "Player %d's username is set to: %s", seat + 1, players[seat].username);
                
                game.currRound = READY;
                break;
            }
        }
        if (seat > MAX_PLAYERS) 
        {
            snprintf(SendBuf, sizeof(SendBuf), "ERROR no available seat to set username");
        }
    }

    else if(game.totalPlayers == 1 && game.currRound == READY  && (strncmp(RecvBuf, "START", 5) == 0)){
        snprintf(SendBuf, sizeof(SendBuf), "ONE PlAYER IS PlAYING. VIEW YOUR CARDS. PLAYER 1 IS SMALL BLIND. PLEASE MAKE YOUR BET.\n");
        game.roundBegin = 1;
        
        game.currRound = PREFLOP;
        for (int i = 0; i < MAX_PLAYERS; i++){
            players[i].points += 1000;
            players[i].bet = 0;
            players[i].raise = 0;
            /*card 1 rank and suit = 0*/
            /*card 2 rank and suit = 0*/
            
        }
    }

    else if(game.totalPlayers == 2 && game.currRound == READY  && (strncmp(RecvBuf, "START", 5) == 0)){
        snprintf(SendBuf, sizeof(SendBuf), "TWO PLAYERS ARE PlAYING. VIEW YOUR CARDS. PLAYER 1 IS SMALL BLIND. PLEASE MAKE YOUR BET.\n");
        game.roundBegin = 1;
        
        game.currRound = PREFLOP;
        for (int i = 0; i < MAX_PLAYERS; i++){
            players[i].points += 1000;
            players[i].bet = 0;
            players[i].raise = 0;
            /*card 1 rank and suit = 0*/
            /*card 2 rank and suit = 0*/
            
        }
    }

    else if((strncmp(RecvBuf, "BET 1 ", 6) == 0) && game.totalPlayers == 1 && game.currRound == PREFLOP && players[0].isOccupied == 1 ){
        
        if(game.currRound <= 2)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing the game commands!\n");
                        break;
                }

                int userBet;

                sscanf(RecvBuf + 6, "%d", &userBet);
                if(userBet <= 0  && game.currRound == PREFLOP)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Small blind must bet above 0! Reinitialize your bet\n");
                        break;
                }

                if(game.currRound > PREFLOP)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Bet command is only for initialization. Use check/call/raise/fold command!\n");
                        break;
                }

            
            players[0].bet = userBet;
            table->minBet = players[0].bet;

            table->pot = players[0].bet;
            players[0].points = players[0].points - userBet;
            players[1].bet = 2 * players[0].bet;
            players[1].points = players[1].points - players[1].bet;
            table->pot = table->pot + players[1].bet;

            players[2].fold = 1;
            players[3].fold = 1;
            game.actioncounter += 1;
            snprintf(SendBuf, sizeof(SendBuf), "PLAYER 1(%s) has bet %d.\n PLAYER 2 BOT HAS BET %d\n PLAYER 3 BOT HAS BET HAS FOLDED\n PLAYER 4 HAS FOLDED\n", players[0].username, players[0].bet, players[1].bet);
            
            game.currRound = FLOP;


    }

    else if(players[0].isOccupied == 1 && game.totalPlayers == 1 && game.actioncounter == 1 && game.currRound == FLOP && (((strncmp(RecvBuf, "CHECK 1", 7) == 0)) ||((strncmp(RecvBuf, "RAISE 1", 7) == 0))) ){
        if(game.currRound <= 2)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing the game commands!\n");
                        break;
                }
            players[1].fold = 1;
            players[0].points += table->pot;
            table->pot = 0;

            snprintf(SendBuf, sizeof(SendBuf), "PLAYER 1(%s) HAS MADE THERE ACTION.\n PLAYER 2 BOT HAS FOLDED\n PLAYER 3 BOT HAS BET HAS FOLDED\n PLAYER 4 HAS FOLDED\n PLAYER 1(%s), YOU ARE THE WINNER!\n", players[0].username, players[0].username);

                

    }


    else if(game.totalPlayers == 3 && game.currRound == READY  && (strncmp(RecvBuf, "START", 5) == 0)){
        snprintf(SendBuf, sizeof(SendBuf), "THREE PLAYERS ARE PlAYING. VIEW YOUR CARDS. PLAYER 1 IS SMALL BLIND.\n");
        game.roundBegin = 1;
        
        game.currRound = PREFLOP;
        for (int i = 0; i < MAX_PLAYERS; i++){
            players[i].points += 1000;
            players[i].bet = 0;
            players[i].raise = 0;
            /*card 1 rank and suit = 0*/
            /*card 2 rank and suit = 0*/
            
        }
    }





    else if(game.totalPlayers == 4 && game.currRound == READY && (strncmp(RecvBuf, "START", 5) == 0))
    {
        snprintf(SendBuf, sizeof(SendBuf), "ALL PLAYERS READY. DEALING YOUR CARDS. VIEW YOUR CARDS. Player %s you are small blind. Make your initial bet", players[0].username);
        printf("game is ready to start");
        game.roundBegin = 1;
        
        game.currRound = PREFLOP;
        for (int i = 0; i < MAX_PLAYERS; i++){
            players[i].points += 1000;
            players[i].bet = 0;
            players[i].raise = 0;
            /*card 1 rank and suit = 0*/
            /*card 2 rank and suit = 0*/
            
        }
    }

	else if(strncmp(RecvBuf, "CC", 2) == 0){


		char card1[2];
		char card2[2];
		char card3[2];
		char card4[2];
		char card5[2];
		
		int rank1, suit1, rank2, suit2, rank3, suit3, rank4, suit4, rank5, suit5;

		rank1 = table->communitycards[0].rank;
        	suit1 = table->communitycards[0].suit;
        	rank2 = table->communitycards[1].rank;
        	suit2 = table->communitycards[1].suit;
		rank3 = table->communitycards[2].rank;
        	suit3 = table->communitycards[2].suit;
		rank4 = table->communitycards[3].rank;
        	suit4 = table->communitycards[3].suit;
		rank5 = table->communitycards[4].rank;
        	suit5 = table->communitycards[4].suit;


		card1[0] = convertRank(rank1);
                card1[1] = convertSuit(suit1);

                card2[0] = convertRank(rank2);
                card2[1] = convertSuit(suit2);

		card3[0] = convertRank(rank3);
                card3[1] = convertSuit(suit3);

		card4[0] = convertRank(rank4);
                card4[1] = convertSuit(suit4);

		card5[0] = convertRank(rank5);
                card5[1] = convertSuit(suit5);


		switch(game.currRound){
			case 3:
				snprintf(SendBuf, sizeof(SendBuf), "This is PREFLOP round, all community cards are faced down\n");
				break;
			case 4:
				snprintf(SendBuf, sizeof(SendBuf), "This is FLOP round, three cards are revealed\n"); 
				snprintf(SendBuf, sizeof(SendBuf), "\nCard 1: Rank is %c Suit is %c\nCard 2: Rank is %c Suit is %c\nCard 3: Rank is %c Suit is %c\nCard 4: Faced down\nCard 5: Faced down\n", card1[0], card1[1], card2[0], card2[1], card3[0], card3[1]); 
				break;
			case 5:
				snprintf(SendBuf, sizeof(SendBuf), "This is Turn round, four cards are revealed\n");
                                snprintf(SendBuf, sizeof(SendBuf), "\nCard 1: Rank is %c Suit is %c\nCard 2: Rank is %c Suit is %c\nCard 3: Rank is %c Suit is %c\nCard 4: Rank is %c, Suit is %c\nCard 5: Faced down\n", card1[0], card1[1], card2[0], card2[1], card3[0], card3[1], card4[0], card4[1]);
				break;
			case 6:
				snprintf(SendBuf, sizeof(SendBuf), "This is River round, all cards are revealed\n");
                                snprintf(SendBuf, sizeof(SendBuf), "\nCard 1: Rank is %c Suit is %c\nCard 2: Rank is %c Suit is %c\nCard 3: Rank is %c Suit is %c\nCard 4: Rank is %c, Suit is %c\nCard 5: Rank is %c Suit is %c\n", card1[0], card1[1], card2[0], card2[1], card3[0], card3[1], card4[0], card4[1], card5[0], card5[1]); 
                                printf("rank %d, suit %d\n", card5[0], card5[1]);
                                break;
			default:
				snprintf(SendBuf, sizeof(SendBuf), "Game is not set, please start the game!\n");
			}
	}	
	    
        else if(strncmp(RecvBuf, "MINE ", 5) == 0){

	    if(game.currRound <= 2)
            {
                snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing game commands!\n");
                break;
            }

            int seatnumber;
            int rank1, rank2;
            int suit1, suit2;

            char card1[2];
            char card2[2];
            
            
            sscanf(RecvBuf + 5, "%d", &seatnumber);
            switch(seatnumber){
                case 1:
                    
                    printf("CARD 1: rank is %d and suit is %d CARD 2: rank is %d and suit is %d", players[0].card[0].rank, players[0].card[0].suit, players[0].card[1].rank, players[0].card[1].suit);
                    rank1 = players[0].card[0].rank;
                    suit1 = players[0].card[0].suit;
                    rank2 = players[0].card[1].rank;
                    suit2 = players[0].card[1].suit;

                    card1[0] = convertRank(rank1);
                    card1[1] = convertSuit(suit1);

                    card2[0] = convertRank(rank2);
                    card2[1] = convertSuit(suit2);

                    

                    // test = checkOnePair(p1cmp);
                    // printf("matching pair: %d", test);

                    

                    snprintf(SendBuf, sizeof(SendBuf), "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c ",card1[0], card1[1], card2[0], card2[1] );

                    printf("CARD 1: RANK is %c and SUIT is %c\n", card1[0], card1[1]);
                    printf("CARD 2: RANK is %c and SUIT is %c\n", card2[0], card2[1]);
                    players[0].mineCheck += 1;


                    break;

                case 2:
                    printf("CARD 1: rank is %d and suit is %d CARD 2: rank is %d and suit is %d", players[1].card[0].rank, players[1].card[0].suit, players[1].card[1].rank, players[1].card[1].suit);


		            rank1 = players[1].card[0].rank;
                    suit1 = players[1].card[0].suit;
                    rank2 = players[1].card[1].rank;
                    suit2 = players[1].card[1].suit;

                    card1[0] = convertRank(rank1);
                    card1[1] = convertSuit(suit1);

                    card2[0] = convertRank(rank2);
                    card2[1] = convertSuit(suit2);
                    
                    snprintf(SendBuf, sizeof(SendBuf), "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c ",card1[0], card1[1], card2[0], card2[1] );

                    printf("CARD 1: RANK is %c and SUIT is %c\n", card1[0], card1[1]);
                    printf("CARD 2: RANK is %c and SUIT is %c\n", card2[0], card2[1]);
                    break;


                case 3:
                printf("CARD 1: rank is %d and suit is %d CARD 2: rank is %d and suit is %d", players[2].card[0].rank, players[2].card[0].suit, players[2].card[1].rank, players[2].card[1].suit);

		            rank1 = players[2].card[0].rank;
                    suit1 = players[2].card[0].suit;
                    rank2 = players[2].card[1].rank;
                    suit2 = players[2].card[1].suit;

                    card1[0] = convertRank(rank1);
                    card1[1] = convertSuit(suit1);

                    card2[0] = convertRank(rank2);
                    card2[1] = convertSuit(suit2);

                    snprintf(SendBuf, sizeof(SendBuf), "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c ",card1[0], card1[1], card2[0], card2[1] );

                    printf("CARD 1: RANK is %c and SUIT is %c\n", card1[0], card1[1]);
                    printf("CARD 2: RANK is %c and SUIT is %c\n", card2[0], card2[1]);
                    break;

                case 4:
                printf("CARD 1: rank is %d and suit is %d CARD 2: rank is %d and suit is %d", players[3].card[0].rank, players[3].card[0].suit, players[3].card[1].rank, players[3].card[1].suit);
			
		            rank1 = players[3].card[0].rank;
                    suit1 = players[3].card[0].suit;
                    rank2 = players[3].card[1].rank;
                    suit2 = players[3].card[1].suit;

                    card1[0] = convertRank(rank1);
                    card1[1] = convertSuit(suit1);

                    card2[0] = convertRank(rank2);
                    card2[1] = convertSuit(suit2);
                    snprintf(SendBuf, sizeof(SendBuf), "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c ",card1[0], card1[1], card2[0], card2[1] );


                    
                    printf("CARD 1: RANK is %c and SUIT is %c\n", card1[0], card1[1]);
                    printf("CARD 2: RANK is %c and SUIT is %c\n", card2[0], card2[1]);
                    break;

                default:
			snprintf(SendBuf, sizeof(SendBuf), "Invalid Seat Number! Please type valid seat number\n");
                        break;
            }

        }
        
          
        else if(strncmp(RecvBuf, "BET 1 ", 6) == 0){
            
	    if(game.currRound <= 2)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing the game commands!\n");
                        break;
                }

                int userBet;

                sscanf(RecvBuf + 6, "%d", &userBet);
                if(userBet <= 0  && game.currRound == PREFLOP)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Small blind must bet above 0! Reinitialize your bet\n");
                        break;
                }

                if(game.currRound > PREFLOP)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Bet command is only for initialization. Use check/call/raise/fold command!\n");
                        break;
                }

            
            players[0].bet = userBet;
            table->minBet = players[0].bet;

            table->pot = players[0].bet;
            players[0].points = players[0].points - userBet;
 
            game.actioncounter += 1;

	    snprintf(SendBuf, sizeof(SendBuf), "Small Blind bet is %d", userBet);

            // Process the bet
            // You can add more code here to handle the bet
     
        }

	else if(strncmp(RecvBuf, "BET 2 ", 6) == 0)
	{
		if(game.currRound <= 2)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing the game commands!\n");
                        break;
                }

		int userBet;

		sscanf(RecvBuf + 6, "%d", &userBet);
		if(userBet < 2*(table->minBet) && game.currRound == PREFLOP)
		{
			snprintf(SendBuf, sizeof(SendBuf), "Big Blind must bet equal or greater than twice the bet of small blind! Reinitialize your bet\n");
			break;
		}

		if(game.currRound > PREFLOP)
		{
			snprintf(SendBuf, sizeof(SendBuf), "Bet command is only for initialization. Use check/call/raise/fold command!\n");
			break;
		}

		players[1].bet = userBet;
            	table->minBet = players[1].bet;

            	table->pot = players[1].bet;
           	players[1].points = players[1].points - userBet;

                game.actioncounter += 1;
		snprintf(SendBuf, sizeof(SendBuf), "Big Blind bet is %d", userBet);
	}

	else if(strncmp(RecvBuf, "CHECK ", 6) == 0)
	{

		if(game.currRound <= 2)
		{
			snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing the game commands!\n");
			break;
		}

		if(game.currRound == 3)
		{
			snprintf(SendBuf, sizeof(SendBuf), "You can't check in PREFLOP round\n");
			break;
		}


		if(table->minBet != table->prevminBet)
		{
			snprintf(SendBuf, sizeof(SendBuf), "You can't check after previous player raised!\n");
			break;
		}

		int seat;
		table -> minBet = 0;
		sscanf(RecvBuf + 6, "%d", &seat);
		switch(seat)
		{
			case 1:
				table->prevminBet = table-> minBet;
				players[0].bet = table->minBet;
				players[0].points -= players[0].bet;
				table->pot += players[0].bet;
				game.actioncounter += 1;
				snprintf(SendBuf, sizeof(SendBuf), "Player %s has checked. Current Pot: %d\n", players[0].username, table->pot);
				break;
			
			case 2:
				table->prevminBet = table-> minBet;
                                players[1].bet = table->minBet;
                                players[1].points -= players[1].bet;
                                table->pot += players[1].bet;
				game.actioncounter += 1;
                                snprintf(SendBuf, sizeof(SendBuf), "Player %s has checked. Current Pot: %d\n", players[1].username, table->pot);
                                break;

			case 3:
				table->prevminBet = table-> minBet;
                                players[2].bet = table->minBet;
                                players[2].points -= players[2].bet;
                                table->pot += players[2].bet;
				game.actioncounter += 1;
                                snprintf(SendBuf, sizeof(SendBuf), "Player %s has checked. Current Pot: %d\n", players[2].username, table->pot);
                                break;

			case 4:
				table->prevminBet = table-> minBet;
                                players[3].bet = table->minBet;
                                players[3].points -= players[3].bet;
                                table->pot += players[3].bet;
				game.actioncounter += 1;
                                snprintf(SendBuf, sizeof(SendBuf), "Player %s has checked. Current Pot: %d\n", players[3].username, table->pot);
                                break;

			default:
				snprintf(SendBuf, sizeof(SendBuf), "Invalid seat number! Please type valid seat number\n");
		}
	}					
	else if(strncmp(RecvBuf, "CALL ", 5) == 0){

        if(game.currRound <= 2)
        {
                snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing game commands!\n");
                break;
        }

        int seat;
        sscanf(RecvBuf + 5, "%d", &seat);

	if(game.currRound == PREFLOP && (seat == 1 || seat == 2))
	{
		snprintf(SendBuf, sizeof(SendBuf), "Small and Big blind can't call at PREFLOP round please initialize your bet!\n");
		break;
	}

        switch(seat){

	    case 1:	
		table->prevminBet = table->minBet;
                players[0].bet = table->minBet;
                players[0].points -= players[0].bet;
                table->pot += players[0].bet;

                snprintf(SendBuf, sizeof(SendBuf), "Player %s has called. %s's remaining balance is %d. Current Pot: %d\n", players[0].username, players[0].username, players[0].points, table->pot);
                printf("Player 1 called.\n");

                game.actioncounter += 1;

                break;

            case 2:
		table->prevminBet = table->minBet;
                players[1].bet = table->minBet;
                players[1].points -= players[1].bet;
                table->pot += players[1].bet;

                snprintf(SendBuf, sizeof(SendBuf), "Player %s has called. %s's remaining balance is %d. Current Pot: %d\n", players[1].username, players[1].username, players[1].points, table->pot);
                printf("Player 2 called.\n");

                game.actioncounter += 1;

                break;
            case 3:
		table->prevminBet = table->minBet;
                players[2].bet = table->minBet;
                players[2].points -= players[2].bet;
                table->pot += players[2].bet;

                snprintf(SendBuf, sizeof(SendBuf), "Player %s has called. %s's remaining balance is %d. Current Pot: %d\n", players[2].username, players[2].username, players[2].points, table->pot);
                printf("Player 3 called.\n");

                game.actioncounter += 1;
                break;
            case 4:
		table->prevminBet = table->minBet;
                players[3].bet = table->minBet;
                players[3].points -= players[3].bet;
                table->pot += players[3].bet;

                snprintf(SendBuf, sizeof(SendBuf), "Player %s has called. %s's remaining balance is %d. Current Pot: %d\n", players[3].username, players[3].username, players[3].points, table->pot);
                printf("Player 4 called.\n");

                game.actioncounter += 1;

                break;
            default:
		snprintf(SendBuf, sizeof(SendBuf), "Invalid seat number! Please type valid seat number\n");
                break;

        }
    }

	else if(strncmp(RecvBuf, "ALLIN ", 6) == 0){

        if(game.currRound <= 2)
        {
                snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing game commands!\n");
                break;
        }

        int seat;
        sscanf(RecvBuf + 6, "%d", &seat);

        if(game.currRound == PREFLOP && (seat == 1 || seat == 2))
        {
                snprintf(SendBuf, sizeof(SendBuf), "Small and Big blind can't call at PREFLOP round please initialize your bet!\n");
                break;
        }

        switch(seat){

            case 1:
                table->prevminBet = table->minBet;
                players[0].bet = players[0].points;
                players[0].points -= players[0].bet;
                table->pot += players[0].bet;

                snprintf(SendBuf, sizeof(SendBuf), "Player %s has All-in. %s's remaining balance is 0. Current Pot: %d\n", players[0].username, players[0].username, table->pot);
                printf("Player 1 All-in.\n");

                game.actioncounter += 1;

                break;

            case 2:
                table->prevminBet = table->minBet;
                players[1].bet = players[1].points;
                players[1].points -= players[1].bet;
                table->pot += players[1].bet;

                snprintf(SendBuf, sizeof(SendBuf), "Player %s has All-in. %s's remaining balance is 0. Current Pot: %d\n", players[1].username, players[1].username, table->pot);
                printf("Player 2 All-in.\n");

                game.actioncounter += 1;

                break;
            case 3:
                table->prevminBet = table->minBet;
                players[2].bet = players[2].points;
                players[2].points -= players[2].bet;
                table->pot += players[2].bet;

                snprintf(SendBuf, sizeof(SendBuf), "Player %s has All-in. %s's remaining balance is 0. Current Pot: %d\n", players[2].username, players[2].username, table->pot);
                printf("Player 3 All-in.\n");

                game.actioncounter += 1;
                break;
            case 4:
                table->prevminBet = table->minBet;
                players[3].bet = players[3].points;
                players[3].points -= players[3].bet;
                table->pot += players[3].bet;

                snprintf(SendBuf, sizeof(SendBuf), "Player %s has All-in. %s's remaining balance is 0. Current Pot: %d\n", players[3].username, players[3].username, table->pot);
                printf("Player 4 All-in.\n");

                game.actioncounter += 1;

                break;
            default:
                snprintf(SendBuf, sizeof(SendBuf), "Invalid seat number! Please type valid seat number\n");
                break;

        }
    }

       

	else if(strncmp(RecvBuf, "RAISE ", 5) == 0){

        if(game.currRound <= 2)
        {
                snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing game commands!\n");
                break;
        }

        int seat;
        int raiseAmt;

        sscanf(RecvBuf + 6, "%d", &seat);
        printf("seat is %d\n", seat);
        sscanf(RecvBuf + 8, "%d", &raiseAmt);

        if(game.currRound == PREFLOP && (seat == 1 || seat == 2))
        {
                snprintf(SendBuf, sizeof(SendBuf), "Small and Big blind can only raise starting from flop round\n");
                break;
        }

	if(players[seat].points < (raiseAmt + table->minBet))
	{
		snprintf(SendBuf, sizeof(SendBuf), "You don't have enough points to raise! Please select other option\n");
		break;
	}

        switch(seat){
                 case 1:
			table->prevminBet = table->minBet;
                        players[0].bet = table->minBet + raiseAmt;
                        players[0].points -= players[0].bet;
                        table->minBet = players[0].bet;
                        table->pot += players[0].bet;

                        snprintf(SendBuf, sizeof(SendBuf), "Player 1 has raised by %d", raiseAmt );
                        printf("Player 1 raised by %d.\n", raiseAmt);

                        game.actioncounter += 1;
                        break;



                case 2:
			table->prevminBet = table->minBet;
                        players[1].bet = table->minBet + raiseAmt;
                        players[1].points -= players[1].bet;
                        table->minBet = players[1].bet;
                        table->pot += players[1].bet;

                        snprintf(SendBuf, sizeof(SendBuf), "Player 2 has raised by %d", raiseAmt );
                        printf("Player 2 raised by %d.\n", raiseAmt);

                        game.actioncounter += 1;
                        break;

                case 3:
			table->prevminBet = table->minBet;
                        players[2].bet = table->minBet + raiseAmt;
                        players[2].points -= players[2].bet;
                        table->minBet = players[2].bet;
                        table->pot += players[2].bet;

                        snprintf(SendBuf, sizeof(SendBuf), "Player 3 has raised by %d", raiseAmt );
                        printf("Player 3 raised by %d.\n", raiseAmt);

                        game.actioncounter += 1;
                        break;

                case 4:
			table->prevminBet = table->minBet;
                        players[3].bet = table->minBet + raiseAmt;
                        players[3].points -= players[3].bet;
                        table->minBet = players[3].bet;
                        table->pot += players[3].bet;

                        snprintf(SendBuf, sizeof(SendBuf), "Player 4 has raised by %d", raiseAmt );
                        printf("Player 4 raised by %d.\n", raiseAmt);

                        game.actioncounter += 1;
                        break;

                default:
                        snprintf(SendBuf, sizeof(SendBuf), "Invalid amount. Please type correct value\n");

         }

   }
 
        
	
	else if(strncmp(RecvBuf, "FOLD ", 5) == 0){

        if(game.currRound <= 2)
        {
                snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing game commands!\n");
                break;
        }

        int seat;
        sscanf(RecvBuf + 5, "%d", &seat);

        if(game.currRound == PREFLOP && (seat == 1 || seat == 2))
        {
                snprintf(SendBuf, sizeof(SendBuf), "Small and Big blind can only fold starting from flop round\n");
                break;
        }

        switch(seat){

            case 1:
                players[0].fold = 1;
                snprintf(SendBuf, sizeof(SendBuf), "Player %s has folded. Player %s's turn.\n", players[0].username, players[1].username);
                printf("Player 1 folded.\n");
                game.actioncounter += 1;
                break;

            case 2:
                players[1].fold = 1;
                snprintf(SendBuf, sizeof(SendBuf), "%s has folded. %s's turn.", players[1].username, players[2].username);
                printf("Player 2 folded.\n");
                game.actioncounter += 1;
                break;

            case 3:
                players[2].fold = 1;
                snprintf(SendBuf, sizeof(SendBuf), "%s has folded. %s's turn.", players[2].username, players[3].username);
                printf("Player 2 folded.\n");
                game.actioncounter += 1;
                break;

            case 4:
                players[3].fold = 1;

                snprintf(SendBuf, sizeof(SendBuf), "%s has folded.", players[3].username);
                printf("Player 4 folded.\n");
                game.actioncounter += 1;
                break;

            default:
		snprintf(SendBuf, sizeof(SendBuf), "Invalid seat number! Please type valid seat number\n");
                break;
        }

    }

        else if(0 == strncmp(RecvBuf, "WINNER", 6)){

	    if(game.currRound != SHOWDOWN)
	    {
		snprintf(SendBuf, sizeof(SendBuf), "Please type WINNER in the final round\n");
		break;
	    }


	    int p1hand;
	    int p2hand;
    	    int p3hand;
    	    int p4hand;

    	    int bestplayerHand;
    	    int bestPlayer;


    	    for(int i = 0; i < 2; i++)
    	    {
        	p1cmp[i] = players[0].card[i];
    	    }

    	    for(int i = 0; i < 5; i++)
	    {
        	p1cmp[i + 2] = table->communitycards[i];
    	    }

    	    for(int i = 0; i < 2; i++)
    	    {
       		p2cmp[i] = players[1].card[i];
    	    }

    	    for(int i = 0; i < 5; i++)
	    {
        	p2cmp[i + 2] = table->communitycards[i];
    	    }


    	    for(int i = 0; i < 2; i++)
    	    {
       		p3cmp[i] = players[2].card[i];
    	    }

    	    for(int i = 0; i < 5; i++){
        	p3cmp[i + 2] = table->communitycards[i];
    	    }


    	    for(int i = 0; i < 2; i++)
    	    {
        	p4cmp[i] = players[3].card[i];
    	    }

    	    for(int i = 0; i < 5; i++)
    	    {
            	p4cmp[i + 2] = table->communitycards[i];
    	    }

            p1hand = bestHand(p1cmp, 1);
            p2hand = bestHand(p2cmp, 2);
            p3hand = bestHand(p3cmp, 3);
            p4hand = bestHand(p3cmp, 4);

            int hands[4] = {p1hand, p2hand, p3hand, p4hand};
	    int betterplayer = 0;
            bestPlayer = 0;
            bestplayerHand = hands[0]; 
            for(int i = 0; i < 4; i++){
                if(hands[i] > bestplayerHand){
                    bestplayerHand = hands[i];
                    bestPlayer = i + 1;

                }
		else if(hands[i] == 10 && bestplayerHand == 10)
		{
			snprintf(SendBuf, sizeof(SendBuf), "Player %s and Player %s are tie. Pots are splitted\n", players[bestPlayer].username, players[i].username);
			players[bestPlayer].points += (table -> pot) / 2;
			players[i].points += (table -> pot) / 2;
			table -> pot = 0;
			break;
		}

		else if(hands[i] == bestplayerHand)
		{
			betterplayer = tiecondition(bestplayerHand, i);
			
			if(betterplayer == i)
			{
				bestplayerHand = hands[i];
				bestPlayer = i + 1;
			} 
           	}

            }
           
	    players[bestPlayer-1].points += table -> pot;
	    printf("bestplayer: %d\n", bestPlayer-1);
            snprintf(SendBuf, sizeof(SendBuf), "The player with the best hand is player %s!", players[bestPlayer-1].username);             
            printf("%d - %d - %d - %d\n", p1hand, p2hand, p3hand, p4hand);

        }
        
	else if(strncmp(RecvBuf, "STATUS ", 7) == 0)
        {
                if(game.currRound <= 2)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing game commands!\n");
                        break;
                }

                int seat;
                sscanf(RecvBuf + 7, "%d", &seat);

                switch(seat){

                case 1:
                        snprintf(SendBuf, sizeof(SendBuf), "Player %s's status:\n\n Player %s has %d points.\n", players[0].username, players[0].username, players[0].points);
                        printf("Player 1 typed status.\n");
                        break;

                case 2:
                        snprintf(SendBuf, sizeof(SendBuf), "Player %s's status:\n\n Player %s has %d points.\n", players[1].username, players[1].username, players[1].points);
                        printf("Player 2 typed status.\n");
                        break;

                case 3:
                         snprintf(SendBuf, sizeof(SendBuf), "Player %s's status:\n\n Player %s has %d points.\n", players[2].username, players[2].username, players[2].points);
                        printf("Player 1 typed status.\n");
                        break;

                case 4:
                         snprintf(SendBuf, sizeof(SendBuf), "Player %s's status:\n\n Player %s has %d points.\n", players[3].username, players[3].username, players[3].points);
                        printf("Player 1 typed status.\n");
                        break;


                default:
                        break;

                }
        }

        else if(strncmp(RecvBuf, "OTHER", 5) == 0)
        {
                if(game.currRound <= 2)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "Please START the game before typing game commands!\n");
                        break;
                }

                snprintf(SendBuf, sizeof(SendBuf), "STATUS OF ALL PLAYERS:\n\n Player %s has %d points.\n Player %s has %d points.\n Player %s has %d points.\n Player %s has %d points.\n", players[0].username, players[0].points, players[1].username, players[1].points, players[2].username, players[2].points, players[3].username, players[3].points);
        }

        else if(game.actioncounter == 16 && game.currRound == RIVER && 0 == strncmp(RecvBuf, "NEXT", 4)){
            snprintf(SendBuf, sizeof(SendBuf), "TYPE 'CHECK' TO REVEAL ALL BEST HANDS AND REVEAL WINNER");
            game.currRound = SHOWDOWN;
        }

        else if(strncmp(RecvBuf, "POT", 3) == 0)
        {
                snprintf(SendBuf, sizeof(SendBuf), "\nCurrent POT on the table is %d\n", table->pot);
        }

        
        else if(strncmp(RecvBuf, "NEWGAME", 7) == 0)
	{
		if(game.currRound != SHOWDOWN)
		{
			snprintf(SendBuf, sizeof(SendBuf), "You can only restart the game once game is finished!\n");
			break;
		}
		snprintf(SendBuf, sizeof(SendBuf), "New game starts! Cards are re-distributed\n");
		game.currRound = PREFLOP;
		table->deck = ShuffleDeck(&(table->deck));
		table->minBet = 0;
		table->prevminBet = 0;
		table->pot = 0;
		table->currentBet = 0;


		for(int r = 0; r < 4; r++)
    		{
        		for(int s = 0; s < 7; s++)
        		{
                		players[r].combination[s].rank = 0;
        		}
    		}
		
	}


	else if(strncmp(RecvBuf, "SCOREBOARD", 10) == 0)
	{	
		if(game.currRound < SHOWDOWN)
		{
			snprintf(SendBuf, sizeof(SendBuf), "You can only end the game once game is finished!\n");
			break;
		}

		for (int i = 0; i < 4; i++)
		{
        		for (int j = 0; j < 3- i; j++)
			{
            			if (players[j].points < players[j + 1].points)
				{
                                	PLAYER temp = players[j];
                                        players[j] = players[j + 1];
                                        players[j + 1] = temp;
				}
			}
		}
		snprintf(SendBuf, sizeof(SendBuf), "Score Board:\n First place: %s with score of %d\n Second Place: %s with score of %d\n Third Place: %s with score of %d\n Last place: %s with score of %d\n\n", players[0].username, players[0].points, players[1].username, players[1].points, players[2].username, players[2].points, players[3].username, players[3].points);
	}



	else if(strncmp(RecvBuf, "ZOT ", 3) == 0)
        {
                if(game.currRound > READY && game.currRound < SHOWDOWN)
                {
                        snprintf(SendBuf, sizeof(SendBuf), "You can only refill your account before starting the game or at the end of each game!\n");
                        break;
                }
		int seat;
        	int withdrawAmt;

        	sscanf(RecvBuf + 4, "%d", &seat);
        	sscanf(RecvBuf + 6, "%d", &withdrawAmt);

                switch(seat){

                case 1:
			players[0].points += withdrawAmt;
                        snprintf(SendBuf, sizeof(SendBuf), "Player %s has deposited %d\n Player %s has %d points.\n", players[0].username, withdrawAmt, players[0].username, players[0].points);
                        break;

                case 2:
			players[1].points += withdrawAmt;	
                        snprintf(SendBuf, sizeof(SendBuf), "Player %s has deposited %d\n Player %s has %d points.\n", players[1].username, withdrawAmt, players[1].username, players[1].points);
                        break;

                case 3:
			players[2].points += withdrawAmt;
                        snprintf(SendBuf, sizeof(SendBuf), "Player %s has deposited %d\n Player %s has %d points.\n", players[2].username, withdrawAmt, players[2].username, players[2].points);
                        break;

                case 4:
			players[3].points += withdrawAmt;
                        snprintf(SendBuf, sizeof(SendBuf), "Player %s has deposited %d\n Player %s has %d points.\n", players[3].username, withdrawAmt, players[3].username, players[3].points);
                        break;


                default:
                        break;

                }
        }


    /*original code starts*/
    else if (strcmp(RecvBuf, "TIME") == 0) {
        snprintf(SendBuf, sizeof(SendBuf), "OK TIME: %s", ClockBuffer);
    } else if (strcmp(RecvBuf, "SHUTDOWN") == 0) {
        Shutdown = 1;
        snprintf(SendBuf, sizeof(SendBuf), "OK SHUTDOWN");
    } else {
        snprintf(SendBuf, sizeof(SendBuf), "ERROR unknown command: %s", RecvBuf);
    }

    if(game.actioncounter == 4 && game.currRound == PREFLOP)
    {
    	snprintf(SendBuf, sizeof(SendBuf), "FLOP round, Community cards are open, Please type CC to view community cards\n");
    	printf("flop stage");
    	game.currRound = FLOP;
    }


    if(game.actioncounter == 4 && game.currRound == PREFLOP)
    {
        snprintf(SendBuf, sizeof(SendBuf), "FLOP round, Community cards are open, Please type CC to view community cards\n");
        printf("flop stage\n");
        game.currRound = FLOP;
    }

    if(game.actioncounter == 8 && game.currRound == FLOP)
    {
        snprintf(SendBuf, sizeof(SendBuf), "TURN round, New community card is open, Please type CC to view community cards\n");
        printf("Turn stage\n");
        game.currRound = TURN;
    }

    if(game.actioncounter == 12 && game.currRound == TURN)
    {
        snprintf(SendBuf, sizeof(SendBuf), "RIVER round, All community cards are open, Please type CC to view community cards\n");
        printf("River stage\n");
        game.currRound = RIVER;
    }

    if(game.actioncounter == 16 && game.currRound == RIVER)
    {
        snprintf(SendBuf, sizeof(SendBuf), "Game ended. All players reveal their cards and winner gets the pot. TYPE 'WINNER' to evaluate your cards\n");
        printf("SHOWDOWN stage\n");
        game.currRound = SHOWDOWN;
    }

    
    game.roundCounter = 11;
    }
    printf("Player 1's username is set to: %s\n", players[0].username);
    printf("total players %d\n", game.totalPlayers);
    printf("MINE counter: %d\n", players[0].mineCheck);
    printf("The curr pot is: %d\n", table->pot);
    l = strlen(SendBuf);
    n = write(DataSocketFD, SendBuf, l);
    if (n < 0) {
        FatalError("writing to data socket failed");
    }
}

void ServerMainLoop(		/* simple server main loop */
	int ServSocketFD,		/* server socket to wait on */
	int Timeout)			/* timeout in micro seconds */
{
    int DataSocketFD;	/* socket for a new client */
    socklen_t ClientLen;
    struct sockaddr_in
	ClientAddress;	/* client address we connect with */
    fd_set ActiveFDs;	/* socket file descriptors to select from */
    fd_set ReadFDs;	/* socket file descriptors ready to read from */
    struct timeval TimeVal;
    int res, i;

    FD_ZERO(&ActiveFDs);		/* set of active sockets */
    FD_SET(ServSocketFD, &ActiveFDs);	/* server socket is active */


    TABLE table = inittable(); /* Creates table */
    table.minBet = 0;
    
   for(int r = 0; r < 4; r++)
    {
	for(int s = 0; s < 7; s++)
	{
		players[r].combination[s].rank = 0;
	}
    } 

    while(!Shutdown)
    {   ReadFDs = ActiveFDs;
	TimeVal.tv_sec  = Timeout / 1000000;	/* seconds */
	TimeVal.tv_usec = Timeout % 1000000;	/* microseconds */
	/* block until input arrives on active sockets or until timeout */
	res = select(FD_SETSIZE, &ReadFDs, NULL, NULL, &TimeVal);
	if (res < 0)
	{   FatalError("wait for input or timeout (select) failed");
	}
	if (res == 0)	/* timeout occurred */
	{
#ifdef DEBUG
	    printf("%s: Handling timeout...\n", Program);
#endif
	    PrintCurrentTime();
	}
	else		/* some FDs have data ready to read */
	{   for(i=0; i<FD_SETSIZE; i++)
	    {   if (FD_ISSET(i, &ReadFDs))
		{   if (i == ServSocketFD)
		    {	/* connection request on server socket */
#ifdef DEBUG
			printf("%s: Accepting new client %d...\n", Program, i);
#endif
			ClientLen = sizeof(ClientAddress);
			DataSocketFD = accept(ServSocketFD,
				(struct sockaddr*)&ClientAddress, &ClientLen);
			if (DataSocketFD < 0)
			{   FatalError("data socket creation (accept) failed");
			}
#ifdef DEBUG
			printf("%s: Client %d connected from %s:%hu.\n",
				Program, i,
				inet_ntoa(ClientAddress.sin_addr),
				ntohs(ClientAddress.sin_port));
#endif
			FD_SET(DataSocketFD, &ActiveFDs);
		    }
		    else
		    {   /* active communication with a client */
#ifdef DEBUG
			printf("%s: Dealing with client %d...\n", Program, i);
#endif
			ProcessRequest(DataSocketFD, &table);
#ifdef DEBUG
			printf("%s: Closing client %d connection.\n", Program, i);
#endif
			close(i);
			FD_CLR(i, &ActiveFDs);
		    }
		}
	    }
	}
    }
} /* end of ServerMainLoop */

/*** main function *******************************************************/

int main(int argc, char *argv[])
{
    int ServSocketFD;	/* socket file descriptor for service */
    int PortNo;		/* port number */

    Program = argv[0];	/* publish program name (for diagnostics) */
#ifdef DEBUG
    printf("%s: Starting...\n", Program);
#endif
    if (argc < 2)
    {   fprintf(stderr, "Usage: %s port\n", Program);
	exit(10);
    }
    PortNo = atoi(argv[1]);	/* get the port number */
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program, PortNo);
        exit(10);
    }
#ifdef DEBUG
    printf("%s: Creating the server socket...\n", Program);
#endif
    ServSocketFD = MakeServerSocket(PortNo);
    printf("%s: Providing current time at port %d...\n", Program, PortNo);
    ServerMainLoop(ServSocketFD, 250000);
    printf("\n%s: Shutting down.\n", Program);
    close(ServSocketFD);
    return 0;
}

/* EOF ClockServer.c */
 
