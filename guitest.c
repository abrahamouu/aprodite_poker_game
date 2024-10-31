#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guitest.h"
#include "pokerguiserver.h"
#include "player.h"


/*
int seat_number = 0;
*/

/*DRAWING THE TABLE*/
GtkWidget *card_placeholders[14];
// Function to draw the poker table
gboolean draw_poker_table(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    // Get the drawable surface
    cairo_t *cr = gdk_cairo_create(widget->window);

    // Set the color to green
    cairo_set_source_rgb(cr, 0, 1, 0);

    // Draw a smaller rectangle lower down in the drawing area
    cairo_rectangle(cr, 75, 100, 350, 200); // Position: (75, 100), Size: 250x100
    cairo_fill(cr);

    // Set the color to black for the border
    cairo_set_source_rgb(cr, 0, 0, 0);

    // Set the line width for the border
    cairo_set_line_width(cr, 5.0); // Adjust the thickness here (default is 1.0)

    // Draw the border around the rectangle
    cairo_rectangle(cr, 75, 100, 350, 200); // Position: (75, 100), Size: 250x100
    cairo_stroke(cr);

    // Destroy the cairo context
    cairo_destroy(cr);

    return FALSE;
}


// Function to draw a card placeholder with an image
void set_card_image(GtkWidget *card, const char *file_path) {
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(file_path, NULL);
    if (pixbuf) {
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 50, 60, GDK_INTERP_BILINEAR);
        GtkWidget *image = gtk_image_new_from_pixbuf(scaled_pixbuf);

        gtk_container_remove(GTK_CONTAINER(card), gtk_bin_get_child(GTK_BIN(card)));
        gtk_container_add(GTK_CONTAINER(card), image);

        gtk_widget_show_all(card);

        g_object_unref(pixbuf);
        g_object_unref(scaled_pixbuf);
    }
}

/*--------START NAME FUNCTIONS-------------------------*/
/*function used to send the name to server*/
void on_sendname_clicked(GtkButton *button, gpointer user_data){
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *name_input =  gtk_entry_get_text(GTK_ENTRY(entry));
    char RecvBuf[256];
    char message[256];
    
    snprintf(message, sizeof(message), "NAME %s", name_input);

    const char *Response = Talk2Server(message, RecvBuf);

    char expected_response[256];
    snprintf(expected_response, sizeof(expected_response), "Player 1's username is set to: %s", name_input);

    if (strncmp(Response, expected_response, strlen(expected_response)) == 0) {
        g_print("Username set successfully\n");
    } else {
        g_print("Unexpected response: %s\n", Response);
    }
}
/*function used to call the on_sendname after user types name into small window*/
void on_name_clicked(GtkButton *button, gpointer user_data)
{
    
    GtkWidget *vbox;
    GtkWidget *name_entry;
    GtkWidget *sendbutton;
    GtkWidget *new_window;
    /*opens small window after button is clicked*/
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Enter Name");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 300, 100);
    /*sets up box to receive actualy typed input from user*/
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(new_window), vbox);
    name_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), name_entry, FALSE, FALSE, 0);
    /*send button to use the sendname function to send name to server*/
    sendbutton = gtk_button_new_with_label("Send Name");
    gtk_box_pack_start(GTK_BOX(vbox), sendbutton, FALSE, FALSE, 0);
    g_signal_connect(sendbutton, "clicked", G_CALLBACK(on_sendname_clicked), name_entry);

    gtk_widget_show_all(new_window);
}
/*--------END NAME FUNCTIONS------------*/


/*function to close the pokertable window and get booted from client*/
void on_leave_poker_table_clicked(GtkButton *button, gpointer user_data)
{
    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("BYE", RecvBuf);
    GtkWidget *window = GTK_WIDGET(user_data);
    gtk_widget_destroy(window);

}
/*START BUTTON FUNCTION*/
void on_startbutton_clicked(GtkButton *button, gpointer user_data){
    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("START", RecvBuf);

    if (strncmp(Response, "ALL PLAYERS READY. DEALING YOUR CARDS.", 38) == 0) {
        // Print the response to the console for debugging
        
        
    } else {
        // Print the response to the console for debugging
    }
}

