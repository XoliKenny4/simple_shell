/* custom_getline.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  /* Include this header for read and STDIN_FILENO */

#define BUFFER_SIZE 1024

char *custom_getline(void) {
    static char buffer[BUFFER_SIZE];
    static size_t buffer_index = 0;

    char current_char;
    size_t line_index;
    
    char *line = NULL;  /* Declare 'line' separately */

    if (buffer_index == 0 || buffer[buffer_index - 1] == '\0') {
        ssize_t bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);

        if (bytes_read <= 0) {
            return NULL;
        }

        buffer[bytes_read] = '\0';
        buffer_index = 0;
    }

    current_char = buffer[buffer_index++];
    line_index = 0;

    while (current_char != '\n' && current_char != '\0') {
        char *temp = realloc(line, line_index + 1);
        if (temp == NULL) {
            perror("realloc");
            free(line);
            return NULL;
        }
        line = temp;
        line[line_index++] = current_char;
        current_char = buffer[buffer_index++];
    }

    if (line != NULL) {
        line[line_index] = '\0';
    }

    return line;
}
