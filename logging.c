#include <stdio.h>
#include "logging.h"

void print_error(char* error) {
    printf("%s", RED);
    printf("[!] %s\n", error);
    printf("%s", NORMAL);
    exit(0);   
}