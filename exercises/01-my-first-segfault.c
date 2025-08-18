////////////////////////////////////////////////////////////////////////////////
// My First Segfault
////////////////////////////////////////////////////////////////////////////////
// Task:
// * Something is wrong with y_ptr. Fix it so that it points to y instead of NULL.
//
// See https://github.com/hintron/nando-lang/blob/main/docs/01-my-first-segfault.md
// for more background information on segfaults.
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>

// Define a "family" struct called family_t (t == type)
// We need to forward declare the struct first, so that it can reference itself.
typedef struct family family_t;
struct family {
    char *people[10]; // Array of people, max 10 people. father is 0, mother is 1
    family_t *mothers_family;
    family_t *fathers_family;
};

void print_family(family_t *family) {
    // NOTE: If family is 0, then family->people will cause a segfault
    if (family->people[2]) {
        printf("  - Father: %s\n", family->people[0]);
        printf("  - Mother: %s\n", family->people[1]);
    } else if (family->people[0]) {
        printf("  - Bachelor: %s\n", family->people[0]);
    } else if (family->people[1]) {
        printf("  - Bachelorette: %s\n", family->people[1]);
    } else {
        printf("ERROR: Unknown family configuration");
        return;
    }

    // Print any children
    for (int i = 2; family->people[i] != 0; i++) {
        printf("    * Child %d: %s\n", i - 1, family->people[i]);
    }
}

int main() {
    family_t house_atkins = {}; // Use {} to zero-initialize the struct
    house_atkins.people[0] = "Atkins"; // Father
    house_atkins.people[1] = "Alice"; // Mother
    house_atkins.people[2] = "Bob"; // Child
    house_atkins.people[1] = "Bernice"; // Child
    house_atkins.people[2] = "Dave"; // Child

    family_t house_dave = {}; // Use {} to zero-initialize the struct
    house_dave.people[0] = "Dave"; // Father
    house_dave.people[1] = "Alice"; // Mother
    house_dave.people[2] = "Bob"; // Child
    house_dave.people[1] = "Emily"; // Child
    house_dave.people[2] = "Freddy"; // Child
    house_dave.people[3] = "Mary"; // Child

    family_t house_freddy = {}; // Use {} to zero-initialize the struct
    house_freddy.people[0] = "Freddy"; // Father
    house_freddy.people[1] = "Gina"; // Mother
    house_freddy.people[2] = "Hank"; // Child
    house_freddy.people[3] = "Ivy"; // Child
    house_freddy.people[4] = "Jill"; // Child

    family_t house_zion = {}; // Use {} to zero-initialize the struct
    house_zion.people[0] = "Zion"; // Father
    house_zion.people[1] = "Yvette"; // Mother
    house_zion.people[2] = "Jack"; // Child

    family_t house_jill = {}; // Use {} to zero-initialize the struct
    house_jill.people[1] = "Jill";

    // The connections in the family tree are missing!
    // We are trying to print out Jill's line up to her great grandfather, Atkins.
    // Jill is the daughter of Freddy.
    // Freddy is the son of Dave.
    // Dave is the son of Atkins.
    // Set mothers_family and fathers_family with addresses of the correct families.

    ////////////////////////////////////////////////////////////////////////////
    // TODO: fix the program in the section below
    ///////////////////////////////--START--////////////////////////////////////



    ///////////////////////////////--END--//////////////////////////////////////
    printf("Jill's family:\n");
    print_family(&house_jill);
    printf("Jill's father's family:\n");
    print_family(house_jill.mothers_family);
    printf("Jill's father's father's family:\n");
    print_family(house_jill.mothers_family->fathers_family);
    printf("Jill's father's father's father's family:\n");
    print_family(house_jill.mothers_family->fathers_family->fathers_family);
    return 0;
}