/*-------------START MINE BUTTON FUNCTIONS-----------------*/
/*function to get your cards to the gui*/
void on_minebutton_clicked(GtkButton *button, gpointer user_data){

if(seat_number1 == 1){
    char RecvBuf[256];
    char message[256];
    snprintf(message, sizeof(message), "MINE %d", seat_number1);
    const char *Response;
    
    Response = Talk2Server(message, RecvBuf);

    char rank1, suit1, rank2, suit2;

    if (sscanf(Response, "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c", &rank1, &suit1, &rank2, &suit2) == 4) {
        // Set the card images based on the received rank and suit
        char card1[20], card2[20];
        snprintf(card1, sizeof(card1), "%c%c.png", rank1, suit1); // Example: "5s.png"
        snprintf(card2, sizeof(card2), "%c%c.png", rank2, suit2); // Example: "Ah.png"

            set_card_image(card_placeholders[0], card1);
            set_card_image(card_placeholders[1], card2);

            set_card_image(card_placeholders[2], "redcard.png");
            set_card_image(card_placeholders[3], "redcard.png");
            set_card_image(card_placeholders[4], "redcard.png");
            set_card_image(card_placeholders[5], "redcard.png");
            set_card_image(card_placeholders[6], "redcard.png");
            set_card_image(card_placeholders[7], "redcard.png");
    }
}

if(seat_number2 == 1){
    char RecvBuf[256];
    char message[256];
    snprintf(message, sizeof(message), "MINE %d", seat_number2 + 1);
    const char *Response;
    
    Response = Talk2Server(message, RecvBuf);

    char rank1, suit1, rank2, suit2;
    
    if (sscanf(Response, "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c", &rank1, &suit1, &rank2, &suit2) == 4) {
        // Set the card images based on the received rank and suit
        char card1[20], card2[20];
        snprintf(card1, sizeof(card1), "%c%c.png", rank1, suit1); // Example: "5s.png"
        snprintf(card2, sizeof(card2), "%c%c.png", rank2, suit2); // Example: "Ah.png"

            set_card_image(card_placeholders[6], card1);
            set_card_image(card_placeholders[7], card2);

            set_card_image(card_placeholders[0], "redcard.png");
            set_card_image(card_placeholders[1], "redcard.png");
            set_card_image(card_placeholders[2], "redcard.png");
            set_card_image(card_placeholders[3], "redcard.png");
            set_card_image(card_placeholders[4], "redcard.png");
            set_card_image(card_placeholders[5], "redcard.png");
    }
}

if(seat_number3 == 1){
    char RecvBuf[256];
    char message[256];
    snprintf(message, sizeof(message), "MINE %d", seat_number3 + 2);
    const char *Response;
    
    Response = Talk2Server(message, RecvBuf);

    char rank1, suit1, rank2, suit2;
    
    if (sscanf(Response, "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c", &rank1, &suit1, &rank2, &suit2) == 4) {
        // Set the card images based on the received rank and suit
        char card1[20], card2[20];
        snprintf(card1, sizeof(card1), "%c%c.png", rank1, suit1); // Example: "5s.png"
        snprintf(card2, sizeof(card2), "%c%c.png", rank2, suit2); // Example: "Ah.png"

            set_card_image(card_placeholders[2], card1);
            set_card_image(card_placeholders[3], card2);

            set_card_image(card_placeholders[0], "redcard.png");
            set_card_image(card_placeholders[1], "redcard.png");
            set_card_image(card_placeholders[4], "redcard.png");
            set_card_image(card_placeholders[5], "redcard.png");
            set_card_image(card_placeholders[6], "redcard.png");
            set_card_image(card_placeholders[7], "redcard.png");
    }
}

if(seat_number4 == 1){
    char RecvBuf[256];
    char message[256];
    snprintf(message, sizeof(message), "MINE %d", seat_number4 + 3);
    const char *Response;
    
    Response = Talk2Server(message, RecvBuf);

    char rank1, suit1, rank2, suit2;
    
    if (sscanf(Response, "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c", &rank1, &suit1, &rank2, &suit2) == 4) {
        // Set the card images based on the received rank and suit
        char card1[20], card2[20];
        snprintf(card1, sizeof(card1), "%c%c.png", rank1, suit1); // Example: "5s.png"
        snprintf(card2, sizeof(card2), "%c%c.png", rank2, suit2); // Example: "Ah.png"

            set_card_image(card_placeholders[4], card1);
            set_card_image(card_placeholders[5], card2);

            set_card_image(card_placeholders[0], "redcard.png");
            set_card_image(card_placeholders[1], "redcard.png");
            set_card_image(card_placeholders[2], "redcard.png");
            set_card_image(card_placeholders[3], "redcard.png");
            set_card_image(card_placeholders[6], "redcard.png");
            set_card_image(card_placeholders[7], "redcard.png");
    }
}
}
/*void on_minebutton2_clicked(GtkButton *button, gpointer user_data){
    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("MINE 2", RecvBuf);

    char rank1, suit1, rank2, suit2;

    if (sscanf(Response, "CARD 1: RANK is %c and SUIT is %c\n CARD 2: RANK is %c and SUIT is %c", &rank1, &suit1, &rank2, &suit2) == 4) {
        // Set the card images based on the received rank and suit
        char card1[20], card2[20];
        snprintf(card1, sizeof(card1), "%c%c.png", rank1, suit1); // Example: "5s.png"
        snprintf(card2, sizeof(card2), "%c%c.png", rank2, suit2); // Example: "Ah.png"
        
        set_card_image(card_placeholders[6], card1); // Assuming card_placeholders[6] is for the first card of seat 2
        set_card_image(card_placeholders[7], card2); // Assuming card_placeholders[7] is for the second card of seat 2
    } else {
        set_card_image(card_placeholders[6], "5S.png");
        set_card_image(card_placeholders[7], "5S.png");
    }
}
*/

