#include <stdio.h>
#include "error_reporting.h"

void print_error(char* error) {
    printf("%s", RED);
    printf("[!] %s\n", error);
    printf("%s", NORMAL);
    exit(0);   
}