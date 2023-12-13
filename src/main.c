#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include "common.h"
#include "local.h"

int fetch_bin(char *bin)
{
    char sPath[MAXPATH] = "";
    char binary_p[MAXPATH] = "";
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
        n_path++;
        token = strtok(NULL, ":");
    }
    DIR *d;
    struct dirent *dir;

    for (int i = 0; i < n_path; i++)
    {
        d = opendir(path_list[i]);
        if (d == NULL)
            return;

        // while ((dir = readdir(d)) != NULL)
        // {
        //     if (strcmp(dir->d_name, bin) == 0)
        //     {
        //         snprintf(binary_p, sizeof(binary_p), "%s/%s", path_list[i], bin);
        //         pid_t p = fork();
        //         if (p < 0)
        //         {
        //             perror("fork fail");
        //             exit(1);
        //         }
        //         char *const argv[] = {binary_p, "-l", NULL};
        //         execvp(binary_p, argv);
        //         waitpid(p, NULL, 0);
        //         kill(p, SIGKILL);
        //         closedir(d);
        //         return 0;
        //     }
        // }
        // closedir(d);

        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, bin) == 0)
            {
                char binary_p[1000];
                snprintf(binary_p, sizeof(binary_p), "%s/%s", path_list[i], bin);
                pid_t p = fork();
                if (p < 0)
                {
                    perror("fork fail");
                    exit(1);
                }
                else if (p == 0)
                {
                    char *const argv[] = {binary_p, NULL};
                    execvp(binary_p, argv);
                    perror("execvp");
                    exit(1);
                }
                else
                {
                    // Parent process
                    int status;
                    waitpid(p, &status, 0);

                    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
                    {
                        // Child process exited successfully
                        printf("Child process exited successfully.\n");
                    }
                    else
                    {
                        // Child process encountered an error or was killed
                        printf("Child process encountered an error or was killed.\n");
                    }
                }
            }
        }
        closedir(d);
    }
}

int parse_command(Cmd *cmd, int nb_tokens, dir_info *d)
{
    if (strcmp(cmd->content[0], "exit") == 0)
    {
        exit(0); // for now
    }
    else if (strcmp(cmd->content[0], "cd") == 0)
    {
        cd(cmd->content, d, nb_tokens);
        return 0;
    }
    else if (strcmp(cmd->content[0], "clear") == 0)
    {
        clear();
        return 0;
    }
    else
    {
        fetch_bin(cmd->content[0]);
    }
}

Cmd *store_command(char *cmd, size_t input_size, int *nb_token)
{

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
        if (isalpha(currentChar) || currentChar == '.' || currentChar == '/' || currentChar == '-' || currentChar == '_')
        {
            while (isalnum(cmd[index]) || cmd[index] == '_' || cmd[index] == '.' || cmd[index] == '-' || cmd[index] == '_' || cmd[index] == '/')
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
        buffer[buffer_index++] = currentChar;
        buffer[buffer_index] = '\0';
        tokens = realloc(tokens, (*nb_token + 1) * sizeof(char *));
        tokens[*nb_token] = strdup(buffer);
        (*nb_token)++;
        buffer_index = 0;
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
    size_t path_size = pathconf(".", _PC_PATH_MAX);
    char buffer[path_size];

    d->cur_dir = (char *)malloc((size_t)path_size);
    if (d->cur_dir == NULL)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        if (getcwd(d->cur_dir, path_size) == NULL)
        {
            perror("Impossible d'obtenir le répertoire de travail actuel");
            exit(EXIT_FAILURE);
        }

        fprintf(stdout, "%s ", d->cur_dir);
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
        parse_command(cmd, nb_token, d);

        for (int i = 0; i < nb_token; i++)
        {
            free(cmd->content[i]);
        }
        free(cmd->content);
        free(cmd);
    }
    free(d->cur_dir);
    free(d);

    free(input);

    return 0;
}
