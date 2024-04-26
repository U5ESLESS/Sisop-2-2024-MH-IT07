# Sisop-2-2024-MH-IT07


## Soal 3
Pak Heze adalah seorang admin yang baik. Beliau ingin membuat sebuah program admin yang dapat memantau para pengguna sistemnya. Bantulah Pak Heze untuk membuat program  tersebut!

a.  Nama program tersebut dengan nama admin.c

b. Program tersebut memiliki fitur menampilkan seluruh proses yang dilakukan oleh seorang user dengan menggunakan command:
  ./admin <user>

c. Program dapat memantau proses apa saja yang dilakukan oleh user. Fitur ini membuat program berjalan secara daemon dan berjalan terus menerus. Untuk menjalankan         fitur ini menggunakan command: 
   ./admin -m <user>
   Dan untuk mematikan fitur tersebut menggunakan: 
   ./admin -s <user>
   Program akan mencatat seluruh proses yang dijalankan oleh user di file <user>.log dengan format:
   [dd:mm:yyyy]-[hh:mm:ss]_pid-process_nama-process_GAGAL/JALAN

d. Program dapat menggagalkan proses yang dijalankan user setiap detik secara terus menerus dengan menjalankan: 
  ./admin -c user
  sehingga user tidak bisa menjalankan proses yang dia inginkan dengan baik. Fitur ini dapat dimatikan dengan command:
  ./admin -a user

e. Ketika proses yang dijalankan user digagalkan, program juga akan melog dan menset log tersebut sebagai GAGAL. Dan jika di log menggunakan fitur poin c, log akan
  ditulis dengan JALAN 

### Langkah Pengerjaan
- Buat script `admin.c` melalui editor nano menggunakan command `nano admin.c`
- Isi dari script `admin.c`
 ``` #include <stdio.h>
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
```
- Compile program menggunakan command `gcc -o admin admin.c`
- Run program menggunakan command `./admin <perintah> <user>`

### Penjelasan Script
1. Library dan konstanta yang digunakan
```
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
```
2. Deklarasi Fungsi
- Mencatat aktivitas user kedalam file `.log`
```
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
```
- Menampilkan aktivitas user yang disimpan pada file `.log`
```
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
```
- Melakukan monitoring proses terkait user
```
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
```
- Menghentikan monitoring proses user
```
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
```
- Memblock semua proses terkait user
```
void block(char *user) {
    printf("Memblokir Proses %s\n", user);
    char cmd[100];
    sprintf(cmd, "pkill -u %s", user);
    system(cmd);
}
```
3. Fungsi `main`
```
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
```
