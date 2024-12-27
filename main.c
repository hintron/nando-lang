#include "stdio.h"
#include "stdlib.h"

#define REGISTER_FILE_SIZE 10

typedef enum Command {
        SET,
        ADD,
        SUB,
        MUL,
        DIV,
        NONE
} CommandT;

int main(int argc, char const *argv[]) {
        if (argc != 2) {
                fprintf(stderr, "Usage: %s <input.nando>\n", argv[0]);
                return 1;
        }

        FILE *file = fopen(argv[1], "r");
        if (!file) {
                perror("Error opening file");
                return 1;
        }

        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        float register_file[REGISTER_FILE_SIZE] = {};

        while ((read = getline(&line, &len, file)) != -1) {
                // printf("Line (%zu): %s", len, line);
                // nando lang is guaranteed to have two values
                int first_val_pos = -1;
                int second_val_pos = -1;
                int first_reg = -1;
                int second_reg = -1;
                int second_num = 0;

                CommandT cmd = NONE;
                if (line[0] == '#') {
                        continue;
                }
                if (line[0] == '/') {
                        continue;
                }
                if (len < 9) {
                        printf("Command is too short (len=%zu): %s\n", len, line);
                        continue;
                }
                // Parse the instruction
                if (
                        (line[0] == 's' || line[0] == 'S') &&
                        (line[1] == 'e' || line[1] == 'E') &&
                        (line[2] == 't' || line[2] == 'T')
                ) {
                        cmd = SET;
                } else if (
                        (line[0] == 'a' || line[0] == 'A') &&
                        (line[1] == 'd' || line[1] == 'D') &&
                        (line[2] == 'd' || line[2] == 'D')
                ) {
                        cmd = ADD;
                } else if (
                        (line[0] == 's' || line[0] == 'S') &&
                        (line[1] == 'u' || line[1] == 'U') &&
                        (line[2] == 'b' || line[2] == 'B')
                ) {
                        cmd = SUB;
                } else if (
                        (line[0] == 'm' || line[0] == 'M') &&
                        (line[1] == 'u' || line[1] == 'U') &&
                        (line[2] == 'l' || line[2] == 'L')
                ) {
                        cmd = MUL;
                } else if (
                        (line[0] == 'd' || line[0] == 'D') &&
                        (line[1] == 'i' || line[1] == 'I') &&
                        (line[2] == 'v' || line[2] == 'V')
                ) {
                        cmd = DIV;
                }
                if (cmd == NONE) {
                        printf("Unknown command: %s\n", line);
                        continue;
                }
                // Parse the second value. It is a register.
                for (int i = 4; line[i] != '\0'; i++) {
                        // Find the first non-whitespace character
                        if (first_val_pos == -1) {
                                if (line[i] <= ' ') {
                                        continue;
                                }
                                first_val_pos = i;
                        }
                }
                if (first_val_pos == -1) {
                        printf("ERROR: Could not find first value: %s\n", line);
                        continue;
                }
                if (line[first_val_pos] != 'r') {
                        printf("ERROR: Invalid first value: %s\n", line);
                        continue;
                }
                // Only support registers from r0-r9
                first_reg = line[first_val_pos + 1] - '0';

                // Parse the second value. It could be a register or float.
                for (int i = first_val_pos + 3; line[i] != '\0'; i++) {
                        // Find first non-whitespace character.
                        if (second_val_pos == -1) {
                                if (line[i] <= ' ') {
                                        continue;
                                }
                                second_val_pos = i;
                        }
                }
                if (second_val_pos == -1) {
                        printf("ERROR: Could not find second value: %s\n", line);
                        continue;
                }
                if (line[second_val_pos] == 'r') {
                        // Only support registers from r0-r9
                        second_reg = line[second_val_pos + 1] - '0';
                } else {
                        // Assume this is a float number
                        char *end_ptr;
                        second_num = strtof(&line[second_val_pos], &end_ptr);

                        // if (*end_ptr != '\0') {
                        //         printf("ERROR: Non-numeric characters found in second value: %s\n", end_ptr);
                        //         continue;
                        // }
                }

                // Execute the command!
                switch (cmd) {
                        case SET:
                                if (second_reg == -1) {
                                        register_file[first_reg] = second_num;
                                } else {
                                        register_file[first_reg] = register_file[second_reg];
                                }
                                break;
                        case ADD:
                                if (second_reg == -1) {
                                        register_file[first_reg] += second_num;
                                } else {
                                        register_file[first_reg] += register_file[second_reg];
                                }
                                break;
                        case SUB:
                                if (second_reg == -1) {
                                        register_file[first_reg] -= second_num;
                                } else {
                                        register_file[first_reg] -= register_file[second_reg];
                                }
                                break;
                        case MUL:
                                if (second_reg == -1) {
                                        register_file[first_reg] *= second_num;
                                } else {
                                        register_file[first_reg] *= register_file[second_reg];
                                }
                                break;
                        case DIV:
                                if (second_reg == -1) {
                                        register_file[first_reg] /= second_num;
                                } else {
                                        register_file[first_reg] /= register_file[second_reg];
                                }
                                break;
                        default:
                                printf("Cannot execute unknown command");
                                break;
                }
        }

        free(line);
        fclose(file);

        printf("Final registers:\n");
        for (int i = 0; i < REGISTER_FILE_SIZE; i++) {
                printf("r%d: %.2f\n", i, register_file[i]);
        }

        return 0;
}
