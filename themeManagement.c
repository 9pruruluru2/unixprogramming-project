#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> //open에서 flags를 이용하기 위한 헤더
#include <string.h>

int addTheme(){

    char themeName[100];
    char arr[12][30];
    int i;
    int max = getthemeList(arr);
    printf("-----테마 목록 출력-----\n");
    for(i=1;i<max;i++){
        printf("%d : %s", i, arr[i]);
    }
    if(max>=11){
        printf("빈 방이 없습니다\n");
        return;
    }

    while(getchar()!='\n');  //입력 버퍼를 비우기 위한 작업

    printf("추가할 테마를 입력해주세요 :");
    gets(themeName);
    int newfd;

    newfd = open(makePath("themelist.txt"),O_RDWR | O_CREAT,0644);

    FILE* themelist = fdopen(newfd,"r+");
    if(themelist ==NULL){printf("NULL");}
    while(feof(themelist) == 0){
        char temp[100];
        fgets(temp, sizeof(temp)/sizeof(char), themelist);
        temp[(int)(strcspn(themeName,"\r\n"))] = '\0';
        temp[(int)(strcspn(themeName,"\n"))] = '\0';

        if(!strcmp(themeName,temp)){
            printf("그 테마는 이미 존재합니다.");
            return 1;
        }
    }
    char timeTableName[110]="";
    strcpy(timeTableName,themeName);

    fputs(strcat(timeTableName,"\r\n"), themelist);
    timeTableName[(int)(strcspn(timeTableName,"\r\n"))] = '\0';
    timeTableName[(int)(strcspn(timeTableName,"\n"))] = '\0';
    strcat(timeTableName,"TimeTable.txt");

    fclose(themelist);

    close(newfd);
    newfd = open(makePath(timeTableName),O_RDWR|O_CREAT|O_TRUNC, 0644);

    FILE* timeTableFile = fopen(makePath(timeTableName), "w+");

    for(i=0;i<7;i++) fprintf(timeTableFile, "1 1 1 1 1 1\r\n");
    fclose(timeTableFile);

    printf("%s를 추가했습니다.\n", themeName);
    close(newfd);
    return 0;
}


int removeTheme(){

    char themeName[30];
    char temp[1024];
    int isRemove = 0 ;
    int i;
    char arr[12][30];
    int max = getthemeList(arr);

    printf("-----테마 목록 출력-----\n");
    for(i=1;i<max;i++){
        printf("%d : %s", i, arr[i]);
    }

    while(getchar()!='\n');

    printf("제거할 테마를 입력해주세요 :");
    gets(themeName);

    themeName[(int)(strcspn(themeName,"\r\n"))] = '\0';
    themeName[(int)(strcspn(themeName,"\n"))] = '\0';

    int newfd;
    newfd = open(makePath("themelist.txt"),O_RDWR);
    lseek(newfd,0,SEEK_SET);

    FILE* themelist = fdopen(newfd,"r+");
    FILE* tempThemelist = tmpfile();

    while(fgets(temp, 1024, themelist) != NULL){
        if(strstr(temp,themeName) != NULL){
            isRemove = 1;
        }else{
            fputs(temp, tempThemelist);
        }
    }
    if(isRemove != 0){
        copy(themelist, tempThemelist,"themelist.txt");
        printf("%s를 제거했습니다.\n", themeName);
        remove(strcat(themeName,"TimeTable.txt"));
    }else {
        printf("%s를 찾지 못했습니다.\n", themeName);
    }
    fclose(themelist);
    close(newfd);
    fclose(tempThemelist);
    return 0;
}

int copy(FILE* list,FILE* templist,char fileName[]){
    fseek(templist,0,SEEK_SET);
    list = freopen(makePath(fileName), "w+", list);
    char buf[1024];
    while(fgets(buf, 1024,templist)!=NULL){
        fprintf(list,"%s", buf);
    }
    return 0
}