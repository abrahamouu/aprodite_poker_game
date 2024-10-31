#ifndef POKERGUISERVER_H
#define POKERGUISERVER_H

#include <gtk/gtk.h>

extern int seat_number;
extern int seat_number1; 
extern int seat_number2;
extern int seat_number3;
extern int seat_number4;

char *Talk2Server(const char *Message, char *RecvBuf);
void FatalError(const char *ErrorMsg);
void ShutdownServer(GtkWidget *Widget, gpointer Data);

#endif