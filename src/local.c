#include "local.h"

void remove_last(char *path)
{
    int bound = 0;

    // Find the last '/' in the path
    for (int i = strlen(path) - 1; i >= 0; i--)
    {
        if (path[i] == '/')
        {
            bound = i;
            break; // Exit the loop when the last '/' is found
        }
    }

    // Handle the case where there is no '/' in the path
    if (bound == 0)
    {
        // Modify the path to be "/"
        strcpy(path, "/");
        return;
    }

    // Null-terminate the path at the last '/'
    path[bound] = '\0';
}

void remove_prefix(char *path)
{
    int bound = 0;

    // Find the last '/' in the path
    for (int i = strlen(path) - 1; i >= 0; i--)
    {
        if (path[i] == '/')
        {
            bound = i + 1;
            break; // Exit the loop when the last '/' is found
        }
    }

    if (bound == 0)
    {
        strcpy(path, "");
    }
    else
    {
        // Shift the characters in the path to remove the prefix
        for (int i = 0; i < strlen(path) - bound; i++)
        {
            path[i] = path[i + bound];
        }

        // Null-terminate the modified path
        path[strlen(path) - bound] = '\0';
    }
}

void cd(char **content, dir_info *cur_dir, int nb_tokens)
{
    char prefix[MAXPATH];
    char *path = NULL;

    if (nb_tokens == 1)
    {
        // Change to the default directory if no path is provided
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
        int index = 0; // Initialize index

        while (path[index] != '\0')
        {
            if (path[index] == '.')
            {
                if (path[index + 1] == '.')
                {
                    if (path[index + 2] == '/' || path[index + 2] == '\0')
                    {
                        remove_last(cur_dir->cur_dir); // Go back one folder
                        chdir(cur_dir->cur_dir);

                        if (path[index + 2] == '/')
                        {
                            index += 3; // Move past "../"
                        }
                        else
                        {
                            index += 2; // Move past ".."
                        }
                    }
                    else
                    {
                        perror("Error: wrong input");
                        free(path); // Free the allocated memory
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else if (path[index] == '/') // Use single quotes for character comparison
            {
                if (strlen(path) == 1)
                {
                    chdir("/");
                }
                else if (path[index + 1] == '.')
                {
                    index += 2;
                }
                else
                {
                    chdir(path);
                }
            }
            else
            {
                // Construct the new path by concatenating cur_dir->cur_dir, "/", and path
                snprintf(prefix, sizeof(prefix), "%s/%s", cur_dir->cur_dir, path);

                // Use chdir with the new_path
                if (chdir(prefix) != 0)
                {
                    perror("Error: chdir failed");
                    free(path);
                    exit(EXIT_FAILURE);
                }

                // Skip to the next segment of the path
                while (path[index] != '\0' && path[index] != '/')
                {
                    index++;
                }
            }
        }

        free(path); // Free the allocated memory
    }

    // Update cur_dir->cur_dir with the current working directory
    if (getcwd(cur_dir->cur_dir, MAX_BUFFER) == NULL)
    {
        perror("Error: Cannot get current working directory path");
        exit(EXIT_FAILURE);
    }
}
