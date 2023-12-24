#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <dirent.h>
#include <fcntl.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>



#define MAX_BUFFER 256
#define MAXPATH 1024
#define clear() printf("\033[H\033[J")

#define CHECK_MEMORY(ptr)                                                               \
    do                                                                                  \
    {                                                                                   \
        if (!(ptr))                                                                     \
        {                                                                               \
            fprintf(stderr, "Memory allocation failed at %s:%d\n", __FILE__, __LINE__); \
            exit(EXIT_FAILURE);                                                         \
        }                                                                               \
    } while (0)

#define IS_PIPE_OPRATOR(CH) ( \
    (CH) == '|' || (CH) == '<' || (CH) == '>')

enum Keypress
{
    ENTER = 10,
    ESCAPE = 27,
    BACKSPACE = 127,
    TAB = 9,
    CTRL_C = 3,
    CTRL_S = 19,
    CTRL_V = 22,
    CTRL_X = 24,
    UP_ARROW = 57,
    DOWN_ARROW = 80,
    RIGHT_ARROW = 77,
    LEFT_ARROW = 75
};

typedef struct
{
    char *cur_dir;
    int index;
} dir_info;

typedef struct
{
    char *cont; // command content
    int c_pos;  // cursor position
    int length; // longueur de la chaîne

} Cmd;

enum NodeType
{
    COMMAND_NODE,
    PATH_NODE,
    ARG_NODE,
    SYMBOL_NODE,
    VARIABLE_TYPE,
    PIPE_NODE,
    REDIRECT_OUTPUT_NODE,
    REDIRECT_INPUT_NODE,
    APPEND_OUTPUT_NODE,
    APPEND_INPUT_NODE,
    TEE_NODE,
    APPEND_ERROR_NODE,
    REDIRECT_IO_NODE,
    REDIRECT_ERROR_NODE,
    APPEND_IO_ERROR_NODE,
};

union node_val_type
{
    long sint;
    unsigned long uint;
    long long sllong;
    unsigned long long ullong;
    double sfloat;
    long double ldouble;
    char chr;
    char *str;
};

struct Node
{
    char *node_content;
    union node_val_type v_node_type;
    enum NodeType node_type;
    int pos;
    struct Node *next;
    struct Node *before;
};
typedef struct Node Node;


void insertChar(char addedChar, Cmd *cmd, dir_info *dir);
void addCharToBuffer(const char addedChar, Cmd *cmd, dir_info *dir);
void insert_key_press(int key_press, Cmd *cmd, dir_info *d);
void move_cursor(int x);
void setup_env();
int parse_command(Cmd *cmd, dir_info *d);
void cd(char **content, dir_info *cur_dir, int nb_tokens);
FILE *read_or_write(char *flag);
int store_command(Cmd *cmd, char *n_cmd);
void get_command_history(Cmd *cmd, dir_info *d);
Node *create_node(const char *content, enum NodeType type);
enum NodeType determine_node_type(const char *token);
Node *parse_command_node(Cmd *cmd, int *nb_token);
void print_ast(Node *current);



#endif /* COMMON_H */