/*----------END MINE BUTTON FUNCTIONS--------------*/


/*-------FOLD, CALL, CHECK FUNCTIONS-----------*/
/*START FOLD FUNCTIONS*/
void on_foldbutton1_clicked(GtkButton *button, gpointer user_data){
    char RecvBuf[256];
    char message[256];
    snprintf(message, sizeof(message), "FOLD %d", seat_number);
    const char *Response = Talk2Server(message, RecvBuf);

    
}
/*END FOLD FUNCTIONS*/
/*START CALL FUNCTIONS*/
void on_callbutton1_clicked(GtkButton *button, gpointer user_data){
    char RecvBuf[256];
    char message[256];
    snprintf(message, sizeof(message), "CALL %d", seat_number);
    const char *Response = Talk2Server(message, RecvBuf);

    // Add your response handling code here if needed
}

void on_callbutton3_clicked(GtkButton *button, gpointer user_data){
    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("CALL 3", RecvBuf);

    // Add your response handling code here if needed
}

void on_callbutton4_clicked(GtkButton *button, gpointer user_data){
    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("CALL 4", RecvBuf);

    // Add your response handling code here if needed
}
/*END CALL FUNCTIONS*/


/*START RAISE FUNCTIONS*/
void on_sendraise_clicked(GtkButton *button, gpointer user_data){
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *raise_input =  gtk_entry_get_text(GTK_ENTRY(entry));
    char RecvBuf[256];
    char message[256];
    
    int raise_int = atoi(raise_input);

    snprintf(message, sizeof(message), "RAISE %d", raise_int);

    const char *Response = Talk2Server(message, RecvBuf);

    char expected_response[256];
    snprintf(expected_response, sizeof(expected_response), "Player 1 has raised by %d", raise_int);

    if (strncmp(Response, expected_response, strlen(expected_response)) == 0) {
        g_print("Raised successfully\n");
        
    } else {
        g_print("Unexpected response: %s\n", Response);
    }
}

void on_raisebutton1_clicked(GtkButton *button, gpointer user_data)
{  
    GtkWidget *vbox;
    GtkWidget *raise_entry;
    GtkWidget *sendbutton;
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Enter Raise Amount");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 300, 100);
    
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(new_window), vbox);
    raise_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), raise_entry, FALSE, FALSE, 0);

    sendbutton = gtk_button_new_with_label("Send Raise Amount");
    gtk_box_pack_start(GTK_BOX(vbox), sendbutton, FALSE, FALSE, 0);
    g_signal_connect(sendbutton, "clicked", G_CALLBACK(on_sendraise_clicked), raise_entry);

    gtk_widget_show_all(new_window);
 
}





/*END RAISE FUNCTIONS*/




/*-----------END FOLD, CALL, CHECK FUNCTIONS-----------*/
/*START CHECK FUNCTIONS*/
void on_checkbutton1_clicked(GtkButton *button, gpointer user_data){
    char RecvBuf[256];
    const char *Response;
    Response = Talk2Server("CHECK 1", RecvBuf);

    // Add your response handling code here if needed
}
/*END CHECK FUNCTIONS*/

