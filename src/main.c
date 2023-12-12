#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>
#include "common.h"

#define MAX_BUFFER 256
#define MAXPATH 1024

typedef struct
{
    char *cur_dir;
} dir_info;

typedef struct
{
    char **content;
    struct tm time;
    int fail;
    u_int64_t pid;
} Cmd;

void fetch_bin()
{
    char sPath[MAXPATH] = "";
    char *pTmp;
    char **path_list = NULL;
    int n_path = 0;
    if ((pTmp = getenv("PATH")) != NULL)
    {
        strncpy(sPath, pTmp, MAXPATH - 1);
    }
    char *token;
    /* get the first token */
    token = strtok(pTmp, ":");
    while (token != NULL)
    {
        path_list = realloc(path_list, (n_path + 1) * sizeof(char *));
        path_list[n_path] = strdup(token);
        printf(" %s\n", token);
        token = strtok(NULL, ":");
    }
    for (int i = 0; i < n_path; i++)
    {
        printf("%s", path_list[i]);
    }
    free(path_list);
}

// char *parse_command(Cmd *cmd, int nb_tokens)
// {
//     for (int i = 0; i < nb_tokens; i++)
//     {
//     }
// }

Cmd *store_command(char *cmd, size_t input_size, int *nb_token)
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
    *nb_token = 0; // Initialize nb_token to zero*

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

    dir_info *d = malloc(sizeof(dir_info));
    char buffer[MAX_BUFFER];
    if (getcwd(buffer, MAX_BUFFER) == NULL)
    {
        fprintf(stderr, "Cannot get current working directory path\n");
        if (errno == ERANGE)
        {
            fprintf(stderr, "Buffer size is too small.\n");
        }
        exit(EXIT_FAILURE);
    }
    d->cur_dir = malloc(sizeof(char));
    if (d->cur_dir == NULL)
    {
        fprintf(stderr, "Memory allocation error.\n");
    }
    d->cur_dir = strdup(buffer);

    while (1)
    {
        fprintf(stdout, "\n%s ", d->cur_dir);
        fprintf(stdout, " > ");

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
        Cmd *cmd = store_command(input, input_size, &nb_token);
        for (int i = 0; i < nb_token; i++)
        {
            printf("%s", cmd->content[i]);
        }
        printf("%d-%02d-%02d %02d:%02d:%02d", cmd->time.tm_year + 1900, cmd->time.tm_mon + 1, cmd->time.tm_mday, cmd->time.tm_hour, cmd->time.tm_min, cmd->time.tm_sec);

        for (int i = 0; i < nb_token; i++)
        {
            free((void *)cmd->content[i]);
        }
    }
    fetch_bin();
    free(input);

    return 0;
}
