////////////////////////////////////////////////////////////////////////////////
// Linux/MacOS port of the checker program
////////////////////////////////////////////////////////////////////////////////
// Our code
#include "checker.c"


// Stdlib code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Platform code
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
// #include <signal.h>

#define OUTPUT_BUFFER_SIZE 1024
#define FILEPATH_SIZE 1024
#define INFINITE_LOOP_SECS 5


void remove_directory_unix(const char *path) {
    DIR *d = opendir(path);
    if (!d) {
        return;
    }
    struct dirent *entry;
    char filepath[FILEPATH_SIZE];
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


int unix_run_exercise(
    int exercise_number,
    char *input_file,
    char *output_stdout,
    char *output_stderr
) {
    printf("Running exercise %d with input file %s\n", exercise_number, input_file);

    // Check that input file exists and is executable
    struct stat st;
    if (stat(input_file, &st) != 0) {
        printf("ERROR: Input file '%s' does not exist\n", input_file);
        return 1;
    }
    if (!(st.st_mode & S_IXUSR)) {
        printf("ERROR: Input file '%s' is not executable\n", input_file);
        return 1;
    }

    // Before fork, create a pipe in order to capture child process's stdout
    // Item [1] is always the input into the pipe (write end), while [0] is the output (read end)
    // See The Linux Programming Interface, Chapter 44

    //         pipe
    //      +-------+
    // [1]  |data-->|  [0]
    //      +-------+

    // Since both parent and child will have access to both input and output of
    // the pipe after a fork, we want to close the write end of the parent and
    // the read end of the child so that we have a pipe from child to parent.
    int stdout_pipe[2], stderr_pipe[2];
    if (pipe(stdout_pipe) == -1 || pipe(stderr_pipe) == -1) {
        printf("ERROR: Failed to set up one-way pipes from child to parent\n");
        return 1;
    }
    // Do Linux/macOS child fork of program
    // See https://chatgpt.com/c/6896fe64-24e4-8322-869b-f0e32ef22a52
    pid_t pid = fork();

    if (pid == -1) {
        printf("ERROR: fork failed\n");
        return 1;
    }

    if (pid == 0) {
        // Child process
        printf("Child process is running...\n");

        // Child only writes to pipes, so close read ends
        if (close(stdout_pipe[0]) == -1) { printf("ERROR: Child close stdout pipe read end failed\n"); }
        if (close(stderr_pipe[0]) == -1) { printf("ERROR: Child close stderr pipe read end failed\n"); }

        // Redirect child's stderr and stdout to the pipes
        if (dup2(stdout_pipe[1], STDOUT_FILENO) == -1) {
            printf("ERROR: Child dup2 stdout pipe failed\n");
        }
        if (dup2(stderr_pipe[1], STDERR_FILENO) == -1) {
            printf("ERROR: Child dup2 stderr pipe failed\n");
        }

        // Exec into the user-supplied program
        if (execl(input_file, input_file, (char *)NULL) == -1) {
            printf("ERROR: exec failed\n");
            return 1;
        }
        printf("ERROR: This is an unreachable print statement after exec call\n");
        return 1;
    }

    // Parent process
    printf("Parent process is running...\n");
    printf("Spawned child process with pid %d\n", pid);

    // Parent only reads pipe output from child, so close write ends
    if (close(stdout_pipe[1]) == -1) { printf("ERROR: Parent close stdout pipe write end failed\n"); }
    if (close(stderr_pipe[1]) == -1) { printf("ERROR: Parent close stderr pipe write end failed\n"); }

    // Set pipes to non-blocking mode, so if the child hasn't output anything to
    // stdout (infinite loop) or stderr (no error messages printed), it doesn't
    // block forever at read().
    // See TLPI 44.9;
    int flags = fcntl(stdout_pipe[0], F_GETFL, 0);
    if (flags == -1) {
        printf("ERROR: fcntl get flags failed for stdout pipe [0]\n");
    }
    // Set child stdout to non-blocking
    if (fcntl(stdout_pipe[0], F_SETFL, flags | O_NONBLOCK) == -1) {
        printf("ERROR: fcntl set flags failed for stdout pipe [0]\n");
    }
    flags = fcntl(stderr_pipe[0], F_GETFL, 0);
    if (flags == -1) {
        printf("ERROR: fcntl get flags failed for stderr pipe [0]\n");
    }
    // Set child stderr to non-blocking
    if (fcntl(stderr_pipe[0], F_SETFL, flags | O_NONBLOCK) == -1) {
        printf("ERROR: fcntl set flags failed for stderr pipe [0]\n");
    }
    // Now, subsequent read() calls on STDOUT_FILENO will be non-blocking.
    // Check errno for EWOULDBLOCK/EAGAIN to determine if no data is available.

    // Read child process's stdout and stderr from the pipes and send it back to the caller
    // Do this in a for loop with sleeps, for simplicity, and break when child
    // returns (or time out if an infinite loop is detected. Child should never
    // take longer than a few seconds)

    int child_status;
    int infinite_loop_protector = INFINITE_LOOP_SECS;
    bool child_running = true;
    ssize_t total_bytes_read_stdout = 0;
    ssize_t total_bytes_read_stderr = 0;
    while (child_running) {
        // Check child's stdout
        errno = 0;
        ssize_t bytes_read_stdout = read(stdout_pipe[0], &output_stdout[total_bytes_read_stdout], OUTPUT_BUFFER_SIZE);
        if (bytes_read_stdout < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No data available, not an error
            } else {
                printf("ERROR: Failed to read from stdout pipe\n");
                return 1;
            }
        }

        // Now, check child's stderr
        errno = 0;
        ssize_t bytes_read_stderr = read(stderr_pipe[0], &output_stderr[total_bytes_read_stderr], OUTPUT_BUFFER_SIZE);
        if (bytes_read_stderr < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No data available, not an error
            } else {
                printf("ERROR: Failed to read from stderr pipe\n");
                return 1;
            }
        }

        // printf("bytes_read_stdout: %zd\n", bytes_read_stdout);
        // printf("bytes_read_stderr: %zd\n", bytes_read_stderr);

        if (bytes_read_stdout > 0) {
            total_bytes_read_stdout += bytes_read_stdout;
            if (total_bytes_read_stdout > OUTPUT_BUFFER_SIZE) {
                printf("\nERROR: Output buffer overflow! Your program exceeded %d bytes of stdout output\n", OUTPUT_BUFFER_SIZE);
                return 1;
            }
            output_stdout[total_bytes_read_stdout] = '\0';
            printf("%s", &output_stdout[total_bytes_read_stdout - bytes_read_stdout]);
        }

        if (bytes_read_stderr > 0) {
            total_bytes_read_stderr += bytes_read_stderr;
            if (total_bytes_read_stderr > OUTPUT_BUFFER_SIZE) {
                printf("\nERROR: Output buffer overflow! Your program exceeded %d bytes of stderr output\n", OUTPUT_BUFFER_SIZE);
                return 1;
            }
            output_stderr[total_bytes_read_stderr] = '\0';
            printf("%s", &output_stderr[total_bytes_read_stderr - bytes_read_stderr]);
        }

        // Only sleep if the child has already sent back bytes and is stalling for some reason
        if ((bytes_read_stdout <= 0) && (bytes_read_stderr <= 0)) {
            sleep(1);
            infinite_loop_protector--;
            if (infinite_loop_protector <= 0) {
                printf("ERROR: Infinite loop detected (program did not finish running after %d seconds)\n", INFINITE_LOOP_SECS);
                return 1;
            }
            continue;
        }

        // Wait for child to finish
        switch (waitpid(pid, &child_status, WNOHANG)) {
            case -1:
                printf("ERROR: waitpid failed\n");
                return 1;
            case 0:
                // Child is still running
                break;
            default:
                // Child has exited
                child_running = false;
                break;
        }
    }

    assert(total_bytes_read_stderr <= OUTPUT_BUFFER_SIZE);
    assert(total_bytes_read_stdout <= OUTPUT_BUFFER_SIZE);

    // Null-terminate the strings
    // TODO: Instead of null terminating, make length-based strings
    output_stdout[total_bytes_read_stdout] = '\0';
    output_stderr[total_bytes_read_stderr] = '\0';

    if (WIFSIGNALED(child_status)) {
        int sig = WTERMSIG(child_status);
        fprintf(stderr, "Child terminated by signal %d (%s)\n", sig, strsignal(sig));
        if (sig == SIGSEGV) {
            fprintf(stderr, "Segmentation fault detected\n");
        }
    } else if (WIFEXITED(child_status)) {
        int code = WEXITSTATUS(child_status);
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
    int current_exercise = 0;
    rc = checker_read_progress_state(".progress.txt", TOTAL_EXERCISES, progress_items, &current_exercise);
    if (rc != 0) {
        return 1;
    }

    if (current_exercise == -1) {
        printf("You have completed all exercises!\n");
        printf("TODO: Print outro\n");
        return 0;
    }

    // Delete the solutions directory if not in dev mode
    if (!args.dev_mode) {
        unix_delete_solutions();
    }

    if (checker_print_intro(current_exercise, args.input_file != NULL)) {
        return 0;
    }

    // Start at exercise 0
    if (current_exercise < 0) {
        current_exercise = 0;
    }

    // Run exercise executable and save stdout/stderr to a string.
    // Pass output to the checker
    // Add +1 to buffer size to accomodate null terminator, since read() doesn't add one in.
    // See TLPI 4.4
    char captured_stdout[OUTPUT_BUFFER_SIZE + 1] = {0};
    char captured_stderr[OUTPUT_BUFFER_SIZE + 1] = {0};
    if (unix_run_exercise(current_exercise, args.input_file, captured_stdout, captured_stderr) != 0) {
        printf("ERROR: Checker encountered problems running the program you provided\n");
        return 1;
    }

    rc = checker_check_output(current_exercise, captured_stdout, captured_stderr);
    if (!checker_write_progress_state(progress_items, current_exercise, rc)) {
        printf("ERROR: Failed to write exercise state to file\n");
    }

    return 0;
}
