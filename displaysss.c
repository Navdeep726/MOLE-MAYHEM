#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void mainscreen();
void choose_map();
void choose_difficulty();
void create_game(char map[100], int difficulty);

/* 1 main, 2 choose map, 3 choose difficulty,
   4 game, 5 leaderboard, 6 game over */
int currentscreen = 1;
int difficulty = 1;
bool quitgame = false;
char selected_map[100] = "";

Texture2D menuBg, map_window, difficulty_window;
Texture2D mapBg[5];     // night, village, day, sunset, fence

float hard_radius = 46;

int main()
{
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

    while (!WindowShouldClose() && !quitgame)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentscreen)
        {
            case 1: mainscreen(); break;
            case 2: choose_map();                         break;
            case 3: choose_difficulty();                  break;
            case 4: create_game(selected_map, difficulty); break;
        }
        DrawText(TextFormat("screen %d  difficulty %d  map '%s'", currentscreen, difficulty, selected_map),
         20, 20, 30, RED);
        EndDrawing();
    }

    UnloadTexture(menuBg);
    UnloadTexture(map_window);
    UnloadTexture(difficulty_window);
    for (int i = 0; i < 5; i++) UnloadTexture(mapBg[i]);
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
    }
    if (CheckCollisionPointRec(mouse, leaderboardarea)) {
        DrawRectangleRec(leaderboardarea, bright);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentscreen = 5;
    }
    if (CheckCollisionPointRec(mouse, quitarea)) {
        DrawRectangleRec(quitarea, bright);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) quitgame = true;
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
    const char *names[5] = {"night", "village", "day", "sunset", "fence"};

    Color glow = {255, 255, 255, 70};

    for (int i = 0; i < 6; i++)
    {
        if (CheckCollisionPointRec(mouse, areas[i]))
        {
            DrawRectangleRec(areas[i], glow);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                int pick = (i == 5) ? GetRandomValue(0, 4) : i;
                strcpy(selected_map, names[pick]);
                printf("map: %s\n", selected_map);
                currentscreen = 3;
            }
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) currentscreen = 1;
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
                currentscreen = 4;
                printf("difficulty: %d\n", difficulty);
            }
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE)) currentscreen = 2;
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
                    DrawCircle(posx, posy, 88, BROWN);
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
                    DrawCircle(posp, posq, 58.67f, BROWN);
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
                    DrawCircle(posa, posb, 46, BROWN);
                    DrawCircleLines(posa, posb, 46, BEIGE);
                }
                for(int k = 0; k < 2; k++)
                {
                    float posc = 512.5f + (i * 230);
                    float posd = 555 + (k * 250);      // rows at 555 and 805
                    DrawCircle(posc, posd, 46, BROWN);
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

}