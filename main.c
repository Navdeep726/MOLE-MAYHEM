/* MODULE 1: main.c  -  main loop and menus
   Opens the window, loads the pictures, switches between screens,
   and handles the main menu, choose map and choose difficulty screens.
   draw_centered() is a small text helper the other screens use too. */

#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* functions of this module */
void mainscreen();
void choose_map();
void choose_difficulty();
void draw_centered(const char *text, int cx, int y, int size, Color c);

/* from module 2: login_register.c */
void choice(char *n);

/* from module 3: game.c */
void create_game(char map[100], int difficulty);
void build_holes(int mode);

/* from module 4: scores.c */
void leaderboard();
void game_over_screen();
void my_scores();
void load_leaderboard();
void load_my_scores();

/* 1 main, 2 choose map, 3 choose difficulty,
   4 game, 5 leaderboard, 6 game over, 7 my high scores */

int currentscreen = 1;
int difficulty = 1;
char selected_map[100] = "";
char player_name[100] = "";
int quitGame = 0;

Texture2D menuBg, map_window, difficulty_window;
Texture2D moleTex[3];
Texture2D gameOverBg, leaderboardBg, myScoresBg;
Texture2D mapBg[5];
Music bgmusic;

int main()
{
    choice(player_name);               // NEW: login / register in the console first
    if (strlen(player_name) == 0)      // chose quit, so don't open the game
        return 0;

    InitWindow(1600, 900, "Whack-a-Mole");
    InitAudioDevice();
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);                      // ESC no longer closes the window

    bgmusic=LoadMusicStream("music.mp3");
    SetMusicVolume(bgmusic,0.5f);
    PlayMusicStream(bgmusic);

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
        UpdateMusicStream(bgmusic);
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
    UnloadTexture(myScoresBg);  
    UnloadMusicStream(bgmusic);              // NEW
    CloseAudioDevice();                      // NEW
    CloseWindow();               // NEW
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

/* draws text so that its middle is at x = cx */
void draw_centered(const char *text, int cx, int y, int size, Color c)
{
    DrawText(text, cx - MeasureText(text, size) / 2, y, size, c);
}
