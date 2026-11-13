#include <stdio.h>
#include <string.h>

typedef struct {      //structure to store player details 
    char username[100];
    char password[50];
    char mail[100];
} pi;

typedef struct {    //structure to store player scores
    char username[100];
    int score;
} ps;
void choice(char *n);

// returns 1 on success, 0 on failure
int register_p(char *name)
{
    pi p1;
    char m;
    // printf("enter m to go back to main screen\nenter anything else to continue: ");
    // scanf(" %c",&m);
    // if(m=='m'){
    //     choice(name);
    //     return 0;
    // }
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
            //to check if username already exists
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

void show_all_players(void)
{
    FILE *fp = fopen("players_details.txt", "r");
    if (fp == NULL) return;

    pi p;
    while (fscanf(fp, "%99s %49s %99s", p.username, p.password, p.mail) == 3)
    {
        printf("User: %s | Mail: %s\n", p.username, p.mail);
    }
    fclose(fp);
}
int login_p(char *name)
{
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
            strcmp(p1.password, p2.password) == 0)//checking if enterd details are correct
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
        return login_p(name);

    }
    
    ;// recursive call if details are wrong
}
void display_score(char name[100]){
    FILE *fp;
    char username[100];
    int found=0,score,h_score;
    fp=fopen("player_scores.txt","r");
    if (fp==NULL){
        printf("unable to open file.");
        return;
    }
    printf("your previous scores are:\n");
    while(fscanf(fp," %99s %d",username,&score)==2){
        h_score=score;
        if(score>h_score){
            h_score=score;
        }
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


void choice(char *n){
    int c;
    printf("enter 1 to register\nenter 2 to login\nenter 3 to quit\nenter 4 to display score\n");
    scanf(" %d",&c);
    switch(c){
        case 1:
            register_p(n);//for new players
            printf("hiii %s\n",n);
            break;
        case 2:
            login_p(n);//for already registered users
            printf("hiii %s\n",n);
            break;
        case 3:
            break;//to quit
        case 4:
            {
            display_score(n);
            break;
        }
        
    }
}
int player_highest(char *player);
int highest();

int main(void)
{
    //char n[100];
    //choice(n);
    //printf("thank you");
    int i;
    i=highest();
    printf("%d",i);
    return 0;
    
}
int player_highest(char player[]){
    FILE *ptr=fopen("player_scores.txt","r");
    char ply[100];
    int scr;
    int high_s=0;
    while(fscanf(ptr,"%99s %d",ply,&scr)==2){
        if(strcmp(player,ply)==0){
            if(scr>high_s){
                high_s=scr;
            }
        }
    }
    fclose(ptr);
    return high_s;
}
int highest(){//to fimnd the highest score.
    char players[100];
    int scr;
    int h1,h2,h3;
    FILE *ptr=fopen("player_scores.txt","r");
    while(fscanf(ptr,"%99s %d",players,&scr)==2){
        scr=player_highest(players);
    }
    return scr;
    fclose(ptr);
}



