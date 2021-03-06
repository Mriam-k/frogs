#include "TXLib.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

const double GLOBAL_SLEEP = txQueryPerformance () * 100;

const int CAPTURE_BALL = 1;
const int FREE_BALL    = 0;

const int N_BALLS = 7;

const int BUTTON_USERNAME         = 0;
const int BUTTON_HEROES_1       = 2;
const int BUTTON_HEROES_2 = 3;
const int BUTTON_HEROES_3 = 4;
const int BUTTON_HEROES_SIZE_40   = 6;
const int BUTTON_HEROES_SIZE_60   = 7;

const bool DEBUG = false;

const int SIZ_STR = 50;

//-----------------------------------------------------------------------------

struct Ball
    {
    int x,  y;
    int vx, vy;
    int r;
    HDC picture;
    int state_ball;

    void Drive ();
    void MoveStrategiya (int dt);
    void Level_game ();
    };

struct Button
    {
    RECT     area;
    COLORREF color;
    char     text[50];

    void Drow_Button ();
    };

//-----------------------------------------------------------------------------

void     MoveBalls                (char* name_user, int* continue_game, int* r_ball, int* number_main_heroes,
                                   char* name_main_heroes, char* heroes_4, const char* folder_heroes);

void     DrawBackground           ();
void     Count                    (int* balli, int* level, int* counter, Ball balls[]);
void     CalculateBalli           (Ball balls[], int* balli);
double   CalculateDistance        (Ball *b1, Ball *b2);

int      ReadFromFile             (int* balli, int* level, Ball balls[], char* name_user);
int      WriteToFile              (int* balli, int* level, Ball balls[], char* name_user);

int      Menu                     (Button buttons[], int N_Button = 0);

int      DialogueWithUser_Username(char* name_user, int* continue_game);

int      Pause                    ();
bool     In_area                  (POINT mouse_pos, RECT area);
void     Rect_Area_Button         (RECT area, COLORREF color, const char text[]);

int      CreateFileConfig         (const char name_file[], const char folder_heroes[]);
int      Read_Configuration_File  (char* heroes_1, char* heroes_2, char* heroes_3, char* heroes_4, char* folder_heroes);

int      Press_Button_MenuSkin    (char* heroes_1,              char* heroes_2,  char* heroes_3,
                                   char* heroes_4,              char* name_user, int*  continue_game,
                                   int*  number_main_heroes,    int* r_ball,     char* name_main_heroes,
                                   char* folder_heroes);

int      Press_Button_Menu        (char* heroes_1,              char* heroes_2,  char* heroes_3,
                                   char* heroes_4,              char* name_user, int* continue_game,
                                   int* number_main_heroes,     int* r_ball,     char* name_main_heroes,
                                   char* folder_heroes);

//-----------------------------------------------------------------------------

int main ()
    {
    txCreateWindow (900, 700);

    txBegin ();

    char folder_heroes[SIZ_STR]    = "v1_Images";
    char heroes_1[SIZ_STR]         = "";
    char heroes_2[SIZ_STR]         = "";
    char heroes_3[SIZ_STR]         = "";
    char heroes_4[SIZ_STR]         = "frog";
    char name_main_heroes[SIZ_STR] = "fly";

    int  number_main_heroes   = 1;

    int  continue_game        = 0;
    int  r_ball               = 30;

    char name_user[100]       = "user";



    Press_Button_MenuSkin (heroes_1, heroes_2, heroes_3, heroes_4, name_user, &continue_game,
                           &number_main_heroes, &r_ball, name_main_heroes, folder_heroes);

    MoveBalls (name_user, &continue_game, &r_ball, &number_main_heroes, name_main_heroes, heroes_4,
               folder_heroes);

    txEnd ();

    return 0;
    }

//-----------------------------------------------------------------------------

void DrawBackground ()
    {
    txSetColor     (TX_ORANGE, 5);
    txSetFillColor (TX_NULL);
    txRectangle (180, 70, 700, 600);
    }

//-----------------------------------------------------------------------------

