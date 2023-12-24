#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

int compare_files(const char *file1, const char *file2) {
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");

    if (f1 == NULL || f2 == NULL) {
        perror("error opening file");
        return -1;
    }

    long size1 = ftell(f1);
    long size2 = ftell(f2);
    char *buffer1 = (char*) malloc(sizeof(char) * size1);
    char *buffer2 = (char*) malloc(sizeof(char) * size2);
    int result = 0;
    size_t bytes1, bytes2;

    do {
        bytes1 = fread(buffer1, 1, size1, f1);
        bytes2 = fread(buffer2, 1, size2, f2);
        if (bytes1 != bytes2 || memcmp(buffer1, buffer2, bytes1) != 0) {
            result = 1;
            break;
        }
    } while (bytes1 > 0);

    fclose(f1);
    fclose(f2);

    return result;
}

void print_file_info(const char *file, FILE *output_file) {
    struct stat file_stat;

    if (stat(file, &file_stat) == -1) {
        perror("error getting file information");
        return;
    }

    fprintf(output_file, "File: %s\n", file);
    fprintf(output_file, "Size: %ld bytes\n", file_stat.st_size);
    fprintf(output_file, "Creation time: %s", ctime(&file_stat.st_ctime));
    fprintf(output_file, "Permissions: %o\n", file_stat.st_mode & 0777);
    struct passwd *pwd = getpwuid(file_stat.st_uid);
    struct group *grp = getgrgid(file_stat.st_gid);

    if (pwd != NULL) {
        fprintf(output_file, "Owner: %s\n", pwd->pw_name);
    }

    if (grp != NULL) {
        fprintf(output_file, "Group: %s\n", grp->gr_name);
    }

    fprintf(output_file, "Inode number: %ld\n", file_stat.st_ino);
    fprintf(output_file, "\n");
    printf("File: %s\n", file);
    printf("Size: %ld bytes\n", file_stat.st_size);
    printf("Creation time: %s", ctime(&file_stat.st_ctime));
    printf("Permissions: %o\n", file_stat.st_mode & 0777);
    printf("Owner: %s\n", pwd->pw_name);
    printf("Group: %s\n", grp->gr_name);
    printf("Inode number: %ld\n", file_stat.st_ino);
    printf("\n");
}

void find_duplicate_files(const char *dir1, const char *dir2, const char *output) {
    FILE *output_file = fopen(output, "w");
    if (output_file == NULL) {
        perror("error opening output file");
        return 1;
    }

    DIR *dp1, *dp2;
    struct dirent *entry1, *entry2;

    if ((dp1 = opendir(dir1)) == NULL || (dp2 = opendir(dir2)) == NULL) {
        printf("error opening directory %s %s", dir1, dir2);
        return 1;
    }

    while ((entry1 = readdir(dp1)) != NULL) {
        if (entry1->d_type == DT_REG) {
            char path1[PATH_MAX];
            snprintf(path1, PATH_MAX, "%s/%s", dir1, entry1->d_name);

            while ((entry2 = readdir(dp2)) != NULL) {
                if (entry2->d_type == DT_REG) {
                    char path2[PATH_MAX];
                    snprintf(path2, PATH_MAX, "%s/%s", dir2, entry2->d_name);
                    if (compare_files(path1, path2) == 0) {
                        print_file_info(path1, output_file);
                        print_file_info(path2, output_file);
                    }
                }
                else if (entry2->d_type == DT_DIR && strcmp(entry2->d_name, ".") != 0 && strcmp(entry2->d_name, "..") != 0) {
                    char subdir1[PATH_MAX];
                    snprintf(subdir1, PATH_MAX, "%s", dir1);
                    char subdir2[PATH_MAX];
                    snprintf(subdir2, PATH_MAX, "%s/%s", dir2, entry2->d_name);
                    find_duplicate_files(subdir1, subdir2, output);
                }
            }
        } else if (entry1->d_type == DT_DIR && strcmp(entry1->d_name, ".") != 0 && strcmp(entry1->d_name, "..") != 0) {
            char subdir1[PATH_MAX];
            snprintf(subdir1, PATH_MAX, "%s/%s", dir1, entry1->d_name);
            char subdir2[PATH_MAX];
            snprintf(subdir2, PATH_MAX, "%s", dir2);
            find_duplicate_files(subdir1, subdir2, output);
        }
    }
    closedir(dp1);
    closedir(dp2);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("wrong args count(should be 3)");
        return 1;
    }

    find_duplicate_files(argv[1], argv[2], argv[3]);

    return 0;
}