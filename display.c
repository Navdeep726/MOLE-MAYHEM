#include "raylib.h"
#include <stdio.h>
void mainscreen();
void gamebackground(int n);
void creategame(int gamemode,int holes,int map);

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

void mainscreen(){
    InitWindow(1600, 900, "MOLE MAYHEM");

    while (!WindowShouldClose())
    {
        BeginDrawing();
       
        Texture2D background = LoadTexture("menu.png");

        ClearBackground(RAYWHITE);

        DrawTexture(background, 0, 0, WHITE);
        Vector2 mouse = GetMousePosition();
        Rectangle playarea = {537,360,534,107};
        Rectangle scoresarea = {537,483,534,107};
        Rectangle leaderboardarea ={537,603,534,107};
        Rectangle quitarea = {537,725,534,107};
        bool hovering_p = CheckCollisionPointRec(mouse,playarea);
        bool hovering_s = CheckCollisionPointRec(mouse,scoresarea);
        bool hovering_l = CheckCollisionPointRec(mouse,leaderboardarea);
        bool hovering_q = CheckCollisionPointRec(mouse,quitarea); 
        Color bright={255,255,255,60};
        if (hovering_p){
            DrawRectangleRec(playarea,bright);
        }

        if (hovering_s){
            DrawRectangleRec(scoresarea,bright);
        }

        if (hovering_l){
            DrawRectangleRec(leaderboardarea,bright);
        }

        if (hovering_q){
            DrawRectangleRec(quitarea,bright);
        }

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