void MoveBalls (char* name_user, int* continue_game, int* r_ball, int* number_main_heroes,
                char* name_main_heroes, char* heroes_4, const char* folder_heroes)
    {
    char name_file_image[100] = "";

    sprintf (name_file_image, "%s%s%d%s%d%s%s%s", folder_heroes, "\\", *number_main_heroes, "_", *r_ball, "_", name_main_heroes, ".bmp");
    if (DEBUG) printf  ("(%s)", name_file_image);
    HDC main_character = txLoadImage (name_file_image);

    sprintf (name_file_image, "%s%s%d%s%d%s%s%s", folder_heroes, "\\", 4, "_", *r_ball, "_", heroes_4, ".bmp");
    if (DEBUG) printf  (" (%s)", name_file_image);
    HDC character1 = txLoadImage (name_file_image);

    if (!main_character) printf ("�� ���������� �������� �������� ���������");
    if (!character1)     printf ("�� ���������� �������� ��������� N1");

    Ball balls[N_BALLS] = {{320, 110, 0, 0, *r_ball, main_character},
                           {360, 400, 2, 2, *r_ball, character1    },
                           {420, 250, 3, 6, *r_ball, character1    },
                           {320, 630, 4, 1, *r_ball, character1    },
                           {530, 120, 5, 7, *r_ball, character1    },
                           {490, 300, 3, 2, *r_ball, character1    },
                           {510, 600, 2, 5, *r_ball, character1    }};

    int  balli = 0;
    int  level = 1;
    int  dt = 2;
    int  counter = 0;

    txSetTextAlign (TA_CENTER);

    if (*continue_game == 1)
        ReadFromFile (&balli, &level, balls, name_user);

    while (!txGetAsyncKeyState (VK_ESCAPE))
        {
        txSetFillColor (RGB (0, 0, 0));
        txClear ();

        DrawBackground ();

        balls[0].Drive ();

        Count (&balli, &level, &counter, balls);

        for (int i = 0; i < N_BALLS; i++) balls[i].MoveStrategiya (dt);

        CalculateBalli (balls, &balli);

        for (int i = 0; i < N_BALLS; i++)
            txTransparentBlt (txDC(), (balls[i].x - balls[i].r), (balls[i].y - balls[i].r), 0, 0, balls[i].picture);

        char print [100] = "";
        txSetColor (RGB(128, 255, 39), 10);
        sprintf (print, "����� ����� ������ = %d, ������� ���� %d       \r", balli, level);
        txTextOut (txGetExtentX()/2, 5, print);

        txSleep (GLOBAL_SLEEP);
        }

        txDeleteDC (character1);
        txDeleteDC (main_character);

    WriteToFile (&balli, &level, balls, name_user);

    }

//-----------------------------------------------------------------------------

void Ball::Drive ()
    {
    if (GetAsyncKeyState (VK_RIGHT)) this->vx =  4, this->vy =  0;
    if (GetAsyncKeyState (VK_LEFT))  this->vx = -4, this->vy =  0;
    if (GetAsyncKeyState (VK_UP))    this->vx =  0, this->vy = -4;
    if (GetAsyncKeyState (VK_DOWN))  this->vx =  0, this->vy =  4;

    if (GetAsyncKeyState (VK_SHIFT))
        {
        if (this->vx != 0)
            {
            if (this->vx > 0) this->vx = this->vx + 4;
            else              this->vx = this->vx - 4;
            }
        else
            {
            if (this->vy > 0) this->vy = this->vy + 4;
            else              this->vy = this->vy - 4;
            }
        }
    }

//-----------------------------------------------------------------------------

void Ball::MoveStrategiya (int dt)
    {
    this->x  = (this->x) + (this->vx) * dt;
    this->y  = (this->y) + (this->vy) * dt;

    if (this->x >= (695 - r))   this->vx = -this->vx,   this->x = (695 - r);
    if (this->y >= (595 - r))   this->vy = -this->vy,   this->y = (595 - r);
    if (this->x <= (185 + r))   this->vx = -this->vx,   this->x = (185 + r);
    if (this->y <= (75  + r))   this->vy = -this->vy,   this->y = (75  + r);
    }

//-----------------------------------------------------------------------------

