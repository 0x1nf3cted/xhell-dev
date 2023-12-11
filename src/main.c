#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "common.h"

#define MAX_BUFFER 256

typedef struct
{
    char *content;
    struct tm time;
    int fail;
    u_int64_t pid;
} cmd;

cmd *parse_command(char *cmd, size_t input_size)
{

    //TODO: should implement a tokenizer to parse the input
    /*
    * first check if the command is a local one (exit, cd, ect...)
    * or else load it from /usr/bin or for PATH
    */
    char buffer[MAX_BUFFER];
    int bufferIndex = 0;
    int index = 0;
    while (cmd[index] != '\0')
    {
        char currentChar = cmd[index];

        if (isspace(currentChar))
        {
            index++;
            continue;
        }

        if (IS_PIPE_OPRATOR(currentChar))
        {
            buffer[bufferIndex] = currentChar;
            buffer[bufferIndex + 1] = '\0'; // Null-terminate the buffer
            bufferIndex = 0;
            index++;
            continue;
        }
        buffer[bufferIndex++] = currentChar;
        buffer[bufferIndex] = '\0';
        bufferIndex = 0;
        index++;
    }
}

int main()
{
    char *input = NULL;
    size_t input_size = 0;

    while (1)
    {
        fprintf(stdout, "> ");

        ssize_t read_bytes = getline(&input, &input_size, stdin);

        if (read_bytes == -1)
        {
            perror("getline");
            exit(EXIT_FAILURE);
        }

        if (read_bytes > 0 && input[read_bytes - 1] == '\n')
        {
            input[read_bytes - 1] = '\0';
            read_bytes--;
        }

        if (read_bytes == 0)
        {
            continue;
        }

        fprintf(stdout, "You entered: %s\n", input);
    }

    free(input);

    return 0;
}
