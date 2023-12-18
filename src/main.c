#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <curses.h>
#include <termios.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include "common.h"
#include "completions.h"

typedef struct
{
    Cmd *cmd;
    int nb_token;
    dir_info *d;
} ThreadArgs;

// void *thread_func(void *arg)
// {
//     ThreadArgs *args = (ThreadArgs *)arg;
//     int res = parse_command(args->cmd, args->nb_token, args->d);
//     // Do something with res if needed
//     free(args); // Don't forget to free the allocated memory
//     return NULL;
// }

void enableRawMode(struct termios *origTermios)
{
    struct termios raw;
    setvbuf(stdout, NULL, _IONBF, 0); // Disable output buffering

    tcgetattr(STDIN_FILENO, origTermios);
    raw = *origTermios;

    raw.c_lflag &= ~(ECHO | ECHOE | ICANON);
    // tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disableRawMode(struct termios *origTermios)
{
    tcsetattr(STDIN_FILENO, TCSANOW, origTermios);
}

int main()
{
    setup_env(); // Set up config files
    struct termios origTermios;
    enableRawMode(&origTermios);

    int history_index = 0;
    int nb_token = 0;
    size_t path_size = pathconf(".", _PC_PATH_MAX);

    dir_info *d = malloc(sizeof(dir_info));
    d->index = 0;
    if (!d || !(d->cur_dir = (char *)malloc(path_size)))
    {
        perror("malloc failed");
        free(d);
        exit(EXIT_FAILURE);
    }

    int w_command = 1;
    char input[256]; // Input buffer
    Cmd *cmd = (Cmd *)malloc(sizeof(Cmd));
    cmd->c_pos = 0;
    cmd->length = 0;
    cmd->cont = (char *)malloc(256 * sizeof(char));
    if (cmd->cont == NULL)
    {
        perror("Failed to allocate memory for cmd->cont");
        exit(EXIT_FAILURE);
    }

    while (1)
    {

        strcpy(cmd->cont, "");

        if (getcwd(d->cur_dir, path_size) == NULL)
        {
            perror("Unable to get current working directory");
            free(d->cur_dir);
            free(d);
            exit(EXIT_FAILURE);
        }
        printf("%s > ", d->cur_dir); // Print prompt

        w_command = 1;
        char c;

        while (w_command)
        {
            if (read(STDIN_FILENO, &c, 1) == 1)
            {
                if (c == CTRL_X)
                {
                    break;
                }
                else if (c == 27)
                {
                    if (read(STDIN_FILENO, &c, 1) == 1 && c == 91)
                    {
                        if (read(STDIN_FILENO, &c, 1) == 1)
                        {
                            if (c == 'A')
                            {
                                insert_key_press(57, cmd, d);
                            }
                            else if (c == 'B')
                            {
                                insert_key_press(80, cmd, d);
                            }
                            else if (c == 'C')
                            {
                                insert_key_press(77, cmd, d);
                            }
                            else if (c == 'D')
                            {
                                insert_key_press(75, cmd, d);
                            }
                        }

                        else
                        {
                            lseek(STDIN_FILENO, -1, SEEK_CUR);
                        }
                    }
                }
                else if (c == ENTER)
                {

                    w_command = 0;
                    printf("\n");
                    d->index = 0;
                    break;
                }

                else
                {
                    insertChar(c, cmd, d);
                }
            }
        }

        parse_command(cmd, d);

        // memset(cmd->cont, '\0', cmd->length);
        free(cmd->cont);
        cmd->cont = (char *)malloc(sizeof(char));
        if (cmd->cont == NULL)
        {
            perror("Failed to allocate memory for cmd->cont");
            exit(EXIT_FAILURE);
        }
        cmd->c_pos = 0;
        cmd->length = 0;
    }

    // cmd = store_command(input, strlen(input), &nb_token);

    // pthread_t thread_id;
    // ThreadArgs *args = malloc(sizeof(ThreadArgs));
    // if (!args)
    // {
    //     perror("Failed to allocate memory for thread arguments");
    //     // Handle error...
    //     break;
    // }

    // // Set up arguments for the thread
    // args->cmd = cmd;
    // args->nb_token = nb_token;
    // args->d = d;

    // // Create the thread
    // if (pthread_create(&thread_id, NULL, thread_func, args) != 0)
    // {
    //     perror("Failed to create thread");
    //     // Handle error...
    //     break;
    // }

    // // Wait for the thread to complete
    // pthread_join(thread_id, NULL);

    // // Free the command structure
    // for (int i = 0; i < nb_token; i++)
    // {
    //     free(cmd->content[i]);
    // }
    // free(cmd->content);
    // free(cmd);

    disableRawMode(&origTermios);

    // Clean-up
    free(d->cur_dir);
    free(d);

    return 0;
}