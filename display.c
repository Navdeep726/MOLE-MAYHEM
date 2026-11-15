#include "raylib.h"
#include <stdio.h>
#include <string.h>

void mainscreen();
void choose_map();
void choose_difficulty();
<<<<<<< HEAD
=======
void holes();
>>>>>>> 7a9e5951f440f6ada450be1cd82cec0d00b32272

/* 1 main, 2 choose map, 3 choose difficulty,
   4 game, 5 leaderboard, 6 game over */
int currentscreen = 1,difficulty;      // global so every screen can change it
Texture2D menuBg,map_window,difficulty_window;           // loaded once in main
char selected_map[100];

int main()
{
    InitWindow(1600, 900, "Whack-a-Mole");
    SetTargetFPS(60);

    menuBg = LoadTexture("menu.png");    // must come after InitWindow
    map_window = LoadTexture("choose_map.png");
    difficulty_window = LoadTexture("choose_difficulty.png");
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentscreen)
        {
            case 1: mainscreen();  break;
            case 2: choose_map();  break;
            case 3: choose_difficulty(); break;
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

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                int pick = (i == 5) ? GetRandomValue(0, 4) : i;
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
                currentscreen = 4;           // go to the game
                printf("difficulty: %d\n", difficulty);
            }
        }
    }

}
void holes(int mode)
{
    switch(mode)
    {
        case 1:
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
        case 2:
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
        case 3:
            for(int i = 0; i < 4; i++)
            {
                for(int j = 0; j < 2; j++)
                {
                    float posa = 397.5f + (i * 230);
                    float posb = 430 + (j * 150);
                    DrawCircle(posa, posb, 46, BLACK);
                    DrawCircleLines(posa, posb, 46, BEIGE);
                }
                for(int k = 0; k < 2; k++)
                {
                    float posc = 512.5f + (i * 230);
                    float posd = 555 + (k * 150);
                    DrawCircle(posc, posd, 46, BLACK);
                    DrawCircleLines(posc, posd, 46, BEIGE);
                }
            }
            break;
    }
}