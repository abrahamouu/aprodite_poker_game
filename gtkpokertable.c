#include <gtk/gtk.h>
#include <stdlib.h>

//Callback functions for button clicks 
void on_foldbutton_clicked (){

}

void on_callbutton_clicked(){

}

void on_raisebutton_clicked(){

}

void on_checkbutton_clicked(){

}

void Poker_Table_Initiate(GtkWidget *window) {

    GtkWidget *table;
    GtkWidget *card1, *card2;
    GtkWidget *label;
    GtkWidget *hbox, *vbox;
    GtkWidget *foldbutton, *callbutton, *raisebutton, *checkbutton;

    // create a vertical box to hold the table and buttons within 
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // creates a horizontal box to store the 4 action buttons at the bottom  
    hbox = gtk_hbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(event_box), hbox);

    foldbutton = gtk_button_new_with_label("FOLD");
    g_signal_connect(foldbutton, "clicked", G_CALLBACK(on_foldbutton_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), foldbutton, FALSE, FALSE, 0);

    callbutton = gtk_button_new_with_label("CALL");
    g_signal_connect(callbutton, "clicked", G_CALLBACK(on_callbutton_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), callbutton, FALSE, FALSE, 0);

    raisebutton = gtk_button_new_with_label("RAISE");
    g_signal_connect(raisebutton, "clicked", G_CALLBACK(on_raisebutton_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), raisebutton, FALSE, FALSE, 0);

    checkbutton = gtk_button_new_with_label("CHECK");
    g_signal_connect(checkbutton, "clickec", G_CALLBACK(on_checkbutton_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), checkbutton, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

}

int main(int argc, char *argv[]){
    GtkWidget *window;
    
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Poker Table");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 450);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create an event box to set the background color
    event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(window), event_box);

    // Set the initial background color to green
    GdkColor color;
    gdk_color_parse("green", &color);
    gtk_widget_modify_bg(event_box, GTK_STATE_NORMAL, &color);

    Poker_Table_Initiate(window);

    gtk_main();

    return 0;

}


