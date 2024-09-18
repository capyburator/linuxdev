#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <ncurses.h>

enum ShowConfig {
    /// `x` coordinate of left up corner of the window.
    START_X = 3,

    /// `y` coordinate of left up corner of the window.
    START_Y = 3,

    /// Number of digits reserved for a line number.
    LINE_NUM_NDIGITS = 4,

    /// Number of characters reserved for displaying a line number.
    LINE_NUM_PREFIX = LINE_NUM_NDIGITS + sizeof(" ") - 1,
};

/// Handle to opened file.
typedef struct FileHandle {
    /// File descriptor.
    int fd;

    /// File size in bytes.
    size_t size;
} FileHandle;

/// Context of mapped file.
typedef struct MapFileCtx {
    /// File content.
    void* ptr;

    /// File size rounded up to page size.
    size_t len;
} MapFileCtx;

/// Lines of an ASCII-string.
typedef struct Lines {
    /// Array of zero-terminated lines.
    char **inner;

    /// The greatest len.
    size_t max_len;

    /// Number of lines.
    size_t size;

    /// A flag whether last line must be freed.
    /// See `new_lines` for more details.
    int free_last_line;
} Lines;

/// Slice of an instance of `Lines`.
typedef struct LinesSlice {
    /// Assosiated instance of `Lines`.
    const Lines *const lines;

    /// Index of first line.
    size_t line_start;

    /// Index of first character of each line.
    size_t col_start;
} LinesSlice;

/// Exit process with custom message and exit code set to `EXIT_FAILURE`.
/// This function never returns.
void
fatal(const char *fmt, ...);

/// Open regular file in read and write mode.
/// Return handle to opened file if it is regular.
/// # Errors
/// - Panics if file does not exist or file is not regular.
/// - Panics if `fstat` fails to get its attributes.
FileHandle
open_regular_file(const char *path);

/// Map file contents to memory in private mode. File must be opened in read and write mode.
/// # Return value
/// - Return context of mapped file if file is not empty.
/// - For empty file default context is returned, i.e. `ptr` is set to `NULL` and `len` is zero.
/// - If file opened in not read and write mode, default context is returned.
/// # Errors
/// - Panics if `mmap` fails to map file while file is not empty.
MapFileCtx
map_file(const FileHandle *handle);

/// Work-loop subroutine.
void
show_lines(const char *file_name, const Lines *lines);

/// Render lines according to provided slice.
void
render_slice(WINDOW *win, const LinesSlice *slice);

/// Create new `Lines` instance.
/// # Errors
/// - Panics if `calloc` fails.
Lines
new_lines(char *content, size_t content_size);

/// `Lines` destructor.
/// # Safety
/// This function assumes that `Lines` instance was provided by call of `new_lines`.
void
free_lines(Lines *const this);

int
main(int argc, char **argv) {
    // Check whether path to file is provided.
    if (argc == 1) {
        fatal(
            "Expected 1 argument: <FILE PATH>, but got 0.\n"
            "USAGE: Show <FILE PATH>\n"
            "Note: provided file must be regular."
        );
    }

    // Open file and map its content to memory.
    FileHandle handle = open_regular_file(argv[1]);
    MapFileCtx ctx = map_file(&handle);

    // SAFETY: file can be closed immediately after `mmap` 
    // without invalidating the mapping.
    close(handle.fd);

    // Handle empty file.
    if (ctx.ptr == NULL) {
        exit(EXIT_SUCCESS);
    }

    Lines lines = new_lines(ctx.ptr, handle.size);

    // Run work-loop subroutine.
    show_lines(argv[1], &lines);

    // Deallocate file mapping and instance of `Lines`.
    munmap(ctx.ptr, ctx.len);
    free_lines(&lines);
}

void
fatal(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "%s", "\n");
    exit(EXIT_FAILURE);
}

FileHandle
open_regular_file(const char *path) {
    struct stat stb;    /* stat buffer */
    int fd;             /* file descriptor */

    // Open file in read and write mode.
    fd = open(path, O_RDWR);
    if (fd < 0) {
        perror(path);
        exit(EXIT_FAILURE);
    }

    // Get attributes.
    if (fstat(fd, &stb) < 0) {
        fatal("[%s]: fstat failed to get attributes", path);
    }

    // Check whether file is regular.
    if (!S_ISREG(stb.st_mode)) {
        fatal("[%s]: file is not regular", path);
    }

    return (FileHandle) {
        .fd = fd,
        .size = stb.st_size,
    };
}

MapFileCtx
map_file(const FileHandle *handle) {
    void *p = NULL;     /* pointer to file contents */
    size_t pgs;         /* page size */
    size_t len;         /* file length rounded up to page size */

    // Handle empty file.
    // Note: mapping of empty file is UB.
    if (handle->size == 0) {
        return (MapFileCtx) { 0 };
    }

    // Check whether file is opened in read and write mode.
    if ((fcntl(handle->fd, F_GETFL) & O_RDWR) != O_RDWR) {
        return (MapFileCtx) { 0 };
    }

    // Round len up to page size.
    pgs = sysconf(_SC_PAGESIZE);
    len = ((handle->size - 1) / pgs + 1) * pgs;

    // SAFETY:
    // 1. We have already checked that file opened in read and write mode, so
    // `PROT_READ|PROT_WRITE` is correct;
    // 2. Mapping is private, so file content remains unchanged. 
    p = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_PRIVATE, handle->fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    return (MapFileCtx) {
        .ptr = p,
        .len = len
    };
}

