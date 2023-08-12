#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


void sigHandlerHUP(int signum){

  printf("Ouch!\n");
}
void sigHandlerINT(int signum){

  printf("Yeah!\n");
}


int main(int argc, char* argv[]) {
    int even = 0;
    int n = atoi(argv[1]);

    signal(SIGHUP, sigHandlerHUP);
    signal(SIGINT, sigHandlerINT);

    
    for (int i = 0; i < n; i++){
        printf("%d\n", even);
        fflush(stdout);
        even +=2;
        sleep(5);
    }
    return 0;
}