void on_sendbet1_clicked(GtkButton *button, gpointer user_data){
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *bet1_input =  gtk_entry_get_text(GTK_ENTRY(entry));
    char RecvBuf[256];
    char message[256];
    
    int bet1_int = atoi(bet1_input);

    snprintf(message, sizeof(message), "BET 1 %d", bet1_int);

    const char *Response = Talk2Server(message, RecvBuf);

    char expected_response[256];
    snprintf(expected_response, sizeof(expected_response), "Small Blind bet is %d", bet1_int);

    if (strncmp(Response, expected_response, strlen(expected_response)) == 0) {
        g_print("Bet successfully\n");
        
    } else {
        g_print("Unexpected response: %s\n", Response);
    }
}

void on_bet1_clicked(GtkButton *button, gpointer user_data){
    GtkWidget *vbox;
    GtkWidget *bet1_entry;
    GtkWidget *sendbutton;
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Enter Small Blind Amount");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 300, 100);
    
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(new_window), vbox);
    bet1_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), bet1_entry, FALSE, FALSE, 0);

    sendbutton = gtk_button_new_with_label("Send Bet Amount");
    gtk_box_pack_start(GTK_BOX(vbox), sendbutton, FALSE, FALSE, 0);
    g_signal_connect(sendbutton, "clicked", G_CALLBACK(on_sendbet1_clicked), bet1_entry);

    gtk_widget_show_all(new_window);
}

void on_sendbet2_clicked(GtkButton *button, gpointer user_data){
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *bet2_input =  gtk_entry_get_text(GTK_ENTRY(entry));
    char RecvBuf[256];
    char message[256];
    
    int bet2_int = atoi(bet2_input);

    snprintf(message, sizeof(message), "BET 2 %d", bet2_int);

    const char *Response = Talk2Server(message, RecvBuf);

    char expected_response[256];
    snprintf(expected_response, sizeof(expected_response), "Big Blind bet is %d", bet2_int);

    if (strncmp(Response, expected_response, strlen(expected_response)) == 0) {
        g_print("Bet successfully\n");
        
    } else {
        g_print("Unexpected response: %s\n", Response);
    }
}

void on_bet2_clicked(GtkButton *button, gpointer user_data){
    GtkWidget *vbox;
    GtkWidget *bet2_entry;
    GtkWidget *sendbutton;
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Enter Big Blind Amount");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 300, 100);
    
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(new_window), vbox);
    bet2_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), bet2_entry, FALSE, FALSE, 0);

    sendbutton = gtk_button_new_with_label("Send Bet Amount");
    gtk_box_pack_start(GTK_BOX(vbox), sendbutton, FALSE, FALSE, 0);
    g_signal_connect(sendbutton, "clicked", G_CALLBACK(on_sendbet2_clicked), bet2_entry);

    gtk_widget_show_all(new_window);
}
void on_rules_clicked(GtkButton *button, gpointer user_data){
    GtkWidget *vbox;
    GtkWidget *new_window;
    new_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_window), "Rules of poker");
    gtk_window_set_default_size(GTK_WINDOW(new_window), 400, 400);
    
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(new_window), vbox);

    GtkWidget *label = gtk_label_new("These are the rules of Poker: \nEach player in Texas Hold em poker receives two private cards, and their\nobjective is to use the five community cards: the Flop, Turn, and River. These\ncommunity cards are dealt face-up in stages to assemble the best possible five \ncard hand. With the goal of either holding the best hand at the endgame or\npressuring rivals to fold, players (starting with the small blind and continuing\nclockwise) place bets per round. The player who has the best hand or is the\nlast player standing wins the pot. There are rounds of betting after each set of\ncommunity cards are shown.");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);


    gtk_widget_show_all(new_window);
}


void GetPotFromServer(		/* ask server for current pot, display it */
	GtkWidget *Widget,
	gpointer Data)
{
    GtkLabel *LabelToUpdate;
    char RecvBuf[256];
    const char *Response;

#ifdef DEBUG
    /*printf("%s: GetpotFromServer callback starting...\n", Program);*/
#endif
    LabelToUpdate = Data;
    /*assert(LabelToUpdate);*/
    Response = Talk2Server("POT", RecvBuf);
    if (0 == strncmp(Response, "Current POT on the table is %d ", 31))
    {	/* ok, strip off the protocol header and display the time */
	gtk_label_set_label(LabelToUpdate, Response + 31);
    }
    else
    {	/* unexpected, display entire response */
	gtk_label_set_label(LabelToUpdate, Response);
    }
#ifdef DEBUG
   /* printf("%s: GetpotFromServer callback done.\n", Program);*/
#endif
} 

