#include "raylib.h"
#include <stdio.h>
void mainscreen();
void gamebackground(int n);

int main()
{
    // int a;
    // printf("enter a: ");
    // scanf("%d", &a);
    // switch(a)
    // {
    //     case 2:
    //         gamebackground(a);
    //         break;
    //     case 3:
    //         gamebackground(a);
    //         break;
    // }
    mainscreen();


    return 0;
}

void mainscreen()

{
    InitWindow(1672, 941, "MOLE MAYHEM");

    while (!WindowShouldClose())
    {
        BeginDrawing();
       
        Texture2D background = LoadTexture("menu.png");

        ClearBackground(RAYWHITE);

        DrawTexture(background, 0, 0, WHITE);

        EndDrawing();
    }

    CloseWindow();
}
void gamebackground(int n)

{
    InitWindow(1672, 941, "MOLE MAYHEM");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        Texture2D background = LoadTexture("day.png");

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
