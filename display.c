#include "raylib.h"
#include <stdio.h>
void mainscreen();
void gamebackground();

int main()
{
    gamebackground();

    return 0;
}

void mainscreen()

{
    InitWindow(2000, 1500, "MOLE MAYHEM");

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawRectangle(540, 90, 920, 220, RED);

        DrawText("MOLE MAYHEM", 550, 100, 120, DARKGREEN);

        DrawEllipse(1000, 500, 300, 50, PINK);

        DrawText("START", 850, 460, 80, BLACK);

        EndDrawing();
    }

    CloseWindow();
}
void gamebackground()

{
    InitWindow(2000, 1500, "MOLE MAYHEM");

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        Rectangle myBox =  {90, 90, 1710, 1010};

        DrawRectangle(120, 120, 1660, 960, GREEN);

        DrawRectangleLinesEx(myBox, 30, YELLOW);

        for(int r = 0; r < 2; r++)
        {
            for(int c = 0; c < 3; c++)
            {
                int posx = 350 + (c * 400);
                int posy = 350 + (r * 300);
                DrawEllipse(posx + 100, posy + 50, 120, 50, GRAY);
            }
        }

        EndDrawing();
    }
    CloseWindow();
}
