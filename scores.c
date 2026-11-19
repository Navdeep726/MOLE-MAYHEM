/* MODULE 4: scores.c  -  scores and leaderboard
   The game over screen, the leaderboard, the my high scores screen,
   and reading the top 3 scores from the easy / medium / hard files. */

#include "raylib.h"
#include <stdio.h>
#include <string.h>

typedef struct {    // same structure as in login_register.c
    char username[100];
    int score;
} ps;

/* functions of this module */
void game_over_screen();
void leaderboard();
void my_scores();
void top3(char *file, char *player, ps best[3]);
void load_leaderboard();
void load_my_scores();
int highest();

/* from module 1: main.c */
extern int currentscreen;
extern int difficulty;
extern int quitGame;
extern char player_name[100];
extern Texture2D gameOverBg, leaderboardBg, myScoresBg;
void draw_centered(const char *text, int cx, int y, int size, Color c);

/* from module 3: game.c */
void build_holes(int mode);

int lastScore = 0;
int scoreHistory[100];
int scoreCount = 0;
char *scoreFiles[3] = {"easy_scores.txt", "medium_scores.txt", "hard_scores.txt"};
ps board[3][3];
ps mine[3][3];

void game_over_screen()
{
    DrawTexture(gameOverBg, 0, 0, WHITE);

    // score inside the "Your Score:" box
    draw_centered(TextFormat("%d", lastScore), 800, 385, 70, YELLOW);

    Vector2 mouse = GetMousePosition();

    Rectangle areas[3] = {
        {505, 487, 590, 96},     // 0 play again
        {505, 605, 590, 96},     // 1 main menu
        {505, 732, 590, 101}     // 2 exit
    };

    Color glow = {255, 255, 255, 70};

    for (int i = 0; i < 3; i++)
    {
        if (CheckCollisionPointRec(mouse, areas[i]))
        {
            DrawRectangleRec(areas[i], glow);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (i == 0)                   // same map and difficulty again
                {
                    build_holes(difficulty);  // also resets score and lives
                    currentscreen = 4;
                }
                else if (i == 1)
                    currentscreen = 1;
                else
                    quitGame = 1;
            }
        }
    }
}

void leaderboard()
{
    DrawTexture(leaderboardBg, 0, 0, WHITE);

    int playerX[3] = {289, 801, 1313};   // middle of "Player" column: easy, medium, hard
    int scoreX[3]  = {440, 954, 1466};   // middle of "Score" column
    int coverX[3]  = {205, 715, 1228};   // where the "---" area starts
    int rowY[3]    = {451, 528, 606};    // middle of rank 1, 2, 3 rows
    Color rowBrown = {133, 71, 24, 255}; // colour of the rows in the picture

    for (int d = 0; d < 3; d++)
    {
        for (int r = 0; r < 3; r++)
        {
            if (board[d][r].score < 0) continue;     // empty place, keep the "---"

            DrawRectangle(coverX[d], rowY[r] - 20, 285, 40, rowBrown);   // hide "---"
            draw_centered(board[d][r].username, playerX[d], rowY[r] - 14, 28, WHITE);
            draw_centered(TextFormat("%d", board[d][r].score), scoreX[d], rowY[r] - 14, 28, YELLOW);
        }
    }

    draw_centered("Press ESC to go back", 802, 862, 30, BLACK);   // shadow
    draw_centered("Press ESC to go back", 800, 860, 30, WHITE);

    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
        currentscreen = 1;
}

