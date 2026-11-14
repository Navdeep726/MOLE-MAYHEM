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
int highest(){//to find the highest score.
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
