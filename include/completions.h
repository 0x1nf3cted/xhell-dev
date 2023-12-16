#ifndef COMPLETIONS_H
#define COMPLETIONS_H


#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "local.h"


char *text_completion(char *key_word, char *path);

#endif /* COMPLETIONS_H */