Lines
new_lines(char *content, size_t content_size) {
    char **lines = NULL;        /* array of lines */
    size_t nlines = 0;          /* number of lines */
    size_t offset = 0;          /* length if current line */
    size_t max_len = 0;         /* the greatest len over all lines */
    size_t index = 0;           /* index in array of lines */
    size_t cur_len = 0;
    int free_last_line = 0;

    if (content == NULL) {
        return (Lines) { 0 };
    }

    // Count number of lines.
    for (size_t i = 0; i < content_size; i++) {
        nlines += (content[i] == '\n');
    }

    // Allocate memory for `nlines` and one more pointers,
    // since last line may not terminate with end-of-line.
    lines = calloc(nlines + 1, sizeof(*lines));
    if (lines == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    // Build array of pointers to zero-terminated strings
    // from continuous string by replacing end-of-lines with zero.
    for (size_t i = 0; i < content_size; i++) {
        if (content[i] == '\n') {
            max_len = max_len < cur_len ? cur_len : max_len;
            content[i] = '\0';
            lines[index++] = &content[offset];
            offset += cur_len + 1;
            cur_len = 0;
        } else {
            cur_len++;
        }
    }

    // Handle last line, since it may not end with end-of-line.
    if (offset < content_size) {
        size_t page_size = sysconf(_SC_PAGESIZE);

        if (content_size % page_size) {
            // File size in bytes is not a multiple of page size,
            // so the remaining bytes in the partial page at
            // the end of the mapping are zeroed.
            // See: NOTES of manual page for `mmap`.
            //
            // That is, we already have zero-terminated string.
            // Just push it in the array of lines.
            lines[nlines++] = &content[offset];
        } else {
            // File size in bytes is a multiple of page size,
            // so no partial page with zeroed bytes will be provided.
            // That is, we have last line which is not zero-terminated,
            // and there is no room after the last line to put zero byte.
            //
            // We need to duplicate not zero-terminated last line and
            // allocate one more byte to put zero. That is, in the instance's
            // destructor we have to free the manually-allocated last line,
            // so we set the appropriate flag `Lines.free_last_line`.
            char *last_line = calloc(cur_len + 1, sizeof(*last_line));
            memcpy(last_line, &content[offset], cur_len);
            lines[nlines++] = last_line;

            free_last_line = 1;
        }
    }

    return (Lines) {
        .inner = lines,
        .max_len = max_len,
        .size = nlines,
        .free_last_line = free_last_line
    };
}

void
free_lines(Lines *const this) {
    free(this->inner);

    if (this->free_last_line) {
        free(this->inner[this->size - 1]);
    }
}

int
is_quit_key(int key) {
    static const int quit_keys[] = { '\e', 'q', 'Q' };

    for (size_t i = 0; i < sizeof(quit_keys) / sizeof(quit_keys[0]); i++) {
        if (quit_keys[i] == key) {
            return 1;
        }
    }

    return 0;
}

void
show_lines(const char *file_name, const Lines *lines) {
    WINDOW *win = NULL;
    int key;

    LinesSlice slice = (LinesSlice) {
        .lines = lines,
        .line_start = 0,
        .col_start = 0,
    };

    initscr();
    noecho();
    cbreak();

    win = newwin(LINES - START_Y * 2, COLS - START_X * 2, START_Y, START_X);

    // Get screen's height and width without borders.
    size_t height = getmaxy(win) - 2;
    size_t width = getmaxx(win) - 2;

    keypad(win, TRUE);
    scrollok(win, TRUE);

    printw("File: %s; size: %" PRIuPTR " lines\n", file_name, lines->size);
    refresh();
    render_slice(win, &slice);

    while (key = wgetch(win), !is_quit_key(key)) {
        int render = 1;
        switch (key) {
            case ' ':
            case KEY_DOWN: slice.line_start += (slice.line_start + height < slice.lines->size); break;
            case KEY_UP: slice.line_start -= (slice.line_start > 0); break;
            case KEY_LEFT: slice.col_start -= (slice.col_start > 0); break;
            case KEY_RIGHT: slice.col_start += (slice.col_start + width < slice.lines->max_len + LINE_NUM_PREFIX); break;
            case KEY_PPAGE: slice.line_start = slice.line_start < height ? 0 : slice.line_start - height; break;
            case KEY_NPAGE: slice.line_start += height * (slice.line_start + height < slice.lines->size); break;
            default: render = 0; break;
        }
        if (render) {
            render_slice(win, &slice);
        }
    }

    delwin(win);
    endwin();
}

void
render_slice(WINDOW *win, const LinesSlice *slice) {
    werase(win);
    wmove(win, 1, 0);

    size_t line_end = slice->line_start + getmaxy(win) - 2;

    if (line_end > slice->lines->size) {
        line_end = slice->lines->size;
    }

    for (size_t i = slice->line_start; i < line_end; i++) {
        const char *line = slice->lines->inner[i];
        size_t len = strlen(line);
        int screen_width = getmaxx(win) - 2;

        // Leave a room for digits, one more for border and print line number.
        wprintw(win, "%*."PRIuPTR " ", LINE_NUM_NDIGITS + 1, i + 1);

        if (slice->col_start < len) {
            // Print slice of the current line.
            wprintw(win, "%.*s", screen_width - LINE_NUM_PREFIX, &line[slice->col_start]);
        }
        waddch(win, '\n');
    }

    box(win, 0, 0);
    wrefresh(win);
}
