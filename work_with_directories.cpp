#include "TXLib.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

//-----------------------------------------------------------------------------

int main ()
    {
    DIR *dir_game = opendir("v1_Images");

    if (!dir_game)
        {
        perror("При открытии директории, произошла ошибка: ");
        return 0;
        }

    int n = 0;
    while (true)
        {
        dirent *file_data = readdir(dir_game);

        if (file_data == NULL) break;

        n = n + 1;
        printf ("%d, %d, %s\n", n, file_data -> d_namlen, file_data -> d_name);
        }

    closedir (dir_game);

    return 0;
    }

//-----------------------------------------------------------------------------
