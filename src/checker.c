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
#define text_title_00 "Hello, world!\n"
#define text_expected_output_00 "Expected output 1\n"
#define text_title_01 "My first segfault\n"
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
        // Progress file not found - starting from scratch
    }

    return rc;
}

// MGH TODO: This is platform-specific. Move it to unix_main.c
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
void remove_directory_unix(const char *path) {
    DIR *d = opendir(path);
    if (!d) {
        return;
    }
    struct dirent *entry;
    char filepath[1024];
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
        struct stat st;
        if (stat(filepath, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                remove_directory_unix(filepath);
            } else {
                remove(filepath);
            }
        }
    }
    closedir(d);
    rmdir(path);
}

void checker_delete_solutions() {
    remove_directory_unix(".solutions");
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int checker_run_exercise(int exercise_number, char *input_file) {
    if (exercise_number < 0 && input_file == NULL) {
        printf("%s", text_introduction_msg);
        return 0;
    }

    if (exercise_number < 0) {
        exercise_number = 0;
    }

    printf("Running exercise %d with input file %s\n", exercise_number, input_file);

    // Do Linux/macOS child fork of program
    // See https://chatgpt.com/c/6896fe64-24e4-8322-869b-f0e32ef22a52

    // TODO: Before fork, create a pipe in order to capture child process's stdout

    pid_t pid = fork();

    printf("pid: %d\n", pid);
    if (pid == -1) {
        printf("ERROR: fork failed\n");
        return 1;
    }

    if (pid == 0) {
        // Child process: replace with another program
        // TODO: Debug this exec call
        execlp(input_file, input_file, (char *)NULL);
        printf("ERROR: Should not have got here after an exec call\n");
        return 1;
    }
    // Parent process: wait for child to finish

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        printf("ERROR: waitpid failed\n");
        return 1;
    }

    if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        fprintf(stderr, "Child terminated by signal %d (%s)\n", sig, strsignal(sig));
        if (sig == SIGSEGV) {
            fprintf(stderr, "Segmentation fault detected\n");
        }
    } else if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        printf("Child exited with code %d\n", code);
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
    if (args.unhandled_arg != NULL) {
        printf("ERROR: Unhandled argument: %s\n", args.unhandled_arg);
        return 1;
    }

    // Get progress state from the progress file
    progress_item_t progress_items[TOTAL_EXERCISES] = {0};
    int current_exercise;
    rc = checker_get_progress_data(".progress.txt", TOTAL_EXERCISES, progress_items, &current_exercise);
    if (rc != 0) {
        return 1;
    }

    // Delete the solutions directory if not in dev mode
    if (!args.dev_mode) {
        checker_delete_solutions();
    }
    // Run exercise executable and save stdout/stderr to a string.
    // Pass output to the checker

    if (checker_run_exercise(current_exercise, args.input_file) != 0) {
        printf("ERROR: Checker found problems with the program you provided\n");
        return 1;
    }

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
