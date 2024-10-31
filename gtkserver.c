#include <pthread.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include "guitest.h"



/* delete widgets function */
void delete_widgets(GtkWidget *vbox) {
    GList *widgets, *iter;

    widgets = gtk_container_get_children(GTK_CONTAINER(vbox));
    for (iter = widgets; iter != NULL; iter = g_list_next(iter)) {
        if (GTK_IS_BUTTON(iter->data)) {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }
        if (GTK_IS_LABEL(iter->data)) {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }
    }
    g_list_free(widgets);
}
// Callback functions for button clicks to open new windows
void on_pvpbutton_clicked(GtkButton *button, gpointer user_data) {
    // GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // gtk_window_set_title(GTK_WINDOW(window), "Poker Table");
    // gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    // g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);
    
    g_list_free(widgets);
}

/*starts a server based on what user inputs after selecting first option*/
void start_server(const char *port) {
    char command[256];
    snprintf(command, sizeof(command), "./betaserver %s &", port);
    system(command);
}

/* Function to start the client */
void start_client(const char *terminal_type, const char *port) {
    char command[256];
    snprintf(command, sizeof(command), "./betaclient %s %s", terminal_type, port);
    system(command);
}


/* Function to handle port entry dialog */
/*void on_port_entry_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    if (response_id == GTK_RESPONSE_OK) {
        GtkWidget *entry = g_object_get_data(G_OBJECT(dialog), "port_entry");
        const char *port = gtk_entry_get_text(GTK_ENTRY(entry));
        if (strlen(port) == 5 && strspn(port, "0123456789") == 5) {
            start_server(port);
            gtk_widget_destroy(GTK_WIDGET(dialog));
            delete_widgets(widgets->vbox);
            create_poker_table(widgets);
            gtk_widget_show_all(widgets->window);
        } else {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->window),
                                                            GTK_DIALOG_MODAL,
                                                            GTK_MESSAGE_ERROR,
                                                            GTK_BUTTONS_OK,
                                                            "Invalid port number. Please enter a 5-digit number.");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    } else {
        gtk_widget_destroy(GTK_WIDGET(dialog));
    }
}*/

// Function to handle client connecting dialog
/*void on_client_connection_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    if (response_id == GTK_RESPONSE_OK) {
        GtkWidget *terminal_entry = g_object_get_data(G_OBJECT(dialog), "terminal_entry");
        GtkWidget *port_entry = g_object_get_data(G_OBJECT(dialog), "port_entry");
        const char *terminal_type = gtk_entry_get_text(GTK_ENTRY(terminal_entry));
        const char *port = gtk_entry_get_text(GTK_ENTRY(port_entry));
        if (strlen(port) == 5 && strspn(port, "0123456789") == 5 && strlen(terminal_type) > 0) {
            strcpy(widgets->terminal_type, terminal_type);
            strcpy(widgets->port, port);

            start_client(terminal_type, port);

            gtk_widget_destroy(GTK_WIDGET(dialog));
            delete_widgets(widgets->vbox);
            create_poker_table(widgets);
            gtk_widget_show_all(widgets->window);
        } else {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(widgets->window),
                                                            GTK_DIALOG_MODAL,
                                                            GTK_MESSAGE_ERROR,
                                                            GTK_BUTTONS_OK,
                                                            "Invalid input. Please enter a valid terminal type and a 5-digit port number.");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
    } else {
        gtk_widget_destroy(GTK_WIDGET(dialog));
    }
}*/

void on_vsAIbutton_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Button 2 Window");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

/* Callback functions for button clicks to open new windows */
void on_button1_clicked(GtkButton *button, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    // Create a dialog to prompt for the port number
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Enter Port Number",
                                                    GTK_WINDOW(widgets->window),
                                                    GTK_DIALOG_MODAL,
                                                    "_OK",
                                                    GTK_RESPONSE_OK,
                                                    "_Cancel",
                                                    GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *port_label = gtk_label_new("Port Number (5 digits):");
    GtkWidget *port_entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(port_entry), 5);

    gtk_box_pack_start(GTK_BOX(content_area), port_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), port_entry, FALSE, FALSE, 5);

    g_object_set_data(G_OBJECT(dialog), "port_entry", port_entry);

    g_signal_connect(dialog, "response", G_CALLBACK(on_port_entry_dialog_response), widgets);

    gtk_widget_show_all(dialog);
}

