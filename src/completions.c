#include "completions.h"

char *text_completion(char *key_word, char *path)
{
    DIR *d;
    struct dirent *dir;
    char *k = NULL;
    int bound = 0;

    for (int i = strlen(path) - 1; i >= 0; i--)
    {
        if (path[i] == '/')
        {
            bound = i;
            break;
        }
    }
    
    k = (char *)malloc(sizeof(char));

    if (bound == 0)
    {
        strcpy(k, key_word);
        return;
    }
    strncpy(k, key_word, strlen(path) - bound);

    d = opendir(path);
    while ((dir = readdir(d)) != NULL)
    {
        if (strncmp(dir->d_name, k, strlen(k)) == 0)
        {
            return dir->d_name;
        }
    }
    closedir(d);
    return k;
}