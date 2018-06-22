
/*
    Wojciech Fica
    index: 280180
    zad 1 lista 6
*/


#include "kolejka.h"

#include<stdbool.h>

void Clear( Queue *wsk)
{
    wsk->dl=0;
    wsk->kon= wsk ->tab;
    wsk->pocz=wsk -> tab;
}

bool Add( int elem, Queue *wsk)
{
    if( wsk->dl == MAXINQUEUE ) return 0;

    *(wsk->kon) = elem;

    wsk->dl = wsk -> dl +1;

    wsk-> kon  = wsk -> kon +1;
    if( wsk->kon  == wsk->tab + MAXINQUEUE ) wsk ->kon = wsk->tab;

    return 1;
}

bool Get( int *elem, Queue *wsk)
{
    if( Isempty(wsk)) return 0;

    *elem = *(wsk->pocz);

    wsk->pocz = wsk->pocz + 1;
    if( wsk->pocz  == wsk->tab + MAXINQUEUE ) wsk ->pocz = wsk->tab;

    wsk -> dl = wsk -> dl -1;

    return 1;
}

bool Isempty(Queue *wsk)
{
    return wsk->dl == 0 ? 1 :0;
}



