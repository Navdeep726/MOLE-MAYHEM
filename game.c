/* MODULE 3: game.c  -  gameplay
   Holes, moles, clicking on moles, score and lives during the game,
   and saving the score to the difficulty file when the game ends. */

#include "raylib.h"
#include <stdio.h>
#include <string.h>

/* functions of this module */
void create_game(char map[100], int difficulty);
void draw_mole(int difficulty);
void hole_coordinate(int mode);
void holes(int mode);
void check_hit();
void draw_hud();
void game_over();

/* variables from module 1: main.c */
extern int currentscreen;
extern int difficulty;
extern char player_name[100];
extern Texture2D moleTex[3];
extern Texture2D mapBg[5];

/* variables from module 4: scores.c */
extern int lastScore;
extern int scoreHistory[100];
extern int scoreCount;
extern char *scoreFiles[3];

Vector2 holePos[16];
int holeCount = 0;
int activeHole = -1;
double spawnTime = 0;

int score = 0;
int lives = 3;

float hard_radius = 46;

void hole_coordinates(int mode)
{
    holeCount = 0;

    /* FIXED: added braces so holeCount++ runs for every hole */
    if (mode == 1)   // easy: 6 holes
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                holePos[holeCount] = (Vector2){413.33f + i*386.67f, 500 + j*220};
                holeCount++;
            }
        }
    }
    else if (mode == 2)   // medium: 9 holes
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                holePos[holeCount] = (Vector2){413.33f + i*386.67f, 463.33f + j*146.67f};
                holeCount++;
            }
        }
    }
    else if (mode == 3)   // hard: 16 holes
    {
        /* FIXED: assign first, then increment (was writing to holePos[16]) */
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 2; j++){
                holePos[holeCount] = (Vector2){397.5f + i*230, 430 + j*250};
                holeCount++;
            }
            for (int k = 0; k < 2; k++){
                 holePos[holeCount] = (Vector2){512.5f + i*230, 555 + k*250};
                 holeCount++;
            }
        }
    }

    activeHole = -1;   // reset the mole whenever a new game is set up
    score = 0;         // NEW: fresh score for every new game
    lives = 3;         // NEW: fresh lives for every new game
}

void holes(int mode)
{
    switch(mode)
    {
        case 1:   // easy: 6 holes
            for(int i = 0; i < 3; i++)
            {
                for(int j = 0; j < 2; j++)
                {
                    float posx = 413.33f + (i * 386.67f);
                    float posy = 500 + (j * 220);
                    DrawCircle(posx, posy, 88, BLACK);
                    DrawCircleLines(posx, posy, 88, BEIGE);
                }
            }
            break;

        case 2:   // medium: 9 holes
            for(int i = 0; i < 3; i++)
            {
                for(int j = 0; j < 3; j++)
                {
                    float posp = 413.33f + (i * 386.67f);
                    float posq = 463.33f + (j * 146.67f);
                    DrawCircle(posp, posq, 58.67f, BLACK);
                    DrawCircleLines(posp, posq, 58.67f, BEIGE);
                }
            }
            break;

        case 3:   // hard: 16 holes, staggered
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 2; j++)
                {
                    float posa = 397.5f + (i * 230);
                    float posb = 430 + (j * 250);      // rows at 430 and 680
                    DrawCircle(posa, posb, 46, BLACK);
                    DrawCircleLines(posa, posb, 46, BEIGE);
                }
                for(int k = 0; k < 2; k++)
                {
                    float posc = 512.5f + (i * 230);
                    float posd = 555 + (k * 250);      // rows at 555 and 805
                    DrawCircle(posc, posd, 46, BLACK);
                    DrawCircleLines(posc, posd, 46, BEIGE);
                }
            }
            break;
    }
}

void create_game(char map[100], int difficulty)
{
    if      (strcmp(map, "night")   == 0) DrawTexture(mapBg[0], 0, 0, WHITE);
    else if (strcmp(map, "village") == 0) DrawTexture(mapBg[1], 0, 0, WHITE);
    else if (strcmp(map, "sunset")  == 0) DrawTexture(mapBg[3], 0, 0, WHITE);
    else if (strcmp(map, "fence")   == 0) DrawTexture(mapBg[4], 0, 0, WHITE);
    else                                  DrawTexture(mapBg[2], 0, 0, WHITE);

    holes(difficulty);
    draw_mole(difficulty);
    check_hit();     // NEW
    draw_hud();      // NEW


}

void draw_mole(int difficulty)
{
    double now = GetTime();

    if (holeCount == 0) return;

    if (activeHole == -1 || now - spawnTime >= 1.0)
    {
        activeHole = GetRandomValue(0, holeCount - 1);
        spawnTime = now;
    }

    Texture2D mole = moleTex[difficulty - 1];   // NEW: pick the mole for this difficulty

    DrawTexture(mole,
                holePos[activeHole].x - mole.width / 2,
                holePos[activeHole].y - mole.height / 2,
                WHITE);
}

void check_hit()
{
    // only do something on the frame the left button is clicked
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    if (activeHole == -1) return;

    // hitbox = the hole the mole is in (same radius holes() draws with)
    float radius;
    if      (difficulty == 1) radius = 88;
    else if (difficulty == 2) radius = 58.67f;
    else                      radius = hard_radius;   // 46

    if (CheckCollisionPointCircle(GetMousePosition(), holePos[activeHole], radius))
    {
        score++;                 // hit!
        activeHole = -1;         // mole disappears, a new one spawns next frame
        printf("hit! score: %d\n", score);
    }
    else
    {
        lives--;                 // missed
        printf("miss! lives left: %d\n", lives);

        if (lives <= 0)
            game_over();
    }
}

void draw_hud()
{
    DrawText(TextFormat("Score: %d", score), 1300, 20, 40, YELLOW);
    DrawText(TextFormat("Lives: %d", lives), 1300, 70, 40, RED);
}

void game_over()
{
    lastScore = score;

    if (scoreCount < 100)
    {
        scoreHistory[scoreCount] = score;   // store this game's score
        scoreCount++;
    }

    // NEW: keep adding the score to the file of this difficulty
    FILE *fp = fopen(scoreFiles[difficulty - 1], "a");
    if (fp != NULL)
    {
        fprintf(fp, "%s %d\n", player_name, score);
        fclose(fp);
    }

    // NEW: also add it to player_scores.txt so display_score() in
    // login_register.c (console option 4) still shows every score
    fp = fopen("player_scores.txt", "a");
    if (fp != NULL)
    {
        fprintf(fp, "%s %d\n", player_name, score);
        fclose(fp);
    }

    printf("GAME OVER! final score: %d\n", score);

    activeHole = -1;
    currentscreen = 6;           // CHANGED: was 1, now goes to game over screen
}
