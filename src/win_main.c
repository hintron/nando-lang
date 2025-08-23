// Windows port of the checker program
#include "checker.c"

// Stdlib code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Windows platform code
#include <windows.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <process.h>

#define FILEPATH_SIZE 1024
#define INFINITE_LOOP_SECS 5
#define COPY_BUFFER_SIZE (1 << 14) // 16 KB

// Add -DDEBUG to compile command to enable debug prints


int win_copy_file(const char *src, const char *dst) {
    if (CopyFileA(src, dst, FALSE)) {
        return 0; // Success
    } else {
        DWORD error = GetLastError();
        printf("ERROR: Cannot copy file '%s' to '%s'. Error code: %lu\n", src, dst, error);
        return 1;
    }
}


// Make a copy of a directory, but only if the destination directory doesn't already exist.
int win_copy_directory(const char *src, const char *dst) {
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    char searchPath[FILEPATH_SIZE];

    // Check if destination already exists
    DWORD dstAttribs = GetFileAttributesA(dst);
    if (dstAttribs != INVALID_FILE_ATTRIBUTES && (dstAttribs & FILE_ATTRIBUTE_DIRECTORY)) {
        #ifdef DEBUG
        printf("Destination directory '%s' already exists\n", dst);
        #endif
        return 0;
    }

    // Create destination directory
    if (!CreateDirectoryA(dst, NULL)) {
        DWORD error = GetLastError();
        if (error != ERROR_ALREADY_EXISTS) {
            printf("ERROR: Cannot create destination directory '%s'. Error code: %lu\n", dst, error);
            return 1;
        }
    }

    // Prepare search pattern
    snprintf(searchPath, sizeof(searchPath), "%s\\*", src);

    // Find first file
    hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("ERROR: Cannot open source directory '%s'\n", src);
        return 1;
    }

    do {
        // Skip . and .. entries
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
            continue;

        char srcPath[FILEPATH_SIZE], dstPath[FILEPATH_SIZE];
        snprintf(srcPath, sizeof(srcPath), "%s\\%s", src, findData.cFileName);
        snprintf(dstPath, sizeof(dstPath), "%s\\%s", dst, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Recursively copy subdirectory
            if (win_copy_directory(srcPath, dstPath) != 0) {
                FindClose(hFind);
                return 1;
            }
        } else {
            // Copy file
            if (win_copy_file(srcPath, dstPath) != 0) {
                FindClose(hFind);
                return 1;
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
    return 0;
}


void win_remove_directory(const char *path) {
    WIN32_FIND_DATAA findData;
    HANDLE hFind;
    char searchPath[FILEPATH_SIZE];

    // Prepare search pattern
    snprintf(searchPath, sizeof(searchPath), "%s\\*", path);

    // Find first file
    hFind = FindFirstFileA(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return; // Directory doesn't exist or can't be opened
    }

    do {
        // Skip . and .. entries
        if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
            continue;

        char filepath[FILEPATH_SIZE];
        snprintf(filepath, sizeof(filepath), "%s\\%s", path, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Recursively remove subdirectory
            win_remove_directory(filepath);
        } else {
            // Remove file
            DeleteFileA(filepath);
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
    RemoveDirectoryA(path);
}


int win_run_executable(
    char *input_file,
    char *output_stdout,
    char *output_stderr
) {
    // Check that input file exists
    DWORD fileAttribs = GetFileAttributesA(input_file);
    if (fileAttribs == INVALID_FILE_ATTRIBUTES) {
        printf("ERROR: Input file '%s' does not exist\n", input_file);
        return 1;
    }

    // Create pipes for stdout and stderr
    HANDLE stdout_read, stdout_write;
    HANDLE stderr_read, stderr_write;
    SECURITY_ATTRIBUTES sa;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&stdout_read, &stdout_write, &sa, 0)) {
        printf("ERROR: Failed to create stdout pipe\n");
        return 1;
    }
    if (!CreatePipe(&stderr_read, &stderr_write, &sa, 0)) {
        printf("ERROR: Failed to create stderr pipe\n");
        CloseHandle(stdout_read);
        CloseHandle(stdout_write);
        return 1;
    }

    // Ensure the read handles are not inherited by child process
    SetHandleInformation(stdout_read, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stderr_read, HANDLE_FLAG_INHERIT, 0);

    // Setup process startup info
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdError = stderr_write;
    si.hStdOutput = stdout_write;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    ZeroMemory(&pi, sizeof(pi));

    // Create the child process
    if (!CreateProcessA(
        input_file,     // Application name
        NULL,           // Command line
        NULL,           // Process security attributes
        NULL,           // Primary thread security attributes
        TRUE,           // Handles are inherited
        0,              // Creation flags
        NULL,           // Use parent's environment
        NULL,           // Use parent's current directory
        &si,            // Startup info
        &pi))           // Process info
    {
        DWORD error = GetLastError();
        printf("ERROR: Failed to create process. Error code: %lu\n", error);
        CloseHandle(stdout_read);
        CloseHandle(stdout_write);
        CloseHandle(stderr_read);
        CloseHandle(stderr_write);
        return 1;
    }

    // Close write ends of pipes in parent process
    CloseHandle(stdout_write);
    CloseHandle(stderr_write);

    // Read output from child process
    DWORD total_bytes_read_stdout = 0;
    DWORD total_bytes_read_stderr = 0;
    DWORD timeout_count = 0;
    const DWORD MAX_TIMEOUT_COUNT = INFINITE_LOOP_SECS * 10; // 10 checks per second
    // printf("MAX_TIMEOUT_COUNT: %d\n", MAX_TIMEOUT_COUNT);

    while (timeout_count < MAX_TIMEOUT_COUNT) {
        // printf("Start of loop; timeout_count: %d\n", timeout_count);

        // Try to read from stdout
        DWORD bytes_available = 0;
        if (PeekNamedPipe(stdout_read, NULL, 0, NULL, &bytes_available, NULL) && bytes_available > 0) {
            // printf("Peek stdout\n");

            DWORD bytes_read;
            DWORD bytes_to_read = min(bytes_available, OUTPUT_BUFFER_SIZE - total_bytes_read_stdout - 1);
            if (bytes_to_read > 0 && ReadFile(stdout_read, &output_stdout[total_bytes_read_stdout], bytes_to_read, &bytes_read, NULL)) {
                total_bytes_read_stdout += bytes_read;
                output_stdout[total_bytes_read_stdout] = '\0';
                printf("%s", &output_stdout[total_bytes_read_stdout - bytes_read]);

                if (total_bytes_read_stdout >= OUTPUT_BUFFER_SIZE - 1) {
                    printf("\nERROR: Output buffer overflow! Your program exceeded %d bytes of stdout output\n", OUTPUT_BUFFER_SIZE);
                    break;
                }
            }
            timeout_count = 0; // Reset timeout since we got data
        }

        // Try to read from stderr
        bytes_available = 0;
        if (PeekNamedPipe(stderr_read, NULL, 0, NULL, &bytes_available, NULL) && bytes_available > 0) {
            // printf("Peek stderr\n");

            DWORD bytes_read;
            DWORD bytes_to_read = min(bytes_available, OUTPUT_BUFFER_SIZE - total_bytes_read_stderr - 1);
            if (bytes_to_read > 0 && ReadFile(stderr_read, &output_stderr[total_bytes_read_stderr], bytes_to_read, &bytes_read, NULL)) {
                total_bytes_read_stderr += bytes_read;
                output_stderr[total_bytes_read_stderr] = '\0';
                printf("%s", &output_stderr[total_bytes_read_stderr - bytes_read]);

                if (total_bytes_read_stderr >= OUTPUT_BUFFER_SIZE - 1) {
                    printf("\nERROR: Output buffer overflow! Your program exceeded %d bytes of stderr output\n", OUTPUT_BUFFER_SIZE);
                    break;
                }
            }
            timeout_count = 0; // Reset timeout since we got data
        }

        // Check if process is still running
        DWORD exit_code;
        if (GetExitCodeProcess(pi.hProcess, &exit_code) && exit_code != STILL_ACTIVE) {
            // printf("exit_code: %d\n", exit_code);
            break;
        }

        // Sleep and increment timeout counter
        Sleep(100); // 100ms
        timeout_count++;
        // printf("sleep; timeout_count++\n");
    }

    // printf("Check for timeout\n");

    // Check for timeout
    DWORD final_exit_code;
    if (GetExitCodeProcess(pi.hProcess, &final_exit_code) && final_exit_code == STILL_ACTIVE) {
        printf("ERROR: Infinite loop detected (program did not finish running after %d seconds)\n", INFINITE_LOOP_SECS);
        TerminateProcess(pi.hProcess, 1);
        WaitForSingleObject(pi.hProcess, INFINITE);
    } else {
        // printf("Wait\n");

        // Wait for process to complete
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &final_exit_code);

        if (final_exit_code != 0) {
            fprintf(stderr, "\n\nProgram exited with code %lu\n", final_exit_code);
        } else {
            #ifdef DEBUG
            printf("\n\nProgram exited successfully\n");
            #endif
        }
    }

    // Cleanup
    CloseHandle(stdout_read);
    CloseHandle(stderr_read);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Null-terminate the output strings
    output_stdout[total_bytes_read_stdout] = '\0';
    output_stderr[total_bytes_read_stderr] = '\0';
    // printf("cleanup\n");

    return 0;
}


////////////////////////////////
// Main
////////////////////////////////
int main(int argc, char *argv[]) {
    printf("Running Nando Lang checker program on Windows\n");
    return checker_main(
        argc,
        argv,
        win_run_executable,
        win_remove_directory,
        win_copy_directory
    );
}
