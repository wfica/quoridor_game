/*
    Wojciech Fica
    index: 280180
    zad 1 lista 6
*/

#include<stdbool.h>

#define MAXINQUEUE 400

typedef struct kolejka{
    int tab[MAXINQUEUE];
    int dl;
    int *pocz, *kon;
} Queue;

void Clear( Queue *wsk);
bool Add( int elem, Queue *wsk);
bool Get( int *elem, Queue *wsk);
bool Isempty(Queue *wsk);


