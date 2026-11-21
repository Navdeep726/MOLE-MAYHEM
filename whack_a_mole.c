/* ======================================================================
   WHACK-A-MOLE  (whack_a_mole.c)
   A whack-a-mole game written in C using the raylib graphics library.

   The player logs in or registers in the console, picks a map and a
   difficulty, and clicks the moles as they pop out of the holes.
   A hit adds 1 point, a miss costs 1 of the 3 lives. Scores are saved
   to a separate file for each difficulty and shown on a leaderboard.

   MODULE 1: main loop and menus
   MODULE 2: login and accounts
   MODULE 3: gameplay
   MODULE 4: scores and leaderboard

   Files used: players_details.txt, player_scores.txt,
               easy_scores.txt, medium_scores.txt, hard_scores.txt,
               music.mp3 (background music)
   ====================================================================== */

#include <raylib.h>
#include <stdio.h>
#include <string.h>

typedef struct {      // structure to store player details
    char username[100];
    char password[50];
    char mail[100];
} pi;

typedef struct {    // structure to store one score of a player
    char username[100];
    int score;
} ps;

/* ---------- FUNCTION DECLARATIONS ---------- */

/* MODULE 1: main loop and menus */
void mainscreen();
void choose_map();
void choose_difficulty();
void draw_centered(const char *text, int cx, int y, int size, Color c);
void hole_coordinate(int mode);

/* MODULE 2: login and accounts */
void choice(char *n);
int register_p(char *name);
int login_p(char *name);
void display_score(char name[100]);

/* MODULE 3: gameplay */
void create_game(char map[100], int difficulty);
void draw_mole(int difficulty);
void holes(int mode);
void check_hit();
void draw_hud();
void game_over();

/* MODULE 4: scores and leaderboard */
void game_over_screen();
void leaderboard();
void my_scores();
void top3(char *file, char *player, ps best[3]);
void load_leaderboard();
void load_my_scores();

/* ---------- GLOBAL VARIABLES ---------- */

/* MODULE 1: main loop and menus
   currentscreen: 1 main, 2 choose map, 3 choose difficulty,
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
Music bgMusic;           // background music, plays during the whole game

/* MODULE 3: gameplay */
Vector2 holePos[16];
int holeCount = 0;
int activeHole = -1;
double spawnTime = 0;

int score = 0;
int lives = 3;

float hard_radius = 46;

/* MODULE 4: scores and leaderboard */
int lastScore = 0;
char *scoreFiles[3] = {"easy_scores.txt", "medium_scores.txt", "hard_scores.txt"};

ps board[3][3];
ps mine[3][3];

/* ==================================================================
   MODULE 1: MAIN LOOP AND MENUS
   main(), main menu, choose map, choose difficulty, draw_centered()
   ================================================================== */

/* Program starts here. Logs the player in, opens the game window,
   loads all pictures and the music, and runs the game loop that draws
   the current screen. */
int main()
{
    choice(player_name);               // login / register in the console first
    if (strlen(player_name) == 0)      // player chose quit, so don't open the game
        return 0;

    InitWindow(1600, 900, "Whack-a-Mole");
    SetTargetFPS(60);

    InitAudioDevice();                         // turn on sound (must come before loading music)
    bgMusic = LoadMusicStream("music.mp3");    // streamed from the file while it plays
    SetMusicVolume(bgMusic, 0.5f);             // 0.0 = silent, 1.0 = full volume
    PlayMusicStream(bgMusic);                  // music streams loop by default

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
    gameOverBg    = LoadTexture("game_over_screen.png");
    leaderboardBg = LoadTexture("leaderboards.png");
    myScoresBg    = LoadTexture("player_scores.png");

    while (!WindowShouldClose() && !quitGame)   // runs until the window closes or quit is clicked
    {
        UpdateMusicStream(bgMusic);            // keeps the music playing, must run every frame

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
            case 5: leaderboard();
            break;
            case 6: game_over_screen();
            break;
            case 7: my_scores();
            break;
        }
        EndDrawing();
    }

    UnloadTexture(menuBg);
    UnloadTexture(map_window);
    UnloadTexture(difficulty_window);
    for (int i = 0; i < 5; i++) UnloadTexture(mapBg[i]);
    for (int i = 0; i < 3; i++) UnloadTexture(moleTex[i]);
    UnloadTexture(gameOverBg);
    UnloadTexture(leaderboardBg);
    UnloadTexture(myScoresBg);
    UnloadMusicStream(bgMusic);                // free the music
    CloseAudioDevice();                        // turn off sound
    CloseWindow();
    return 0;
}

/* Main menu: Play, Scores, Leaderboard and Quit buttons.
   A button glows when the mouse is over it. */
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
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            load_my_scores();                            // read scores before showing them
            currentscreen = 7;
        }
    }
    if (CheckCollisionPointRec(mouse, leaderboardarea)) {
        DrawRectangleRec(leaderboardarea, bright);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            load_leaderboard();                          // read scores before showing them
            currentscreen = 5;
        }
    }
    if (CheckCollisionPointRec(mouse, quitarea)) {
        DrawRectangleRec(quitarea, bright);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) quitGame = 1;
    }
}

