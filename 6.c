#include<stdio.h>
#include<dirent.h>

int processDirectory(char *dirName){
    DIR *dir = opendir(dirName);
    if(dir == NULL){
        printf("cannot open %s directory", dirName);
        return 1;
    }   
    printf("directory %s:\n", dirName); 
    struct dirent *d;
    while ((d = readdir(dir)) != NULL) 
        printf("%s\n", d->d_name);
    
    if(closedir(dir) != 0){
        printf("cannot close %s directory\n", dirName); 
        return 1;
    }
    return 0; 
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("wrong args count(should be 1)\n");
        return 1;
    }

    int res1 = processDirectory(".");
    if(res1 != 0){
        return res1;
    }
    printf("\n");

    int res2 = processDirectory(argv[1]);
    if(res2 != 0){
        return res2;
    }

    return 0;
}