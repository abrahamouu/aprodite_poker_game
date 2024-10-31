#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <assert.h>

#include "guitest.h"

/* #define DEBUG */	/* be verbose */
#define BUFFSIZE 256

int seat_number = 0;
int seat_number1 = 0;
int seat_number2 = 0;
int seat_number3 = 0;
int seat_number4 = 0;

/*** global variables ****************************************************/

const char *Program	/* program name for descriptive diagnostics */
	= "FTC Poker Game";
struct sockaddr_in
	ServerAddress;	/* server address we connect with */

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

char *Talk2Server(		/* communicate with the server */
	const char *Message,
	char *RecvBuf)
{
    int n;
    int SocketFD;

    SocketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (SocketFD < 0)
    {   FatalError("socket creation failed");
    }
#ifdef DEBUG
    printf("%s: Connecting to the server at port %d...\n",
		Program, ntohs(ServerAddress.sin_port));
#endif
    if (connect(SocketFD, (struct sockaddr*)&ServerAddress,
		sizeof(struct sockaddr_in)) < 0)
    {   FatalError("connecting to server failed");
    }
#ifdef DEBUG
    printf("%s: Sending message '%s'...\n", Program, Message);
#endif
    n = write(SocketFD, Message, strlen(Message));
    if (n < 0)
    {   FatalError("writing to socket failed");
    }
#ifdef DEBUG
    printf("%s: Waiting for response...\n", Program);
#endif
    n = read(SocketFD, RecvBuf, BUFFSIZE-1);
    if (n < 0) 
    {   FatalError("reading from socket failed");
    }
    RecvBuf[n] = 0;
#ifdef DEBUG
    printf("%s: Received response: %s\n", Program, RecvBuf);
    printf("%s: Closing the connection...\n", Program);
#endif
    close(SocketFD);
    return(RecvBuf);
} /* end of Talk2Server */

void GetTimeFromServer(		/* ask server for current time, display it */
	GtkWidget *Widget,
	gpointer Data)
{
    GtkLabel *LabelToUpdate;
    char RecvBuf[BUFFSIZE];
    const char *Response;

#ifdef DEBUG
    printf("%s: GetTimeFromServer callback starting...\n", Program);
#endif
    LabelToUpdate = Data;
    assert(LabelToUpdate);
    Response = Talk2Server("TIME", RecvBuf);
    if (0 == strncmp(Response, "OK TIME: ", 9))
    {	/* ok, strip off the protocol header and display the time */
	gtk_label_set_label(LabelToUpdate, Response + 9);
    }
    else
    {	/* unexpected, display entire response */
	gtk_label_set_label(LabelToUpdate, Response);
    }
#ifdef DEBUG
    printf("%s: GetTimeFromServer callback done.\n", Program);
#endif
} /* end of GetTimeFromServer */

void ShutdownServer(		/* ask server to shutdown */
	GtkWidget *Widget,
	gpointer Data)
{
    char RecvBuf[BUFFSIZE];
    const char *Response;

#ifdef DEBUG
    printf("%s: ShutdownServer callback starting...\n", Program);
#endif
    Response = Talk2Server("SHUTDOWN", RecvBuf);
    if (0 == strcmp(Response, "OK SHUTDOWN"))
    {	/* ok, the server shuts down, so should this client */
	gtk_main_quit();
    }
    else
    {	/* unexpected response, ignore it as invalid */
    }
#ifdef DEBUG
    printf("%s: ShutdownServer callback done.\n", Program);
#endif
} /* end of ShutdownServer */

/* Function prototypes */
void on_seat1_clicked(GtkButton *button, gpointer user_data);
void on_seat2_clicked(GtkButton *button, gpointer user_data);
void on_seat3_clicked(GtkButton *button, gpointer user_data);
void on_seat4_clicked(GtkButton *button, gpointer user_data);

void on_seat1_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Seat 1");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 600, 500);
    create_poker_table(new_window);
    gtk_widget_show_all(new_window);

    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("SEAT 1", RecvBuf);

    if (0 == strncmp(Response, "YOU ARE NOW SEAT 1", 18))
    {	/* ok, the server shuts down, so should this client */
        seat_number1 = 1;
        seat_number = 1;
    }
    else
    {	/* unexpected response, ignore it as invalid */
    }
}

void on_seat2_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Seat 2");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 600, 500);
    create_poker_table(new_window);
    gtk_widget_show_all(new_window);

    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("SEAT 2", RecvBuf);
    
    if (0 == strncmp(Response, "YOU ARE NOW SEAT 2", 18))
    {	/* ok, the server shuts down, so should this client */
        seat_number2 = 1;
        seat_number = 2;
    }
    else
    {	/* unexpected response, ignore it as invalid */
    }
}

void on_seat3_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Seat 3");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 600, 500);
    create_poker_table(new_window);
    gtk_widget_show_all(new_window);

    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("SEAT 3", RecvBuf);
    
    if (0 == strncmp(Response, "YOU ARE NOW SEAT 3", 18))
    {	/* ok, the server shuts down, so should this client */
        seat_number3 = 1;
        seat_number = 3;
    }
    else
    {	/* unexpected response, ignore it as invalid */
    }
}

