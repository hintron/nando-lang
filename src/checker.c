#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

////////////////////////////////
// #defines
////////////////////////////////

#define TOTAL_EXERCISES sizeof(g_exercises) / sizeof(g_exercises[0])
#define PROGRESS_FILE ".progress.txt"
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


////////////////////////////////
// Struct Definitions
////////////////////////////////

typedef struct {
    char *exercise;    // Name of the exercise to run
    char *input_file;  // Input file for the exercise
    char *output_file; // Output file for the exercise
    int debug_mode;    // Debug mode flag (0 or 1)
    int verbose_mode;  // Verbose mode flag (0 or 1)
    bool print_help;
} arg_t;

typedef struct {
    int exercise_number;
    int status; // 0 = not completed, 1 = completed
    int fail_count;
} progress_item_t;

typedef struct {
    char name[256];
    char expected_output[1024];
} exercise_info_t;


////////////////////////////////
// Globals
////////////////////////////////
exercise_info_t g_exercises[] = {
    {"Hello, world!", "Expected output 1"},
    {"My first segfault", "Expected output 2"}
};
char *usage_msg = "USAGE: checker [your_program] [-h]\n";
char *help_msg =
"\n"
"Runs the checker on your program for the next exercise to pass off.\n"
"\n"
"[your_program]    The program to check for the current exercise.\n"
"-h|--help]        Print this message.\n"
;

////////////////////////////////
// Functions
////////////////////////////////

arg_t checker_parse_args(int argc, char **argv) {
    arg_t args = {0};
    char *program_name = argv[0];
    int curr_arg_index = 1;
    while (curr_arg_index < argc) {
        bool single_option = false;
        bool double_option = false;
        char *curr_arg = argv[curr_arg_index];
        if (curr_arg[0] && curr_arg[0] == '-') {
            if (curr_arg[1] && curr_arg[1] == '-') {
                double_option = true;
            } else {
                single_option = true;
            }

            if (
                (single_option && curr_arg[1] == 'h') ||
                (double_option && curr_arg[2] == 'h')
            ) {
                args.print_help = true;
            }
        }
        curr_arg_index++;
    }
    return args;
}

void print_help_msg() {
    printf("%s", usage_msg);
    printf("%s", help_msg);
    printf("\n");
}

// progress_item_t *checker_parse_progress_state(
//     char *progress_file_bytes,
//     progress_item_t *progress_items,
//     int *num_items
// ) {
//     printf("TODO: Parse progress file bytes into a progress_item_t array...\n");
//     // TODO: Fill out the empty progress_items array
//     return progress_items;
// }

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

////////////////////////////////
// Main
// For now, just do a single main and try to use c std lib functions
////////////////////////////////
int main(int argc, char **argv) {
    arg_t args = checker_parse_args(argc, argv);
    if (args.print_help) {
        print_help_msg();
        return 0;
    }

    printf("Size of exercises in bytes: %ld\n", sizeof(g_exercises));
    printf("Number of items in exercises: %ld\n", sizeof(g_exercises) / sizeof(g_exercises[0]));
    // Get progress state from the progress file
    progress_item_t progress_items[TOTAL_EXERCISES] = {0};
    int processed_progress_items = 0;
    FILE *fp = fopen(PROGRESS_FILE, "r+");
    if (fp) {
        for (int i = 0; i < TOTAL_EXERCISES; i++) {
            progress_item_t *item = &progress_items[i];
            int rc = fscanf(fp, "%d %d %d",
                &item->exercise_number,
                &item->status,
                &item->fail_count
            );
            if (item->exercise_number != i) {
                printf(
                    "Error: exercise number mismatch (expected %d, got %d)\n",
                    i,
                    item->exercise_number
                );
            }
            if (rc == 3) {
                processed_progress_items++;
                printf("Exercise %d (%s):\n", i, g_exercises[i].name);
                printf(
                    "    status: %s; fail count: %d\n",
                    item->status == 1 ? "COMPLETED" : "UNFINISHED",
                    item->fail_count
                );
            } else {
                printf("Error reading progress file at exercise %d (fscanf() rc = %d)\n", i, rc);
                break;
            }
        }
        fclose(fp);
    } else {
        printf("Progress file '%s' not found. Assuming it doesn't exist.\n", PROGRESS_FILE);
    }

    // checker_parse_progress_state(NULL, NULL, 0);
    // TODO: Pass progress state into checker_select_exercise()
    checker_select_exercise();
    // TODO: Run exercise executable and save stdout/stderr to a string.
    // Pass output to the checker
    checker_run_exercise();

    // TODO: Write out the exercise state to the state file


    return 0;
}


// // Finally, include the implementation-specific main() function
// // https://stackoverflow.com/questions/47045629/is-it-legal-to-use-elif-with-ifdef
// #if defined(WINDOWS)
//     #include "windows_main.c"
// #else
//     // Unix main should work for both Linux and macOS
//     #include "unix_main.c"
// #endif
