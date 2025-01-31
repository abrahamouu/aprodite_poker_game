/* ClockClient.c: simple interactive TCP/IP client for ClockServer
 * Author: Rainer Doemer, 5/15/23 (prior versions 2/16/15, 2/20/17)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>

#include "card.h"
#include "table.h"
/* #define DEBUG */	/* be verbose */

/*** global variables ****************************************************/

const char *Program	/* program name for descriptive diagnostics */
	= NULL;

/*** global functions ****************************************************/

void FatalError(		/* print error diagnostics and abort */
	const char *ErrorMsg)
{
    fputs(Program, stderr);
    fputs(": ", stderr);
    perror(ErrorMsg);
    fputs(Program, stderr);
    fputs(": Exiting!\n", stderr);
    exit(20);
} /* end of FatalError */

int main(int argc, char *argv[])
{
    int l, n;
    int SocketFD,	/* socket file descriptor */
	PortNo;		/* port number */
    struct sockaddr_in
	ServerAddress;	/* server address we connect with */
    struct hostent
	*Server;	/* server host */
    char SendBuf[256];	/* message buffer for sending a message */
    char RecvBuf[256];	/* message buffer for receiving a response */

    Program = argv[0];	/* publish program name (for diagnostics) */
#ifdef DEBUG
    printf("%s: Starting...\n", argv[0]);
#endif
    if (argc < 3)
    {   fprintf(stderr, "Usage: %s hostname port\n", Program);
	exit(10);
    }
    Server = gethostbyname(argv[1]);
    if (Server == NULL)
    {   fprintf(stderr, "%s: no such host named '%s'\n", Program, argv[1]);
        exit(10);
    }
    PortNo = atoi(argv[2]);
    if (PortNo <= 2000)
    {   fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program, PortNo);
        exit(10);
    }
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(PortNo);
    ServerAddress.sin_addr = *(struct in_addr*)Server->h_addr_list[0];
    int action;
    action = 0;
    do
    {	printf("%s: Enter a command to send to the game server:\n\n"
		"       'SEAT seat_number (1-4) to enter a seat number\n"
		"       'NAME your_name' to input username to your seat\n"
		"       'type 'START' once all players set their username\n"
		"       'type 'CC' to view the community cards\n"
		"       'MINE seat_number' to view your own cards\n"
		"       'BET seat_number bet_amt to initiate bet for small and big blind\n"
		"       'CALL seat_number' to call\n"
	        "       'CHECK seat_number' to check\n"
		"       'RAISE seat_number raise_amt to raise bet\n"
		"       'ALLIN seat_number to all in\n"
		"       'FOLD seat_number to fold\n"
		"       'WINNER' to evaluate cards and winner\n"
		"       'STATUS seat_number to view status of you\n"
		"       'OTHER' to view other status and points\n"
		"       'POT' to view current pot\n"
		"       'NEWGAME' to start a new game!\n"
		"       'SCOREBOARD' to print out scoreboard!\n"
		"       'ZOT seat_number deposit_amount' to refill your point at the every end of the round\n"
		"       'TIME' to obtain the current time,\n"
		"       'BYE' to quit this client, or\n"
		"       'SHUTDOWN' to terminate the server\n\n"
		"command: ", argv[0]);
	
	
	fgets(SendBuf, sizeof(SendBuf), stdin);
	l = strlen(SendBuf);
	if (SendBuf[l-1] == '\n')
	{   SendBuf[--l] = 0;
	}
	if (0 == strcmp("BYE", SendBuf))
	{   break;
	}
	if (l)
	{   SocketFD = socket(AF_INET, SOCK_STREAM, 0);
	    if (SocketFD < 0)
	    {   FatalError("socket creation failed");
	    }
	    printf("%s: Connecting to the server at port %d...\n",
			Program, PortNo);
	    if (connect(SocketFD, (struct sockaddr*)&ServerAddress,
			sizeof(ServerAddress)) < 0)
	    {   FatalError("connecting to server failed");
	    }
	    printf("%s: Sending message '%s'...\n", Program, SendBuf);
	    n = write(SocketFD, SendBuf, l);
	    if (n < 0)
	    {   FatalError("writing to socket failed");
	    }
#ifdef DEBUG
	    printf("%s: Waiting for response...\n", Program);
#endif
	    n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	    if (n < 0) 
	    {   FatalError("reading from socket failed");
	    }
	    RecvBuf[n] = 0;
	    printf("%s: Received response: %s\n", Program, RecvBuf);
#ifdef DEBUG
	    printf("%s: Closing the connection...\n", Program);
#endif
	    close(SocketFD);
	}
    } while(0 != strcmp("SHUTDOWN", SendBuf));
    printf("%s: Exiting...\n", Program);
    return 0;
}

/* EOF ClockClient.c */
