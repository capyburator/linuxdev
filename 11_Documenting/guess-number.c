#include <stdio.h>
#include <libintl.h>
#include <locale.h>
#include <strings.h>
#include <string.h>
#include <getopt.h>

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
 * @brief Check whether string `str` starts with specified prefix `prefix`.
 * 
 * @param str       string
 * @param prefix    prefix
 * 
 * @retval          1 if string starts with prefix
 * @retval          0 otherwise
 */
int
starts_with(const char *str, const char *prefix) {
    const char *s, *p;
    for (s = str, p = prefix; *s && *s == *p; s++, p++) {}

    return *p == '\0';
}

/**
 * @brief Replaces all the occurrencese of `old` with `new` in a string `str`.
 * 
 * @param str       string
 * @param old       substring to replace
 * @param new       new value of replacing substring
 */
void
replace(
    char *str,
    const char *old,
    const char *new
) {
    static char buf[RIGHT + 1] = { 0 };

    char *head = str;
    size_t copy_size = 0;
    size_t old_len = strlen(old);
    size_t new_len = strlen(new);
    const char *p = str;
    char *q = buf;

    while (p = strstr(str, old), p)
    {
        copy_size = p - str;
        memmove(q, str, copy_size);
        memmove(q + copy_size, new, new_len);
        q += copy_size + new_len;
        str += copy_size + old_len;
    }

    strcpy(q, str);
    strcpy(head, buf);
}

/**
 * @brief Convert decimal number to roman.
 * 
 * @param num       number
 * @param res       buffer to store the result
 * 
 * @retval          0 on success
 * @retval          -1 if a given number cannot be converted to roman      
 */
int
to_roman(int num, char *res) {
    if (num < 0 || num > RIGHT) {
        return -1;
    }

    char s[num + 1];
    memset(s, 'I', num);
    s[num] = '\0';

    replace(s, "IIIII", "V");
    replace(s, "VV", "X");
    replace(s, "XXXXX", "L");
    replace(s, "LL", "C");
    replace(s, "CCCCC", "D");
    replace(s, "DD", "M");

    replace(s, "DCCCC", "CM");
    replace(s, "CCCC", "CD");
    replace(s, "LXXXX", "XC");
    replace(s, "XXXX", "XL");
    replace(s, "VIIII", "IX");
    replace(s, "IIII", "IV");

    strcpy(res, s);

    return 0;
}


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
