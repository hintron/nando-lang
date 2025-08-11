////////////////////////////////////////////////////////////////////////////////
// This file contains all the checker code that can be reused across platforms
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

////////////////////////////////
// Text
////////////////////////////////
#define text_title_00 "Hello, world!"
#define text_expected_output_00 "Expected output 1\n"
#define text_title_01 "My first segfault"
#define text_expected_output_01 "Expected output 2\n"

char *usage_msg = "USAGE: checker [your_program] [-h]\n";
char *help_msg =
"\n"
"Runs the checker on your program for the next exercise to pass off.\n"
"\n"
"[your_program]    The program to check for the current exercise.\n"
"-h|--help         Print this message.\n"
"--dev             Developer mode.\n"
;
char *text_introduction_msg =
"Welcome to nando-lang C exercises!\n"
"************************************************************************\n"
"Try compiling exercises/00-hello-world.c. It will fail to compile.\n"
"Fix that compiler error and make the exercise print the expected output.\n"
"Once your program is ready for submission, run the checker on the compiled output:\n"
"\n"
"    ./checker <your_program>\n"
"\n"
"Good luck! Remember, the goal is to learn and have fun with C programming!\n"
"************************************************************************\n"
;

////////////////////////////////
// Struct Definitions
////////////////////////////////

typedef struct {
    char *exercise;    // Name of the exercise to run
    char *input_file;  // Input file for the exercise
    bool print_help;
    bool dev_mode;
    char *unhandled_arg;
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
    {text_title_00, text_expected_output_00},
    {text_title_01, text_expected_output_01}
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
            } else if (double_option && curr_arg[2] == 'd') {
                args.dev_mode = true;
            } else {
                args.unhandled_arg = curr_arg;
            }
        } else if (args.input_file == NULL) {
            args.input_file = curr_arg;
        } else {
            args.unhandled_arg = curr_arg;
            break;
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
        // Progress file not found - starting from scratch
    }

    return rc;
}


int checker_check_output(int current_exercise, char *captured_stdout, char* captured_stderr) {
    // Compare captured output with expected output
    if (strcmp(captured_stdout, g_exercises[current_exercise].expected_output) == 0) {
        printf("Exercise %d completed successfully!\n", current_exercise);
        return 1;
    } else {
        printf("Exercise %d failed:\n", current_exercise);
        printf("Expected output:\n%s\n", g_exercises[current_exercise].expected_output);
        printf("Actual output:\n%s\n", captured_stdout);
        return 0;
    }

    // TODO: Check stderr
}
