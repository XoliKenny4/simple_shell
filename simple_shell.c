#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void display_prompt(void) {
    printf("#cisfun$ ");
}

void execute_command(char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* Child process */
        if (execvp(args[0], args) == -1) {
            perror(args[0]);  /* Use the program name for the error message */
            exit(EXIT_FAILURE);
        }
    } else {
        /* Parent process */
        int status;
        waitpid(pid, &status, 0);

        if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT) {
            /* Handle Ctrl+C */
            printf("\n");
        }
    }
}

int main(void) {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    char *token;
    int i;

    while (1) {
        display_prompt();

        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            printf("\n");
            break; /* Handle end-of-file condition (Ctrl+D) */
        }

        /* Remove newline character */
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            break;
        } else {
            /* Tokenize input into arguments */
            token = strtok(input, " ");

            for (i = 0; token != NULL && i < MAX_ARGS - 1; i++) {
                args[i] = token;
                token = strtok(NULL, " ");
            }

            args[i] = NULL; /* Set the last element to NULL */

            execute_command(args);
        }
    }

    return 0;
}
