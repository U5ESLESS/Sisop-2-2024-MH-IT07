#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("User: %s <user>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *user = argv[1];

    // COMMAND
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        execlp("ps", "ps", "-u", user, NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }

    return EXIT_SUCCESS;
}
