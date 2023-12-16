#include "common.h"

static char **split_string(char *cmd, int *nb_token);

int parse_command(Cmd *cmd, dir_info *d)
{

    int nb_token = 0;
    char **tokens = split_string(cmd->cont, &nb_token);


/* the bug is here
it execute the command, then cause segmentation fault
*/
    if (strcmp(tokens[0], "exit") == 0)
    {
        exit(0); // for now
    }
    else if (strcmp(tokens[0], "cd") == 0)
    {
        cd(tokens, d);
        return 0;
    }
    else if (strcmp(tokens[0], "clear") == 0)
    {
        clear();
        return 0;
    }
    else
    {
        char *argv;
        fetch_bin(tokens[0], argv);
    }
}
static char **split_string(char *str, int *count)
{
    int capacity = 10; // Initial capacity for the array of strings
    char **tokens = malloc(capacity * sizeof(char *));
    if (!tokens)
    {
        perror("Memory allocation failed");
        exit(1);
    }

    *count = 0;
    char *token = strtok(str, " ");
    while (token != NULL)
    {
        if (*count >= capacity)
        {
            // Resize the tokens array if necessary
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(char *));
            if (!tokens)
            {
                perror("Memory allocation failed");
                exit(1);
            }
        }

        tokens[*count] = token;
        (*count)++;
        token = strtok(NULL, " ");
    }

    return tokens;
}
// char *get_command_history(int index)
// {

//     const char *home_directory = getenv("HOME");
//     char resolved_file[MAXPATH];

//     if (home_directory == NULL)
//     {
//         fprintf(stderr, "Error: HOME environment variable not set.\n");
//         return;
//     }

//     snprintf(resolved_file, sizeof(resolved_file), "%s/.config/xhell/xhell_history.txt", home_directory);

//     FILE *file = fopen(resolved_file, "r");
//     if (!file)
//     {
//         perror("Error opening file");
//         return NULL;
//     }

//     char **content = NULL;
//     char *line = NULL;
//     size_t len = 0;
//     ssize_t read;
//     int line_count = 0;

//     while ((read = getline(&line, &len, file)) != -1)
//     {
//         // Remove newline character if present
//         if (line[read - 1] == '\n')
//         {
//             line[read - 1] = '\0';
//         }

//         // Allocate memory for the new line and add it to the content array
//         content = realloc(content, sizeof(char *) * (line_count + 1));
//         content[line_count] = strdup(line); // strdup allocates memory and copies the string
//         (line_count)++;
//     }

//     free(line); // Free the buffer allocated by getline
//     fclose(file);

//     return content[line_count - index];
// }

// void free_content(char **content, int line_count)
// {
//     for (int i = 0; i < line_count; i++)
//     {
//         free(content[i]);
//     }
//     free(content);
// }

// // Cmd *store_command(char *cmd, size_t input_size, int *nb_token)
// // {

// //     char **tokens = NULL;
// //     char buffer[MAX_BUFFER];
// //     int buffer_index = 0;
// //     int index = 0;
// //     *nb_token = 0;

// //     while (cmd[index] != '\0')
// //     {
// //         char currentChar = cmd[index];

// //         if (isspace(currentChar))
// //         {
// //             index++;
// //             continue;
// //         }
// //         if (IS_PIPE_OPRATOR(currentChar))
// //         {
// //             buffer[buffer_index] = currentChar;
// //             buffer[buffer_index + 1] = '\0';
// //             tokens = realloc(tokens, (*nb_token + 1) * sizeof(char *));
// //             tokens[*nb_token] = strdup(buffer);
// //             buffer_index = 0;
// //             buffer[buffer_index] = '\0';
// //             (*nb_token)++;
// //             index++;
// //             continue;
// //         }
// //         if (isalpha(currentChar) || currentChar == '.' || currentChar == '/' || currentChar == '-' || currentChar == '_' || currentChar == '~')
// //         {
// //             while (isalnum(cmd[index]) || cmd[index] == '_' || cmd[index] == '.' || cmd[index] == '-' || cmd[index] == '_' || cmd[index] == '/' || cmd[index] == '~')
// //             {
// //                 buffer[buffer_index++] = cmd[index++];
// //             }
// //             buffer[buffer_index] = '\0';

// //             tokens = realloc(tokens, (*nb_token + 1) * sizeof(char *));
// //             tokens[*nb_token] = strdup(buffer);
// //             buffer_index = 0;
// //             buffer[buffer_index] = '\0';
// //             (*nb_token)++;
// //             continue;
// //         }

// //         index++;
// //     }

// //     Cmd *command = malloc(sizeof(Cmd));
// //     command->content = malloc((*nb_token) * sizeof(char *));
// //     for (int i = 0; i < (*nb_token); i++)
// //     {
// //         command->content[i] = strdup(tokens[i]);
// //     }

// //     time_t t = time(NULL);
// //     struct tm tm = *localtime(&t);
// //     command->time = tm;

// //     free(tokens);

// //     char *home = getenv("HOME");
// //     if (home == NULL)
// //     {
// //         fprintf(stderr, "La variable d'environnement HOME n'est pas définie.\n");
// //         exit(1);
// //     }

// //     char filename[256];
// //     snprintf(filename, sizeof(filename), "%s/.config/xhell/xhell_history.txt", home);

// //     FILE *file = fopen(filename, "a+");
// //     if (file == NULL)
// //     {
// //         perror("Impossible d'ouvrir ou de créer le fichier");
// //         exit(1);
// //     }

// //     for (int i = 0; i < *nb_token; i++)
// //     {
// //         fprintf(file, "%s ", command->content[i]);
// //     }
// //     fprintf(file, "\n");

// //     fclose(file);

// //     return command;
// // }

void setup_env()
{
    const char *directory_path = "~/.config/xhell/";
    const char *file_path = "~/.config/xhell/xhell_history.txt";

    const char *home_directory = getenv("HOME");
    char resolved_directory[MAXPATH];
    char resolved_file[MAXPATH];

    if (home_directory == NULL)
    {
        fprintf(stderr, "Error: HOME environment variable not set.\n");
        return;
    }

    snprintf(resolved_directory, sizeof(resolved_directory), "%s/.config/xhell/", home_directory);
    snprintf(resolved_file, sizeof(resolved_file), "%s/.config/xhell/xhell_history.txt", home_directory);

    struct stat st;

    if (stat(resolved_directory, &st) == 0)
    {
        if (!S_ISDIR(st.st_mode))
        {
            fprintf(stderr, "Error: A file with the same name exists. Please remove or rename it.\n");
            return;
        }
    }
    else
    {
        if (mkdir(resolved_directory, 0700) != 0)
        {
            fprintf(stderr, "Error: Failed to create directory.\n");
            return;
        }
    }

    if (stat(resolved_file, &st) == 0)
    {
        if (!S_ISREG(st.st_mode))
        {
            fprintf(stderr, "Error: A directory with the same name exists. Please remove or rename it.\n");
            return;
        }
    }
    else
    {
        int fd = open(resolved_file, O_CREAT | O_WRONLY, 0600);
        if (fd == -1)
        {
            fprintf(stderr, "Error: Failed to create file.\n");
            return;
        }
        close(fd);
    }

    printf("Setup completed successfully.\n");
}
