#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


typedef enum {
    MOVE_SUCCESS,
    E_INVAL_PARAMS,
    E_INVAL_SRC,
    E_INVAL_DEST,
    E_READ_FAIL,
    E_WRITE_FAIL,
} MoveResult;


const char *
moveerror(MoveResult this) {
    static const char *names[] = {
        [MOVE_SUCCESS] = "move success",
        [E_INVAL_PARAMS] = "invalid parameters",
        [E_INVAL_SRC] = "invalid source file",
        [E_INVAL_DEST] = "invalid destination file",
        [E_READ_FAIL] = "failed to read from file",
        [E_WRITE_FAIL] = "failed to write to file"
    };

    return names[this];
}

int
write_to_file(int fd, const char *buf, ssize_t size) {
    ssize_t r;

    while (r = write(fd, buf, size), r > 0) {
        if (r == size) {
            return 0;
        }
        buf += r;
        size -= r;
    }

    return -1;
}

MoveResult
move(int wfd, int rfd) {
    char buf[4096] = { 0 };
    ssize_t r;

    while (r = read(rfd, buf, sizeof(buf)), r > 0) {
        if (write_to_file(wfd, buf, r) < 0) {
            return E_WRITE_FAIL;
        }
    }

    return r < 0 ? E_READ_FAIL : MOVE_SUCCESS;
}

int
main(int argc, char **argv) {
    MoveResult res;
    const char *in, *out;
    int wfd, rfd;

    if (argc <= 2) {
        fputs("USAGE: ./move <infile> <outfile>\n", stderr);
        exit(E_INVAL_PARAMS);
    }

    in = argv[1];
    out = argv[2];

    if (rfd = open(in, O_RDONLY), rfd < 0) {
        perror(in);
        exit(E_INVAL_SRC);
    }

    if (wfd = open(out, O_WRONLY|O_CREAT|O_TRUNC, 0666), wfd < 0) {
        perror(out);
        close(rfd);
        exit(E_INVAL_DEST);
    }

    res = move(wfd, rfd);

    close(rfd);
    close(wfd);

    if (res != MOVE_SUCCESS) {
        fprintf(stderr, "Move error: %s\n", moveerror(res));
        remove(out);
        exit(res);
    }

    remove(in);
}
