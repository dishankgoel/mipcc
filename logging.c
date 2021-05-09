#include "logging.h"

const char* argp_program_version = "mipcc 1.0";
const char* argp_program_bug_address = "dishank.goel@iitgn.ac.in";

void print_error(char* error) {
    printf("%s", RED);
    printf("\n%*s\n[!] %*s\n", column, "^", column, error);
    printf("%s", RESET);
    exit(0);   
}

void print_log(char* msg) {
    printf("%s", YELLOW);
    printf("[*] %s\n", msg);
    printf("%s\n", RESET);
}

void print_success(char* msg) {
    printf("%s", GREEN);
    printf("[+] %s\n", msg);
    printf("%s", RESET);
}