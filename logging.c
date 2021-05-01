#include <stdio.h>
#include "logging.h"

void print_error(char* error) {
    printf("%s", RED);
    printf("\n%*s\n[!] %*s\n", column, "^", column, error);
    printf("%s", NORMAL);
    exit(0);   
}