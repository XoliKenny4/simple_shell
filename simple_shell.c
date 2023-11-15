/* simple_shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "custom_getline.h"  /* Include the custom_getline header */

#define MAX_ARGS 64

void display_prompt(void) {
    printf("$ ");
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
            perror("shell");
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

void print_environment(void) {
    extern char **environ;
    char **env = environ;

    while (*env != NULL) {
        printf("%s\n", *env);
        env++;
    }
}

int main(void) {
    char *input;
    char *args[MAX_ARGS];
    char *token;
    int i;

    while (1) {
        display_prompt();

        input = custom_getline();  /* Use the custom_getline function */

        if (input == NULL) {
            printf("\n");
            break; /* Handle end-of-file condition (Ctrl+D) */
        }

        /* Remove newline character */
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            break;
        } else if (strcmp(input, "env") == 0) {
            print_environment();
        } else {
            /* Tokenize input into arguments */
            token = strtok(input, " ");

            for (i = 0; token != NULL && i < MAX_ARGS - 1; i++) {
                args[i] = token;
                token = strtok(NULL, " ");
            }

            args[i] = NULL; /* Set the last element to NULL */

            if (access(args[0], X_OK) == 0) {
                /* Execute the command directly if it is in the current directory */
                execute_command(args);
            } else {
                /* Check the PATH environment variable */
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
                        args[0] = full_path;
                        execute_command(args);
                        free(full_path);
                        break;
                    }

                    free(full_path);
                    dir = strtok(NULL, ":");
                }

                free(path_copy);

                if (dir == NULL) {
                    /* Command not found */
                    fprintf(stderr, "./shell: %s: command not found\n", args[0]);
                }
            }
        }

        free(input);
    }

    return 0;
}
