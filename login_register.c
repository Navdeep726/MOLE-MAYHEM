#include<stdio.h>
#include<string.h>
typedef struct {
    char username[100];
    char password[50];
    char mail[100];
}pi;
 typedef struct {
    char username[100];
    int score;
}ps;
void register_p(char *name){
     pi p1;
     char username[50],password[50], email[50];
     printf("enter username without space ");
     scanf("%s",&p1.username);
     printf("set password without space");
     scanf("%s",&p1.password);
     printf("enter mail");
     scanf("%s",&p1.mail);
     printf("*************************\nTIME TO SMASHHHHHHHH!!!!!!!\n");
     strcpy(name,p1.username);
     //to store the player information in file.
     FILE *ptr;
     ptr=fopen("players_details.txt","w");
     fprintf(ptr,"username\t password\t email\n");
     fprintf(ptr,"%s\t%s\t%s",p1.username,p1.password,p1.mail);
     fclose(ptr);

}
int main(){
     pi p1;
     ps p2;
     char n[50];
     register_p(n);
     printf("%s",n);
     //FILE *ptr;timepass
     //ptr=fopen("players_details.txt","a");
     //fprintf(ptr,"username\t password\t email\n");
     //fprintf(ptr,"%s\t%s\t%s",p1.username,p1.password,p1.mail);
     //fclose(ptr);
}
