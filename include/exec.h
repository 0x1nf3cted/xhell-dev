#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include "local.h"
#include <sys/wait.h> // for waitpid
#include <unistd.h>   // for fork, execvp
    
int fetch_bin(char *bin, char* arg[]);

#endif /* EXEC_H */