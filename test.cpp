#include "TXLib.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>


//-----------------------------------------------------------------------------
int main ()
    {
    HDC main_character = txLoadImage ("v2_Images\1_30_worm2.bmp");
    if (!main_character) printf ("Ќе заружаетс€ картинка главного персонажа");



    txDeleteDC (main_character);

    return 1;
    }
