#include "TXLib.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
//#include "R_W_Color_in_from_file.h"

const double GLOBAL_SLEEP = txQueryPerformance () * 100;

const int CAPTURE_BALL = 1;
const int FREE_BALL    = 0;

const int N_BALLS = 7;

const int BUTTON_USERNAME         = 0;
const int BUTTON_HEROES_FLY       = 2;
const int BUTTON_HEROES_DRAGONFLY = 3;
const int BUTTON_HEROES_BUMBLEBEE = 4;
const int BUTTON_HEROES_SIZE_40   = 6;
const int BUTTON_HEROES_SIZE_60   = 7;
const int VERSION_GAME_v1         = 9;
const int VERSION_GAME_v2         = 10;

//-----------------------------------------------------------------------------

struct Ball
    {
    int x, y;
    int vx, vy;
    int r;
    HDC picture;
    //COLORREF color_contour, color_ball;
    //int thickness_contour;
    int state_ball;

    //void Drow ();
    void Drive ();
    void MoveStrategiya (int dt);
    void Level_game ();
    };

struct Button
    {
    RECT area;
    COLORREF color;
    char text[50];

    void Drow_Button ();
    };

//-----------------------------------------------------------------------------

//void     MoveBalls                (char* name_user, int* continue_game, COLORREF* ball_0_color, int* r_ball, int* sign_color_change);
void     MoveBalls                (char* name_user, int* continue_game, int* r_ball, int* main_heroes, char* dislocation_file);
void     DrawBackground           ();
void     Count                    (int* balli, int* level, int* counter, Ball balls[]);
void     CalculateBalli           (Ball balls[], int* balli);
double   CalculateDistance        (Ball *b1, Ball *b2);

int      ReadFromFile             (int* balli, int* level, Ball balls[], char* name_user);
int      WriteToFile              (int* balli, int* level, Ball balls[], char* name_user);

int      Menu               (Button buttons[], int N_Button = 0);

int      DialogueWithUser_Username(char* name_user, int* continue_game);

int      Pause                    ();
bool     In_area                  (POINT mouse_pos, RECT area);
void     Rect_Area_Button         (RECT area, COLORREF color, const char text[]);

int CreateFileConfig (const char name_file[50]);

//-----------------------------------------------------------------------------

