
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
}ps;
void register_p(char *name)
{
    pi p1;

    printf("Enter username without space: ");
    scanf("%99s", p1.username);
    printf("Set password without space: ");
    scanf("%49s", p1.password);
    printf("Enter mail: ");
    scanf("%99s", p1.mail);

    strcpy(name, p1.username);

    FILE *ptr = fopen("players_details.txt", "a");
    if (ptr == NULL) {
        printf("Could not open file.\n");
        return;
    }
    fprintf(ptr, "%s %s %s\n", p1.username, p1.password, p1.mail);
    fclose(ptr);

    printf("*************************\nTIME TO SMASHHHHHHHH!!!!!!!\n");
}

// Reads every record back and prints it
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

int main(void)
{
    char n[100];
    register_p(n);
    printf("Hello, %s\n", n);

    show_all_players();
    return 0;
}