CC = gcc 
CFLAGS = -Wall
LDFLAGS = -I/opt/homebrew/Cellar/sdl2/2.30.11/include -L/opt/homebrew/Cellar/sdl2/2.30.11/lib -lSDL2
EXEC = server_TCP client_TCP server_UDP client_UDP
SRC = server_TCP.c client_TCP.c server_UDP.c client_UDP.c 
OBJ = $(SRC:.c=.o)

all : $(EXEC)

server_TCP : server_TCP.o 
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

client_TCP : client_TCP.o 
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

server_UDP : server_UDP.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

client_UDP : client_UDP.o 
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)




%.o : %.c 
	$(CC) $(CFLAGS) -o $@ -c $< $(LDFLAGS)

.PHONY : clean mrproper all 

clean : 
	rm -f $(OBJ)

mrproper : clean
	rm -f $(EXEC)

