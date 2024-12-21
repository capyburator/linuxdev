#include <stdlib.h>
#include <string.h>

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
    static char buf[4000] = { 0 };

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
    if (num <= 0 || num > 3999) {
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
