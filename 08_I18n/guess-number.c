#include <stdio.h>
#include <libintl.h>
#include <locale.h>
#include <strings.h>

#include "config.h"

#define _(STR) gettext(STR)

enum {
    LEFT = 1,
    RIGHT = 8
};

int
starts_with(const char *str, const char *prefix) {
    const char *s, *p;
    for (s = str, p = prefix; *s && *s == *p; s++, p++) {}

    return *p == '\0';
}

int
main() {
    char input[256] = { 0 };
    int mid;
    int left = LEFT - 1, right = RIGHT;
    int yes = 0;

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, ".");
    textdomain(PACKAGE);

    printf(_("Pick a number within [%d, %d]\n"), LEFT, RIGHT);

    while (left + 1 < right) {
        mid = (left + right) / 2;
        yes = 0;

        printf(_("Is your number greater than %d? [y/n]: "), mid);
        fflush(stdout);

        while (
            fgets(input, sizeof(input), stdin),
            yes = starts_with(input, _("y")),
            !yes && !starts_with(input, _("n"))
        ) {
            printf(_("Type `yes` or `no`: "));
            fflush(stdout);
        }

        yes ? (left = mid) : (right = mid);
    }

    printf(_("Your number is %d.\n"), left + 1);
}
