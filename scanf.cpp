#include "TXLib.h"
#include <stdio.h>

//-----------------------------------------------------------------------------

int main ()
    {
    int  number_hero = 0,   size_hero = 0;
    char name_hero[50] = "";

    printf ("������� �������� �����\n");

    if (scanf ("%d_%d_%[a-z-A-Z]",&number_hero, &size_hero, name_hero) == 3)
        printf ("����� �����: %d, ������: %d, ��� �����: %s", number_hero, size_hero, name_hero);
    else printf ("���� ��� ������ ����� �����");

    return 0;
    }
