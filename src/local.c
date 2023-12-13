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
     char modifiedPath[strlen(path) + 1];
    strcpy(modifiedPath, path);

     char* prevOccurrence = NULL;
    char* occurrence = strstr(modifiedPath, "../");
    while (occurrence != NULL) {
         if (occurrence == modifiedPath || *(occurrence - 1) == '/') {
             memmove(occurrence, occurrence + 3, strlen(occurrence + 3) + 1);

             prevOccurrence = occurrence;
            occurrence = strstr(modifiedPath, "../");
        } else {
             occurrence = strstr(occurrence + 1, "../");
        }
    }

     return strdup(modifiedPath);
}
 
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

        //TODO: check for cases (., ./, /..)
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
                 snprintf(prefix, sizeof(prefix), "%s/%s", cur_dir->cur_dir, path);
                if (strstr(path, "..") == NULL)
                {
                     if (chdir(prefix) != 0)
                    {
                        perror("Error: chdir failed");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    char *new_p = fixPath(prefix);
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
