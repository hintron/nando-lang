#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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
// Text
////////////////////////////////
#define text_title_hello_world "Hello, world!\n"
#define text_expected_output_hello_world "Expected output 1\n"
#define text_title_my_first_segfault "My first segfault\n"
#define text_expected_output_my_first_segfault "Expected output 2\n"

char *usage_msg = "USAGE: checker [your_program] [-h]\n";
char *help_msg =
"\n"
"Runs the checker on your program for the next exercise to pass off.\n"
"\n"
"[your_program]    The program to check for the current exercise.\n"
"-h|--help]        Print this message.\n"
;
char *introduction_msg =
"Welcome to nando-lang C exercises!\n"
"************************************************************************\n"
"To run the exercises, use the following command:\n"
"  ./checker\n"
"\n"
"The first exercise will fail, because you haven't fixed it yet!\n"
"\n"
"Edit exercise file 00-hello-world/main.c and try compiling and running\n"
"it until it passes. Then move on to exercise 01 and repeat.\n"
"\n"
"If you mess up your file, you can always refer to the starting point of original.c.\n"
"If you get stumped and need to look at the solution, visit\n"
"https://github.com/hintron/nando-lang and view the solution files there.\n"
"\n"
"Good luck! Remember, the goal is to learn and have fun with C programming!\n"
"\n"
"(Avoid the temptation to look at solutions or to use AI - these exercises\n"
"are designed to help you learn by doing, so try to solve them on your own,\n"
"and type them out manually.)\n"
"\n";


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
    {text_title_hello_world, text_expected_output_hello_world},
    {text_title_my_first_segfault, text_expected_output_my_first_segfault}
};
#define TOTAL_EXERCISES sizeof(g_exercises) / sizeof(g_exercises[0])

////////////////////////////////
// Functions
////////////////////////////////

arg_t checker_parse_args(int argc, char **argv) {
    arg_t args = {0};
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

// Fill a pre-allocated progress_item_t array with progress data from progress_file
// g_
int checker_get_progress_data(
    char *progress_file,
    int total_exercises,
    progress_item_t *out_progress_items,
    int *out_current_exercise
) {
    int rc = 0;
    *out_current_exercise = -1;
    FILE *fp = fopen(progress_file, "r+");
    if (fp) {
        for (int i = 0; i < total_exercises; i++) {
            progress_item_t *item = &out_progress_items[i];
            int rc2 = fscanf(fp, "%d %d %d",
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
            if (rc2 == 3) {
                printf("Exercise %d (%s):\n", i, g_exercises[i].name);
                printf(
                    "    status: %s; fail count: %d\n",
                    item->status == 1 ? "COMPLETED" : "UNFINISHED",
                    item->fail_count
                );
                // Record the first unfinished exercise. That's the next one to do
                if (item->status == 0 && *out_current_exercise == -1) {
                    *out_current_exercise = i;
                }
            } else {
                printf("Error reading progress file at exercise %d (fscanf() rc = %d)\n", i, rc2);
                rc = rc2;
                break;
            }
        }
        fclose(fp);
    } else {
        printf("Progress file '%s' not found. Starting from scratch.\n", progress_file);
    }

    return rc;
}

int checker_run_exercise(int exercise_number, char *input_file) {
    if (exercise_number > 0) {
        printf("Starting fresh!\n");
        printf("TODO: Introductory blurb\n");
    } else {
        printf("TODO: Running exercise %d with input file %s\n", exercise_number, input_file);
    }
    return 0;
}

////////////////////////////////
// Main
// For now, just do a single main and try to use c std lib functions
////////////////////////////////
int main(int argc, char **argv) {
    int rc = 0;
    arg_t args = checker_parse_args(argc, argv);
    if (args.print_help) {
        print_help_msg();
        return 0;
    }

    printf("Size of exercises in bytes: %ld\n", sizeof(g_exercises));
    printf("Number of items in exercises: %ld\n", sizeof(g_exercises) / sizeof(g_exercises[0]));
    // Get progress state from the progress file
    progress_item_t progress_items[TOTAL_EXERCISES] = {0};
    int current_exercise;
    rc = checker_get_progress_data(".progress.txt", TOTAL_EXERCISES, progress_items, &current_exercise);
    if (rc != 0) {
        return 1;
    }

    // Run exercise executable and save stdout/stderr to a string.
    // Pass output to the checker

    checker_run_exercise(current_exercise, args.input_file);

    // Write out the exercise state to the state file


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
