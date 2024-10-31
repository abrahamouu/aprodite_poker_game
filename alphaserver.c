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

/*** global variables ****************************************************/

const char *Program = NULL;
int Shutdown = 0;
char ClockBuffer[26] = "";

/*** global functions ****************************************************/
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
    int currentBet;
    int totalActivePlayers; /*players who haven't folded*/
    CARD communitycards[5];

}TABLE;


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

// int cmpCard(CARD player1flop[5]){
//     int temp;
//     for(int i = 0; i < 5; i++){
//         for(int j = i + 1; j < 5; j++)
//         if(player1flop[i].rank == player1flop[j].rank){
//             return player1flop[i].rank;
//         }

//     }
//     return 0;

// }

// int checkOnePair(CARD player1flop[5]) {
//     int rankForPair;
//     for (int i = 0; i < 4; i++) {
//         rankForPair = player1flop[i].rank;
//         for (int j = i + 1; j < 5; j++) {
//             if (player1flop[j].rank == rankForPair) {
//                 return 1;
//             }
//         }
//     }
//     return 0;
// }
int checkOnePair(CARD cards[7]) {
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

int checkTwoPair(CARD cards[7]) {
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

int checkThreeOfAKind(CARD cards[7]) {
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

int checkStraight(CARD cards[7]) {
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

int checkFlush(CARD cards[7]) {
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

int checkFullHouse(CARD cards[7]) {
    if (checkThreeOfAKind(cards) && checkTwoPair(cards)) {
        return 1;
    }
    else {
		return 0;
	}
}

int checkFourOfAKind(CARD cards[7]) {
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

int checkStraightFlush(CARD cards[7]) {
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

int checkRoyalFlush(CARD cards[7]) {
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

int bestHand(CARD cards[7]){
    if (checkRoyalFlush(cards)) {
        return 10; // Royal Flush
    } 
    else if (checkStraightFlush(cards)) {
        return 9; // Straight Flush
    } 
    else if (checkFourOfAKind(cards)) {
        return 8; // Four of a Kind
    } 
    else if (checkFullHouse(cards)) {
        return 7; // Full House
    } 
    else if (checkFlush(cards)) {
        return 6; // Flush
    } 
    else if (checkStraight(cards)) {
        return 5; // Straight
    } 
    else if (checkThreeOfAKind(cards)) {
        return 4; // Three of a Kind
    } 
    else if (checkTwoPair(cards)) {
        return 3; // Two Pair
    } 
    else if (checkOnePair(cards)) {
        return 2; // One Pair
    } 
    else {
        return 1; // High Card
    }
    
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
    game.roundCounter = 1;
    n = read(DataSocketFD, RecvBuf, sizeof(RecvBuf) - 1);
    if (n < 0) {
        FatalError("reading from data socket failed");
    }
    RecvBuf[n] = 0;
    int p1hand;
    int p2hand;
    int p3hand;
    int p4hand;
    
    int bestplayerHand;
    int bestPlayer;



    // for(int i = 0; i < 52; i++)`
    // {
    //     printf("Suit is %d and rank is %d\n", table->deck.cards[i].suit, table->deck.cards[i].rank);
    // }

    for(int i = 0; i < 2; i++)
    {
        p1cmp[i] = players[0].card[i];
    }

    for(int i = 0; i < 5; i++){
        p1cmp[i + 2] = table->communitycards[i];
    }

    // for(int i = 0; i < 7; i ++){
    //     printf("PLAYER 1 Card %d, Rank: %d, Suit: %d\n", i+1, p1cmp[i].rank, p1cmp[i].suit);
    // }


    for(int i = 0; i < 2; i++)
    {
        p2cmp[i] = players[1].card[i];
    }

    for(int i = 0; i < 5; i++){
        p2cmp[i + 2] = table->communitycards[i];
    }

    // for(int i = 0; i < 7; i ++){
    //     printf("PLAYER 2 Card %d, Rank: %d, Suit: %d\n", i+1, p2cmp[i].rank, p2cmp[i].suit);
    // }

    for(int i = 0; i < 2; i++)
    {
        p3cmp[i] = players[2].card[i];
    }

    for(int i = 0; i < 5; i++){
        p3cmp[i + 2] = table->communitycards[i];
    }

    // for(int i = 0; i < 7; i ++){
    //     printf("PLAYER 3 Card %d, Rank: %d, Suit: %d\n", i+1, p3cmp[i].rank, p3cmp[i].suit);
    // }

    for(int i = 0; i < 2; i++)
        {
            p4cmp[i] = players[3].card[i];
        }

        for(int i = 0; i < 5; i++){
            p4cmp[i + 2] = table->communitycards[i];
        }

        // for(int i = 0; i < 7; i ++){
        //     printf("PLAYER 4 Card %d, Rank: %d, Suit: %d\n", i+1, p4cmp[i].rank, p4cmp[i].suit);
        // }
            
            



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
                snprintf(SendBuf, sizeof(SendBuf), "Player%d username is set to: %s", seat + 1, players[seat].username);
                game.currRound = READY;
                break;
            }
        }
        if (seat > MAX_PLAYERS) 
        {
            snprintf(SendBuf, sizeof(SendBuf), "ERROR no available seat to set username");
        }
    }

    else if(game.totalPlayers == 4 && game.currRound == READY && (strncmp(RecvBuf, "START", 5) == 0))
    {
        snprintf(SendBuf, sizeof(SendBuf), "ALL PLAYERS READY. DEALING YOUR CARDS. VIEW YOUR CARDS. %s you are small blind. Make your initial bet", players[0].username);
        printf("game is ready to start");
        game.roundBegin = 1;
        
        game.currRound = PREFLOP;
        for (int i = 0; i < MAX_PLAYERS; i++){
            players[i].points = 1000;
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
                    break;
            }

        }
        
          
        else if(strncmp(RecvBuf, "BET 1 ", 6) == 0 && game.currRound == PREFLOP){
            snprintf(SendBuf,sizeof(SendBuf),"THE CURRENT ROUND IS %d\n", game.currRound);
            snprintf(SendBuf, sizeof(SendBuf), "%s starts first. Your balance is %d.\nMake your initial bet.", players[0].username, players[0].points);
            int userBet;
            
            
            sscanf(RecvBuf + 6, "%d", &userBet);

            players[0].bet = userBet;
            table->minBet = players[0].bet;

            printf("Player 1 bet: %d\n", players[0].bet);
            table->minBet = players[0].bet; /*Minimum bet of table is set*/
            table->pot = players[0].bet;
            players[0].points = players[0].points - userBet;


            printf("Minimum bet is %d", table->minBet);

            snprintf(SendBuf, sizeof(SendBuf), "%s has bet %d. Your remaining balance is %d. %s's turn. Your balance is %d.\n'CALL seat_number' to make the same bet \n'FOLD seat_number' to fold ",players[0].username, players[0].bet, players[0].points, players[1].username, players[1].points);
            
            printf("bet done\n");
            game.actioncounter += 1;
            // Process the bet
            // You can add more code here to handle the bet
     
        }
        else if(strncmp(RecvBuf, "CALL 2", 6) == 0 && game.currRound == PREFLOP){
            
            printf("player2 calls\n");
            
            players[1].bet = table->minBet;
            players[1].points = players[1].points - players[1].bet;
            snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d.\n %s type 'CALL' 'FOLD' or 'RAISE'.", players[1].username, players[1].username, players[1].points, players[2].username);
            printf("player2 has made their bet\n");
            table->pot += players[1].bet;
            game.actioncounter += 1;
            

            



        }

        else if(strncmp(RecvBuf, "FOLD 2", 6) == 0 && game.currRound == PREFLOP){
            printf("player2 folds\n");
            players[1].fold = 1; /*player2 folded*/
            snprintf(SendBuf, sizeof(SendBuf), "%s has folded. %s's turn. type 'CALL' 'FOLD' or 'RAISE'", players[1].username, players[2].username);
            game.actioncounter += 1;


        }

        else if(strncmp(RecvBuf, "RAISE 2 ", 8) == 0 && game.currRound == PREFLOP){
            int raiseAmt;
            sscanf(RecvBuf + 8, "%d", &raiseAmt);
            players[1].bet = table->minBet + raiseAmt;

            players[1].points -= players[1].bet;
            table->minBet = players[1].bet; /*new raised amount is now minbet*/

            table->pot += players[1].bet;
            snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. %s remaining balance is %d. %s's turn. type 'CALL' 'FOLD' or 'RAISE' Current Pot: %d", players[1].username, raiseAmt,players[1].username, players[1].points, players[2].username, table->pot);
            game.actioncounter += 1;

        }

        else if(strncmp(RecvBuf, "CALL 3", 6) == 0 && game.currRound == PREFLOP){
            
            printf("player3 calls\n");
            players[2].bet = table->minBet;
            players[2].points -= players[2].bet;
            table->pot += players[2].bet;
            snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d.\n %s type 'CALL' 'FOLD' or 'RAISE'. Current Pot: %d", players[2].username, players[2].username, players[2].points, players[3].username, table->pot);
            printf("player3 has made their bet\n");
            
            game.actioncounter += 1;
            



        }

        else if(strncmp(RecvBuf, "FOLD 3", 6) == 0 && game.currRound == PREFLOP){
            printf("player3 folds\n");
            players[2].fold = 1; /*player2 folded*/
            snprintf(SendBuf, sizeof(SendBuf), "%s has folded. %s's turn. type 'CALL' 'FOLD' or 'RAISE'", players[2].username, players[3].username);
            game.actioncounter += 1;

        }

        else if(strncmp(RecvBuf, "RAISE 3 ", 8) == 0 && game.currRound == PREFLOP){
            int raiseAmt;
            sscanf(RecvBuf + 8, "%d", &raiseAmt);
            players[2].bet = table->minBet + raiseAmt;

            players[2].points -= players[2].bet;
            table->minBet = players[2].bet; /*new raised amount is now minbet*/

            table->pot += players[2].bet;
            snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. %s remaining balance is %d. %s's turn. type 'CALL' 'FOLD' or 'RAISE' Current Pot: %d", players[2].username, raiseAmt,players[2].username, players[2].points, players[3].username, table->pot);
            game.actioncounter += 1;

        }

        else if(strncmp(RecvBuf, "CALL 4", 6) == 0 && game.currRound == PREFLOP){
            
            printf("player4 calls\n");
            players[3].bet = table->minBet;
            players[3].points -= players[3].bet;
            table->pot += players[3].bet;
            snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d.\n Current Pot: %d", players[3].username, players[3].username, players[3].points, table->pot);
            printf("player4 has made their bet\n");
            
            game.actioncounter += 1;
            



        }

        else if(strncmp(RecvBuf, "FOLD 4", 6) == 0 && game.currRound == PREFLOP){
            printf("player4 folds\n");
            players[3].fold = 1; /*player2 folded*/
            snprintf(SendBuf, sizeof(SendBuf), "%s has folded.", players[3].username);
            game.actioncounter += 1;

        }

        else if(strncmp(RecvBuf, "RAISE 4 ", 8) == 0 && game.currRound == PREFLOP){
            int raiseAmt;
            sscanf(RecvBuf + 8, "%d", &raiseAmt);
            players[3].bet = table->minBet + raiseAmt;

            players[3].points -= players[3].bet;
            table->minBet = players[3].bet; /*new raised amount is now minbet*/

            table->pot += players[3].bet;
            snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. %s remaining balance is %d. Current Pot: %d", players[3].username, raiseAmt,players[3].username, players[3].points, table->pot);
            game.actioncounter += 1;

        }


        else if (strncmp(RecvBuf, "BET 1 ", 6) == 0 && game.currRound == FLOP) {
        int userBet;
        sscanf(RecvBuf + 6, "%d", &userBet);
        
        players[0].bet = userBet;
        players[0].points -= userBet;
        table->pot += userBet;
        table->minBet = players[0].bet;
        snprintf(SendBuf, sizeof(SendBuf), "%s has bet %d. Current Pot: %d. %s's turn.", players[0].username, userBet, table->pot, players[1].username);
        printf("Player 1 bet: %d\n", userBet);

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "CALL 2", 6) == 0 && game.currRound == FLOP) {
        players[1].bet = table->minBet;
        players[1].points -= players[1].bet;
        table->pot += players[1].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. %s's turn.", players[1].username, players[1].username, players[1].points, players[2].username);
        printf("Player 2 called.\n");

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "FOLD 2", 6) == 0 && game.currRound == FLOP) {
        players[1].fold = 1;

        snprintf(SendBuf, sizeof(SendBuf), "%s has folded. %s's turn.", players[1].username, players[2].username);
        printf("Player 2 folded.\n");

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "RAISE 2 ", 8) == 0 && game.currRound == FLOP) {
        int raiseAmt;
        sscanf(RecvBuf + 8, "%d", &raiseAmt);

        players[1].bet = table->minBet + raiseAmt;
        players[1].points -= players[1].bet;
        table->minBet = players[1].bet;
        table->pot += players[1].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. Current Pot: %d. %s's turn.", players[1].username, raiseAmt, table->pot, players[2].username);
        printf("Player 2 raised by %d.\n", raiseAmt);

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "CALL 3", 6) == 0 && game.currRound == FLOP) {
        players[2].bet = table->minBet;
        players[2].points -= players[2].bet;
        table->pot += players[2].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. %s's turn.", players[2].username, players[2].username, players[2].points, players[3].username);
        printf("Player 3 called.\n");

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "FOLD 3", 6) == 0 && game.currRound == FLOP) {
        players[2].fold = 1;

        snprintf(SendBuf, sizeof(SendBuf), "%s has folded. %s's turn.", players[2].username, players[3].username);
        printf("Player 3 folded.\n");

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "RAISE 3 ", 8) == 0 && game.currRound == FLOP) {
        int raiseAmt;
        sscanf(RecvBuf + 8, "%d", &raiseAmt);

        players[2].bet = table->minBet + raiseAmt;
        players[2].points -= players[2].bet;
        table->minBet = players[2].bet;
        table->pot += players[2].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. Current Pot: %d. %s's turn.", players[2].username, raiseAmt, table->pot, players[3].username);
        printf("Player 3 raised by %d.\n", raiseAmt);

        game.actioncounter += 1;} 
        else if (strncmp(RecvBuf, "CALL 4", 6) == 0 && game.currRound == FLOP) {
        players[3].bet = table->minBet;
        players[3].points -= players[3].bet;
        table->pot += players[3].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. Current Pot: %d", players[3].username, players[3].username, players[3].points, table->pot);
        printf("Player 4 called.\n");

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "FOLD 4", 6) == 0 && game.currRound == FLOP) {
        players[3].fold = 1;

        snprintf(SendBuf, sizeof(SendBuf), "%s has folded.", players[3].username);
        printf("Player 4 folded.\n");

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "RAISE 4 ", 8) == 0 && game.currRound == FLOP) {
        int raiseAmt;
        sscanf(RecvBuf + 8, "%d", &raiseAmt);

        players[3].bet = table->minBet + raiseAmt;
        players[3].points -= players[3].bet;
        table->minBet = players[3].bet;
        table->pot += players[3].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. Current Pot: %d.", players[3].username, raiseAmt, table->pot);
        printf("Player 4 raised by %d.\n", raiseAmt);

        game.actioncounter += 1;
        } 
        
        else if (game.actioncounter == 8 && game.currRound == FLOP && 0 == strncmp(RecvBuf, "NEXT", 4)) {
        // transition to turn
        snprintf(SendBuf, sizeof(SendBuf), "TURN STAGE. TYPE 'CC' TO VIEW UPDATE COMMUNITY CARDS.\n %s START BET AGAIN.", players[0].username);
        printf("Turn stage.\n");
        game.currRound = TURN;
    }

    else if (strncmp(RecvBuf, "BET 1 ", 6) == 0 && game.currRound == TURN) {
        int userBet;
        sscanf(RecvBuf + 6, "%d", &userBet);

        players[0].bet = userBet;
        players[0].points -= userBet;
        table->pot += userBet;
        table->minBet = players[0].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has bet %d. Current Pot: %d. %s's turn.", players[0].username, userBet, table->pot, players[1].username);
        printf("Player 1 bet: %d\n", userBet);

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "CALL 2", 6) == 0 && game.currRound == FLOP) {
        players[1].bet = table->minBet;
        players[1].points -= players[1].bet;
        table->pot += players[1].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. %s's turn.", players[1].username, players[1].username, players[1].points, players[2].username);
        printf("Player 2 called.\n");

        game.actioncounter += 1;
        }

    else if(strncmp(RecvBuf, "CALL ", 5) == 0 && game.currRound == TURN){
        int seat;
        sscanf(RecvBuf + 5, "%d", &seat);
        switch(seat){
            case 2:
                players[1].bet = table->minBet;
                players[1].points -= players[1].bet;
                table->pot += players[1].bet;

                snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. %s's turn. Current Pot: %d", players[1].username, players[1].username, players[1].points, players[2].username, table->pot);
                printf("Player 2 called.\n");

                game.actioncounter += 1;

                break;
            case 3:
                players[2].bet = table->minBet;
                players[2].points -= players[2].bet;
                table->pot += players[2].bet;

                snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. %s's turn. Current Pot: %d", players[2].username, players[2].username, players[2].points, players[3].username, table->pot);
                printf("Player 3 called.\n");

                game.actioncounter += 1;
                break;    
            case 4:
                players[3].bet = table->minBet;
                players[3].points -= players[3].bet;
                table->pot += players[3].bet;

                snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. Current Pot: %d", players[3].username, players[3].username, players[3].points, table->pot);
                printf("Player 4 called.\n");

                game.actioncounter += 1;

                break;
            default:
                break;

        }
    }

    else if(strncmp(RecvBuf, "FOLD ", 5) == 0 && game.currRound == TURN){
        int seat;
        sscanf(RecvBuf + 5, "%d", &seat);
        switch(seat){
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
                break;
            case 4:
                players[3].fold = 1;

                snprintf(SendBuf, sizeof(SendBuf), "%s has folded.", players[3].username);
                printf("Player 4 folded.\n");

                game.actioncounter += 1;
                break;
            default:
                break;
        }

    }

    else if (strncmp(RecvBuf, "RAISE 2 ", 8) == 0 && game.currRound == TURN) {
        int raiseAmt;
        sscanf(RecvBuf + 8, "%d", &raiseAmt);

        players[1].bet = table->minBet + raiseAmt;
        players[1].points -= players[1].bet;
        table->minBet = players[1].bet;
        table->pot += players[1].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. Current Pot: %d. %s's turn.", players[1].username, raiseAmt, table->pot, players[2].username);
        printf("Player 2 raised by %d.\n", raiseAmt);

        game.actioncounter += 1;
        }

         else if (strncmp(RecvBuf, "RAISE 3 ", 8) == 0 && game.currRound == TURN) {
        int raiseAmt;
        sscanf(RecvBuf + 8, "%d", &raiseAmt);

        players[2].bet = table->minBet + raiseAmt;
        players[2].points -= players[2].bet;
        table->minBet = players[2].bet;
        table->pot += players[2].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. Current Pot: %d. %s's turn.", players[2].username, raiseAmt, table->pot, players[3].username);
        printf("Player 3 raised by %d.\n", raiseAmt);

        game.actioncounter += 1;
        } 

        else if(strncmp(RecvBuf, "RAISE 4 ", 8) == 0 && game.currRound == TURN){
            int raiseAmt;
            sscanf(RecvBuf + 8, "%d", &raiseAmt);
            players[3].bet = table->minBet + raiseAmt;

            players[3].points -= players[3].bet;
            table->minBet = players[3].bet; /*new raised amount is now minbet*/

            table->pot += players[3].bet;
            snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. %s remaining balance is %d. Current Pot: %d", players[3].username, raiseAmt,players[3].username, players[3].points, table->pot);
            game.actioncounter += 1;

        }

        else if(game.actioncounter == 12 && game.currRound == TURN && 0 == strncmp(RecvBuf, "NEXT", 4)){
            snprintf(SendBuf, sizeof(SendBuf), "ALL COMMUNITY CARDS ARE NOW REVEALED. %s START FINAL BETS.", players[0].username);
            game.currRound = RIVER;
        }

        else if (strncmp(RecvBuf, "BET 1 ", 6) == 0 && game.currRound == RIVER) {
        int userBet;
        sscanf(RecvBuf + 6, "%d", &userBet);

        players[0].bet = userBet;
        players[0].points -= userBet;
        table->pot += userBet;
        table->minBet = players[0].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has bet %d. Current Pot: %d. %s's turn.", players[0].username, userBet, table->pot, players[1].username);
        printf("Player 1 bet: %d\n", userBet);

        game.actioncounter += 1;
        } 
        else if (strncmp(RecvBuf, "CALL 2", 6) == 0 && game.currRound == RIVER) {
        players[1].bet = table->minBet;
        players[1].points -= players[1].bet;
        table->pot += players[1].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. %s's turn.", players[1].username, players[1].username, players[1].points, players[2].username);
        printf("Player 2 called.\n");

        game.actioncounter += 1;
        }

    else if(strncmp(RecvBuf, "CALL ", 5) == 0 && game.currRound == RIVER){
        int seat;
        sscanf(RecvBuf + 5, "%d", &seat);
        switch(seat){
            case 2:
                players[1].bet = table->minBet;
                players[1].points -= players[1].bet;
                table->pot += players[1].bet;

                snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. %s's turn. Current Pot: %d", players[1].username, players[1].username, players[1].points, players[2].username, table->pot);
                printf("Player 2 called.\n");

                game.actioncounter += 1;

                break;
            case 3:
                players[2].bet = table->minBet;
                players[2].points -= players[2].bet;
                table->pot += players[2].bet;

                snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. %s's turn. Current Pot: %d", players[2].username, players[2].username, players[2].points, players[3].username, table->pot);
                printf("Player 3 called.\n");

                game.actioncounter += 1;
                break;    
            case 4:
                players[3].bet = table->minBet;
                players[3].points -= players[3].bet;
                table->pot += players[3].bet;

                snprintf(SendBuf, sizeof(SendBuf), "%s has called. %s's remaining balance is %d. Current Pot: %d", players[3].username, players[3].username, players[3].points, table->pot);
                printf("Player 4 called.\n");

                game.actioncounter += 1;

                break;
            default:
                break;

        }
    }

    else if(strncmp(RecvBuf, "FOLD ", 5) == 0 && game.currRound == RIVER){
        int seat;
        sscanf(RecvBuf + 5, "%d", &seat);
        switch(seat){
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
                break;
            case 4:
                players[3].fold = 1;

                snprintf(SendBuf, sizeof(SendBuf), "%s has folded.", players[3].username);
                printf("Player 4 folded.\n");

                game.actioncounter += 1;
                break;
            default:
                break;
        }

    }

    else if (strncmp(RecvBuf, "RAISE 2 ", 8) == 0 && game.currRound == RIVER) {
        int raiseAmt;
        sscanf(RecvBuf + 8, "%d", &raiseAmt);

        players[1].bet = table->minBet + raiseAmt;
        players[1].points -= players[1].bet;
        table->minBet = players[1].bet;
        table->pot += players[1].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. Current Pot: %d. %s's turn.", players[1].username, raiseAmt, table->pot, players[2].username);
        printf("Player 2 raised by %d.\n", raiseAmt);

        game.actioncounter += 1;
        }

         else if (strncmp(RecvBuf, "RAISE 3 ", 8) == 0 && game.currRound == RIVER) {
        int raiseAmt;
        sscanf(RecvBuf + 8, "%d", &raiseAmt);

        players[2].bet = table->minBet + raiseAmt;
        players[2].points -= players[2].bet;
        table->minBet = players[2].bet;
        table->pot += players[2].bet;

        snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. Current Pot: %d. %s's turn.", players[2].username, raiseAmt, table->pot, players[3].username);
        printf("Player 3 raised by %d.\n", raiseAmt);

        game.actioncounter += 1;
        } 

        else if(strncmp(RecvBuf, "RAISE 4 ", 8) == 0 && game.currRound == RIVER){
            int raiseAmt;
            sscanf(RecvBuf + 8, "%d", &raiseAmt);
            players[3].bet = table->minBet + raiseAmt;

            players[3].points -= players[3].bet;
            table->minBet = players[3].bet; /*new raised amount is now minbet*/

            table->pot += players[3].bet;
            snprintf(SendBuf, sizeof(SendBuf), "%s has raised by %d. %s remaining balance is %d. Current Pot: %d", players[3].username, raiseAmt,players[3].username, players[3].points, table->pot);
            game.actioncounter += 1;

        }

        else if(game.actioncounter == 16 && game.currRound == RIVER && 0 == strncmp(RecvBuf, "NEXT", 4)){
            snprintf(SendBuf, sizeof(SendBuf), "TYPE 'CHECK' TO REVEAL ALL BEST HANDS AND REVEAL WINNER");
            game.currRound = SHOWDOWN;
        }
        
        else if(game.currRound == SHOWDOWN && 0 == strncmp(RecvBuf, "CHECK", 5)){
            p1hand = bestHand(p1cmp);
            

            p2hand = bestHand(p2cmp);
            p3hand = bestHand(p3cmp);
            p4hand = bestHand(p3cmp);

            int hands[4] = {p1hand, p2hand, p3hand, p4hand};
            bestPlayer = -1;
            bestplayerHand = hands[0];  /*assume t*/
            for(int i = 0; i < 4; i++){
                if(hands[i] > bestplayerHand){
                    bestplayerHand = hands[i];
                    bestPlayer = i + 1;

                }
            }

            if(bestPlayer == -1){
                snprintf(SendBuf, sizeof(SendBuf), "TIE"); 
            }
            printf("%d - %d - %d - %d\n", p1hand, p2hand, p3hand, p4hand);

            snprintf(SendBuf, sizeof(SendBuf), "The player with the best hand is player %d", bestPlayer);     

        }
        

// int showCommunityCards() {
//     // Show the community cards to all players
//     snprintf(SendBuf, sizeof(SendBuf), "Flop cards dealt: %d of %c, %d of %c, %d of %c", 
//              table->communityCards[0].rank, table->communityCards[0].suit,
//              table->communityCards[1].rank, table->communityCards[1].suit,
//              table->communityCards[2].rank, table->communityCards[2].suit);
//     printf("Flop cards: %s\n", SendBuf);
// }
    

        
        
        
    




    



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

    game.roundCounter = 11;
    }

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
 