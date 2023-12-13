#include "local.h"

void remove_last(char *path)
{
    int bound = 0;

    for (int i = strlen(path) - 1; i >= 0; i--)
    {
        if (path[i] == '/')
        {
            bound = i;
            break;
        }
    }

    if (bound == 0)
    {
        strcpy(path, "/");
        return;
    }

    path[bound] = '\0';
}


char* fixPath(const char* path) {
    // Copier la chaîne d'entrée pour la modification
    char modifiedPath[strlen(path) + 1];
    strcpy(modifiedPath, path);

    // Trouver et supprimer les occurrences de "../"
    char* prevOccurrence = NULL;
    char* occurrence = strstr(modifiedPath, "../");
    while (occurrence != NULL) {
        // S'assurer que l'occurrence se trouve au début de la chaîne
        if (occurrence == modifiedPath || *(occurrence - 1) == '/') {
            // Supprimer l'occurrence de "../"
            memmove(occurrence, occurrence + 3, strlen(occurrence + 3) + 1);

            // Rechercher la prochaine occurrence
            prevOccurrence = occurrence;
            occurrence = strstr(modifiedPath, "../");
        } else {
            // Si l'occurrence n'est pas au début, passer à la suite
            occurrence = strstr(occurrence + 1, "../");
        }
    }

    // Vous pouvez maintenant utiliser modifiedPath comme chemin corrigé
    return strdup(modifiedPath);
}

// static char *concat_strings(char **array, int size)
// {
//     int total_length = 0;
//     for (int i = 0; i < size; i++)
//     {
//         total_length += strlen(array[i]);
//     }

//     char *result = (char *)malloc(total_length + 1);
//     if (result == NULL)
//     {
//         perror("Memory allocation failed");
//         exit(EXIT_FAILURE);
//     }

//     result[0] = '\0';
//     for (int i = 0; i < size; i++)
//     {
//         if (strcmp(array[i], "..") != 0)
//         {
//             strcat(result, array[i]);
//         }
//     }
//     return result;
// }

// char *remove_prefix(char *path)
// {
//     char *token;
//     char **p_list = NULL;
//     int n = 0;
//     token = strtok(path, "/");
//     while (token != NULL)
//     {
//         p_list = realloc(p_list, (n + 1) * sizeof(char *));
//         p_list[n] = strdup(token);
//         token = strtok(NULL, "/");
//     }

//     concat_strings(p_list, n + 1);
// }

void cd(char **content, dir_info *cur_dir, int nb_tokens)
{
    char prefix[MAXPATH];
    char *path = NULL;

    if (nb_tokens == 1)
    {
        chdir("/home/moussacodes");
    }
    else
    {
        path = strdup(content[1]);

        if (path == NULL)
        {
            perror("Error: memory allocation failed");
            exit(EXIT_FAILURE);
        }

        printf("\npath: %s\n", path);
        int index = 0;
        while (path[index] != '\0')
        {
            if (path[index] == '.')
            {
                if (path[index + 1] == '.')
                {
                    if (path[index + 2] == '/' || path[index + 2] == '\0')
                    {
                        remove_last(cur_dir->cur_dir);
                        chdir(cur_dir->cur_dir);

                        if (path[index + 2] == '/')
                        {
                            index += 3;
                        }
                        else
                        {
                            index += 2;
                        }
                    }
                    else
                    {
                        perror("Error: wrong input");
                        free(path);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else if (path[index] == '/')
            {
                if (strlen(path) == 1)
                {
                    chdir("/");
                    index++;
                }
                else if (path[index + 1] == '.')
                {
                    index += 2;
                }
                else
                {
                    chdir(path);
                    index++;
                }
            }
            else
            {
                printf("%s", path);
                snprintf(prefix, sizeof(prefix), "%s/%s", cur_dir->cur_dir, path);
                if (strstr(path, "..") == NULL)
                {
                    printf("new no: :  %s", path);
                    if (chdir(prefix) != 0)
                    {
                        perror("Error: chdir failed");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    char *new_p = fixPath(prefix);
                    printf("new with ::  %s", new_p);
                    if (chdir(new_p) != 0)
                    {
                        perror("Error: chdir failed");
                        free(new_p);
                        exit(EXIT_FAILURE);
                    }
                }

                while (path[index] != '\0' && path[index] != '/')
                {
                    index++;
                }
            }
        }

        free(path);
    }

    if (getcwd(cur_dir->cur_dir, MAX_BUFFER) == NULL)
    {
        perror("Error: Cannot get current working directory path");
        exit(EXIT_FAILURE);
    }
}
