#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <fcntl.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

#define MAX_BUFFER 256
#define MAXPATH 1024
#define clear() printf("\033[H\033[J")


#define IS_PIPE_OPRATOR(CH) ( \
    (CH) == '|' || (CH) == '<' || (CH) == '>')

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


void setup_env();
int parse_command(Cmd *cmd, int nb_tokens, dir_info *d);
Cmd *store_command(char *cmd, size_t input_size, int *nb_token);
char *get_command_history(int index);

#endif /* COMMON_H */