void Ball::Level_game ()
    {
    if (this->vx > 0) this->vx = this->vx + 1;
    else              this->vx = this->vx - 1;
    if (this->vy > 0) this->vy = this->vy + 1;
    else              this->vy = this->vy - 1;
    }

//-----------------------------------------------------------------------------

void Count (int* balli, int* level, int* counter, Ball balls[])
    {
    if ((*balli) % 500 == 0 && ((*balli) > (*counter)))
        {
        (*level) ++;
        *counter = *balli;

        for (int i = 1; i < 7; i++) balls[i].Level_game ();
        }
    }

//-----------------------------------------------------------------------------

void CalculateBalli (Ball balls[], int* balli)
    {
    for (int i = 1; i < 7; i++)
        {
        if (CalculateDistance (&balls[0], &balls[i]) <= balls[0].r + balls[i].r)
            {
            if (balls[0].state_ball == FREE_BALL)
                {
                balls[0].vx = 0;
                balls[0].vy = 0;

                *balli = *balli - 10;
                if (*balli < 0) *balli = 0;
                }
            balls[0].state_ball = CAPTURE_BALL;
            return;
            }
        }

    *balli = *balli + 1;
    balls[0].state_ball = FREE_BALL;
    }

//-----------------------------------------------------------------------------

double CalculateDistance (Ball *b1, Ball *b2)
    {
    double r_r = sqrt ((b1->y - b2->y) * (b1->y - b2->y) + (b1->x - b2->x) * (b1->x - b2->x));
    return r_r;
    }

//-----------------------------------------------------------------------------

int ReadFromFile (int* balli, int* level, Ball balls[], char* name_user)
    {
    FILE *file_uzer = fopen (name_user, "r");

    if (file_uzer == NULL) return 1;

    if (fscanf (file_uzer, "����� = %d\n ������� ���� = %d\n", balli, level) != 2)
            {
            printf ("\n ������ ��� ������ ������ ��� ������ � ����� uzer.txt\n");

            fclose  (file_uzer);
            return 0;
            }

    for (int str = 3; !feof (file_uzer); str++)
            {
            int n = -1;

            int end_file = fscanf (file_uzer, " [%d] ", &n);

            if (end_file == EOF) break;

            if (end_file != 1)
                {
                printf ("\n ������ ��� ������ ������ ������ (%d) � ����� uzer.txt � ������ %d\n", n, str);
                break;
                }

            if (!(0 <= n && n < N_BALLS))
                {
                printf ("\n �� ���������� ����� ������ %d � ����� uzer.txt � ������ %d\n", n, str);
                break;
                }

            if (fscanf (file_uzer, "x = %d, y = %d, vx = %d, vy = %d, ", &balls[n].x, &balls[n].y, &balls[n].vx, &balls[n].vy) != 4)
                {
                printf ("\n ������ ��� ������ ��������� ��� ��������� � ����� uzer.txt � ������ %d\n", str);
                break;
                }

            if (balls[n].x < 205) balls[n].x = 205;
            if (balls[n].x > 675) balls[n].x = 675;
            if (balls[n].y < 95 ) balls[n].y = 95;
            if (balls[n].y > 575) balls[n].y = 575;
            if (30 < balls[n].vx || balls[n].vx < -30) balls[n].vx = 3;
            if (30 < balls[n].vy || balls[n].vy < -30) balls[n].vy = 3;
            }

    fclose (file_uzer);

    return 1;
    }

//-----------------------------------------------------------------------------

int WriteToFile (int* balli, int* level, Ball balls[], char* name_user)
    {
    FILE *file_uzer = fopen (name_user, "w");

    if (file_uzer == NULL)
        {
        printf ("��������� ���� �� ��������");
        return 0;
        }

    fprintf (file_uzer, "����� = %d\n"
                        "������� ���� = %d\n", *balli, *level);

    for (int i = 0; i < N_BALLS; i++)
        {
        fprintf (file_uzer, "[%d] ", i);
        fprintf (file_uzer, "x  = %4d, y  = %4d, ", balls[i].x,  balls[i].y);
        fprintf (file_uzer, "vx = %4d, vy = %4d,",   balls[i].vx, balls[i].vy);
        fprintf (file_uzer, "\n");
        }

    fclose  (file_uzer);

    return 1;
    }

