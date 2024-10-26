#include <string.h>
#include <dlfcn.h>

static const char *PROTECT = "PROTECT";

int remove(const char *filename) {
    int (*_remove)(const char *);

    if (strstr(filename, PROTECT)) {
        return 0;
    }

    if (_remove = dlsym(RTLD_NEXT, "remove"), !_remove) {
        return -1;
    }

    return _remove(filename);
}
