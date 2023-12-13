#include "exec.h"

int fetch_bin(char *bin, char *arg[])
{
    char sPath[MAXPATH] = "";
    char binary_p[MAXPATH] = "";
    char *pTmp;
    char **path_list = NULL;
    int n_path = 0;
    if ((pTmp = getenv("PATH")) != NULL)
    {
        strncpy(sPath, pTmp, MAXPATH - 1);
        sPath[MAXPATH - 1] = '\0';  
    }
     char *token = strtok(sPath, ":");
    while (token != NULL)
    {
        path_list = realloc(path_list, (n_path + 1) * sizeof(char *));
        if (!path_list)
        {
            perror("Failed to allocate memory for path list");
            return -1; 
        }
        path_list[n_path] = strdup(token);
        if (!path_list[n_path])
        {
            perror("Failed to duplicate string");
            return -1;  
        }
        n_path++;
        token = strtok(NULL, ":");
    }

    DIR *d;
    struct dirent *dir;
    int found = 0;

    for (int i = 0; i < n_path; i++)
    {
        d = opendir(path_list[i]);
        if (!d)
        {
            perror("Failed to open directory");
            continue;
        }
        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, bin) == 0)
            {
                snprintf(binary_p, sizeof(binary_p), "%s/%s", path_list[i], bin);
                pid_t p = fork();
                if (p < 0)
                {
                    perror("fork failed");
                    return -1;
                }
                else if (p == 0)
                {
                    char *const argv[] = {binary_p, NULL};
                    execvp(binary_p, argv);
                    perror("execvp failed");
                    exit(1);
                }
                else
                {
                    int status;
                    waitpid(p, &status, 0); // Wait for the child process to finish
                    if (WIFEXITED(status) == NULL || WEXITSTATUS(status) != 0)
                    {
                        printf("Child process encountered an error or was killed.\n");
                    }

                    found = 1;
                    break;  
                }
            }
        }
        closedir(d);
        if (found)
            break;  
    }

     for (int i = 0; i < n_path; i++)
    {
        free(path_list[i]);
    }
    free(path_list);

    return found ? 0 : -1; // Return 0 if found, -1 otherwise
}
