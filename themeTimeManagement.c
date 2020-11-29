#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> //open에서 flags를 이용하기 위한 헤더
#include <string.h>
#include <time.h>


int themeTimeManagement(){
int i, j, num;
    int fd;
    char arr[12][30];
    char str[30] = "";
    int max = getthemeList(arr);
    const char week[7][10] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

    for(i=1;i<max;i++){
        printf("%d : %s", i, arr[i]);
    }
    printf("\n-------------------\n");
    printf("어떤 테마의 방 선택하시겠습니까? : ");
    scanf("%d",&num);

    char timeTableName[40];
    strcpy(timeTableName,arr[num]);
    //timeTableName에 들어 있는 값은 현재 "테마이름\r\n"이 저장되어 있음.
    timeTableName[(int)(strcspn(timeTableName,"\r\n"))] = '\0';
    timeTableName[(int)(strcspn(timeTableName,"\n"))] = '\0';

    strcat(timeTableName,"TimeTable.txt");

    fd = open(makePath(timeTableName), O_RDWR);
    FILE* timeTableFile = fdopen(fd, "r+");
    time_t nowTime;
    struct tm *lt;
    time(&nowTime);
    lt = localtime(&nowTime);
    int timeTableArr[7][15]={0,};

    while(1){
        int dayWeek = 0;
        int timeDay = 0;

        printf("%s", timeTableName);
        printf("----------------------\n");
        printf("변경할 시간대를 선택하십시오. \n");
        printf("\t%3d %3d %3d %3d %3d %3d\n",10,12,2,4,6,8);
        for(i=0;i<7;i++){
            printf("%s\t",week[(lt->tm_wday+i)%7]);
            for(j=0;j<6;j++){
                fscanf(timeTableFile, "%d", &timeTableArr[i][j]);
                printf("%3d ", timeTableArr[i][j]);
            }
            printf("\n");
        }
        printf("시간대 입력 [줄 시간]형태로 입력(그만하고 싶으면 [0 0] 입력): ");
        scanf("%d %d", &dayWeek, &timeDay);

        if(dayWeek== 0 &&timeDay== 0){
            printf("\n해당 테마의 예약 관리를 완료합니다.\n");
            break;
        }else if((dayWeek<0 || timeDay < 0)||(dayWeek>7) ||(timeDay > 6)){
            printf("\n 잘못 입력하셨습니다\n");
            continue;
        }

        if(timeTableArr[dayWeek-1][timeDay-1]==1){
            timeTableArr[dayWeek-1][timeDay-1]=0;
            printf("\n변경되었습니다.\n");
        }else{
            timeTableArr[dayWeek-1][timeDay-1]=1;
            printf("\n변경되었습니다.\n");
        }

        FILE* tempTimeTableFile = tmpfile();

        for(i=0;i<7;i++){
            for(j=0;j<6;j++){
                fprintf(tempTimeTableFile, "%d", timeTableArr[i][j]);
                if(j<5) fprintf(tempTimeTableFile," ");
            }
            fprintf(tempTimeTableFile,"\r\n");
        }
        copy(timeTableFile,tempTimeTableFile,timeTableName);
    }

    fclose(timeTableFile);
    return;
}

//fclose를 안하면 변경사항 저장 x