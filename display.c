#include "raylib.h"
#include <stdio.h>
#include <string.h>

void mainscreen();
void choose_map();

/* 1 main, 2 choose map, 3 choose difficulty,
   4 game, 5 leaderboard, 6 game over */
int currentscreen = 1;      // global so every screen can change it
Texture2D menuBg,map_window;           // loaded once in main
char selected_map[100];

int main()
{
    InitWindow(1600, 900, "Whack-a-Mole");
    SetTargetFPS(60);

    menuBg = LoadTexture("menu.png");    // must come after InitWindow
    map_window = LoadTexture("choose_map.png");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentscreen)
        {
            case 1: mainscreen();  break;
            case 2: choose_map();  break;
        }

        EndDrawing();
    }

    UnloadTexture(menuBg);
    CloseWindow();
    return 0;
}

void mainscreen()
{
    DrawTexture(menuBg, 0, 0, WHITE);

    Vector2 mouse = GetMousePosition();
    Rectangle playarea        = {537,360,534,107};
    Rectangle scoresarea      = {537,483,534,107};
    Rectangle leaderboardarea = {537,603,534,107};
    Rectangle quitarea        = {537,725,534,107};
    Color bright = {255,255,255,60};

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
        // quit: see note below
    }
    if(CheckCollisionPointRec(mouse, quitarea) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
    currentscreen = 2;
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
            DrawRectangleLinesEx(areas[i], 4, YELLOW);

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                int pick = (i == 5) ? GetRandomValue(0, 4) : i;
                strcpy(selected_map, names[pick]);
                printf("map: %s\n", selected_map);
                // currentscreen = 3;
            }
        }
    }

}