int main ()
    {
    txCreateWindow (900, 700);
    //char dislocation_file[50] = "";
    //char dislocation_file[50] = "v3_Images";



    Button buttons[] = {{{700, 70,  885, 115}, RGB (255, 255, 70 ), "Username"     },
                        {{260, 70,  445, 115}, RGB (0  , 0  , 0  ), "Heroes:"      },
                        {{260, 125, 445, 170}, RGB (255, 255, 70 ), "fly"          },
                        {{260, 180, 445, 225}, RGB (255, 255, 70 ), "dragonfly"    },
                        {{260, 235, 445, 280}, RGB (255, 255, 70 ), "bumblebee"    },
                        {{480, 70,  665, 115}, RGB (0  , 0  , 0  ), "Heroes Size:" },
                        {{480, 125, 665, 170}, RGB (255, 255, 70 ), "40 pix"       },
                        {{480, 180, 665, 225}, RGB (255, 255, 70 ), "60 pix"       },
                        {{40,  70,  225, 115}, RGB (0  , 0  , 0  ), "Skin"         },
                        {{40,  125, 225, 170}, RGB (255, 255, 70 ), "version v1"   },
                        {{40,  180, 225, 225}, RGB (255, 255, 70 ), "version v2"   },
                        {{700, 180, 885, 225}, RGB (255, 255, 128), "Game Start"   }};

    char name_user[100] = "user";
    int continue_game = 0;
    int r_ball = 30;
    int main_heroes = 1;

    txBegin ();

    while (true)
        {
        int pressed_buttons = Menu (buttons, sizeof (buttons) / sizeof (buttons[0]));

        switch (pressed_buttons)
            {
            case BUTTON_USERNAME:
                txUpdateWindow (true);
                DialogueWithUser_Username(name_user, &continue_game);
                txUpdateWindow (false);
                break;

            case BUTTON_HEROES_FLY:
                main_heroes = 1;
                printf ("������� ������ ����� ����\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case BUTTON_HEROES_DRAGONFLY:
                main_heroes = 2;
                printf ("������� ������ ����� ��������\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case BUTTON_HEROES_BUMBLEBEE:
                main_heroes = 3;
                printf ("������� ������ ����� �����\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case BUTTON_HEROES_SIZE_40:
                r_ball = 20;
                printf ("radius balls = 20\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case BUTTON_HEROES_SIZE_60:
                r_ball = 30;
                printf ("radius balls = 30\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case VERSION_GAME_v1:
                sprintf (dislocation_file, "%s", "v1_Images");
                printf ("version of game = v1_Images\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            case VERSION_GAME_v2:
                sprintf (dislocation_file, "%s", "v2_Images");
                printf ("version of game = v2_Images\n");
                txSetFillColor (TX_BLACK);
                txClear ();
                break;

            default:
                break;
            }

        if (pressed_buttons == 11) break;
        }

    MoveBalls (name_user, &continue_game, &r_ball, &main_heroes, dislocation_file);

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

void MoveBalls (char* name_user, int* continue_game, int* r_ball, int* main_heroes, char* dislocation_file)
    {
    char name_file_image[100] = "";

    sprintf (name_file_image, "%s%s%d%s%d%s", dislocation_file, "\\", *main_heroes, "_", *r_ball, ".bmp");
    HDC main_character = txLoadImage (name_file_image);
    //txTextOut (txGetExtentX()/2, 5, name_file_image);

    sprintf (name_file_image, "%s%s%d%s", dislocation_file, "\\N1_", *r_ball, ".bmp");
    HDC character1 = txLoadImage (name_file_image);

    //sprintf (name_file_image, "%s%s%d%s", dislocation_file, "\\N2_", *r_ball, ".bmp");
    //HDC character2 = txLoadImage (name_file_image);

    //sprintf (name_file_image, "%s%s%d%s", dislocation_file, "\\N3_", *r_ball, ".bmp");
    //HDC character3 = txLoadImage (name_file_image);

    if (!main_character) printf ("�� ���������� �������� �������� ���������");
    if (!character1)     printf ("�� ���������� �������� ��������� N1");
    //if (!character2)     printf ("�� ���������� �������� ��������� N2");
    //if (!character3)     printf ("�� ���������� �������� ��������� N3");

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
        //{
        ReadFromFile (&balli, &level, balls, name_user);
        //if (*sign_color_change == 1)
        //    {
        //    balls[0].color_ball    = *ball_0_color;
        //    balls[0].color_contour = *ball_0_color;
        //    }
        //}

    while (!txGetAsyncKeyState (VK_ESCAPE))
        {
        txSetFillColor (RGB (0, 0, 0));
        txClear ();

        DrawBackground ();

        balls[0].Drive ();

        Count (&balli, &level, &counter, balls);

        for (int i = 0; i < N_BALLS; i++) balls[i].MoveStrategiya (dt);

        CalculateBalli (balls, &balli);

        for (int i = 0; i < N_BALLS; i++) //balls[i].Drow ();
            txTransparentBlt (txDC(), (balls[i].x - balls[i].r), (balls[i].y - balls[i].r), 0, 0, balls[i].picture);

        char print [100] = "";
        txSetColor (RGB(128, 255, 39), 10);
        sprintf (print, "����� ����� ������ = %d, ������� ���� %d       \r", balli, level);
        txTextOut (txGetExtentX()/2, 5, print);

        txSleep (GLOBAL_SLEEP);
        }

        txDeleteDC (character1);
        //txDeleteDC (character2);
        //txDeleteDC (character3);
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

//void Ball::Drow ()
//    {
//    txSetColor     (this->color_contour, this->thickness_contour);
//    txSetFillColor (color_ball);
//    txCircle (x, y, r);
//    }

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

            //if (n == 0)
            //    {
            //    balls[0].color_ball = ReadFromFile_Color (file_uzer);
            //    balls[0].color_contour = balls[0].color_ball;
            //    }
            //else ReadFromFile_Color (file_uzer);

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
        //WriteToFile_Color (balls[i].color_ball, file_uzer);
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
int CreateFileConfig (const char name_file[50])
    {
    DIR *dir_game = opendir("v1_Images");

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
