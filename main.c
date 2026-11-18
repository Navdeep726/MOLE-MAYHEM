#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void mainscreen();
void choose_map();
void choose_difficulty();
void create_game(char map[100], int difficulty);
void draw_mole(int difficulty);
void build_holes(int mode);
void holes(int mode);
void check_hit();          // NEW: handles clicks during the game
void draw_hud();           // NEW: shows score and lives on screen
void game_over();          // NEW: saves score and goes to game over screen
void leaderboard();        // NEW: screen 5
void game_over_screen();   // NEW: screen 6
void my_scores();          // NEW: screen 7
void load_leaderboard();   // NEW: reads top 3 of every difficulty file
void load_my_scores();     // NEW: reads this player's top 3 of every difficulty
void draw_centered(const char *text, int cx, int y, int size, Color c);

// CHANGED: brings in all of login_register.c (choice(), register_p(), the ps
// struct, ...) so only main.c needs to be compiled.
#include "login_register.c"

void top3(char *file, char *player, ps best[3]);

/* 1 main, 2 choose map, 3 choose difficulty,
   4 game, 5 leaderboard, 6 game over, 7 my high scores */
int currentscreen = 1;
int difficulty = 1;
char selected_map[100] = "";
Vector2 holePos[16];
int holeCount = 0;
int activeHole = -1;      // -1 means no mole yet
double spawnTime = 0;     // when the current mole appeared

/* NEW: score and lives */
int score = 0;
int lives = 3;
int lastScore = 0;        // score of the most recent game
int scoreHistory[100];    // every finished game's score
int scoreCount = 0;       // how many scores are stored

/* NEW: player, score files and leaderboard data */
char player_name[100] = "";          // filled by login / register
int quitGame = 0;                    // set to 1 by quit / exit buttons
char *scoreFiles[3] = {"easy_scores.txt", "medium_scores.txt", "hard_scores.txt"};
ps board[3][3];                      // [difficulty][rank] top 3 of everyone
ps mine[3][3];                       // [difficulty][rank] top 3 of this player


Texture2D menuBg, map_window, difficulty_window;
Texture2D moleTex[3];   // CHANGED: one mole per difficulty, sized to fit its holes
Texture2D gameOverBg, leaderboardBg, myScoresBg;   // NEW
Texture2D mapBg[5];     // night, village, day, sunset, fence

float hard_radius = 46;

int main()
{
    choice(player_name);               // NEW: login / register in the console first
    if (strlen(player_name) == 0)      // chose quit, so don't open the game
        return 0;

    InitWindow(1600, 900, "Whack-a-Mole");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);                      // ESC no longer closes the window

    menuBg            = LoadTexture("menu.png");     // must come after InitWindow
    map_window        = LoadTexture("choose_map.png");
    difficulty_window = LoadTexture("choose_difficulty.png");

    mapBg[0] = LoadTexture("night.png");
    mapBg[1] = LoadTexture("windmill.png");   // village
    mapBg[2] = LoadTexture("day.png");
    mapBg[3] = LoadTexture("sunset.png");
    mapBg[4] = LoadTexture("fence.png");
    moleTex[0] = LoadTexture("angry_mole_176x176.png");   // easy   (hole radius 88)
    moleTex[1] = LoadTexture("angry_mole_118x118.png");   // medium (hole radius 58.67)
    moleTex[2] = LoadTexture("angry_mole_92x92.png");     // hard   (hole radius 46)
    gameOverBg    = LoadTexture("game_over_screen.png");   // NEW
    leaderboardBg = LoadTexture("leaderboards.png");       // NEW
    myScoresBg    = LoadTexture("player_scores.png");      // NEW

    while (!WindowShouldClose() && !quitGame)   // CHANGED: quit buttons end the loop
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentscreen)
        {
            case 1: mainscreen();
            break;
            case 2: choose_map();
            break;
            case 3: choose_difficulty();
            break;
            case 4: create_game(selected_map, difficulty);
            break;
            case 5: leaderboard();            // NEW
            break;
            case 6: game_over_screen();       // NEW
            break;
            case 7: my_scores();              // NEW
            break;
        }
        // CHANGED: debug text commented out, it drew over every screen
        // DrawText(TextFormat("screen %d  difficulty %d  map '%s'", currentscreen, difficulty, selected_map),
        //  20, 20, 30, RED);
        EndDrawing();
    }

    UnloadTexture(menuBg);
    UnloadTexture(map_window);
    UnloadTexture(difficulty_window);
    for (int i = 0; i < 5; i++) UnloadTexture(mapBg[i]);
    for (int i = 0; i < 3; i++) UnloadTexture(moleTex[i]);   // CHANGED: 3 moles now
    UnloadTexture(gameOverBg);                 // NEW
    UnloadTexture(leaderboardBg);              // NEW
    UnloadTexture(myScoresBg);                 // NEW
    CloseWindow();
    return 0;
}

