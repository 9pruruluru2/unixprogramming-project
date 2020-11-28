#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define ADMIN_DIRECTORY "/export/home/stud20/19101224/project/"

int fileopen(char* filename);
int checkfile(int iDs);
int getthemeList(char arr[][30]);
void themeReservation();
void getReservationList();
char* makePath(char* name);

void userSelect(){
    int num;

    printf("-------------\n");
    printf("1. 테마 예약\n");
    //printf("2. 예약 취소\n"); 예약 취소 기능은 이전에 했던 것들의 반복이라 구현에서 제외
    printf("2. 티켓 출력\n");
    printf("-------------\n");
    printf("숫자를 입력하시오 : ");
    scanf("%d", &num);

    switch(num){
        case 1: themeReservation(); break;
        case 2: getReservationList(); break;
        default: break;
    }
}

void themeReservation(){
    int i, j, num;
    char arr[12][30];
    char temp[40] = "";
    int max = getthemeList(arr);
    const char week[7][10] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

    for(i=1;i<max;i++){
        printf("%d : %s", i, arr[i]);
    }
    printf("\n-------------------\n");
    printf("어떤 테마의 방을 선택하시겠습니까?[숫자로 입력] : ");
    scanf("%d",&num);

    char timeTableName[40];
    strcpy(timeTableName,arr[num]);
    timeTableName[(int)(strcspn(timeTableName,"\r\n"))] = '\0';
    timeTableName[(int)(strcspn(timeTableName,"\n"))] = '\0';
    strcpy(temp,timeTableName);
    strcat(temp,"TimeTable.txt");

    FILE* timeTableFile = fopen(makePath(temp), "r+");

    time_t nowTime;
    struct tm *lt;
    time(&nowTime);
    lt = localtime(&nowTime);
    int timeTableArr[7][15]={0,};


    int dayWeek = 0;
    int timeDay = 0;

    printf("%s", timeTableName);
    printf("----------------------\n");
    printf("시간대를 선택하십시오. \n");
    printf("\t%3d %3d %3d %3d %3d %3d\n",10,12,2,4,6,8);
    for(i=0;i<7;i++){
        printf("%s\t",week[(lt->tm_wday+i)%7]);
        for(j=0;j<6;j++){
            fscanf(timeTableFile, "%d", &timeTableArr[i][j]);
            printf("%3d ", timeTableArr[i][j]);
        }
        printf("\n");
    }
    printf("시간대 입력 [줄 시간]형태로 입력: ");
    scanf("%d %d", &dayWeek, &timeDay);
    if((dayWeek<1 || timeDay < 1)||(dayWeek>7) ||(timeDay > 6)){
        printf("잘못 입력하셨습니다.\n");
        return;
    }

    FILE* reservationListFile = fopen(makePath("reservationlist.txt"), "a+");

    time_t reservationTime;
    time(&reservationTime);
    reservationTime = reservationTime+60*60*24*(dayWeek-1);
    struct tm *rt = localtime(&reservationTime);
    lt->tm_sec=0;
    lt->tm_min=0;
    lt->tm_hour=10+ (timeDay-1)*2;
    char nowDays[100];
    strftime(nowDays, 100, "%D %a/%Hh", rt);

    fprintf(reservationListFile,"%d %s %s\r\n",(int)getuid(), timeTableName, nowDays);//유저pid, 테마제목, [날짜, 요일, 시간대]
    fclose(reservationListFile);
    printf("\r\n%d %s %s 예약 중... \r\n",(int)getuid(), timeTableName, nowDays);

    FILE* tempTimeTableFile = tmpfile();

    if(timeTableArr[dayWeek-1][timeDay-1]==1){
        timeTableArr[dayWeek-1][timeDay-1]=0;
        printf("\n예약이 되었습니다\n");
    }else{
        printf("\n그 시간대에는 예약할 수 없습니다.\n");
    }

    for(i=0;i<7;i++){
        for(j=0;j<6;j++){
            printf("%3d ", timeTableArr[i][j]);
            fprintf(tempTimeTableFile, "%d", timeTableArr[i][j]);
            if(j<5) fprintf(tempTimeTableFile," ");
        }
        printf("\n");
        fprintf(tempTimeTableFile,"\r\n");
    }
    copy(timeTableFile,tempTimeTableFile,temp);
    fclose(timeTableFile);
    return;
}

void getReservationList(){
    printf("현재 고객님이 예약하신 내역은 아래와 같습니다.\n");
    FILE* reservationListFile = fopen(makePath("reservationlist.txt"),"r");
    char reservationList[100];
    char uidStr[10];
    sprintf(uidStr,"%d", (int)getuid());
    while(fgets(reservationList,sizeof(reservationList)/sizeof(char),reservationListFile)!=NULL){
        if(strstr(reservationList,uidStr)!=NULL){
            printf("%s", reservationList);
        }
    }
    fclose(reservationListFile);
    return;
}




int getthemeList(char arr[][30]){
        int newfd;

        newfd = open(makePath("themelist.txt"),O_RDONLY | O_CREAT);

        FILE* themeList = fdopen(newfd,"r");

        int selectNum = 0;
        int i;
        for (i = 1 ;fgets(arr[i], sizeof(arr[i])/sizeof(char), themeList)!=NULL; i++);
        return i;
}

int fileopen(char* filename) {
    int iDs;

    iDs = open(filename, O_RDONLY);
    if(iDs == -1){
        printf("%s", filename);
        printf("파일이 존재하지 않습니다.\n");
        exit(1);
    }
    return iDs;
}

int checkfile(int iDs){
    int icnt = 1;
    char tmp;
    FILE *fp = fdopen(iDs, "r");

    while(fscanf(fp,"%c",&tmp)!= EOF){
        if(tmp == '\n') {
        icnt++; }
    }
    return icnt;

}

char* makePath(char* name){
    static char absolutePath[100]=ADMIN_DIRECTORY;
    strcat(strcpy(absolutePath,ADMIN_DIRECTORY),name);
    printf("%s\n", absolutePath);
    return absolutePath;
}