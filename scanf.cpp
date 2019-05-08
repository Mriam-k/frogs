#include "TXLib.h"
#include <stdio.h>

//-----------------------------------------------------------------------------

int main ()
    {
    int  number_hero = 0,   size_hero = 0;
    char name_hero[50] = "";

    printf ("¬ведите название файла\n");

    if (scanf ("%d_%d_%[a-z-A-Z]",&number_hero, &size_hero, name_hero) == 3)
        printf ("номер геро€: %d, размер: %d, им€ геро€: %s", number_hero, size_hero, name_hero);
    else printf ("сбой при чтении имени файла");

    return 0;
    }
