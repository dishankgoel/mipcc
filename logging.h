#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

#define RED "\033[1;31m"
#define RESET "\033[0m"
#define GREEN "\033[01;32m"
#define YELLOW "\033[01;33m"

extern int column;

extern const char* argp_program_version;
extern const char* argp_program_bug_address;

static char doc[] = "A compiler for compiling minimalistic C to MIPS assembly";
static char args_doc[] = "";

static struct argp_option options[] = {
    {"in", 'f', "FILE", 0, "File containing C code"},
    {"run", 'r', 0, 0, "Run the Generated MIPS assembly (make sure spim is installed)"},
    {"out", 'o', "FILE", 0, "Write MIPS assembly to this file"},
    {0}
};

struct arguments {
    char* args[2];
    char* input_file;
    char* output_file;
    int verbose, silent, run;
};

static error_t parse_opt(int key, char* arg, struct argp_state *state) {
    struct arguments* arguments = state->input;

    switch (key)
    {
        case 'f':
            arguments->input_file = arg;
            break;
        case 'r':
            arguments->run = 1;
            break;
        case 'o':
            arguments->output_file = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 4)
                /* Too many arguments. */
                argp_usage (state);

            arguments->args[state->arg_num] = arg;

            break;

        case ARGP_KEY_END:
            if (state->arg_num > 0)
                /* Not enough arguments. */
                argp_usage (state);
            break;

        default:
            return ARGP_ERR_UNKNOWN;

    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

void print_error(char* error);
void print_log(char* msg);
void print_success(char* msg);