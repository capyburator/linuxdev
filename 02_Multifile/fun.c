#include <stdio.h>
#include "outlib.h"

void
output(const char *str) {
    printf("%d: %s\012", COUNT++, str);
}

void
usage(const char *prog) {
    fprintf(
        stderr,
        "%s v%.2f: Print all arguments\012\t"\
        "Usage: %s arg1 [arg2 […]]\012",
        prog,
        VERSION,
        prog
    );
}
