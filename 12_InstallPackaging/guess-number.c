#include <stdio.h>
#include <libintl.h>
#include <locale.h>
#include <strings.h>
#include <string.h>
#include <getopt.h>

#include "roman.h"
#include "config.h"

/**
 * @brief Macro that expands to `gettext`.
 * 
 */
#define _(STR) gettext(STR)

enum {
    LEFT = 1,       /* Min number */
    RIGHT = 3999    /* Max mumber */
};


/**
 * @brief Print program's help info.
 * 
 */
void print_help() {
    puts(_("Program that guesses a number"));
    puts(_("USAGE: [LANGUAGE=ru_RU] ./guess-number [OPTIONS]"));
    puts(_("OPTIONS:"));
    puts(_("   -r, --roman         Print numbers in roman notation"));
    puts(_("   -h, --help          Print help"));
}


/**
 * @mainpage guess-number
 * Guess a number
 * 
 * USAGE: `guess-number` [OPTIONS] 
 * 
 * OPTIONS:
 *  -r, --roman         Print numbers in Roman notation
 *  -h, --help          Print help
 */
int
main(int argc, char **argv) {
    static struct option opts[] = {
        { .name = "roman", .has_arg = no_argument, .flag = NULL, .val = 'r' },
        { .name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h' },
        { 0 }
    };

    char roman[64] = { 0 };
    char input[256] = { 0 };
    int mid;
    int left = LEFT - 1, right = RIGHT;
    int yes = 0;
    int optind, c;
    int roman_flag = 0;

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, ".");
    textdomain(PACKAGE);

    while (c = getopt_long(argc, argv, "rh", opts, &optind), c != -1) {
        switch (c) {
            case 'r': roman_flag = 1; break;
            case 'h': print_help(); return 0;
            default: puts(_("Unrecognized option, try -h or --help")); return 0;
        }
    }

    if (roman_flag) {
        to_roman(LEFT, roman);
        printf(_("Pick a number within [%s, "), roman);
        to_roman(RIGHT, roman);
        printf(" %s]\n", roman);
    } else {
        printf(_("Pick a number within [%d, %d]\n"), LEFT, RIGHT);
    }

    while (left + 1 < right) {
        mid = (left + right) / 2;
        yes = 0;

        if (roman_flag) {
            to_roman(mid, roman);
            printf(_("Is your number greater than %s? [y/n]: "), roman);            
        } else {
            printf(_("Is your number greater than %d? [y/n]: "), mid);
        }
        fflush(stdout);

        while (
            (void) fgets(input, sizeof(input), stdin),
            yes = starts_with(input, _("y")),
            !yes && !starts_with(input, _("n"))
        ) {
            printf(_("Type `yes` or `no`: "));
            fflush(stdout);
        }

        yes ? (left = mid) : (right = mid);
    }

    if (roman_flag) {
        to_roman(left + 1, roman);
        printf(_("Your number is %s.\n"), roman);
    } else {
        printf(_("Your number is %d.\n"), left + 1);
    }
}
