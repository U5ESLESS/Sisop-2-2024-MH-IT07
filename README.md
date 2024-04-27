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

## Soal 4

soal nomor 4 adalah user diminta untuk membuka berbagai macam aplikasi dengan banyak jendela aplikasi dalam satu command dan menamai file programnya setup.c
dan ada beberapa handler untuk argument seperti:
- `-o` yang digunakan untuk open program berdasarkan parameter yang diketikan setelahnya, contoh `./setup -o spotify 1 firefox 2`
- `-f` yang digunakan untuk open program berdasarkan file config yang kita ketikkan setelahnya, contoh `./setup -f file.conf`
- `-k` yang digunakan untuk mengekill semua program yang dijalankan menggunakan `./setup`, ada juga untuk kill sesuai file conf dengan cara `./setup -k file.conf`

maka pada fungsi mainnya memiliki alur seperti ini:
```c
int main(int argc, char *argv[]) {
    loadRunningProcesses();

    if (argc < 2) {
        printf("Usage: %s -o <app1> <num1> <app2> <num2> ... <appN> <numN> or %s -f <filename> or %s -k\n", argv[0], argv[0], argv[0]);
        exit(1);
    }

    if (strcmp(argv[1], "-o") == 0) {
        char *apps[Max_App];
        int num_apps[Max_App];
        int numArgs = argc - 3;

        int i, appIndex = 0;
        for (i = 2; i < argc; i += 2) {
            apps[appIndex] = argv[i];
            num_apps[appIndex] = atoi(argv[i + 1]);
            appIndex++;
        }
        apps[appIndex] = NULL;

        launchApps(apps, num_apps);
        saveRunApps();
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Usage: %s -f <filename>\n", argv[0]);
            exit(1);
        }
        readConfig(argv[2]);
        saveRunApps();
    } else if (strcmp(argv[1], "-k") == 0) {
        if (argc == 3) {
            terminateFromConfig(argv[2]);
        } else {
            terminateProcesses();
            remove(FILENAME);
        }
    } else {
        printf("Invalid\n");
        exit(1);
    }

    return 0;
}
```
## Fungsi Untuk Membuka Aplikasi
```
void loadRunningProcesses() {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        fscanf(file, "%d", &num_run);
        fclose(file);
    }
}
void launchApps(char *apps[], int num_apps[]) {
    pid_t pid;

    for (int i = 0; i < MAX_APPS && apps[i] != NULL; i++) {
        for (int j = 0; j < num_apps[i]; j++) {
            pid = fork();

            if (pid == 0) {
                execlp(apps[i], apps[i], NULL);
                perror("execlp");
                exit(0);
            } else {
                addProcess(pid);
            }
        }
    }
}
```
Lalu setelah itu untuk proses buka app saya menambahkan fungsi untuk menambahkan pid ke file run.txt
```
void addProcess(pid_t pid) {
    if (num_run < MAX_APPS) {
        run[num_run++] = pid;
    }
}

void saveProcesses() {
    FILE *file = fopen(FILENAME, "w");
    if (file != NULL) {
        fprintf(file, "%d\n", num_run);
        for (int i = 0; i < num_run; i++) {
            fprintf(file, "%d\n", run[i]);
        }
        fclose(file);
    }
}

int startApp(char *app_name, int num_app) {
    pid_t pid;

    for (int j = 0; j < MAX_APPS; j++) {
        pid = fork();
        if (pid == 0) {
            execlp(app_name, app_name, NULL);
            perror("execlp");
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            return 1;
        } else {
            addProcess(pid);
        }
    }
    return 0;
}

void readConfig(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    char *app;
    int num;

    while (fgets(line, sizeof(line), file)) {
        app = strtok(line, " ");
        num = atoi(strtok(NULL, " \n"));

        for (int i = 0; i < num; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                execlp(app, app, NULL);
                perror("execlp");
                exit(0);
            } else {
                addProcess(pid);
            }
        }
    }

    fclose(file);
}

int launchFromConfig(char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", config_file);
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    char *app;
    int num;

    while (fgets(line, sizeof(line), file)) {
        app = strtok(line, " ");
        num = atoi(strtok(NULL, " \n"));

        for (int i = 0; i < num; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                execlp(app, app, NULL);
                exit(0);
            } else {
                addProcess(pid);
            }
        }
    }

    fclose(file);
}
```
## Kill Proses
```
void terminateApp(char *app) {
    if (fork() == 0) {
        execlp("pkill", "pkill", app, NULL);
        exit(0);
    }
    sleep(1);
}

void terminateFromConfig(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        char app[256];
        int num;
        while (fscanf(file, "%s %d", app, &num) == 2) {
            terminateApp(app);
        }
        fclose(file);
    }
}

void terminateProcesses() {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        int num_processes;
        fscanf(file, "%d", &num_processes);
        for (int i = 0; i < num_processes; i++) {
            int pid;
            fscanf(file, "%d", &pid);
            if (kill(pid, SIGTERM) == -1) {
                perror("Error killing process");
            }
        }
        fclose(file);
    }
}
```
Fungsi `terminateApp` akan memanggil pkill untuk menghentikan proses berdasarkan nama aplikasi yang diberikan. Fungsi `terminateFromConfig` akan membaca daftar nama aplikasi dari file konfigurasi dan menghentikan proses untuk setiap aplikasi tersebut. Sedangkan fungsi `terminateProcesses` akan membaca daftar PID dari file run.txt dan menghentikan setiap proses dengan menggunakan kill system call.
