# Wojciech Fica
# 280180

CC = gcc
CFLAGS  =  -std=c99 #-Wall -Wextra

default: main

main: fifo.o kolejka.o main.o
	$(CC) $(CFLAGS) -o game main.o kolejka.o fifo.o `pkg-config --cflags --libs gtk+-3.0`

main.o: main.c kolejka.h fifo.h plansza.h
	$(CC) $(CFLAGS) -c main.c `pkg-config --cflags --libs gtk+-3.0`

kolejka.o: kolejka.c kolejka.h
	$(CC) $(CFLAGS) -c kolejka.c 

fifo_projekt.o: fifo.c fifo.h
	$(CC) $(CFLAGS) -c fifo.c

clean: 
	$(RM) game *.o *~