#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void display_prompt(void) {
    printf(":) ");
}

void execute_command(char *args[]) {
    pid_t pid;

    /* Check if the command exists in PATH */
    char *path = getenv("PATH");
    char *path_copy = strdup(path);
    char *dir = strtok(path_copy, ":");

    while (dir != NULL) {
        /* Construct the full path to the executable */
        char *full_path = malloc(strlen(dir) + strlen(args[0]) + 2);
        if (full_path == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        sprintf(full_path, "%s/%s", dir, args[0]);

        if (access(full_path, X_OK) == 0) {
            /* Execute the command */
            pid = fork();

            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {
                /* Child process */
                if (execvp(full_path, args) == -1) {
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

            free(full_path);
            free(path_copy);
            return;
        }

        free(full_path);
        dir = strtok(NULL, ":");
    }

    free(path_copy);

    /* If the loop completes, the command was not found */
    fprintf(stderr, "./shell: %s: command not found\n", args[0]);
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

            if (args[0] != NULL) {
                execute_command(args);
            }
        }
    }

    return 0;
}