void on_button2_clicked(GtkButton *button, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    // Create a dialog to prompt for the terminal type and port number
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Enter Server Details",
                                                    GTK_WINDOW(widgets->window),
                                                    GTK_DIALOG_MODAL,
                                                    "_OK",
                                                    GTK_RESPONSE_OK,
                                                    "_Cancel",
                                                    GTK_RESPONSE_CANCEL,
                                                    NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    /* GETS SERVER/TERMINAL TYPE */
    GtkWidget *terminal_label = gtk_label_new("Terminal Type (e.g., crystalcove, zuma):");
    GtkWidget *terminal_entry = gtk_entry_new();
    /* GETS PORT NUMBER FROM USER */
    GtkWidget *port_label = gtk_label_new("Port Number (5 digits):");
    GtkWidget *port_entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(port_entry), 5);

    gtk_box_pack_start(GTK_BOX(content_area), terminal_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), terminal_entry, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), port_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(content_area), port_entry, FALSE, FALSE, 5);

    g_object_set_data(G_OBJECT(dialog), "terminal_entry", terminal_entry);
    g_object_set_data(G_OBJECT(dialog), "port_entry", port_entry);

    g_signal_connect(dialog, "response", G_CALLBACK(on_client_connection_dialog_response), widgets);

    gtk_widget_show_all(dialog);
}

// Callback function for button 3 click event
void on_rulesbutton_clicked(GtkButton *button, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    // Change window title
    gtk_window_set_title(GTK_WINDOW(widgets->window), "Rules");
    delete_widgets(widgets->vbox);
    
    // Create a label and add it to the vbox
    GtkWidget *label = gtk_label_new("These are the rules of Poker: \nEach player in Texas Hold em poker receives two private cards, and their\nobjective is to use the five community cards: the Flop, Turn, and River. These\ncommunity cards are dealt face-up in stages to assemble the best possible five \ncard hand. With the goal of either holding the best hand at the endgame or\npressuring rivals to fold, players (starting with the small blind and continuing\nclockwise) place bets per round. The player who has the best hand or is the\nlast player standing wins the pot. There are rounds of betting after each set of\ncommunity cards are shown.");
    gtk_box_pack_start(GTK_BOX(widgets->vbox), label, FALSE, FALSE, 0);

    // Change background color to red
    GdkColor color;
    gdk_color_parse("red", &color);
    gtk_widget_modify_bg(widgets->vbox->parent, GTK_STATE_NORMAL, &color);

    GtkWidget *back_rules = gtk_button_new_with_label("Back");
    g_signal_connect(back_rules, "clicked", G_CALLBACK(on_back_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(widgets->vbox), back_rules, FALSE, FALSE, 0);

    GtkWidget *exit_rules = gtk_button_new_with_label("Exit");
    g_signal_connect(exit_rules, "clicked", G_CALLBACK(on_exitbutton_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(widgets->vbox), exit_rules, FALSE, FALSE, 0);

    gtk_widget_show_all(widgets->window);
}

// Callback function for button 4 click event
void on_exitbutton_clicked(GtkButton *button, gpointer user_data) {
    exit(20);
}

/* Function to recreate the main menu */
void reprint_main_menu(GtkWidget *window) {
    GtkWidget *vbox;
    GtkWidget *image1;
    GtkWidget *pvpbutton;
    GtkWidget *vsAIbutton;
    GtkWidget *rulesbutton;
    GtkWidget *exitbutton;
    GtkWidget *event_box;

    // Remove the child widget before adding the new container
    if (GTK_IS_BIN(window)) {
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(window));
    if (child != NULL) {
        gtk_widget_destroy(child);
    }
}


    // Create an event box to set the background color
    event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(window), event_box);

    // Set the initial background color to green
    GdkColor color;
    gdk_color_parse("green", &color);
    gtk_widget_modify_bg(event_box, GTK_STATE_NORMAL, &color);

    // Create a vertical box container to arrange widgets
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(event_box), vbox);

    // Create an image and add it to the top of the window
    image1 = gtk_image_new_from_file("ftcpoker.png");
    if (!GTK_IS_IMAGE(image1)) {
        g_print("Error: Unable to load image 'ftcpoker.png'\n");
    }
    gtk_box_pack_start(GTK_BOX(vbox), image1, FALSE, FALSE, 0);

    // Create the buttons and add them to the vbox
    pvpbutton = gtk_button_new_with_label("Play Poker with Friends (up to 4 players)");
    AppWidgets *widgets = g_malloc(sizeof(AppWidgets));
    widgets->window = window;
    widgets->vbox = vbox;
    g_signal_connect(pvpbutton, "clicked", G_CALLBACK(on_pvpbutton_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(vbox), pvpbutton, FALSE, FALSE, 0);

    vsAIbutton = gtk_button_new_with_label("Play vs. AI");
    g_signal_connect(vsAIbutton, "clicked", G_CALLBACK(on_vsAIbutton_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), vsAIbutton, FALSE, FALSE, 0);

    rulesbutton = gtk_button_new_with_label("Show Rules");
    g_signal_connect(rulesbutton, "clicked", G_CALLBACK(on_rulesbutton_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(vbox), rulesbutton, FALSE, FALSE, 0);

    exitbutton = gtk_button_new_with_label("Exit");
    g_signal_connect(exitbutton, "clicked", G_CALLBACK(on_exitbutton_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), exitbutton, FALSE, FALSE, 0);

    // Show all widgets
    gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;

    gtk_init(&argc, &argv);

    // Create a new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Main Menu");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create the main menu
    reprint_main_menu(window);

    gtk_main();

    /**/

    return 0;
}
