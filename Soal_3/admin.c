#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

#define LOG_FILE_EXTENSION ".log"
#define LOG_MAX_LENGTH 200

void log_activity(char *user, char *activity, int status) {
    char log_filename[50];
    time_t now;
    struct tm *tm_info;
    FILE *log_file;

    sprintf(log_filename, "%s%s", user, LOG_FILE_EXTENSION);

    log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    // realtime
    now = time(NULL);
    tm_info = localtime(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%d:%m:%Y-%H:%M:%S", tm_info);

    // save .log
    fprintf(log_file, "[%s]-%d-%s_%s\n", timestamp, getpid(), activity, status ? "JALAN" : "GAGAL");
    fclose(log_file);
}

void monitor(char *user) {
    char log_filename[50];
    sprintf(log_filename, "%s%s", user, LOG_FILE_EXTENSION);
    FILE *log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    fprintf(log_file, "Monitoring: %s\n", user);
    fflush(log_file);

    while (1) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Error forking process");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            dup2(fileno(log_file), STDOUT_FILENO);
            execlp("ps", "ps", "-u", user, NULL);
            perror("Error executing ps command");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
        }

        sleep(1);
    }

    fclose(log_file);
}

void block(char *user) {
    printf("Memblokir proses untuk pengguna: %s\n", user);
    char cmd[100];
    sprintf(cmd, "pkill -u %s", user);
    system(cmd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Perintah: %s -m <user> | -s <user> | -c <user> | -a <user>\n", argv[0]);
        return 1;
    }

    char *user = argv[2];

    if (strcmp(argv[1], "-m") == 0) {
        monitor(user);
    } else if (strcmp(argv[1], "-s") == 0) {
        // kill
        printf("Fitur stop belum diimplementasikan\n");
    } else if (strcmp(argv[1], "-c") == 0) {
        block(user); // block
        log_activity(user, "Menggagalkan proses", 0); 
    } else if (strcmp(argv[1], "-a") == 0) {
        // unblock
        printf("Fitur unblock belum diimplementasikan\n");
    } else {
        printf("Perintah tidak diketahui!\n");
        return 1;
    }

    return 0;
}
