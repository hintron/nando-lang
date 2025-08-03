// Linux/MacOS port of the checker program

int main(int argc, char **argv) {
    arg_t args = checker_parse_args(argc, argv);
    if (args.print_help) {
        print_help_msg();
        return 0;
    }

    // TODO: Get progress state from the progress file
    checker_parse_progress_state(NULL, NULL, 0);
    // TODO: Pass progress state into checker_select_exercise()
    checker_select_exercise();
    // TODO: Run exercise executable and save stdout/stderr to a string.
    // Pass output to the checker
    return checker_run_exercise();
}
