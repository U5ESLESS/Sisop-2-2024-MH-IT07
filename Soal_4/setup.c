#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#define FILENAME "run.txt"
#define Max_Line 100
#define Max_App 100

pid_t run[Max_App];
int num_run = 0;

void addProcess(pid_t pid) {
    if (num_run < Max_App) {
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

    for (int j = 0; j < Max_App; j++) {
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

int launchFromConfig(char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        printf("Error opening file %s\n", config_file);
        exit(1);
    }

    char line[Max_Line];
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

void loadRunningProcesses() {
    FILE *file = fopen(FILENAME, "r");
    if (file != NULL) {
        fscanf(file, "%d", &num_run);
        fclose(file);
    }
}

void launchApps(char *apps[], int num_apps[]) {
    pid_t pid;

    for (int i = 0; i < Max_App && apps[i] != NULL; i++) {
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

void readConfig(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        exit(1);
    }

    char line[Max_Line];
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
        printf("Invalid option\n");
        exit(1);
    }

    return 0;
}