/* Map selection: five maps plus a RANDOM option.
   Saves the chosen map name in selected_map. */
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
                currentscreen = 3;
            }
        }
    }

}

/* Difficulty selection: easy, medium or hard.
   Sets up the holes for that difficulty and starts the game. */
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
                hole_coordinate(difficulty);
                currentscreen = 4;
            }
        }
    }

}

/* Draws text so that its middle is at x = cx.
   Used by the score screens. */
void draw_centered(const char *text, int cx, int y, int size, Color c)
{
    DrawText(text, cx - MeasureText(text, size) / 2, y, size, c);
}

/* ==================================================================
   MODULE 2: LOGIN AND ACCOUNTS
   register, login, console menu and console scores
   ================================================================== */

/* Registers a new player and saves them in players_details.txt.
   Username and mail must not already exist.
   Returns 1 on success, 0 on failure. */
int register_p(char *name)
{
    pi p1;
    char m;

    // input details
    printf("Enter username without space: ");
    scanf(" %99s", p1.username);
    printf("Set password without space: ");
    scanf(" %49s", p1.password);
    printf("Enter mail: ");
    scanf(" %99s", p1.mail);

    FILE *fp = fopen("players_details.txt", "r");
    if (fp != NULL)                       // file may not exist yet, that's fine
    {
        pi p2;
        while (fscanf(fp, " %99s %49s %99s", p2.username, p2.password, p2.mail) == 3)
        {
            // to check if username or mail already exists
            if (strcmp(p1.username, p2.username) == 0 ||
                strcmp(p1.mail, p2.mail) == 0)
            {
                printf("Username or mail already registered.\n");
                fclose(fp); 
                printf("enter m to go back to main screen\nenter anything else to continue: ");
                scanf(" %c",&m);
                if(m=='m'){
                    choice(name);
                    return 0;
                }    
                else{
                    return register_p(name);// recursive call if username already registered

                }            
                
                
            }
        }
        fclose(fp);
    }
    //entering username in text file
    FILE *ptr = fopen("players_details.txt", "a");
    if (ptr == NULL)
    {
        printf("Could not open file.\n");
        return 0;
    }
    fprintf(ptr, "%s %s %s\n", p1.username, p1.password, p1.mail);
    fclose(ptr);

    strcpy(name, p1.username);
    printf("*************************\nTIME TO SMASHHHHHHHH!!!!!!!\n");
    return 1;
}

/* Logs in an existing player by checking username, password and mail
   against players_details.txt. Returns 1 on success, 0 on failure. */
int login_p(char *name){
    pi p1;

    // input details
    printf("Enter username without space: ");
    scanf(" %99s", p1.username);
    printf("Enter password without space: ");
    scanf(" %49s", p1.password);
    printf("Enter mail: ");
    scanf(" %99s", p1.mail);

    FILE *fp = fopen("players_details.txt", "r");
    if (fp == NULL)
    {
        printf("No players registered yet.\n");
        return 0;                         
    }

    pi p2;
    while (fscanf(fp, " %99s %49s %99s", p2.username, p2.password, p2.mail) == 3)
    {
        if (strcmp(p1.username, p2.username) == 0 &&
            strcmp(p1.mail, p2.mail) == 0 &&
            strcmp(p1.password, p2.password) == 0)   // checking if entered details are correct
        {
            fclose(fp);                   
            strcpy(name, p1.username);
            printf("*************************\nTIME TO SMASHHHHHHHH!!!!!!!\n");
            return 1;
        }
    }

    fclose(fp);                            
    printf("invalid credentials try again\n");
    char m;
    printf("enter m to go back to main screen\nenter anything else to continue ");
    scanf(" %c",&m);
    if(m=='m'){
        choice(name);
        return 0;
    }
    else{
        return login_p(name);   // recursive call if details are wrong

    }
}
/* Prints every previous score of a player from player_scores.txt. */
void display_score(char name[100]){
    FILE *fp;
    char username[100];
    int found=0,score;
    fp=fopen("player_scores.txt","r");
    if (fp==NULL){
        printf("unable to open file.");
        return;
    }
    printf("your previous scores are:\n");
    while(fscanf(fp," %99s %d",username,&score)==2){
        if (strcmp(username,name)==0){
            printf("%d\n",score);
            found=1;
        }
    }
    if(found==0){
        printf("no records found!");
    }
    fclose(fp);
    
}

/* Console menu shown before the game: register, login, quit or
   display scores. The logged in name is stored in n. */
