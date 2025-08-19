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
char text_title_00[] = "Hello, world!";
char text_expected_output_00[] = "Hello, world!\n";
char text_title_01[] = "My first segfault";
char text_expected_output_01[] =
"Jill's family:\n"
"  - Bachelorette: Jill\n"
"Jill's father's family:\n"
"  - Father: Freddy\n"
"  - Mother: Gina\n"
"    * Child 1: Hank\n"
"    * Child 2: Ivy\n"
"    * Child 3: Jill\n"
"Jill's father's father's family:\n"
"  - Father: Dave\n"
"  - Mother: Emily\n"
"    * Child 1: Freddy\n"
"    * Child 2: Mary\n"
"Jill's father's father's father's family:\n"
"  - Father: Atkins\n"
"  - Mother: Bernice\n"
"    * Child 1: Dave\n"
;

char *usage_msg = "USAGE: checker [your_program] [-h][--reset]\n";
char *help_msg =
"\n"
"Runs the checker on your program for the next exercise to pass off.\n"
"\n"
"[your_program]    The program to check for the current exercise.\n"
"-h|--help         Print this message.\n"
"--reset           Delete progress file to reset progress.\n"
"--dev             Developer mode.\n"
;
char *text_introduction_msg =
"Welcome to nando-lang C exercises!\n";
char *text_introduction_00 =
"************************************************************************\n"
"Try compiling exercises/00-hello-world.c. It will fail to compile.\n"
"Fix that compiler error and make the exercise print the expected output.\n"
"Once your program is ready for submission, run the checker on the compiled output:\n"
"\n"
"    ./checker <your_program>\n"
"\n"
"Good luck! Remember, the goal is to learn and have fun with C programming!\n"
"************************************************************************\n";
char *text_introduction_01 =
"************************************************************************\n"
"Try fixing exercises/01-my-first-segfault.c. It will compile, but hit a segfault when run.\n"
"\n"
"Tips:\n"
" * Pointers are variables that point to other variables in memory.\n"
" * To declare a pointer to a variable of <type>, do `<type> * <pointer_name>`.\n"
" * To make <pointer_name> point to another variable, assign `<pointer_name> = &<variable>`\n"
" * To dereference a pointer (make the pointer give the value it's pointing at), do `*<pointer_name>`\n"
"************************************************************************\n"
;

char *outro_msg =
"************************************************************************\n"
"           Congratulations! You have completed all exercises!\n"
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
    bool reset_progress;
    char *unhandled_arg;
} arg_t;

typedef struct {
    int exercise_number;
    int status; // 0 = unfinished, 1 = completed
    int try_count;
} progress_item_t;

typedef struct {
    char *name;
    char *expected_output;
    char *exercise_file;
    char *solution_file;
    char *solved_exercise_file;
} exercise_info_t;

////////////////////////////////
// Globals
////////////////////////////////

exercise_info_t g_exercises[] = {
    {text_title_00, text_expected_output_00, "exercises/00-hello-world.c", ".solutions/00.c", ".solved/00.c"},
    {text_title_01, text_expected_output_01, "exercises/01-my-first-segfault.c", ".solutions/01.c", ".solved/01.c"}
};
#define TOTAL_EXERCISES (sizeof(g_exercises) / sizeof(g_exercises[0]))
#define SOLUTION_START "//--START--//"
#define SOLUTION_END   "//--END--//"
#define REPLACEMENT_TEXT_SIZE 8192
#define MAX_LINE_SIZE 1024


////////////////////////////////
// Functions
////////////////////////////////

void _delete_progress() {
    if (remove(".progress") != 0) {
        #ifdef DEBUG
        printf("ERROR: Failed to delete progress file\n");
        #endif
    } else {
        #ifdef DEBUG
        printf("Successfully deleted progress file\n");
        #endif
    }
}

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
            } else if (double_option && curr_arg[2] == 'r') {
                args.reset_progress = true;
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

    // Handle any generic args here
    if (args.reset_progress) {
        _delete_progress();
    }

    return args;
}


void print_help_msg() {
    printf("%s", usage_msg);
    printf("%s", help_msg);
    printf("\n");
}

