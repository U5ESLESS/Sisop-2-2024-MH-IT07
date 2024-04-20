#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define MAX_LINE_LENGTH 100
#define MAX_PROCESSES 100

pid_t child_pids[MAX_PROCESSES];
int num_child_pids = 0;

// Fungsi untuk meluncurkan aplikasi dengan jumlah jendela yang ditentukan
void launch_application(char *app_name, int num_windows) {
    pid_t pid;

    // Loop sebanyak jumlah jendela yang diinginkan
    for (int j = 0; j < num_windows; j++) {
        pid = fork();
        if (pid == 0) {
            // Proses anak
            execlp(app_name, app_name, NULL);
            // Jika execlp gagal
            perror("execlp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            // Penanganan error saat fork()
            perror("fork");
            exit(EXIT_FAILURE);
        } else {
            // Proses induk
            child_pids[num_child_pids++] = pid;
        }
    }
}

// Fungsi untuk membaca dan menjalankan aplikasi dari file konfigurasi
void launch_from_config(char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char app_name[MAX_LINE_LENGTH];
        int num_windows;
        sscanf(line, "%s %d", app_name, &num_windows);
        launch_application(app_name, num_windows);
    }

    fclose(file);
}

// Fungsi untuk menutup semua proses anak yang berjalan
void cleanup_child_processes() {
    for (int i = 0; i < num_child_pids; i++) {
        if (kill(child_pids[i], SIGTERM) == -1) {
            // Penanganan error saat mengirim sinyal kill
            perror("kill");
            exit(EXIT_FAILURE);
        }
    }
}

// Penanganan sinyal SIGINT (Ctrl+C)
void sigint_handler(int signum) {
    // Menutup semua proses anak sebelum keluar dari program
    cleanup_child_processes();
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    // Menetapkan penanganan sinyal SIGINT
    signal(SIGINT, sigint_handler);

    // Memeriksa argumen command line
    if (argc == 1) {
        printf("Usage: %s [-o <app1> <num1> <app2> <num2> ... <appN> <numN>] | [-f file.conf] | [-k] | [-k file.conf]\n", argv[0]);
        return 1;
    }

    // Jika argumen adalah -o
    if (strcmp(argv[1], "-o") == 0) {
        if (argc < 4 || (argc - 2) % 2 != 0) {
            printf("Invalid arguments\n");
            return 1;
        }
        for (int i = 2; i < argc; i += 2) {
            launch_application(argv[i], atoi(argv[i + 1]));
        }
    }

    // Jika argumen adalah -f
    else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Invalid arguments\n");
            return 1;
        }
        launch_from_config(argv[2]);
    }

    // Jika argumen adalah -k
    else if (strcmp(argv[1], "-k") == 0) {
        if (argc == 2) {
            // Menutup semua proses
            cleanup_child_processes();
            return 0;
        } else if (argc == 3 && strcmp(argv[2], "-f") != 0) {
            // Menutup proses sesuai dengan file konfigurasi
            cleanup_child_processes();
            return 0;
        } else if (argc == 4 && strcmp(argv[2], "-f") == 0) {
            // Menutup proses sesuai dengan file konfigurasi
            launch_from_config(argv[3]);
            return 0;
        } else {
            printf("Invalid arguments\n");
            return 1;
        }
    }

    // Argumen tidak valid
    else {
        printf("Invalid arguments\n");
        return 1;
    }

    // Menunggu semua proses anak selesai
    for (int i = 0; i < num_child_pids; i++) {
        // Menunggu proses anak dan menangani error jika ada
        if (wait(NULL) == -1 && errno != ECHILD) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
