////////////////////////////////////////////////////////////////////////////////
// Checker program top-level file
////////////////////////////////////////////////////////////////////////////////

// Include the implementation-specific main() function. That function will
// call generic checker functions defined in checker.c.
// https://stackoverflow.com/questions/47045629/is-it-legal-to-use-elif-with-ifdef
#if defined(WINDOWS)
    #include "windows_main.c"
#else
    // Unix main should work for both Linux and macOS
    #include "unix_main.c"
#endif
