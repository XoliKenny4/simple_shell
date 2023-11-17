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
        if (execve(args[0], args, NULL) == -1) {
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

void print_environment(void) {
    extern char **environ;
    char **env = environ;

    while (*env != NULL) {
        printf("%s\n", *env);
        env++;
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

                int command_found = 0;

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
                        command_found = 1;
                        break;
                    }

                    free(full_path);
                    dir = strtok(NULL, ":");
                }

                free(path_copy);

                if (!command_found) {
                    /* Command not found */
                    fprintf(stderr, "%s: 1: %s: not found\n", args[0], args[0]);
                }
            }
        }
    }

    return 0;
}
