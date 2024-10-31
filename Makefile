# Makefile: Poker


#FLAGS= -Wall -std=c11 -O2
FLAGS= -Wall -std=c11 -g
#FLAGS= -Wall -std=c11 -g -DDEBUG
DEBUG= -O2
CFLAGS= -Wall -std=c11 $(DEBUG) $(GTKINC) -c
LFLAGS= -Wall -std=c11 $(DEBUG) $(GTKLIBS) -lm
  
# compiler flags for GTK usage
GTKINC	= `pkg-config --cflags gtk+-2.0`
GTKLIBS	= `pkg-config --libs gtk+-2.0 cairo`
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-2.0)
SRCS = pokerguiserver.c guitest.c
CLIENT = pokerclient
SERVER = pokerserver
GUISERVER = GTK_Server
TARGET = poker_table


all: $(CLIENT) $(SERVER) $(TARGET) 


tar: 
	gtar -czvf Poker_V1.0_src.tar.gz README COPYRIGHT INSTALL Makefile bin doc src

clean:
	rm -f *.o $(CLIENT) $(SERVER) $(TARGET) $(GUISERVER)
	rm -f poker client pokerclient pokerserver pokercommclient copy poker_table gtkserver
# residual file cleanup
	rm -f betaserver clientcopy copy1 

	
test_server: pokertestserver

test_client: pokertest0

test: pokertest

test_comm: pokertestcomm

run-gui-on-open: poker_table
	./poker_table &

pokerclient: pokerclient.c
	gcc pokerclient.c $(FLAGS) -o pokerclient

test_gui: run-gui-on-open
	@echo "opening gui"

pokertest: pokerclient pokerserver
	./pokerserver 10030 & ./pokerclient crystalcove 10030 

pokerserver: pokerserver.c
	gcc pokerserver.c $(FLAGS) -o pokerserver

pokertestserver: pokerserver
	./pokerserver 10031

pokercommclient: pokertestclient.c
	gcc pokertestclient.c $(FLAGS) -o pokercommclient

pokertestcomm: pokercommclient
	./pokerserver 10039 & ./pokercommclient crystalcove 10039

poker_table: $(SRCS)
	gcc -std=c99  $(GTK_CFLAGS) -o $(TARGET) $(SRCS) $(GTK_LIBS)

# EOF
