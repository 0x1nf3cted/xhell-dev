#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include "common.h"

#define MAX_BUFFER 256

typedef struct
{
    char **content;
    struct tm time;
    int fail;
    u_int64_t pid;
} Cmd;

Cmd *parse_command(char *cmd, size_t input_size, int *nb_token)
{
    // TODO: should implement a tokenizer to parse the input
    /*
     * first check if the command is a local one (exit, cd, ect...)
     * or else load it from /usr/bin or for PATH
     */
    char **tokens = NULL;
    char buffer[MAX_BUFFER];
    int buffer_index = 0;
    int index = 0;
    *nb_token = 0; // Initialize nb_token to zero

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
            buffer[buffer_index] = currentChar;
            buffer[buffer_index + 1] = '\0'; // Null-terminate the buffer
            tokens = realloc(tokens, (*nb_token + 1) * sizeof(char *));
            tokens[*nb_token] = strdup(buffer);
            buffer_index = 0;
            buffer[buffer_index] = '\0';
            (*nb_token)++;
            index++;
            continue;
        }
        if (isalpha(currentChar))
        {
            while (isalnum(cmd[index]) || cmd[index] == '_' || isalpha(cmd[index]))
            {
                buffer[buffer_index++] = cmd[index++];
            }
            buffer[buffer_index] = '\0';

            tokens = realloc(tokens, (*nb_token + 1) * sizeof(char *));
            tokens[*nb_token] = strdup(buffer);
            buffer_index = 0;
            buffer[buffer_index] = '\0';
            (*nb_token)++;
            continue;
        }

        index++;
    }

    Cmd *command = malloc(sizeof(Cmd));
    command->content = malloc((*nb_token) * sizeof(char *));
    for (int i = 0; i < (*nb_token); i++)
    {
        command->content[i] = strdup(tokens[i]);
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    command->time = tm;

    free(tokens); // Ne pas oublier de libérer la mémoire allouée pour tokens
    return command;
}

int main()
{
    char *input = NULL;
    size_t input_size = 0;
    int nb_token = 0;

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
        Cmd *cmd = parse_command(input, input_size, &nb_token);
        for (int i = 0; i < nb_token; i++)
        {
            printf("%s", cmd->content[i]);
        }
        for (int i = 0; i < nb_token; i++)
        {
            free((void *)cmd->content[i]);
        }
    }

    free(input);

    return 0;
}