/* CREATE POKER TABLE FUNCTION (BASICALLY A MAIN FUNCTION)*/
void create_poker_table(GtkWidget *window){
    GtkWidget *fixed;
    GtkWidget *drawing_area;
    GtkWidget *foldbutton, 
                *callbutton, 
                *raisebutton, 
                *checkbutton, 
                *leavebutton, *namebutton, *startbutton,
                *minebutton;
    GtkWidget *potlabel, *potvalue, *playerMoneyLabel, *playerValue;
    GtkWidget *bet1button, *bet2button, *potbutton;
    GtkWidget *rulesbutton;
    int i;
    int totalPot = 0;
    char potValueText[25];
    int playerTotal = 0;
    char playerValueText[25];

    // Set the default size of the window to be larger
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create a fixed container to place widgets at specific positions
    fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    // Create a drawing area for the poker table
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, 550, 400);
    gtk_fixed_put(GTK_FIXED(fixed), drawing_area, 0, 0);

    // Connect the expose-event signal to the drawing function
    g_signal_connect(G_OBJECT(drawing_area), "expose-event", G_CALLBACK(draw_poker_table), NULL);


 /*----------FOLD, CALL, RAISE, CHECK BUTTONS--------------- */

/*START FOLD BUTTONS*/


/*-------FOLD BUTTONS-----------*/
foldbutton = gtk_button_new_with_label("Fold");
g_signal_connect(foldbutton, "clicked", G_CALLBACK(on_foldbutton1_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), foldbutton, 50, 440);

/*-------CALL BUTTONS-----------*/
callbutton = gtk_button_new_with_label("Call");
g_signal_connect(callbutton, "clicked", G_CALLBACK(on_callbutton1_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), callbutton, 150, 440);



/*-------RAISE BUTTONS-----------*/
raisebutton = gtk_button_new_with_label("Raise");
g_signal_connect(raisebutton, "clicked", G_CALLBACK(on_raisebutton1_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), raisebutton, 250, 440);



/*-------CHECK BUTTONS-----------*/
checkbutton = gtk_button_new_with_label("Check");
g_signal_connect(checkbutton, "clicked", G_CALLBACK(on_checkbutton1_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), checkbutton, 350, 440);

/*small / big blind*/
bet1button = gtk_button_new_with_label("Bet 1 (Small Blind)");
g_signal_connect(bet1button, "clicked", G_CALLBACK(on_bet1_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), bet1button, 350, 490);

bet2button = gtk_button_new_with_label("Bet 2 (Big Blind)");
g_signal_connect(bet2button, "clicked", G_CALLBACK(on_bet2_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), bet2button, 350, 530);





/*checkbutton2 = gtk_button_new_with_label("Check(Player2)");
g_signal_connect(checkbutton2, "clicked", G_CALLBACK(on_checkbutton2_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), checkbutton2, 350, 490);*/



/*checkbutton3 = gtk_button_new_with_label("Check(Player3)");
g_signal_connect(checkbutton3, "clicked", G_CALLBACK(on_checkbutton3_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), checkbutton3, 350, 540);

checkbutton4 = gtk_button_new_with_label("Check(Player4)");
g_signal_connect(checkbutton4, "clicked", G_CALLBACK(on_checkbutton4_clicked), NULL);
gtk_fixed_put(GTK_FIXED(fixed), checkbutton4, 350, 590);*/
/*END CHECK BUTTONS*/


/*MINE BUTTON LABELS*/
    minebutton = gtk_button_new_with_label("Mine(Player1)");
    g_signal_connect(minebutton, "clicked", G_CALLBACK(on_minebutton_clicked), NULL);
    gtk_fixed_put(GTK_FIXED(fixed), minebutton, 800, 150);
