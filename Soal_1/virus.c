#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

void replaceString(char *str) {
    char *ptr;
    while ((ptr = strstr(str, "m4LwAr3")) != NULL) {
        memcpy(ptr, "[MALWARE]", 9);
    }
    while ((ptr = strstr(str, "5pYw4R3")) != NULL) {
        memcpy(ptr, "[SPYWARE]", 9);
    }
    while ((ptr = strstr(str, "R4nS0mWaR3")) != NULL) {
        memcpy(ptr, "[RANSOMWARE]", 12);
    }
}

void writeToLog(const char *filename) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buffer[26];
    strftime(buffer, 26, "[%d-%m-%Y][%H:%M:%S]", tm_info);

    FILE *logFile = fopen("virus.log", "a");
    fprintf(logFile, "%s Suspicious string at %s successfully replaced!\n", buffer, filename);
    fclose(logFile);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid, sid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    if ((chdir(argv[1])) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1) {
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(argv[1])) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_type == DT_REG) {
                    char filename[512];
                    snprintf(filename, sizeof(filename), "%s/%s", argv[1], ent->d_name);
                    FILE *file = fopen(filename, "r+");
                    if (file != NULL) {
                        fseek(file, 0, SEEK_END);
                        long fileSize = ftell(file);
                        fseek(file, 0, SEEK_SET);
                        char *fileContent = (char *)malloc(fileSize + 1);
                        fread(fileContent, 1, fileSize, file);
                        fileContent[fileSize] = '\0';
                        replaceString(fileContent);
                        fseek(file, 0, SEEK_SET);
                        fwrite(fileContent, 1, fileSize, file);
                        fclose(file);
                        free(fileContent);
                        writeToLog(filename);
                    }
                }
            }
            closedir(dir);
        }
        sleep(15);
    }

    exit(EXIT_SUCCESS);
}