//-----------------------------------------------------------------------------

int DialogueWithUser_Username(char* name_user, int* continue_game)
    {
    printf ("\n\n\n\n\n\n\n����� ���� ���\n");
    scanf ("%90s", name_user);
    printf ("����������� ���� %s\n", name_user);

    strcat (name_user, ".txt");

    printf ("���� ������ ������ ���� ������, �����: 0, ���� ����������: 1\n");
    scanf ("%d", continue_game);

    if (*continue_game == 0)
        printf ("����� �������� ����� ����\n\n");
    else
        {
        if (*continue_game == 1)
            printf ("���� ����� ����������\n\n");
        else
            {
            printf ("�� ���� �� ������ �������, �������� ���� � ������\n\n");

            *continue_game = 0;

            txSleep (GLOBAL_SLEEP);
            txClearConsole ( );

            return 1;
            }
        }

    Pause ();
    txSleep (GLOBAL_SLEEP);
    txClearConsole ( );

    return 0;
    }

//-----------------------------------------------------------------------------

int Pause ()
    {
    while (!txGetAsyncKeyState (VK_SPACE))
        {
        txTextCursor (false);
        printf ("��� ����������� ������ � ����, ����� ������\r");
        }
    return 0;
    }

//-----------------------------------------------------------------------------

int Menu (Button buttons[], int N_Button)
    {
    int button_pressed = -1;
    txSelectFont ("Comic Sans MS", 40);

    txBegin ();

    while (true)
        {
        txSetColor (TX_ORANGE, 2);

        for (int i = 0; i < N_Button; i++)
            {
            if (buttons[i].area.left == 0) break;
            buttons[i].Drow_Button ();
            }

        POINT mouse_pos = txMousePos();

        txSleep (GLOBAL_SLEEP);

        if (txMouseButtons() != 1) continue;

        for (int i = 0; buttons[i].area.left != 0 || i < N_Button; i++)
            if (In_area (mouse_pos, buttons[i].area))
                 {
                 printf ("������ ����� %d\n", i);
                 button_pressed = i;

                 txEnd ();

                 txClearConsole ( );

                 return button_pressed;
                 }
        }
    }

//-----------------------------------------------------------------------------

void Button::Drow_Button ()
    {
    txSetFillColor (this->color);
    txRectangle (this->area.left, this->area.top, this->area.right, this->area.bottom);

    int centre = (this->area.right + this->area.left)/2;
    txSetTextAlign (TA_CENTER);
    txTextOut (centre, this->area.top, this->text);
    }

//-----------------------------------------------------------------------------

void Rect_Area_Button (RECT area, COLORREF color, const char text[])
    {
    txSetFillColor (color);
    txRectangle (area.left, area.top, area.right, area.bottom);

    int centre = (area.right + area.left)/2;
    txSetTextAlign (TA_CENTER);
    txTextOut (centre, area.top, text);
    }

//-----------------------------------------------------------------------------

bool In_area (POINT mouse_pos, RECT area)
    {
    return ((area.left <= mouse_pos.x) && (mouse_pos.x <= area.right ) &&
            (area.top  <= mouse_pos.y) && (mouse_pos.y <= area.bottom));
    }

//-----------------------------------------------------------------------------

int CreateFileConfig (const char name_file[], const char folder_heroes[])
    {
    DIR *dir_game = opendir(folder_heroes);

    if (!dir_game)
        {
        perror ("��� �������� ����������, ��������� ������");
        return 0;
        }

    FILE *file_config_w = fopen (name_file, "w");

    if (file_config_w == NULL)
        {
        printf ("������ ��� �������� ����� � �������������, �� ������");
        closedir (dir_game);
        return 0;
        }

    int n = 0;

    fprintf (file_config_w, "%s\n", folder_heroes);

    while (true)
        {
        dirent *file_data = readdir(dir_game);

        if (file_data == NULL) break;

        n = n + 1;

        if (file_data -> d_name[0] == '.') continue;

        fprintf (file_config_w, "%s\n", file_data -> d_name);
        }

    fclose  (file_config_w);

    closedir (dir_game);

    return 1;
    }