void on_seat4_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Seat 4");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 600, 500);
    create_poker_table(new_window);
    gtk_widget_show_all(new_window);

    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("SEAT 4", RecvBuf);
    
    if (0 == strncmp(Response, "YOU ARE NOW SEAT 4", 18))
    {	/* ok, the server shuts down, so should this client */
       seat_number4 = 1;
       seat_number = 4;
    }
    else
    {	/* unexpected response, ignore it as invalid */
    }
}
/*--------------START OF MAIN--------------------*/
int main(int argc, char *argv[]){
    int PortNo;
    struct hostent *Server;
    GtkWidget *Window;
    GtkWidget *VBox, *RequestButton, *Frame, *Label, *StartButton;
    GtkWidget *HButtons, *QuitButton, *ShutdownButton, *Seat1, *Seat2, *Seat3, *Seat4, *Start;

    Program = argv[0];
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

    /* build the GUI */
    /* (adapted from https://en.wikipedia.org/wiki/GTK%2B#Example) */

    /* initialize the GTK libraries */
    gtk_init(&argc, &argv);

    /* create the main, top level window */
    Window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    /* give it the title of this program */
    gtk_window_set_title(GTK_WINDOW(Window), Program);

    /* center the window */
    gtk_window_set_position(GTK_WINDOW(Window), GTK_WIN_POS_CENTER);

    /* set the window's default size */
    gtk_window_set_default_size(GTK_WINDOW(Window), 500, 500);
    gtk_container_set_border_width (GTK_CONTAINER(Window), 10);

    /* map the destroy signal of the window to gtk_main_quit;
     * when the window is about to be destroyed, we get a notification and
     * stop the main GTK+ loop by returning 0 */
    g_signal_connect(Window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* overall vertical arrangement in the window */
    VBox = gtk_vbox_new(TRUE, 10);
    gtk_container_add(GTK_CONTAINER(Window), VBox);

    /* on the top, put a button to request the current time from the server */
    RequestButton = gtk_button_new_with_label("Request Time from Server");
    gtk_container_add(GTK_CONTAINER(VBox), RequestButton);

    StartButton = gtk_button_new_with_label("Start Game");
    gtk_container_add(GTK_CONTAINER(VBox), StartButton);
    g_signal_connect(Window, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    Seat1 = gtk_button_new_with_label("Seat 1");
    gtk_container_add(GTK_CONTAINER(VBox), Seat1);
    g_signal_connect(Seat1, "clicked", G_CALLBACK(on_seat1_clicked), NULL);

    Seat2 = gtk_button_new_with_label("Seat 2");
    gtk_container_add(GTK_CONTAINER(VBox), Seat2);
    g_signal_connect(Seat2, "clicked", G_CALLBACK(on_seat2_clicked), NULL);

    Seat3 = gtk_button_new_with_label("Seat 3");
    gtk_container_add(GTK_CONTAINER(VBox), Seat3);
    g_signal_connect(Seat3, "clicked", G_CALLBACK(on_seat3_clicked), NULL);

    Seat4 = gtk_button_new_with_label("Seat 4");
    gtk_container_add(GTK_CONTAINER(VBox), Seat4);
    g_signal_connect(Seat4, "clicked", G_CALLBACK(on_seat4_clicked), NULL);

    Start = gtk_button_new_with_label("Start");
    gtk_container_add(GTK_CONTAINER(VBox), Start);
    g_signal_connect(Window, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    /* in the middle, a frame with the display of the time */
    Frame = gtk_frame_new("Time received:");
    gtk_container_add(GTK_CONTAINER(VBox), Frame);
    Label = gtk_label_new("<no time received yet>");
    gtk_container_add(GTK_CONTAINER(Frame), Label);

    /* on the bottom, two buttons to quit client and shutdown server */
    HButtons = gtk_hbutton_box_new();
    gtk_container_add(GTK_CONTAINER(VBox), HButtons);
    QuitButton = gtk_button_new_with_label("Quit Client");
    gtk_container_add(GTK_CONTAINER(HButtons), QuitButton);
    ShutdownButton = gtk_button_new_with_label("Shutdown Server");
    gtk_container_add(GTK_CONTAINER(HButtons), ShutdownButton);

    /* make sure that everything becomes visible */
    gtk_widget_show_all(Window);

    /* connect request button with function asking server for time */
    g_signal_connect(RequestButton, "clicked",
			G_CALLBACK(GetTimeFromServer), Label);

    /* connect quit button with function terminating this client */
    /* (note the 'swapped' call; try without to see the effect) */
    g_signal_connect_swapped(QuitButton, "clicked",
			G_CALLBACK(gtk_widget_destroy), Window);

    /* connect shutdown button with function terminating server and client */
    g_signal_connect(ShutdownButton, "clicked",
			G_CALLBACK(ShutdownServer), NULL);

    /* start the main loop, handle all registered events */
    gtk_main();

#ifdef DEBUG
    printf("%s: Exiting...\n", Program);
#endif
    return 0;
} /* end of main */

/* EOF GTK_ClockClient.c */

