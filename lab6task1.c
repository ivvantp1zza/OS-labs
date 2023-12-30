#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>



void print_time() {
    struct timeval tv;
    struct tm* time_info;
    char time_str[40];

    gettimeofday(&tv, NULL);
    time_info = localtime(&tv.tv_sec);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", time_info);
    sprintf(time_str + strlen(time_str), ":%03ld", tv.tv_usec / 1000);
    printf("Текущее время: %s\n", time_str);
}

int main() {
    pid_t process1, process2;
    if ((process1 = fork()) == 0) {
        printf("Process 1 - pid: %d, ppid: %d ", getpid(), getppid());
        print_time();
        exit(EXIT_SUCCESS);
    }

    if ((process2 = fork()) == 0) {
        printf("Process 2 - pid: %d, ppid: %d ", getpid(), getppid());
        print_time();
        exit(EXIT_SUCCESS);
    }

    printf("Parent - pid: %d ", getpid());
    print_time();

    if (system("ps -x | grep \"lab6task1\"") == -1) {
        exit(EXIT_FAILURE);
    }
    waitpid(process1, NULL, 0);
    waitpid(process2, NULL, 0);
}
