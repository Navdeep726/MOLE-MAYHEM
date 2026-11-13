#include "raylib.h"
#include <stdio.h>
void mainscreen();
void gamebackground(int n);

int main()
{
    int a;
    printf("enter a: ");
    scanf("%d", &a);
    switch(a)
    {
        case 2:
            gamebackground(a);
            break;
        case 3:
            gamebackground(a);
            break;
    }


    return 0;
}

void mainscreen()

{
    InitWindow(1920, 1200, "MOLE MAYHEM");

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
void gamebackground(int n)

{
    InitWindow(1600, 1000, "MOLE MAYHEM");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        Texture2D background = LoadTexture("sunlightbg.png");

        DrawTexture(background, 0, 0, WHITE);

        for(int r = 0; r < n; r++)
        {
            for(int c = 0; c < 3; c++)
            {
                int posx = 350 + (c * 400);
                int posy = 500 + (r * 200);
                DrawCircle(posx, posy, 60, BLACK);
            }
        }
        
        EndDrawing();
    }
    
    CloseWindow();

}