void choice(char *n){
    int c;
    printf("enter 1 to register\nenter 2 to login\nenter 3 to quit\nenter 4 to display score\n");
    scanf(" %d",&c);
    switch(c){
        case 1:
            register_p(n);   // for new players
            printf("hiii %s\n",n);
            break;
        case 2:
            login_p(n);      // for already registered users
            printf("hiii %s\n",n);
            break;
        case 3:
            break;           // to quit
        case 4:
            {
            // nobody is logged in yet here, so ask whose scores to show
            char who[100];
            printf("enter username: ");
            scanf(" %99s", who);
            display_score(who);
            printf("\n");
            choice(n);          // back to main screen
            break;
        }
        
    }
}

/* ==================================================================
   MODULE 3: GAMEPLAY
   holes, moles, hitting, score, lives and saving the score
   ================================================================== */

/* Calculates and stores the hole positions for the difficulty:
   6 holes (easy), 9 holes (medium) or 16 staggered holes (hard).
   Also resets the mole, score and lives for a new game. */
void hole_coordinate(int mode)
{
    holeCount = 0;

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
    score = 0;         // fresh score for every new game
    lives = 3;         // fresh lives for every new game
}

/* Draws the holes for the difficulty. */
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

/* Game screen: draws the chosen map, the holes and the mole,
   checks clicks and shows the score and lives. */
void create_game(char map[100], int difficulty)
{
    if      (strcmp(map, "night")   == 0) DrawTexture(mapBg[0], 0, 0, WHITE);
    else if (strcmp(map, "village") == 0) DrawTexture(mapBg[1], 0, 0, WHITE);
    else if (strcmp(map, "sunset")  == 0) DrawTexture(mapBg[3], 0, 0, WHITE);
    else if (strcmp(map, "fence")   == 0) DrawTexture(mapBg[4], 0, 0, WHITE);
    else                                  DrawTexture(mapBg[2], 0, 0, WHITE);

    holes(difficulty);
    draw_mole(difficulty);
    check_hit();
    draw_hud();

}

/* Moves the mole to a random hole every second and draws it. */
void draw_mole(int difficulty)
{
    double now = GetTime();

    if (holeCount == 0) return;

    if (activeHole == -1 || now - spawnTime >= 1.0)
    {
        activeHole = GetRandomValue(0, holeCount - 1);
        spawnTime = now;
    }

    Texture2D mole = moleTex[difficulty - 1];   // mole picture that fits this difficulty's holes

    DrawTexture(mole,
                holePos[activeHole].x - mole.width / 2,
                holePos[activeHole].y - mole.height / 2,
                WHITE);
}

/* Checks a mouse click. Inside the mole's hole = hit (+1 score),
   anywhere else = miss (-1 life). Ends the game at 0 lives. */
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
    }
    else
    {
        lives--;                 // missed
        if (lives <= 0)
            game_over();
    }
}

/* Shows the score and lives in the top right corner. */
void draw_hud()
{
    DrawText(TextFormat("Score: %d", score), 1300, 20, 40, YELLOW);
    DrawText(TextFormat("Lives: %d", lives), 1300, 70, 40, RED);
}

/* Saves the final score to the difficulty's score file and to
   player_scores.txt, then opens the game over screen. */
void game_over()
{
    lastScore = score;               // shown on the game over screen

    // add the score to the file of this difficulty
    FILE *fp = fopen(scoreFiles[difficulty - 1], "a");
    if (fp != NULL)
    {
        fprintf(fp, "%s %d\n", player_name, score);
        fclose(fp);
    }

    // also add it to player_scores.txt, which display_score() reads
    // for console option 4
    fp = fopen("player_scores.txt", "a");
    if (fp != NULL)
    {
        fprintf(fp, "%s %d\n", player_name, score);
        fclose(fp);
    }

    activeHole = -1;
    currentscreen = 6;           // show the game over screen
}

/* ==================================================================
   MODULE 4: SCORES AND LEADERBOARD
   game over screen, leaderboard, my high scores and top 3
   ================================================================== */

/* Game over screen: shows the final score and the Play Again,
   Main Menu and Exit buttons. */
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
                    hole_coordinate(difficulty);  // also resets score and lives
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

/* Leaderboard screen: top 3 players for easy, medium and hard.
   ESC or Backspace goes back to the main menu. */
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

/* My High Scores screen: the logged in player's top 3 scores for
   each difficulty. Back button or ESC returns to the main menu. */
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
   player == NULL : everyone, one place per player (leaderboard)
   player != NULL : only that player's scores (my high scores)
   An empty place has score -1. */
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

        // insert the score in the right place, shifting lower ones down
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

/* Reads the top 3 of every difficulty file for the leaderboard. */
void load_leaderboard()
{
    for (int d = 0; d < 3; d++)
        top3(scoreFiles[d], NULL, board[d]);
}

/* Reads the logged in player's top 3 of every difficulty file. */
void load_my_scores()
{
    for (int d = 0; d < 3; d++)
        top3(scoreFiles[d], player_name, mine[d]);
}