// Return the first unfinished exercise
int checker_print_progress(progress_item_t *progress_items, int total_exercises) {
    int first_unfinished_exercise = -1;
    printf("Progress report:\n");
    printf("=====================================================================================\n");
    for (int i = 0; i < total_exercises; i++) {
        progress_item_t *item = &progress_items[i];
        // Mark the first unfinished exercise as the next one to do
        if ((first_unfinished_exercise == -1) && (item->status == 0)) {
            first_unfinished_exercise = i;
        }
        printf("Exercise %d: %30s.................%s (%d attempt%s)\n",
            i,
            g_exercises[i].name,
            item->status == 1 ? "COMPLETED " : "unfinished",
            item->try_count,
            item->try_count == 1 ? "" : "s"
        );
    }
    printf("=====================================================================================\n");
    return first_unfinished_exercise;
}


// Print progress data from a progress_file and return the current exercise.
// If current exercise is -1, then no exercises have been attempted.
// If current exercise is >= total_exercises, then all exercises are completed.
int checker_read_progress_state(
    char *progress_file,
    int *out_current_exercise
) {
    *out_current_exercise = -1;
    FILE *fp = fopen(progress_file, "r+");
    if (!fp) {
        #ifdef DEBUG
        printf("Progress file not found - starting from scratch\n");
        #endif
        return 0;
    }

    // There is a progress file, so it should have some entries
    // Iterate through each line of the progress file.
    // The progress file is an append-only database, to keep things dead simple
    int rc = 0;
    int line_count = 0;
    progress_item_t progress_items[TOTAL_EXERCISES] = {0};
    while (true) {
        int exercise_number = -1;
        int status = -1;
        // Scan next line of progress file
        int rc2 = fscanf(fp, "%d %d\n", &exercise_number, &status);
        if (rc2 == EOF) {
            // end of file
            #ifdef DEBUG
            printf("Progress file has been read completely\n");
            #endif
            break;
        } else if (rc2 == 2) {
            if (exercise_number < 0 || exercise_number >= TOTAL_EXERCISES) {
                printf("ERROR: Invalid exercise number %d in progress file\n", exercise_number);
                rc = 1;
                continue;
            }
            progress_item_t *item = &progress_items[exercise_number];
            if (status == 0) {
                if (item->status == 1) {
                    printf("ERROR: Exercise %d was completed but was later marked as unfinished\n", exercise_number);
                }
            }
            item->try_count++;
            item->status = status;
        } else {
            printf("Error reading progress file at exercise %d (fscanf() rc = %d)\n", exercise_number, rc2);
            rc = rc2;
        }
        line_count++;
    }

    if (line_count == 0) {
        #ifdef DEBUG
        printf("No progress entries found in .progress file\n");
        #endif
        return 0;
    }
    *out_current_exercise = checker_print_progress(progress_items, TOTAL_EXERCISES);
    if (*out_current_exercise == -1) {
        // All exercises were completed!
        *out_current_exercise = TOTAL_EXERCISES;
    }

    fclose(fp);
    return rc;
}


int checker_check_output(int current_exercise, char *captured_stdout /*, char* captured_stderr*/) {
    // Compare captured output with expected output
    if (strcmp(captured_stdout, g_exercises[current_exercise].expected_output) == 0) {
        printf("Exercise %d completed successfully!\n", current_exercise);
        return 0;
    } else {
        printf("ERROR: Exercise %d failed:\n", current_exercise);
        printf("Expected output:\n");
        printf("----------------------------------------------------------------\n");
        printf("%s", g_exercises[current_exercise].expected_output);
        printf("----------------------------------------------------------------\n");
        // No need to print actual output, since that is already printed
        if (current_exercise > 0 && strcmp(captured_stdout, g_exercises[current_exercise - 1].expected_output) == 0) {
            printf("HINT: It appears that you reused the program from the last exercise (exercise %d)...\n", current_exercise - 1);
        }
        return -1;
    }

    // TODO: Check stderr
}


int checker_write_progress_state(int current_exercise, int rc) {
    // Append the exercise attempt. This file is an append-only database
    FILE *fp = fopen(".progress", "a");
    if (!fp) {
        printf("ERROR: Failed to open progress file for writing\n");
        return -1;
    }
    // Convert rc == 0 (success) into 1 for exercise completed
    fprintf(fp, "%d %d\n", current_exercise, rc == 0 ? 1 : 0);
    fclose(fp);
    return 0;
}


