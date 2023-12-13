#ifndef LOCAL_H
#define LOCAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include "common.h"
#include <unistd.h>

void cd(char **content, dir_info *cur_dir, int nb_tokens);

#endif /* LOCAL_H */
