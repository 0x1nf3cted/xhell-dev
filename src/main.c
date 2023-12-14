#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include "common.h"
#include "local.h"
#include "exec.h"
#include "completions.h"

typedef struct
{
    Cmd *cmd;
    int nb_token;
    dir_info *d;
} ThreadArgs;

void *thread_func(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    int res = parse_command(args->cmd, args->nb_token, args->d);
    // Do something with res if needed
    free(args); // Don't forget to free the allocated memory
    return NULL;
}


int main() {
    setup_env(); // Set up config files

    int history_index = 0;
    int nb_token = 0;
    size_t path_size = pathconf(".", _PC_PATH_MAX);

    dir_info *d = malloc(sizeof(dir_info));
    if (!d || !(d->cur_dir = (char *)malloc(path_size))) {
        perror("malloc failed");
        free(d);
        exit(EXIT_FAILURE);
    }

    char input[256]; // Input buffer

    while (1) {
        if (getcwd(d->cur_dir, path_size) == NULL) {
            perror("Unable to get current working directory");
            free(d->cur_dir);
            free(d);
            exit(EXIT_FAILURE);
        }
        printf("%s > ", d->cur_dir); // Print prompt

        if (fgets(input, sizeof(input), stdin) == NULL) {
            // Handle input error or EOF
            break;
        }

        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0'; // Remove newline character
        }

        if (strlen(input) == 0) {
            continue;
        }

        Cmd *cmd = store_command(input, strlen(input), &nb_token);

        pthread_t thread_id;
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        if (!args) {
            perror("Failed to allocate memory for thread arguments");
            // Handle error...
            break;
        }

        // Set up arguments for the thread
        args->cmd = cmd;
        args->nb_token = nb_token;
        args->d = d;

        // Create the thread
        if (pthread_create(&thread_id, NULL, thread_func, args) != 0) {
            perror("Failed to create thread");
            // Handle error...
            break;
        }

        // Wait for the thread to complete
        pthread_join(thread_id, NULL);

        // Free the command structure
        for (int i = 0; i < nb_token; i++) {
            free(cmd->content[i]);
        }
        free(cmd->content);
        free(cmd);
    }

    // Clean-up
    free(d->cur_dir);
    free(d);

    return 0;
}