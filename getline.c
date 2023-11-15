#include "custom_getline.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_INPUT_SIZE 1024

char *custom_getline(void) {
    static char buffer[MAX_INPUT_SIZE];
    static int buffer_index = 0;
    static ssize_t bytes_read = 0;

    // If the buffer is empty or if all characters are read, refill the buffer
    if (buffer_index >= bytes_read || bytes_read == 0) {
        bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));

        if (bytes_read == -1 || bytes_read == 0) {
            // Handle read error or end-of-file condition
            return NULL;
        }

        buffer_index = 0;
    }

    // Read one character from the buffer
    char current_char = buffer[buffer_index++];
    
    // If the character is a newline, replace it with null terminator
    if (current_char == '\n') {
        buffer[buffer_index - 1] = '\0';
    }

    // Allocate memory for the line and copy the characters
    char *line = strdup(buffer);

    if (line == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    return line;
}
