#include "TXLib.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

//-----------------------------------------------------------------------------

int CreateFileConfig (const char name_file[50]);

//-----------------------------------------------------------------------------

int main ()
    {
    int  number_hero = 0,   size_hero = 0;
    char name_hero[50] = "";

    if (access ("configuration.txt", 0) != 0) CreateFileConfig ("configuration.txt");

    FILE *file_config_r = fopen ("configuration.txt", "r");

    if (file_config_r == NULL)
        {
        printf ("Ошибка при открытии файла с конфигурацией, для чтения");
        fclose (file_config_r);
        return 0;
        }

    for (int str = 1; !feof (file_config_r); str++)
            {
            char text[100] = "";
            fgets (text, sizeof (text), file_config_r);

            if (text[0] == '\0') continue;

            if (sscanf (text, " %d_%d_%[^.].bmp", &number_hero, &size_hero, name_hero) == 3)
                printf ("номер героя: %d, размер: %d, имя героя: %s\n", number_hero, size_hero, name_hero);
            else
                {
                printf ("Ошибка чтения configuration.txt (%d): неверный формат имени файла %s\n", str, text);

                break;
                }
            }

    fclose (file_config_r);

    return 0;
    }

//-----------------------------------------------------------------------------
int CreateFileConfig (const char name_file[50])
    {
    DIR *dir_game = opendir("v1_Images");

    if (!dir_game)
        {
        perror ("При открытии директории, произошла ошибка");
        return 0;
        }

    FILE *file_config_w = fopen (name_file, "w");

    if (file_config_w == NULL)
        {
        printf ("Ошибка при открытии файла с конфигурацией, на запись");
        closedir (dir_game);
        return 0;
        }

    int n = 0;
    while (true)
        {
        dirent *file_data = readdir(dir_game);

        if (file_data == NULL) break;

        n = n + 1;
        //printf ("%d, %d, %s\n", n, file_data -> d_namlen, file_data -> d_name);

        if (file_data -> d_name[0] == '.') continue;

        fprintf (file_config_w, "%s\n", file_data -> d_name);
        }

    fclose  (file_config_w);

    closedir (dir_game);

    return 1;
    }
