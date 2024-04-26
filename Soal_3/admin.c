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
        perror("Gagal membuka file.log!");
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

void display_user_activity(char *user) {
    char log_filename[50];
    sprintf(log_filename, "%s%s", user, LOG_FILE_EXTENSION);
    FILE *log_file = fopen(log_filename, "r");
    if (log_file == NULL) {
        perror("Gagal membuka file.log!");
        exit(EXIT_FAILURE);
    }

    char line[LOG_MAX_LENGTH];
    printf("Aktivitas pengguna %s:\n", user);
    while (fgets(line, LOG_MAX_LENGTH, log_file) != NULL) {
        printf("%s", line);
    }

    fclose(log_file);
}

void monitor(char *user) {
//    printf("Memulai Monitoring %s\n...", user);
    char log_filename[50];
    sprintf(log_filename, "%s%s", user, LOG_FILE_EXTENSION);
    FILE *log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        perror("Gagal membuka file.log!");
        exit(EXIT_FAILURE);
    }

    fprintf(log_file, "Memulai Monitoring %s\n", user);
    fflush(log_file);

    while (1) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Error!");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            dup2(fileno(log_file), STDOUT_FILENO);
            execlp("ps", "ps", "-u", user, NULL);
            perror("Eksekusi Gagal!");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
        }

        sleep(1);
    }

    fclose(log_file);
}

void stop_monitor(char *user) {
    char log_filename[50];
    sprintf(log_filename, "%s%s", user, LOG_FILE_EXTENSION);
    FILE *log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        perror("Gagal membuka file.log!");
        exit(EXIT_FAILURE);
    }

    fprintf(log_file, "Stop Monitoring %s\n", user);
    fclose(log_file);
    exit(EXIT_SUCCESS);
}

void block(char *user) {
    printf("Memblokir Proses %s\n", user);
    char cmd[100];
    sprintf(cmd, "pkill -u %s", user);
    system(cmd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Perintah Tersedia : %s -m <Start> | -s <Stop> | -c <Block> | -a <Unblock>\n", argv[0]);
        return 1;
    }

    char *user = argv[2];

    if (strcmp(argv[1], "-m") == 0) {
        monitor(user);
        log_activity(user, "Memulai Monitoring...", 0);
    } else if (strcmp(argv[1], "-s") == 0) {
        stop_monitor(user);
    } else if (strcmp(argv[1], "-c") == 0) {
        block(user); // block
        log_activity(user, "Menggagalkan proses", 0); 
    } else if (strcmp(argv[1], "-a") == 0) {
        // unblock
        printf("Invalid! Belum selesai.\n");
    } else {
        printf("Perintah tidak diketahui!\n");
        return 1;
    }

    return 0;
}
