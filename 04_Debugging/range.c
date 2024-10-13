#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>

#define PRIRange PRId64

enum {
    BASE = 10
};

typedef int64_t RangeInt;

typedef enum {
    PARSE_SUCCESS,
    E_START_INVAL,
    E_END_INVAL,
    E_STRIDE_INVAL,
    E_NO_END,
    E_STRIDE_ZERO,
} ParseRangeResult;

typedef struct {
    RangeInt start;
    RangeInt end;
    RangeInt stride;
    RangeInt value;
} Range;

int
parse_int(const char *str, RangeInt *res, int base) {
    char *end_ptr = NULL;
    errno = 0;

    int64_t value = strtoll(str, &end_ptr, base);
    if (errno || *end_ptr || end_ptr == str || (RangeInt) value != value) {
        return -1;
    }

    *res = value;
    return 0;
}

ParseRangeResult
parse_range(
    int argc,
    char *const *argv,
    Range *const range
) {
    range->start = 0;
    range->stride = 1;

    if (argc == 1) {
        range->end = 0;
        return E_NO_END;
    }

    argc = argc > 4 ? 4 : argc;

    RangeInt *ptrs[][3] = { 
        [1] = { &range->end, NULL, NULL },
        [2] = { &range->start, &range->end, NULL },
        [3] = { &range->start, &range->end, &range->stride }
    };

    RangeInt **p = ptrs[argc - 1];
    for (int i = 1; i < argc; i++) {
        if (parse_int(argv[i], p[i - 1], BASE) < 0) {
            return i;
        }
    }

    if (range->stride == 0) {
        return E_STRIDE_ZERO;
    }

    return PARSE_SUCCESS;
}

void
range_init(Range *const this) {
    this->value = this->start;
}

int
range_is_empty(const Range *const this) {
    return this->stride > 0 
        ? this->value >= this->end
        : this->value <= this->end;
}

RangeInt
range_get(const Range *const this) {
    return this->value;
}

void
range_next(Range *const this) {
    if (__builtin_add_overflow(this->value, this->stride, &this->value)) {
        this->value = this->end;
    }
}

const char *
parse_result_name(ParseRangeResult this) {
    static const char *names[] = {
        [PARSE_SUCCESS] = "success",
        [E_NO_END] = "no end",
        [E_START_INVAL] = "invalid start",
        [E_END_INVAL] = "invalid end",
        [E_STRIDE_INVAL] = "invalid start",
        [E_STRIDE_ZERO] = "stride is zero",
    };

    return names[this];
}

int
main(int argc, char **argv) {
    Range R;
    ParseRangeResult res;

    if (res = parse_range(argc, argv, &R), res != PARSE_SUCCESS) {
        fprintf(stderr, "Error parsing range: %s\n", parse_result_name(res));
        exit(EXIT_FAILURE);
    }

    for (range_init(&R); !range_is_empty(&R); range_next(&R)) {
        printf("%"PRIRange"\n", range_get(&R));
    }
}