void mainscreen()
{
    DrawTexture(menuBg, 0, 0, WHITE);

    Vector2 mouse = GetMousePosition();
    Rectangle playarea        = {537, 360, 534, 107};
    Rectangle scoresarea      = {537, 483, 534, 107};
    Rectangle leaderboardarea = {537, 603, 534, 107};
    Rectangle quitarea        = {537, 725, 534, 107};
    Color bright = {255, 255, 255, 60};

    if (CheckCollisionPointRec(mouse, playarea)) {
        DrawRectangleRec(playarea, bright);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentscreen = 2;
    }
    if (CheckCollisionPointRec(mouse, scoresarea)) {
        DrawRectangleRec(scoresarea, bright);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {   // NEW
            load_my_scores();
            currentscreen = 7;
        }
    }
    if (CheckCollisionPointRec(mouse, leaderboardarea)) {
        DrawRectangleRec(leaderboardarea, bright);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            load_leaderboard();                          // NEW
            currentscreen = 5;
        }
    }
    if (CheckCollisionPointRec(mouse, quitarea)) {
        DrawRectangleRec(quitarea, bright);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) quitGame = 1;   // NEW
    }
}

void choose_map()
{
    DrawTexture(map_window, 0, 0, WHITE);

    Vector2 mouse = GetMousePosition();

    Rectangle areas[6] = {
        {123,  246, 376, 267},   // 0 top-left     : night
        {1114, 246, 376, 267},   // 1 top-right    : village
        {612,  395, 376, 267},   // 2 center       : day
        {123,  574, 376, 267},   // 3 bottom-left  : sunset
        {1114, 574, 376, 267},   // 4 bottom-right : fence
        {562,  712, 478,  86}    // 5 RANDOM
    };
    char *names[5] = {"night", "village", "day", "sunset", "fence"};

    Color glow = {255, 255, 255, 70};

    for (int i = 0; i < 6; i++)
    {
        if (CheckCollisionPointRec(mouse, areas[i]))
        {
            DrawRectangleRec(areas[i], glow);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {


                int pick;
                if (i==5){
                    pick=GetRandomValue(0,4);
                }
                else{
                    pick = i;
                }
                strcpy(selected_map, names[pick]);
                printf("map: %s\n", selected_map);
                currentscreen = 3;
            }
        }
    }

}

void choose_difficulty()
{
    DrawTexture(difficulty_window, 0, 0, WHITE);

    Vector2 mouse = GetMousePosition();

    Rectangle areas[3] = {
        {485, 298, 598, 154},    // 0 easy
        {485, 482, 598, 155},    // 1 medium
        {485, 665, 598, 158}     // 2 hard
    };

    Color glow = {255, 255, 255, 70};

    for (int i = 0; i < 3; i++)
    {
        if (CheckCollisionPointRec(mouse, areas[i]))
        {
            DrawRectangleRec(areas[i], glow);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                difficulty = i + 1;          // 1 easy, 2 medium, 3 hard
                build_holes(difficulty);
                currentscreen = 4;
                printf("difficulty: %d\n", difficulty);
            }
        }
    }


}
void build_holes(int mode)
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

/* ===================== NEW CODE BELOW ===================== */

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

/* draws text so that its middle is at x = cx */
void draw_centered(const char *text, int cx, int y, int size, Color c)
{
    DrawText(text, cx - MeasureText(text, size) / 2, y, size, c);
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
