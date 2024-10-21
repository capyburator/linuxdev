#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>

#ifndef unlikely
#define unlikely(expr)  __builtin_expect(!!(expr), 0)
#endif


enum {
    MAX_REFS = 100,
    ERR_BUF_LEN = 64
};


typedef enum {
    SUBST_SUCCESS,
    E_INVAL_REF,
    E_REF_LIMIT_EXCEEDED,
    E_OUT_OF_MEM
} SubstResult;


typedef struct {
    char *inner;
    size_t len;
    size_t capacity;
} String;


typedef struct {
    const char *str;
    size_t start;
    size_t end;
} StrSlice;


typedef struct {
    const regmatch_t *caps;
    size_t ncaps;
} Captures;


typedef struct {
    size_t cap_index;
    size_t start;
    size_t end;
} Ref;


int
string_with_capacity(String *const this, size_t capacity) {
    this->len = 0;
    this->capacity = capacity;
    this->inner = calloc(this->capacity, sizeof(*this->inner));

    return -!this->inner;
}

int
string_push_slice(String *const this, const StrSlice *slice) {
    size_t slice_size = slice->end - slice->start;

    if (this->len + slice_size + 1 >= this->capacity) {
        this->capacity *= 2;
        this->inner = realloc(this->inner, this->capacity * sizeof(*this->inner));
        if (!this->inner) {
            return -1;
        }
    }

    memmove(&this->inner[this->len], &slice->str[slice->start], slice_size);
    this->len += slice_size;

    return 0;
}

void
string_shrink_to_fit(String *const this) {
    this->capacity = this->len;
    this->inner = realloc(this->inner, this->capacity * sizeof(*this->inner));
}

ssize_t
find_refs(const char *subst, Ref *const refs, size_t max_refs) {
    size_t nrefs = 0;
    size_t len = strlen(subst);

    for (size_t i = 0; i < len; i++) {
        if (subst[i] == '\\' && isdigit(subst[i + 1])) {
            if (nrefs > max_refs) {
                return -1;
            }

            refs[nrefs++] = (Ref) {
                .cap_index = subst[i + 1] - '0',
                .start = i,
                .end = i + 2,
            };
        }
    }

    return nrefs;
}

int
verify_refs(const Ref *refs, size_t nrefs, size_t ncaps) {
    for (size_t i = 0; i < nrefs; i++) {
        if (refs[i].cap_index >= ncaps) {
            return -1;
        }
    }

    return 0;
}

SubstResult
subst(
    const Captures *captures,
    const char *subst,
    const char *str,
    String *const result
) {
    regmatch_t main_cap = captures->caps[0];
    Ref refs[MAX_REFS] = { 0 };
    ssize_t nrefs;
    size_t index = 0;

    if (nrefs = find_refs(subst, refs, MAX_REFS), nrefs < 0) {
        return E_REF_LIMIT_EXCEEDED;
    }

    if (verify_refs(refs, nrefs, captures->ncaps) < 0) {
        return E_INVAL_REF;
    }

    if (string_push_slice(result, &(StrSlice) { str, 0, main_cap.rm_so }) < 0) {
        return E_OUT_OF_MEM;
    }

    for (ssize_t i = 0; i < nrefs; i++) {
        Ref ref = refs[i];
        regmatch_t cap = captures->caps[ref.cap_index];

        if (string_push_slice(result, &(StrSlice) { subst, index, ref.start }) < 0
            || string_push_slice(result, &(StrSlice) { str, cap.rm_so, cap.rm_eo }) < 0
        ) {
            return E_OUT_OF_MEM;
        }

        index = ref.end;
    }

    if (string_push_slice(result, &(StrSlice) { subst, index, strlen(subst) }) < 0
        || string_push_slice(result, &(StrSlice) { str, main_cap.rm_eo, strlen(str)}) < 0
    ) {
        return E_OUT_OF_MEM;
    }

    result->inner[result->len++] = '\0';
    string_shrink_to_fit(result);

    return SUBST_SUCCESS;
}

void
remove_double_bslashes(char *str) {
    char *p = str;
    char *q = str;

    while (*q) {
        *p++ = *q++;
        q += q[0] == '\\' && q[1] == '\\';
    }

    *p = '\0';
}

const char *
substerror(SubstResult this) {
    static const char *names[] = {
        [SUBST_SUCCESS] = "substitute success",
        [E_INVAL_REF] = "invalid reference",
        [E_REF_LIMIT_EXCEEDED] = "reference limit exceeded",
        [E_OUT_OF_MEM] = "memory out of bounds"
    };

    return names[this];
}

int
main(int argc, char **argv) {
    String string;
    SubstResult res;
    size_t ncaps;
    regex_t re;
    regmatch_t *caps = NULL;
    char ebuf[ERR_BUF_LEN + 1] = { 0 };
    int status = EXIT_SUCCESS;
    int r;

    if (argc < 4) {
        fputs("USAGE: ./esub <regexp> <pattern> <text>\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (r = regcomp(&re, argv[1], REG_EXTENDED), r != REG_NOERROR) {
        regerror(r, &re, ebuf, ERR_BUF_LEN);
        fprintf(stderr, "Compile error: %s\n", ebuf);
        exit(EXIT_FAILURE);
    }

    ncaps = re.re_nsub + 1;

    if (caps = calloc(ncaps, sizeof(*caps)), caps == NULL) {
        perror("calloc");
        status = EXIT_FAILURE;
        goto finilize;
    }

    if (r = regexec(&re, argv[3], ncaps, caps, 0), r == REG_NOMATCH) {
        puts(argv[3]);
        goto finilize;
    }

    if (r != REG_NOERROR) {
        regerror(r, &re, ebuf, ERR_BUF_LEN);
        fprintf(stderr, "Match error: %s\n", ebuf);
        status = EXIT_FAILURE;
        goto finilize;
    }

    if (string_with_capacity(&string, 64) < 0) {
        perror("calloc");
        status = EXIT_FAILURE;
        goto finilize;
    }

    remove_double_bslashes(argv[2]);
    res = subst(&(Captures) { .caps = caps, .ncaps = ncaps }, argv[2], argv[3], &string);

    if (res != SUBST_SUCCESS) {
        fprintf(stderr, "Substitution error: %s\n", substerror(res));

        if (res == E_OUT_OF_MEM) {
            abort();
        }

        status = EXIT_FAILURE;
        free(string.inner);
        goto finilize;
    }

    puts(string.inner);
    free(string.inner);

finilize:
    regfree(&re);
    free(caps);
    exit(status);
}
