#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("wrong args count(should be 2)\n");
        return 1;
    }

    FILE *source = fopen(argv[1], "r");
    if (source == NULL) {
        printf("cannot open file 1\n");
        return 1;
    }

    FILE *target = fopen(argv[2], "w");
    if (target == NULL) {
        printf("cannot open file 2\n");
        fclose(source);
        return 1;
    }

    char c;
    while ((c = fgetc(source)) != EOF) {
        fputc(c, target);
    }

    int e = fclose(source);
    if (e != 0) {
        return e;
    }

    e = fclose(target);
    if (e != 0) {
        return e;
    }

    struct stat st;
    if (stat(argv[1], &st) != 0 || chmod(argv[2], st.st_mode) != 0) {
        printf("cannot copy premissions\n");
        return 1;
    }

    return 0;
}