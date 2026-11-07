#include <stdio.h>
#include <string.h>

typedef struct {
    char username[100];
    char password[50];
    char mail[100];
} pi;

typedef struct {
    char username[100];
    int score;
} ps;

// returns 1 on success, 0 on failure
int register_p(char *name)
{
    pi p1;

    printf("Enter username without space: ");
    scanf("%99s", p1.username);
    printf("Set password without space: ");
    scanf("%49s", p1.password);
    printf("Enter mail: ");
    scanf("%99s", p1.mail);

    FILE *fp = fopen("players_details.txt", "r");
    if (fp != NULL)                       // file may not exist yet, that's fine
    {
        pi p2;
        while (fscanf(fp, "%99s %49s %99s", p2.username, p2.password, p2.mail) == 3)
        {
            if (strcmp(p1.username, p2.username) == 0 ||
                strcmp(p1.mail, p2.mail) == 0)
            {
                printf("Username or mail already registered.\n");
                fclose(fp);               // close before leaving
                return register_p(name);
                
            }
        }
        fclose(fp);
    }

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

    printf("Enter username without space: ");
    scanf("%99s", p1.username);
    printf("Enter password without space: ");
    scanf("%49s", p1.password);
    printf("Enter mail: ");
    scanf("%99s", p1.mail);

    FILE *fp = fopen("players_details.txt", "r");
    if (fp == NULL)
    {
        printf("No players registered yet.\n");
        return 0;                          // fix 3: nothing to close here
    }

    pi p2;
    while (fscanf(fp, "%99s %49s %99s", p2.username, p2.password, p2.mail) == 3)
    {
        if (strcmp(p1.username, p2.username) == 0 &&
            strcmp(p1.mail, p2.mail) == 0 &&
            strcmp(p1.password, p2.password) == 0)
        {
            fclose(fp);                    // fix 1: close BEFORE returning
            strcpy(name, p1.username);
            printf("*************************\nTIME TO SMASHHHHHHHH!!!!!!!\n");
            return 1;
        }
    }

    fclose(fp);                            // fix 2: close before retrying
    printf("invalid credentials try again\n");
    return login_p(name);
}

int main(void)
{
    char n[100];
    int c;
    printf("enter 1 to register\nenter 2 to login\nenter 3 to quit\n");
    scanf("%d",&c);
    switch(c){
        case 1:
            register_p(n);
            printf("hiii %s\n",n);
            break;
        case 2:
            login_p(n);
            printf("hiii %s\n",n);
            break;
        case 3:
            break;
    }
    printf("thank you");
    return 0;
}