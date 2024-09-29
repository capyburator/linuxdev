#define _GNU_SOURCE

#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>

#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))

void
fatal(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "%s", "\n");
    exit(EXIT_FAILURE);
}

const char *
get_tmp_dir(const char *default_dir) {
    static const char *envs[] = { "XDG_RUNTIME_DIR", "TMPDIR" };
    char *dirname = NULL;

    for (size_t i = 0; i < LEN(envs); i++) {
        dirname = getenv(envs[i]);
        if (dirname) {
            return dirname;
        }
    }
    return default_dir;
}

char *
gen_name(const char *tmp_dir, const char *prefix, const char *ext) {
    unsigned long long randval;

    int rfd = open("/dev/urandom", O_RDONLY);
    if (rfd < 0) {
        perror("/dev/urandom");
        exit(EXIT_FAILURE);
    }

    if (read(rfd, &randval, sizeof(randval)) != sizeof(randval)) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    char *name;
    if (-1 == asprintf(&name, "%s/%s%llx.%s", tmp_dir, prefix, randval, ext)) {
        fatal("An error occurred: asprintf");
    }

    return name;
}

int
run_program(
    const char *program_name,
    const char *out_file,
    char *const *argv
) {
    int pid = fork();

    if (pid < 0) {
        return pid;
    }

    if (!pid) {
        if (out_file) {
            int fd = open(out_file, O_WRONLY|O_CREAT|O_CLOEXEC, 0666);
            if (fd == -1) {
                _exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
        }
        execv(program_name, argv);
        _exit(EXIT_FAILURE);
    }

    int status;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) && EXIT_SUCCESS == WEXITSTATUS(status);
}

void
free_out_files(char *const *out_files, size_t len) {
    for (size_t i = 0; i < len; i++) {
        unlink(out_files[i]);
        free(out_files[i]);
    }
}

int
main(int argc, char **argv) {
    static char *programs[] = { "prog", "prog-a", "prog-so" };
    char *out_files[LEN(programs)] = { 0 };
    (void) argc;

    const char *tmp_dir = get_tmp_dir("/tmp");

    // Sequentially run programs.
    for (size_t i = 0; i < LEN(programs); i++) {
        out_files[i] = gen_name(tmp_dir, programs[i], "txt");
        argv[0] = programs[i];
        int status = run_program(programs[i], out_files[i], argv);

        if (status < 0) {
            free_out_files(out_files, i + 1);
            fatal("Failed to run process `%s`", programs[i]);
        } else if (!status) {
            free_out_files(out_files, i + 1);
            fatal("Program `%s` exited with an error", programs[i]);
        }
    }

    // Compare results.
    for (size_t i = 0; i < LEN(out_files); i++) {
        char *lhs = out_files[i];
        char *rhs = out_files[(i + 1) % LEN(out_files)];
        char *cmp_argv[] = { "/bin/cmp", "--quiet", lhs, rhs, NULL };
        int status = run_program("/bin/cmp", NULL, cmp_argv);

        if (status < 0) {
            fatal("Failed to run `/bin/cmp` for programs `%s` and `%s`", lhs, rhs);
        }

        if (!status) {
            printf(
                "Programs `%s` and `%s` differ on %d args.\n",
                programs[i], 
                programs[(i + 1) % LEN(programs)], 
                argc - 1
            );
        }
    }

    free_out_files(out_files, LEN(programs));
}