// If we print the intro, return 1
void checker_print_intro(int current_exercise) {
    switch (current_exercise) {
        case -1:
            printf("%s", text_introduction_msg);
            printf("%s", text_introduction_00);
            break;
        case 0:
            printf("%s", text_introduction_00);
            break;
        case 1:
            printf("%s", text_introduction_01);
            break;
        default:
            // Print the outro message - all exercises are completed!
            printf("%s\n", outro_msg);
            break;
    }
}


// Delete the solutions directory, using the given platform-specific rmdir
// function.
void checker_delete_solutions(void (*rm_dir_fnptr)(const char *)) {
    rm_dir_fnptr(".solutions");
}

// Create a copy of the exercises folder as a backup
// If in dev mode, have solutions be in a separate folder. If not in dev mode,
// make a backup of the folder and have the user edit in the exercises/ folder.
void checker_backup_exercises(int (*copy_dir_fnptr)(const char *, const char *)) {
    if (copy_dir_fnptr("exercises", ".original-exercises") != 0) {
        printf("ERROR: Failed to copy exercises/ to .original-exercises/\n");
        return;
    }
    // printf("Created backup copy of exercises/ in .original-exercises/\n");
    // printf("Consult those original exercise files if you mess up your exercise files.\n");
    // For dev work, simply create a copy of exercises and name it "my-solutions" in the terminal
}


// Replace a section of a file and write to a separate output file
// Returns 0 on success, -1 on error
int checker_replace_file_section(
    const char *input_file,
    const char *output_file,
    const char *replacement_file
) {
    FILE *replacement_fp = fopen(replacement_file, "r");
    if (!replacement_fp) {
        printf("ERROR: Failed to open replacement file: %s\n", replacement_file);
        return -1;
    }

    // Make a large string big enough for the replacement text
    char replacement_text[REPLACEMENT_TEXT_SIZE];
    int total_read = 0;
    while (fgets(replacement_text + total_read, REPLACEMENT_TEXT_SIZE - total_read, replacement_fp)) {
        total_read = (int)strlen(replacement_text);
        if (total_read >= REPLACEMENT_TEXT_SIZE - 1) {
            printf("ERROR: Replacement text exceeds buffer size (%u bytes) and was truncated\n", REPLACEMENT_TEXT_SIZE);
            break;
        }
    }
    fclose(replacement_fp);

    if (total_read == 0) {
        printf("ERROR: No replacement text given\n");
        return -1;
    }

    FILE *input_fp = fopen(input_file, "r");
    if (!input_fp) {
        printf("ERROR: Failed to open input file: %s\n", input_file);
        return -1;
    }

    FILE *output_fp = fopen(output_file, "w");
    if (!output_fp) {
        printf("ERROR: Failed to open output file: %s\n", output_file);
        fclose(input_fp);
        return -1;
    }

    char line[MAX_LINE_SIZE];
    bool in_replace_section = false;
    bool found_start = false;

    while (fgets(line, MAX_LINE_SIZE, input_fp)) {
        if (in_replace_section) {
            // Lines inside the replace section are skipped
            if (strstr(line, SOLUTION_END)) {
                // We are at the end of the replace section
                // Next line will be written
                in_replace_section = false;
                // Keep end marker line
                fprintf(output_fp, "%s", line);
            }
            continue;
        }

        // Normal line outside the replace section, or start marker line
        fprintf(output_fp, "%s", line);
        if (strstr(line, SOLUTION_START)) {
            // We are at the start of the replace section
            // Write the replacement text instead of lines in the marked section
            in_replace_section = true;
            found_start = true;
            fprintf(output_fp, "%s", replacement_text);
        }
    }

    fclose(input_fp);
    fclose(output_fp);

    if (!found_start) {
        printf("WARNING: Start marker '" SOLUTION_START "' was not found in file %s\n", input_file);
    }

    return 0;
}

int checker_solve_exercise(int current_exercise) {
    // Implementation of exercise solving logic goes here
    return checker_replace_file_section(
        g_exercises[current_exercise].exercise_file,
        g_exercises[current_exercise].solved_exercise_file,
        g_exercises[current_exercise].solution_file
    );
}

void checker_solve_all_exercises() {
    for (int i = 0; i < TOTAL_EXERCISES; i++) {
        if (checker_solve_exercise(i) != 0) {
            printf("ERROR: Failed to solve exercise %d\n", i);
        }
    }
}