void my_scores()
{
    DrawTexture(myScoresBg, 0, 0, WHITE);

    DrawText(player_name, 772, 191, 30, WHITE);      // inside the "Player:" box

    int centreX[3] = {351, 870, 1384};   // middle of the rows: easy, medium, hard
    int rowY[3]    = {417, 512, 608};    // middle of rank 1, 2, 3 rows

    for (int d = 0; d < 3; d++)
    {
        for (int r = 0; r < 3; r++)
        {
            if (mine[d][r].score < 0)
                draw_centered("---", centreX[d], rowY[r] - 18, 36, BEIGE);
            else
                draw_centered(TextFormat("%d", mine[d][r].score), centreX[d], rowY[r] - 18, 36, YELLOW);
        }
    }

    Vector2 mouse = GetMousePosition();
    Rectangle backarea = {555, 723, 490, 108};
    Color glow = {255, 255, 255, 70};

    if (CheckCollisionPointRec(mouse, backarea))
    {
        DrawRectangleRec(backarea, glow);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentscreen = 1;
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
        currentscreen = 1;
}

/* Reads a score file and keeps the 3 highest scores in best[].
   player == NULL : everyone, but only each player's best score (leaderboard)
   player != NULL : only that player's scores (my high scores)
   An empty place has score -1.                                           */
void top3(char *file, char *player, ps best[3])
{
    for (int i = 0; i < 3; i++)
    {
        strcpy(best[i].username, "---");
        best[i].score = -1;
    }

    FILE *fp = fopen(file, "r");
    if (fp == NULL) return;              // no games played yet, that's fine

    ps p1;
    while (fscanf(fp, " %99s %d", p1.username, &p1.score) == 2)
    {
        if (player != NULL && strcmp(player, p1.username) != 0)
            continue;                    // not this player's score

        if (player == NULL)              // leaderboard: one place per player
        {
            int skip = 0;
            for (int i = 0; i < 3; i++)
            {
                if (strcmp(best[i].username, p1.username) == 0)
                {
                    if (p1.score <= best[i].score)
                        skip = 1;        // already has a better score there
                    else
                    {
                        for (int k = i; k < 2; k++)   // remove old score
                            best[k] = best[k + 1];
                        strcpy(best[2].username, "---");
                        best[2].score = -1;
                    }
                    break;
                }
            }
            if (skip) continue;
        }

        // same idea as highest() in login_register.c
        if (p1.score > best[0].score)
        {
            best[2] = best[1];
            best[1] = best[0];
            best[0] = p1;
        }
        else if (p1.score > best[1].score)
        {
            best[2] = best[1];
            best[1] = p1;
        }
        else if (p1.score > best[2].score)
        {
            best[2] = p1;
        }
    }
    fclose(fp);
}

void load_leaderboard()
{
    for (int d = 0; d < 3; d++)
        top3(scoreFiles[d], NULL, board[d]);
}

void load_my_scores()
{
    for (int d = 0; d < 3; d++)
        top3(scoreFiles[d], player_name, mine[d]);
}

/* moved from login_register.c: console version of the overall top 3 search */
int highest(){
    FILE *ptr=fopen("player_scores.txt","r");
    ps p1;
    ps p2[3];
    p2[0].score=0;
    p2[1].score=0;
    p2[2].score=0;
    strcpy(p2[0].username,"---");   // FIXED: names were uninitialised
    strcpy(p2[1].username,"---");
    strcpy(p2[2].username,"---");
    int temp;
    char tempc[100];
    if (ptr==NULL) return 0;        // FIXED: file may not exist
    while(fscanf(ptr,"%99s %d",p1.username,&p1.score)==2){
        if (p2[0].score<p1.score){
            p2[2].score=p2[1].score;
            p2[1].score=p2[0].score;
            p2[0].score=p1.score;
            strcpy(p2[2].username,p2[1].username);
            strcpy(p2[1].username,p2[0].username);
            strcpy(p2[0].username,p1.username);

        }
        else if(p2[1].score<p1.score){
            temp = p2[1].score;
            p2[2].score=p2[1].score;
            p2[1].score=p1.score;;
            strcpy(p2[2].username,p2[1].username);
            strcpy(p2[1].username,p1.username);
        }
        else if(p2[2].score<p1.score){
            p2[2].score=p1.score;
            strcpy(p2[2].username,p1.username);
        }
    }

    fclose(ptr);
    printf("%s %d %s %d %s %d",p2[0].username,p2[0].score,p2[1].username,p2[1].score,p2[2].username,p2[2].score);
    return 0;
    

    }
