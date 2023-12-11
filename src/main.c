#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 





int main() {
    char *input = NULL;
    size_t input_size = 0;

    while (1) {
        fprintf(stdout, "> ");   
        
        ssize_t read_bytes = getline(&input, &input_size, stdin);

        if (read_bytes == -1) {
            perror("getline");
            exit(EXIT_FAILURE);
        }

        if (read_bytes > 0 && input[read_bytes - 1] == '\n') {
            input[read_bytes - 1] = '\0';
            read_bytes--;
        }

        if (read_bytes == 0) {
            continue;
        }

        fprintf(stdout, "You entered: %s\n", input);   
    }

    free(input);

    return 0;
}
