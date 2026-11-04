#include<stdio.h>
#include<string.h>
typedef struct {
    char username[100];
    char password[50];
    char mail[100];
}player_info;
 typedef struct {
    char username[100];
    int score;
}player_scores;
void register_p(char *name){
     char username[50],password[50], email[50];
     printf("enter username");
     scanf("%s",&username);
     printf("set password");
     scanf("%s",&password);
     printf("enter mail");
     scanf("%s",&email);
     printf("*************************\nTIME TO SMASHHHHHHHH!!!!!!!\n");
     strcpy(name,username);

}
int main(){
     char n[50];
     register_p(n);
     printf("%s",n);





}
