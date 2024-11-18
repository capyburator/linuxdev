#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum MazeConfig {
    LEN = 6,
    MAX_LEN = 64,
    MAX_SIZE = MAX_LEN * 2 + 1,
    WALL = '#',
    WAY = '.',
    SEED = 37,
    BASE = 10,
};

typedef enum CellState {
    VISITED,
    ISOLATED,
} CellState;

typedef struct Maze {
    char map[MAX_SIZE][MAX_SIZE];
    int size;
    unsigned seed;
    char wall;
    char way;
} Maze;

typedef struct Pos {
    int i;
    int j;
} Pos;


void
maze_new(
    int size,
    unsigned seed,
    char wall,
    char way,
    Maze *const this
) {
    this->size = size;
    this->seed = seed;
    this->wall = wall;
    this->way = way;

    for (int i = 0; i < this->size; i += 2) {
        memset(&this->map[i][0], this->wall, this->size);
    }

    for (int i = 1; i < this->size; i += 2) {
        for (int j = 1; j < this->size; j += 2) {
            this->map[i][j] = ISOLATED;
        }
        for (int j = 0; j < this->size; j += 2) {
            this->map[i][j] = this->wall;
        }
    }
}

void
maze_dump(const Maze *const this) {
    for (int i = 0; i < this->size; i++) {
        printf("%.*s\n", (int) this->size, this->map[i]);
    }
}

static inline int
maze_is_correct_pos(const Maze *const this, Pos p) {
    return p.i < this->size - 1 && p.j < this->size - 1
        && p.i > 0 && p.j > 0;
}

static inline int
sign(int x) {
    return (x > 0) - (x < 0);
}

void
maze_remove_wall(Maze *const this, Pos p, Pos dp) {
    this->map[p.i + sign(dp.i)][p.j + sign(dp.j)] = this->way;
}

void
shuffle(int arr[], int n) {
    for (int i = 0; i < n - 1; i++)  {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = arr[j];
        arr[j] = arr[i];
        arr[i] = t;
    }
}

void
maze_fill_at(Maze *const this, Pos p) {
    static const Pos dps[] = { { 2, 0 }, { -2, 0}, { 0, 2 }, { 0, -2 } };
    int perm[] = { 0, 1, 2, 3 };
    Pos dp, np;

    /* Mark current cell as visited */
    this->map[p.i][p.j] = VISITED;

    /* Generate random permutation of neighbours */
    shuffle(perm, sizeof(dps) / sizeof(dps[0]));

    /* DFS */
    for (size_t i = 0; i < sizeof(dps) / sizeof(dps[0]); i++) {
        dp = dps[perm[i]];
        np = (Pos) { p.i + dp.i, p.j + dp.j };

        if (
            maze_is_correct_pos(this, np)
            && this->map[np.i][np.j] == ISOLATED
        ) {
            maze_remove_wall(this, p, dp);
            maze_fill_at(this, np);
        }
    }

    /* Normalize visited cells to ways */
    for (int i = 0; i < this->size; i++) {
        for (int j = 0; j < this->size; j++) {
            if (this->map[i][j] == VISITED) {
                this->map[i][j] = this->way;
            }
        }
    }
}

void
maze_fill(Maze *const this) {
    srand(this->seed);
    maze_fill_at(this, (Pos) { 1, 1 });
}

int
main(int argc, char **argv) {
    int len = 0, size = 0;
    int seed = 0;
    char wall = 0, way = 0;
    Maze maze;

    (void) argc;
    (void) argv;

    len = LEN;
    size = len * 2 + 1;
    way = WAY;
    wall = WALL;
    seed = SEED;

    maze_new(size, seed, wall, way, &maze);
    maze_fill(&maze);
    maze_dump(&maze);
}
