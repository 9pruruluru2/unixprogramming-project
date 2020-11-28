#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "reservation.c"
#include "themeManagement.c"
#include "themeTimeManagement.c"

void user();
void admin();
void movieList(int iDs);
void timetableManagement();



int main() {

    int val;

    printf("---------------\n");
    printf(" 1. 관리자 모드\n");
    printf(" 2. 사용자 모드\n");
    printf("---------------\n");
    printf("숫자를 입력하시오 : ");
    scanf("%d", & val);

    switch (val) {
        case 1:
            if((int)getuid()!=1295){
                printf("당신은 관리자 권한이 없습니다. %d\n",getuid());
                return;
            }
            admin();
            break;
        case 2:
            user();
            break;
        default:
            printf("잘못 입력하셨습니다.\n");
            break;
        }

    printf("종료되었습니다.\n");
    return 0;
}

void user() {
    int val;
    int iDs;
    int icnt;

    iDs = fileopen(makePath("themelist.txt"));
    icnt = checkfile(iDs);

    while(1){

        movieList(iDs);

        printf("-------환영합니다.-------\n");
        printf(" 1. 테마보기 \n");
        printf(" 2. 프로그램 종료\n");
        printf("--------------\n");
        printf("숫자를 입력하시오 : ");
        scanf("%d", &val);

        switch(val){
            case(1): userSelect(); break;
            case(2): return; break;
            default: printf("잘못 입력하셨습니다.\n");
        }
    }
}

void admin() {


    int val;
    pid_t pid;
    if((pid=fork())==0){
        while(1){
        timetableManagement();
        }
        return;
    }

    while(1){
    printf("------------------\n");
        printf(" 1. 테마 추가\n");
        printf(" 2. 테마 제거\n");
        printf(" 3. 테마 예약 관리\n");
        printf(" 4. 프로그램 종료\n");
        printf("------------------\n");
        printf("숫자를 입력하시오 : ");
        scanf("%d", &val);

        switch(val){
            case(1): addTheme(); break;
            case(2): removeTheme(); break;
            case(3): themeTimeManagement(); break;
            case(4): return; break;
            default: printf("잘못 입력하셨습니다.\n");
        }
    }

}


void movieList(int iDs){
    char str[30] = "";
    int i;
    int newfd = open(makePath("themelist.txt"),O_RDONLY);
    FILE* file = fdopen(newfd, "r");

    printf("------theme list------\n");
    for(i=1;fgets(str, 30, file)!=NULL;i++){
        printf("%d %s",i,str);
    }
}

void timetableManagement(){
    /*현재 시간을 가져오고, 12시와 비교 비교한후 비교값 이후로 자동 삭제 후, 자동 추가까지*/
    time_t nowTime;
    time_t afterTime;
    struct tm *lt;

    time(&nowTime);
    time(&afterTime);

    /*여기서 고려한 점 원래 struct tm을 이용하여 날짜를 변경하려 했으나,
    그런 식으로 변경하면 31일에서 1일로 넘어갈 때 일 변경과 월 변경을 해야하므로 코드가 길어짐.
    그래서 time_t를 이용하여 다음날로 넘긴 후, tm으로 변환하여 다음날 0시0분0초으로 설정하여
    다시 time_t로 변경하는 방식을 채택
    */

    afterTime = afterTime + 24*60*60+10;//10초정도는 여유
    lt = localtime(&afterTime);

    lt->tm_sec=0;
    lt->tm_min=0;
    lt->tm_hour=0;
    afterTime = mktime(lt);

    unsigned int  sleepTime = (unsigned int)(afterTime - nowTime);

    sleep(sleepTime);
    int k;

    int newfd;

    newfd = open(makePath("themelist.txt"),O_RDONLY | O_CREAT);

    FILE* themeList = fdopen(newfd,"r");

    char themeListArr[20][1024] = {""};

    for (k = 1 ;!(feof(themeList)); k++){
        fgets(themeListArr[k], sizeof(themeListArr[k])/sizeof(char), themeList);
        if(feof(themeList)|| k> 1) break;


        themeListArr[k][(int)(strcspn(themeListArr[k],"\r\n"))] ='\0';//이상하게 나오면 이거 제거
        themeListArr[k][(int)(strcspn(themeListArr[k],"\n"))] ='\0';
        strcat(themeListArr[k],"TimeTable.txt");
        themeListArr[k][strlen(themeListArr[k])] = '\0';

        printf("%s", themeListArr[k]);

        FILE* timeTableFile = fopen(makePath(themeListArr[k]), "r+");
        fseek(timeTableFile,0,SEEK_SET);
        if (timeTableFile == NULL) {
                perror("fopen()");
                return;
            }
        char timeTableArr[7][15]={0,};

        int i=0;

        while(!feof(timeTableFile)){
            fgets(timeTableArr[i],sizeof(timeTableArr[i])/sizeof(char), timeTableFile);
            i++;
        }
        fseek(timeTableFile,0,SEEK_SET);
        i=1;
        while(i<7){
            fputs(timeTableArr[i], timeTableFile);
            i++;
        }fputs("1 1 1 1 1 1\r\n", timeTableFile);
    }
    return;
}