//-----------------------------------------------------------------------------

int Read_Configuration_File (char* heroes_1, char* heroes_2, char* heroes_3,
                             char* heroes_4, char* folder_heroes)
    {
    int  number_hero = 0,   size_hero = 0;
    char name_hero[50] = "";

    if (access ("configuration.txt", 0) != 0) CreateFileConfig ("configuration.txt", "v1_Images");

    FILE *file_config_r = fopen ("configuration.txt", "r");

    if (file_config_r == NULL)
        {
        printf ("������ ��� �������� ����� � �������������, ��� ������");
        fclose (file_config_r);
        return 0;
        }

    fgets (folder_heroes, SIZ_STR, file_config_r);
    int len = strlen (folder_heroes);
    folder_heroes [len - 1] = '\0';
    if (DEBUG) printf ("(%s)\n", folder_heroes);

    for (int str = 1; !feof (file_config_r); str++)
            {
            char text[100] = "";
            fgets (text, sizeof (text), file_config_r);

            if (text[0] == '\0') continue;

            if (sscanf (text, " %d_%d_%[^.].bmp", &number_hero, &size_hero, name_hero) == 3)
                {
                if ((number_hero == 1) && (size_hero == 20)) sprintf (heroes_1, "%s", name_hero);
                if ((number_hero == 2) && (size_hero == 20)) sprintf (heroes_2, "%s", name_hero);
                if ((number_hero == 3) && (size_hero == 20)) sprintf (heroes_3, "%s", name_hero);
                if ((number_hero == 4) && (size_hero == 20)) sprintf (heroes_4, "%s", name_hero);
                }
            else
                {
                printf ("������ ������ configuration.txt (%d): �������� ������ ����� ����� %s\n", str, text);
                break;
                }
            }

    fclose (file_config_r);

    return 0;
    }

//-----------------------------------------------------------------------------

int Press_Button_MenuSkin (char* heroes_1, char* heroes_2, char* heroes_3, char* heroes_4, char* name_user,
                           int* continue_game, int* number_main_heroes, int* r_ball, char* name_main_heroes,
                           char* folder_heroes)
    {
    Button buttons_skin[] = {{{210, 70,  445, 115}, RGB (0  , 0  , 0  ), "Skin"       }, // 0
                             {{210, 180, 445, 225}, RGB (255, 255, 70 ), "version N1" }, // 1
                             {{210, 235, 445, 280}, RGB (255, 255, 70 ), "version N2" }, // 2
                             {{480, 207, 665, 252}, RGB (255, 255, 128), "NEXT"       }};// 3

    while (true)
        {
        int pressed_buttons_skin = Menu (buttons_skin, sizeof (buttons_skin) / sizeof (buttons_skin[0]));

        switch (pressed_buttons_skin)
            {
            case 1:
                strncpy (folder_heroes, "v1_Images", SIZ_STR - 1);
                CreateFileConfig ("configuration.txt", folder_heroes);
                printf ("�� ������� skin N1\n");
                if (DEBUG) printf ("����� � ����������: %s", folder_heroes);
                break;

            case 2:
                strncpy (folder_heroes, "v2_Images", SIZ_STR - 1);
                CreateFileConfig ("configuration.txt", folder_heroes);
                printf ("�� ������� skin N2\n");
                if (DEBUG) printf ("����� � ����������: %s", folder_heroes);
                break;

            case 3:
                Read_Configuration_File (heroes_1, heroes_2, heroes_3, heroes_4, folder_heroes);
                txSetFillColor (TX_BLACK);
                txClear();
                if (DEBUG) printf ("1.%s\n2.%s\n3.%s\n4.%s\n", heroes_1, heroes_2, heroes_3, heroes_4);
                Press_Button_Menu (heroes_1, heroes_2, heroes_3, heroes_4, name_user, continue_game,
                                   number_main_heroes, r_ball, name_main_heroes, folder_heroes);
                return 1;

            default:
                break;
            }

        }

    return 1;
    }

//-----------------------------------------------------------------------------

