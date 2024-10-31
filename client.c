/* ClockClient.c: simple interactive TCP/IP client for ClockServer
 * Author: Rainer Doemer, 5/15/23 (prior versions 2/16/15, 2/20/17)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>

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
    	int SocketFD, /*socket file descriptor*/
    		PortNo;       /*port num*/
    	struct sockaddr_in 
    		ServerAddress; /*server address we connect with*/
    	struct hostent 
    		*Server;    /*server host*/
    	char SendBuf[256];
    	char RecvBuf[256];

    	Program = argv[0];
    	printf("%s: Starting...\n", argv[0]);

    	if (argc < 3)
    	{	   
		fprintf(stderr, "Usage: %s hostname port\n", Program);
		exit(10);
    	}
    	Server = gethostbyname(argv[1]);
    	if (Server == NULL)
    	{
	   	fprintf(stderr, "%s: no such host named '%s'\n", Program, argv[1]);
        	exit(10);
    	}
    	PortNo = atoi(argv[2]);
    	if (PortNo <= 2000)
    	{   
		fprintf(stderr, "%s: invalid port number %d, should be >2000\n",
		Program, PortNo);
        	exit(10);
    	}

	printf("%s: Creating a Socket... \n", argv[0]);
	SocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if(SocketFD < 0)
	{
		FatalError(argv[0], "socket creation failed");
	}
 
	printf("%s: Preparing the server address... \n", argv[0]);
	memset(&ServerAddress, 0, sizeof(ServerAddress));
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(PortNo);
 	memcpy(&ServerAddress.sin_addr.s_addr, Server->h_addr_list[0], Server->h_length);

	printf("%s: Connecting to the server at port %d...\n", Program, PortNo);
	if (connect(SocketFD, (struct sockaddr*)&ServerAddress,sizeof(ServerAddress)) < 0)
	{   
		FatalError(argv[0], "connecting to server failed");
	}
	
	do{
		printf("%s: Enter a message to send to the server: ", argv[0]);
		fgets(SendBuf, sizeof(SendBuf), stdin);
		l = strlen(SendBuf);
		if(SendBuf[l-1] == '\n')
		{
			SendBuf[--l] = 0;
		}

		if(l)
		{
			printf("%s: Sending message '%s'...\n", Program, SendBuf);
			n = write(SocketFD, SendBuf, l);
	
			if (n < 0)
			{   FatalError(argv[0], "writing to socket failed");
			}

	    		printf("%s: Waiting for response...\n", Program);

	    		n = read(SocketFD, RecvBuf, sizeof(RecvBuf)-1);
	    		if (n < 0) 
	    		{   FatalError(argv[0], "reading from socket failed");
	    		}
	    		RecvBuf[n] = 0;
	    		printf("%s: Received response: %s\n", Program, RecvBuf);
		}
	} while(0 != strcmp("SHUTDOWN", RecvBuf));


    	printf("%s: Exiting...\n", Program);
	close(SocketFD);


    return 0;
}

/* EOF ClockClient.c */
