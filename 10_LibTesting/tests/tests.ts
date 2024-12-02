#include <check.h>

#include "buf.h"


#suite Main
#tcase InitClearFree
#test init_test
    float *a = NULL;
    ck_assert_int_eq(buf_capacity(a), 0);
    ck_assert_int_eq(buf_size(a), 0);

#test push_then_clear_test
    float *a = NULL;
    buf_push(a, 1.3f);
    ck_assert_int_eq(buf_size(a), 1);
    ck_assert_float_eq(a[0], 1.3f);

    buf_clear(a);

    ck_assert_int_eq(buf_size(a), 0);
    ck_assert_ptr_nonnull(a);

    buf_free(a);

    ck_assert_ptr_null(a);

#test clear_null_test
    float *a = NULL;
    buf_clear(a);
    ck_assert_int_eq(buf_size(a), 0);
    ck_assert_ptr_null(a);

#tcase PushIndex
#test push_test
    long *ai = NULL;
    int match = 0;

    for (int i = 0; i < 10000; i++) {
        buf_push(ai, i);
    }

    for (int i = 0; i < 10000; i++) {
        match += ai[i] == i;
    }

    ck_assert_int_eq(match, 10000);
    buf_free(ai);

#tcase GrowTrunc
#test grow_test
    long *ai = NULL;
    buf_grow(ai, 1000);
    ck_assert_int_eq(buf_capacity(ai), 1000);
    ck_assert_int_eq(buf_size(ai), 0);
    buf_free(ai);

#test trunc_test
    long *ai = NULL;
    buf_grow(ai, 1000);
    buf_trunc(ai, 100);
    ck_assert_int_eq(buf_capacity(ai), 100);
    buf_free(ai);

#tcase Pop
#test pop_test
    float *a = NULL;
    float values[] = { 1.1, 1.2, 1.3, 1.4 };
    int len = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < len; i++) {
        buf_push(a, values[i]);
    }

    ck_assert_int_eq(buf_size(a), len);
    ck_assert_float_eq(buf_pop(a), values[len - 1]);

    buf_trunc(a, len - 1);
    for (int i = len - 2; i >= 0; i--) {
        ck_assert_float_eq(buf_pop(a), values[i]);
    }

    ck_assert_int_eq(buf_size(a), 0);
    buf_free(a);