int Press_Button_Menu (char* heroes_1, char* heroes_2, char* heroes_3, char* heroes_4, char* name_user,
                       int* continue_game, int* number_main_heroes, int* r_ball, char* name_main_heroes,
                       char* folder_heroes)
    {
    Button buttons[] = {{{40,  70,  225, 115}, RGB (255, 255, 128), "Username"      }, // 0
                        {{260, 70,  445, 115}, RGB (0  , 0  , 0  ), "Heroes:"       }, // 1
                        {{260, 125, 445, 170}, RGB (255, 255, 70 ), "(Hero 1 name)" }, // 2
                        {{260, 180, 445, 225}, RGB (255, 255, 70 ), "(Hero 2 name)" }, // 3
                        {{260, 235, 445, 280}, RGB (255, 255, 70 ), "(Hero 3 name)" }, // 4
                        {{480, 70,  665, 115}, RGB (0  , 0  , 0  ), "Heroes Size:"  }, // 5
                        {{480, 125, 665, 170}, RGB (255, 255, 70 ), "40 pix"        }, // 6
                        {{480, 180, 665, 225}, RGB (255, 255, 70 ), "60 pix"        }, // 7
                        {{700, 70,  885, 115}, RGB (255, 255, 128), "Game Start"    }, // 8
                        {{700, 180, 885, 225}, RGB (255, 255, 128), "return"        }};// 9

    strncpy (buttons[2].text, heroes_1, sizeof (buttons[2].text) - 1);
    strncpy (buttons[3].text, heroes_2, sizeof (buttons[3].text) - 1);
    strncpy (buttons[4].text, heroes_3, sizeof (buttons[4].text) - 1);

    while (true)
        {
        int pressed_buttons = Menu (buttons, sizeof (buttons) / sizeof (buttons[0]));

        switch (pressed_buttons)
            {
            case BUTTON_USERNAME:
                txUpdateWindow (true);
                DialogueWithUser_Username(name_user, continue_game);
                txUpdateWindow (false);
                break;

            case BUTTON_HEROES_1:
                *number_main_heroes = 1;
                strncpy (name_main_heroes, heroes_1, SIZ_STR-1);
                printf ("������� ������ ����� %s\n", name_main_heroes);
                if (DEBUG) printf ("%d%s%d%s%s%s\n", *number_main_heroes, "_", *r_ball, "_", name_main_heroes, ".bmp");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case BUTTON_HEROES_2:
                *number_main_heroes = 2;
                strncpy (name_main_heroes, heroes_2, SIZ_STR-1);
                printf ("������� ������ ����� %s\n", name_main_heroes);
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case BUTTON_HEROES_3:
                *number_main_heroes = 3;
                strncpy (name_main_heroes, heroes_3, SIZ_STR-1);
                printf ("������� ������ ����� %s\n", name_main_heroes);
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case BUTTON_HEROES_SIZE_40:
                *r_ball = 20;
                printf ("radius balls = 20\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case BUTTON_HEROES_SIZE_60:
                *r_ball = 30;
                printf ("radius balls = 30\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case 8:
                return 1;

            case 9:
                txSetFillColor (TX_BLACK);
                txClear();
                Press_Button_MenuSkin (heroes_1, heroes_2, heroes_3, heroes_4, name_user, continue_game,
                                       number_main_heroes, r_ball, name_main_heroes, folder_heroes);
                return 1;

            default:
                break;
            }
        }

        return 1;
    }

//-----------------------------------------------------------------------------
/*
int Loading_Pictures()
    {
    char name_file_image[100] = "";

    sprintf (name_file_image, "%d%s%d%s%s%s", *number_main_heroes, "_", *r_ball, "_", name_main_heroes, ".bmp");
    HDC main_character = txLoadImage (name_file_image);

    sprintf (name_file_image, "%d%s%d%s%s%s", 4, "_", *r_ball, "_", ,".bmp");
    HDC character1 = txLoadImage (name_file_image);

    if (!main_character) printf ("�� ���������� �������� �������� ���������");
    if (!character1)     printf ("�� ���������� �������� ��������� N1");
    }
*/
