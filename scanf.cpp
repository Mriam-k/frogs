#include "TXLib.h"
#include <stdio.h>

//-----------------------------------------------------------------------------

int main ()
    {
    int  number_hero = 0,   size_hero = 0, x = 0;
    char name_hero[50] = "";

    printf ("¬ведите название файла\n");

    if (scanf ("%d_%d_%[a-z-A-Z].bmp%d",&number_hero, &size_hero, name_hero, &x) == 4)
        printf ("номер геро€: %d, размер: %d, им€ геро€: %s, x=%d\n", number_hero, size_hero, name_hero, x);
    else printf ("сбой при чтении имени файла\n");

    return 0;
    }
