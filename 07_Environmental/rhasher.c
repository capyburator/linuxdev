#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>

#include "rhash.h"

#ifdef _READLINE
#include <readline/readline.h>
#endif

int
hash_alg(const char *str, enum rhash_ids *id) {
    for (unsigned hash_id = RHASH_CRC32; hash_id != RHASH_BLAKE2B << 1; hash_id <<= 1) {
        if (!strcasecmp(str, rhash_get_name(hash_id))) {
            *id = hash_id;
            return 0;
        }
    }

    return -1;
}

int
read_command(char **command) {
    printf("> ");

#ifdef _READLINE
    *command = readline(NULL);
    return *command != NULL;
#else
    size_t n;
    return getline(command, &n, stdin);
#endif
}

int
main(void) {
    enum rhash_ids id = RHASH_MD5;
    char *command = NULL;
    char *name = NULL;
    char *arg = NULL;
    char *content;
    unsigned char digest[64] = { 0 };
    char output[130] = { 0 };
    int is_str = 0;
    size_t len;
    int r;

    while (read_command(&command) > 0) {
        if (name = strtok(command, " "), !name || hash_alg(name, &id) < 0) {
            fprintf(stderr, "Unknown hashing algorithm: %s\n", name);
            free(command);
            continue;
        }

        if (arg = strtok(NULL, " "), !arg) {
            fprintf(stderr, "The second parameter required: %s\n", name);
            free(command);
            continue;
        }

        len = strlen(arg);
        if (arg[len - 1] == '\n') {
            arg[len - 1] = '\0';
        }
        is_str = arg[0] == '"';
        content = is_str ? &arg[1] : arg;

        r = is_str
            ? rhash_msg(id, content, strlen(content), digest)
            : rhash_file(id, content, digest);

        if (r < 0) {
            if (is_str) {
                fprintf(stderr, "LibRHash: digest calculation error\n");
            } else {
                fprintf(stderr, "LibRHash error: %s: %s\n", content, strerror(errno));
            }
            free(command);
            continue;
        }

        rhash_print_bytes(
            output, digest, rhash_get_digest_size(id), 
            isupper(name[0]) ? RHPR_HEX : RHPR_BASE64
        );
        printf("%s\n", output);
        free(command);
    }
}
