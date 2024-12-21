#include <check.h>

#include "roman.h"

#suite Main
#tcase ToRoman
#test convertible
    char buf[64] = { 0 };

    ck_assert_int_eq(to_roman(3, buf), 0);
    ck_assert_str_eq(buf, "III");

    ck_assert_int_eq(to_roman(9, buf), 0);
    ck_assert_str_eq(buf, "IX");

    ck_assert_int_eq(to_roman(42, buf), 0);
    ck_assert_str_eq(buf, "XLII");

    ck_assert_int_eq(to_roman(195, buf), 0);
    ck_assert_str_eq(buf, "CXCV");

    ck_assert_int_eq(to_roman(999, buf), 0);
    ck_assert_str_eq(buf, "CMXCIX");

    ck_assert_int_eq(to_roman(1000, buf), 0);
    ck_assert_str_eq(buf, "M");

    ck_assert_int_eq(to_roman(3999, buf), 0);
    ck_assert_str_eq(buf, "MMMCMXCIX");
    

#test inconvertible
    char buf[64] = { 0 };

    ck_assert_int_lt(to_roman(0, buf), 0);
    ck_assert_int_lt(to_roman(-1, buf), 0);
    ck_assert_int_lt(to_roman(4000, buf), 0);
