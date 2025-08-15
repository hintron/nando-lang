// Windows port of the checker program
#include "checker.c"

int main(int argc, char *argv[]) {
    printf("Hello from Windows console!\n");

    arg_t args = checker_parse_args(argc, argv);
    if (args.print_help) {
        print_help_msg();
        return 0;
    }
    if (args.unhandled_arg != NULL) {
        printf("ERROR: Unhandled argument: %s\n", args.unhandled_arg);
        return 1;
    }

    return 0;
}