/*
    minebutton2 = gtk_button_new_with_label("Mine(Player2)");
    g_signal_connect(minebutton2, "clicked", G_CALLBACK(on_minebutton2_clicked), NULL);
    gtk_fixed_put(GTK_FIXED(fixed), minebutton2, 800, 200);

    minebutton3 = gtk_button_new_with_label("Mine(Player3)");
    g_signal_connect(minebutton3, "clicked", G_CALLBACK(on_minebutton3_clicked), NULL);
    gtk_fixed_put(GTK_FIXED(fixed), minebutton3, 800, 250);

    minebutton4 = gtk_button_new_with_label("Mine(Player4)");
    g_signal_connect(minebutton4, "clicked", G_CALLBACK(on_minebutton4_clicked), NULL);
    gtk_fixed_put(GTK_FIXED(fixed), minebutton4, 800, 300);

/*START BUTTON*/
    startbutton = gtk_button_new_with_label("START");
    g_signal_connect(startbutton, "clicked", G_CALLBACK(on_startbutton_clicked), NULL);
    gtk_fixed_put(GTK_FIXED(fixed), startbutton, 800, 400);

/*pot and player money labels*/
    potlabel = gtk_label_new("Total Pot: ");
    gtk_widget_modify_font(potlabel, pango_font_description_from_string("Sans Bold 13"));
    gtk_fixed_put(GTK_FIXED(fixed), potlabel, 800, 500);

    sprintf(potValueText, "%d", totalPot);  // inital pot value
    potvalue = gtk_label_new(potValueText); 
    gtk_widget_modify_font(potvalue, pango_font_description_from_string("Sans Bold 13"));
    gtk_fixed_put(GTK_FIXED(fixed), potvalue, 800, 540);

    potbutton = gtk_button_new_with_label("Get Pot from Server");
    g_signal_connect(potbutton, "clicked",G_CALLBACK(GetPotFromServer), potvalue);
    gtk_fixed_put(GTK_FIXED(fixed), potbutton, 790, 470);

    playerMoneyLabel = gtk_label_new("Your Total: ");
    gtk_widget_modify_font(playerMoneyLabel, pango_font_description_from_string("Sans Bold 13"));
    gtk_fixed_put(GTK_FIXED(fixed), playerMoneyLabel, 800, 590);

    sprintf(playerValueText, "%d", playerTotal);  // Initial pot value
    playerValue = gtk_label_new(playerValueText);
    gtk_widget_modify_font(playerValue, pango_font_description_from_string("Sans Bold 13"));
    gtk_fixed_put(GTK_FIXED(fixed), playerValue, 800, 640);


/*exit button -- closes the window and boots client*/
    leavebutton = gtk_button_new_with_label("Leave Game ");
    g_signal_connect(leavebutton, "clicked", G_CALLBACK(on_leave_poker_table_clicked), window);
    gtk_fixed_put(GTK_FIXED(fixed), leavebutton , 800, 10);


    namebutton = gtk_button_new_with_label("Enter Name");
    g_signal_connect(namebutton, "clicked", G_CALLBACK(on_name_clicked), NULL);
    gtk_fixed_put(GTK_FIXED(fixed),namebutton , 800, 100);

    rulesbutton = gtk_button_new_with_label("Show Rules");
    g_signal_connect(rulesbutton, "clicked", G_CALLBACK(on_rules_clicked), NULL);
    gtk_fixed_put(GTK_FIXED(fixed),rulesbutton , 800, 200);


    // Create and position card placeholders around the rectangle
    for (i = 0; i < 14; i++) {
        card_placeholders[i] = gtk_event_box_new();
        gtk_widget_set_size_request(card_placeholders[i], 50, 60);
        gtk_fixed_put(GTK_FIXED(fixed), card_placeholders[i], 0, 0);

        // Set initial card image
        set_card_image(card_placeholders[i], "redcard.png");
    }

    // Positioning card placeholders
    // Top cards
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[0], 215, 35);
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[1], 265, 35);

    // Bottom cards
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[2], 215, 305);
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[3], 265, 305);

    // Left cards
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[4], 20, 150);
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[5], 20, 215);

    // Right cards
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[6], 430, 150);
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[7], 430, 215);

    // Card deck
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[8], 85,305);

    //table cards
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[9], 105, 180);
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[10], 165, 180);
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[11], 225, 180);
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[12], 285, 180);
    gtk_fixed_move(GTK_FIXED(fixed), card_placeholders[13], 345, 180 );

    gtk_widget_show_all(window);
}





