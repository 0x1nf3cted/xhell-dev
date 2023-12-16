#include "common.h"

static char **split_string(char *cmd, int *nb_token);

int parse_command(Cmd *cmd, dir_info *d)
{

    int nb_token = 0;
    char **tokens = split_string(cmd->cont, &nb_token);

    if (nb_token > 0)
    {
        if (strcmp(tokens[0], "exit") == 0)
        {
            exit(0); // for now
        }
        else if (strcmp(tokens[0], "cd") == 0)
        {
            cd(tokens, d, nb_token);
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
        free(tokens);
    }

    return 0;
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

    tokens[*count] = NULL; // Null-terminate the array
    return tokens;
}
void get_command_history(Cmd *cmd, dir_info *d)
{

    int index = d->index;
    FILE *file = read_or_write("a+");

    char **content = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_count = 0;

    while ((read = getline(&line, &len, file)) != -1)
    {
        // Remove newline character if present
        if (line[read - 1] == '\n')
        {
            line[read - 1] = '\0';
        }

        // Allocate memory for the new line and add it to the content array
        char **temp_content = realloc(content, sizeof(char *) * (line_count + 1));
        if (temp_content == NULL)
        {
            // Handle allocation error, e.g., free allocated memory and exit
            free_content(content, line_count);
            free(line);
            fclose(file);
            perror("Failed to reallocate memory for content");
            exit(EXIT_FAILURE);
        }
        content = temp_content;
        content[line_count] = strdup(line); // strdup allocates memory and copies the string
        (line_count)++;
    }

    free(line); // Free the buffer allocated by getline
    fclose(file);

    write(STDOUT_FILENO, "\x1b[2K", 4);
    write(STDOUT_FILENO, "\r", 1);
    if (d->cur_dir)
    {
        printf("%s > ", d->cur_dir);
    }
    int access_index = line_count - index;
    if (access_index < 0 || access_index >= line_count)
    {
        printf("Invalid index.\n");
    }
    else
    {
        printf("%s", content[access_index]);
        store_command(cmd, content[access_index]);
    }
    free_content(content, line_count);
}

void free_content(char **content, int line_count)
{
    for (int i = 0; i < line_count; i++)
    {
        free(content[i]);
    }
    free(content);
}

int store_command(Cmd *cmd, char *n_cmd)
{
    free(cmd->cont);
    cmd->cont = (char *)malloc(256 * sizeof(char));
    if (cmd->cont == NULL)
    {
        perror("Failed to allocate memory for cmd->cont");
        exit(EXIT_FAILURE);
    }
    cmd->c_pos = 0;
    cmd->length = 0;

    strcpy(cmd->cont, n_cmd);

    return 0;
}

int get_file_len()
{
    FILE *file = read_or_write("r");
    int ch, n_lines = 0;
    do
    {
        ch = fgetc(file);
        if (ch == '\n')
        {
            n_lines++;
        }
    } while (ch != EOF);
    if (ch != '\n' && n_lines != 0)
    {
        n_lines++;
    }
    fclose(file);
    return n_lines;
}

FILE *read_or_write(char *flag)
{
    char *home = getenv("HOME");
    if (home == NULL)
    {
        fprintf(stderr, "La variable d'environnement HOME n'est pas définie.\n");
        exit(1);
    }

    char filename[256];
    snprintf(filename, sizeof(filename), "%s/.config/xhell/xhell_history.txt", home);

    FILE *file = fopen(filename, flag);
    if (file == NULL)
    {
        perror("Impossible d'ouvrir ou de créer le fichier");
        exit(1);
    }
    return file;
}

// Cmd *store_command(char *cmd, size_t input_size, int *nb_token)
// {

//     char **tokens = NULL;
//     char buffer[MAX_BUFFER];
//     int buffer_index = 0;
//     int index = 0;
//     *nb_token = 0;

//     while (cmd[index] != '\0')
//     {
//         char currentChar = cmd[index];

//         if (isspace(currentChar))
//         {
//             index++;
//             continue;
//         }
//         if (IS_PIPE_OPRATOR(currentChar))
//         {
//             buffer[buffer_index] = currentChar;
//             buffer[buffer_index + 1] = '\0';
//             tokens = realloc(tokens, (*nb_token + 1) * sizeof(char *));
//             tokens[*nb_token] = strdup(buffer);
//             buffer_index = 0;
//             buffer[buffer_index] = '\0';
//             (*nb_token)++;
//             index++;
//             continue;
//         }
//         if (isalpha(currentChar) || currentChar == '.' || currentChar == '/' || currentChar == '-' || currentChar == '_' || currentChar == '~')
//         {
//             while (isalnum(cmd[index]) || cmd[index] == '_' || cmd[index] == '.' || cmd[index] == '-' || cmd[index] == '_' || cmd[index] == '/' || cmd[index] == '~')
//             {
//                 buffer[buffer_index++] = cmd[index++];
//             }
//             buffer[buffer_index] = '\0';

//             tokens = realloc(tokens, (*nb_token + 1) * sizeof(char *));
//             tokens[*nb_token] = strdup(buffer);
//             buffer_index = 0;
//             buffer[buffer_index] = '\0';
//             (*nb_token)++;
//             continue;
//         }

//         index++;
//     }

//     Cmd *command = malloc(sizeof(Cmd));
//     command->content = malloc((*nb_token) * sizeof(char *));
//     for (int i = 0; i < (*nb_token); i++)
//     {
//         command->content[i] = strdup(tokens[i]);
//     }

//     free(tokens);

//     char *home = getenv("HOME");
//     if (home == NULL)
//     {
//         fprintf(stderr, "La variable d'environnement HOME n'est pas définie.\n");
//         exit(1);
//     }

//     char filename[256];
//     snprintf(filename, sizeof(filename), "%s/.config/xhell/xhell_history.txt", home);

//     FILE *file = fopen(filename, "a+");
//     if (file == NULL)
//     {
//         perror("Impossible d'ouvrir ou de créer le fichier");
//         exit(1);
//     }

//     for (int i = 0; i < *nb_token; i++)
//     {
//         fprintf(file, "%s ", command->content[i]);
//     }
//     fprintf(file, "\n");

//     fclose(file);

//     return command;
// }

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
