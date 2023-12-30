#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

void search_in_file(const char *file_path, const char *target_bytes, int process_id) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long total_bytes = ftell(file);
    rewind(file);

    unsigned char buffer[256];
    size_t bytesRead;
    int found = 0;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (strstr(buffer, target_bytes) != NULL) {
            found = 1;
            break;
        }
    }

    printf("Process ID: %d, File: %s, Total Bytes: %ld, Result: %s\n", process_id, file_path,
           total_bytes, found ? "Found" : "Not Found");

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <directory_path> <N> <target_bytes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *directory_path = argv[1];
    int max_processes = atoi(argv[2]);
    const char *target_bytes = argv[3];

    DIR *dir = opendir(directory_path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    int processes_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char file_path[256];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, entry->d_name);

            pid_t pid = fork();

            if (pid == 0) {
                search_in_file(file_path, target_bytes, getpid());
                exit(EXIT_SUCCESS);
            } else if (pid > 0) {
                processes_count++;

                if (processes_count >= max_processes) {
                    wait(NULL);
                    processes_count--;
                }
            } else {
                perror("Error forking process");
                exit(EXIT_FAILURE);
            }
        }
    }

    while (wait(NULL) > 0) {
        processes_count--;
    }

    closedir(dir);

    return 0;
}
