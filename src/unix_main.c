////////////////////////////////////////////////////////////////////////////////
// Linux/MacOS port of the checker program
////////////////////////////////////////////////////////////////////////////////
// Our code
#include "checker.c"


// Stdlib code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Platform code
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
// #include <fcntl.h>
// #include <signal.h>


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


void unix_delete_solutions() {
    remove_directory_unix(".solutions");
}


int unix_run_exercise(int exercise_number, char *input_file) {
    if (exercise_number < 0 && input_file == NULL) {
        printf("%s", text_introduction_msg);
        return 0;
    }

    if (exercise_number < 0) {
        exercise_number = 0;
    }

    printf("Running exercise %d with input file %s\n", exercise_number, input_file);

    // TODO: Before fork, create a pipe in order to capture child process's stdout

    // Do Linux/macOS child fork of program
    // See https://chatgpt.com/c/6896fe64-24e4-8322-869b-f0e32ef22a52
    pid_t pid = fork();

    printf("pid: %d\n", pid);
    if (pid == -1) {
        printf("ERROR: fork failed\n");
        return 1;
    }

    if (pid == 0) {
        // Child process: replace with another program
        if (execl(input_file, input_file, (char *)NULL) == -1) {
            printf("ERROR: exec failed\n");
            return 1;
        }
        printf("ERROR: This is an unreachable print statement after exec call\n");
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
        unix_delete_solutions();
    }
    // Run exercise executable and save stdout/stderr to a string.
    // Pass output to the checker

    if (unix_run_exercise(current_exercise, args.input_file) != 0) {
        printf("ERROR: Checker encountered problems running the program you provided\n");
        return 1;
    }

    // Write out the exercise state to the state file


    return 0;
}
