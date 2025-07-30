#include <stdio.h>

#define TOTAL_EXERCISES 2

// https://stackoverflow.com/questions/47045629/is-it-legal-to-use-elif-with-ifdef
// Each platform implementation file will have platform-specific functionality
// that will be called with `platform_xxx()` functions
// will parse args and then call into checker_main().
// #if defined(LINUX)
//     #include "linux_platform.c"
// #elif defined(MACOS)
//     #include "macos_platform.c"
// #elif defined(WINDOWS)
//     #include "windows_platform.c"
// #else
//     #include "generic_platform.c"
// #endif

// TODO: Implement an array type?
// typedef struct {
//     int capacity;
//     int size;
//     int *data;
// } array_t;

// TODO: Finish this
typedef struct {
    char *exercise;    // Name of the exercise to run
    char *input_file;  // Input file for the exercise
    char *output_file; // Output file for the exercise
    int debug_mode;    // Debug mode flag (0 or 1)
    int verbose_mode;  // Verbose mode flag (0 or 1)
} arg_t;


// TODO: Finish this
typedef struct {
    int exercise_number;
    char *exercise_name;
    char *input_file;
    char *output_file;
    char *expected_output;
    int status; // 0 = not started, 1 = in progress, 2 = completed, 3 = failed
    int fail_count;
} progress_item_t;


// TODO:
arg_t *checker_parse_args(int argc, char **argv, arg_t *args) {
    printf("TODO: Parse args into arg_t...\n");
    return args;
}

progress_item_t *checker_parse_progress_state(
    char *progress_file_bytes,
    progress_item_t *progress_items,
    int *num_items
) {
    printf("TODO: Parse progress file bytes into a progress_item_t array...\n");
    // TODO: Fill out the empty progress_items array
    return progress_items;
}

// TODO:
void checker_select_exercise() {
    printf("TODO: Look at progress state and determine which exercise to check next...\n");
    return;
}

// TODO:
int checker_run_exercise() {
    printf("Hello, world!\n");
    printf("TODO: \n");
    return 0;
}


// Finally, include the implementation-specific main() function
// https://stackoverflow.com/questions/47045629/is-it-legal-to-use-elif-with-ifdef
#if defined(WINDOWS)
    #include "windows_main.c"
#else
    // Unix main should work for both Linux and macOS
    #include "unix_main.c"
#endif
