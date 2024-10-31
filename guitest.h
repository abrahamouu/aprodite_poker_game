#ifndef GUITEST_H
#define GUITEST_H

#include <gtk/gtk.h>




extern int seat_number1;
extern int seat_number2;
extern int seat_number3;
extern int seat_number4;
extern int seat_number;


// Function prototypes
GtkWidget *card_placeholders[14];

void create_poker_table(GtkWidget *window);

void set_card_image(GtkWidget *card, const char *file_path);
void on_exitbutton_clicked(GtkButton *button, gpointer user_data);


/*action function calls*/




char *Talk2Server(const char *Message, char *RecvBuf);

#endif // GUITEST_H
