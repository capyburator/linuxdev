#include <stdio.h>
#include "outlib.h"

int
main(int argc, char **argv) {
    COUNT = argc;

    if (COUNT > 1) {
        output("<INIT>");

        for (int i = 1; i < argc; i++) {
            output(argv[i]);
        }

        output("<DONE>");
    } else {
        usage(argv[0]);
    }
}
