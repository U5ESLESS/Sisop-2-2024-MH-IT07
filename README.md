# Sisop-2-2024-MH-IT07

## Soal 1
Gavriel adalah seorang cyber security enthusiast. Suatu hari, ia dikontrak oleh sebuah perusahaan ternama untuk membuat sebuah program yang cukup rumit dengan bayaran jutaan rupiah. Karena tergoda dengan nominal bayaran tersebut, Gavriel langsung menerima tawaran tersebut. Setelah mencoba membuat dan mengembangkan program tersebut selama seminggu, laptop yang digunakan Gavriel mengalami overheat dan mati total sehingga harus dilarikan ke tukang servis terdekat. Karena keterbatasan waktu dalam pembuatan program dan tidak ingin mengecewakan perusahaan, Gavriel meminta bantuan kalian untuk membuat program tersebut dengan ketentuan sebagai berikut:
a. Program dapat menerima input path berupa ‘argv’ untuk mengatur folder dimana file akan dieksekusi
b. Program tersebut berfungsi untuk mereplace string dengan ketentuan sebagai berikut:
    - String m4LwAr3 direplace dengan string [MALWARE]
    - String 5pYw4R3 direplace dengan string [SPYWARE]
    - String R4nS0mWaR3 direplace dengan string [RANSOMWARE]
c. Program harus berjalan secara daemon, dan tidak diperbolehkan menggunakan command system() dalam pembuatan program
d. Program akan secara terus menerus berjalan di background dengan jeda 15 detik
e. Catat setiap penghapusan string yang dilakukan oleh program pada sebuah file bernama virus.log dengan format: [dd-mm-YYYY][HH:MM:SS] Suspicious string at <file_name> successfully replaced!

Contoh penggunaan: ./virus /home/user/virus

Contoh isi file sebelum program dijalankan:
pU=-JWQ$5$)D-[??%AVh]$cB6bm4LwAr3jEQC2p3R{HV]=-AUaxj:Qe+h
!aNX,i:!z3W=2;.tHc3&S+}6F)CFf%tfZLP1*w5m1PAzZJUux(
Pd&f8$F5=E?@#[6jd{TJKj]5pYw4R3{KK1?hz384$ge@iba5GAj$gqB41
#C&&a}M9C#f64Eb.?%c)dGbCvJXtU[?SE4h]BY4e1PR4nS0mWaR3{]S/{w?*


Contoh isi file setelah setelah program dijalankan:
pU=-JWQ$5$)D-[??%AVh]$cB6b[MALWARE]jEQC2p3R{HV]=-AUaxj:Qe+h
!aNX,i:!z3W=2;.tHc3&S+}6F)CFf%tfZLP1*w5m1PAzZJUux(
Pd&f8$F5=E?@#[6jd{TJKj][SPYWARE]{KK1?hz384$ge@iba5GAj$gqB41
#C&&a}M9C#f64Eb.?%c)dGbCvJXtU[?SE4h]BY4e1P[RANSOMWARE]{]S/{w?*



### Langkah Pengerjaan
- Buat file text untuk string yang akan di replace `nano test.txt`
- Isi dari text `test.txt`

pU=-JWQ$5$)D-[??%AVh]$cB6bm4LwAr3jEQC2p3R{HV]=-AUaxj:Qe+h
!aNX,i:!z3W=2;.tHc3&S+}6F)CFf%tfZLP1*w5m1PAzZJUux(
Pd&f8$F5=E?@#[6jd{TJKj]5pYw4R3{KK1?hz384$ge@iba5GAj$gqB41
#C&&a}M9C#f64Eb.?%c)dGbCvJXtU[?SE4h]BY4e1PR4nS0mWaR3{]S/{w?*

- buat file virus.c
- isi file virus.c menggunakan code c berikut

```
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
```

- Compile program menggunakan command `gcc -o virus.c virus`
- Run program menggunakan command `./virus <perintah> <user>`
- program `virus` akan berjalan sebagai daemon dan merubah string yang mencurigakan
- untuk mengecek apakah `virus` berjalan sebagai daemon bisa dilihat menggunakan `ps aux | grep virus`

### Penjelasan Script
1. Library yang digunakan
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
```
2. Fungsi
- void dari replaceString
- Fungsi ini bertugas untuk mencari dan mengganti beberapa string tertentu dalam teks dengan string yang lain
```
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

```
- void dari writeToLog
- Fungsi ini digunakan untuk menulis ke file log bernama "virus.log" dan melakukan berbagai fungsi seperti berikut
`
mencatat waktu menggunakan time(NULL), Membuka file "virus.log" dalam mode "append" ("a"), Menulis ke file log yang berisi waktu dan nama file yang sudah berhasil dirubah, Menutup file log menggunakan fclose().
`
```
void writeToLog(const char *filename) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char buffer[26];
    strftime(buffer, 26, "[%d-%m-%Y][%H:%M:%S]", tm_info);

    FILE *logFile = fopen("virus.log", "a");
    fprintf(logFile, "%s Suspicious string at %s successfully replaced!\n", buffer, filename);
    fclose(logFile);
}

```
- Fungsi utama (int main)
- Memeriksa apakah jumlah argumen yang diberikan benar.
```
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
```
- Menggunakan fork() untuk memulai proses baru dan memastikan bahwa program berjalan sebagai daemon.
```
    pid_t pid, sid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
```
- Mengatur environment dengan umask(0) dan setsid().
```
    umask(0);

    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    if ((chdir(argv[1])) < 0) {
        exit(EXIT_FAILURE);
    }
```
- Mengubah direktori kerja ke direktori yang diberikan sebagai argumen.
- Menutup file descriptor standar.
- Memproses setiap file dalam direktori yang ditentukan:
```
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
```
- Membuka direktori dan membaca setiap entri.
- Jika entri adalah file reguler, membuka file, membaca isinya, dan memanggil replaceString untuk mengganti string yang mencurigakan.
- Menulis kembali konten yang sudah dimodifikasi ke file dan menutup file.
- Membebaskan memori yang dialokasikan untuk konten file.
- Memanggil writeToLog untuk mencatat perubahan yang dilakukan.
- Menunggu selama 15 detik sebelum memeriksa kembali.

```
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
```
## Soal 2
tidak mengerjakan

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
