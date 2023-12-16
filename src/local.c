#include "common.h"

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

void replace_sub_str(char *str, const char *old, const char *new)
{
    char *pos = strstr(str, old);
    if (pos != NULL)
    {
        size_t oldLen = strlen(old);
        size_t newLen = strlen(new);
        size_t taille = strlen(pos + oldLen);

        memmove(pos + newLen, pos + oldLen, taille + 1);
        memcpy(pos, new, newLen);
    }
}

char *fixPath(const char *path)
{
    char modifiedPath[strlen(path) + 1];
    strcpy(modifiedPath, path);

    char *prevOccurrence = NULL;
    char *occurrence = strstr(modifiedPath, "../");
    while (occurrence != NULL)
    {
        if (occurrence == modifiedPath || *(occurrence - 1) == '/')
        {
            memmove(occurrence, occurrence + 3, strlen(occurrence + 3) + 1);

            prevOccurrence = occurrence;
            occurrence = strstr(modifiedPath, "../");
        }
        else
        {
            occurrence = strstr(occurrence + 1, "../");
        }
    }

    return strdup(modifiedPath);
}

void cd(char **content, dir_info *cur_dir, int nb_tokens)
{
    const char *home_dir = getenv("HOME");
    char path[MAXPATH];

    if (nb_tokens == 1)
    {
        strcpy(path, home_dir);
    }
    else
    {
        if (content[1][0] == '~')
        {
            snprintf(path, sizeof(path), "%s%s", home_dir, content[1] + 1);
        }
        else if (content[1][0] == '/' || content[1][0] == '.')
        {
            strncpy(path, content[1], MAXPATH - 1);
        }
        else
        {
            snprintf(path, sizeof(path), "%s/%s", cur_dir->cur_dir, content[1]);
        }
    }

    char resolved_path[MAXPATH];
    if (realpath(path, resolved_path) == NULL)
    {
        perror("Error resolving path");
        return;
    }

    struct stat path_stat;
    if (stat(resolved_path, &path_stat) != 0 || !S_ISDIR(path_stat.st_mode))
    {
        printf("Error: '%s' is not a directory\n", resolved_path);
        return;
    }

    if (chdir(resolved_path) != 0)
    {
        perror("Error changing directory");
        return;
    }

    if (getcwd(cur_dir->cur_dir, MAXPATH) == NULL)
    {
        perror("Error getting current working directory");
        return;
    }
}