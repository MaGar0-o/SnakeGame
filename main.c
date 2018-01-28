

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define CELL_SIZE 10
#define BODY_RADIUS 9
#define FOOD_RADIUS 4
#define EYE_RADIUS 1
#define MAX_MAP_SIZE 100

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    EMPTY,
    FOOD,
    BLOCK
} Cell;

typedef struct {
    int x, y;
} Pair;

int HEIGHT, WIDTH;
Cell map[MAX_MAP_SIZE][MAX_MAP_SIZE];
Pair head, tail;

void generate_food(void) {
    int freecnt = 0;
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++)
            freecnt += map[i][j] == EMPTY;
    assert(freecnt != 0);
    int which = rand() % freecnt;
    for (int i = 0; i < HEIGHT; i++)
        for (int j = 0; j < WIDTH; j++) {
            if (map[i][j] == EMPTY)
                which--;
            if (which == -1) {
                map[i][j] = FOOD;
                return;
            }
        }
}

void load_map(int argc, char *argv[]) {
    FILE *input = stdin;
    if (argc > 1)
        input = fopen(argv[1], "r");
    bool must_load_default_map = (bool) feof(input);
    if (must_load_default_map) {
        HEIGHT = 10;
        WIDTH = 10;
        for (int i = 0; i < HEIGHT; i++)
            for (int j = 0; j < WIDTH; j++)
                map[i][j] = EMPTY;
        map[0][0] = RIGHT;
        head.x = head.y = tail.x = tail.y = 0;
        generate_food();
        return;
    }
    fscanf(input, "%d %d\n", &HEIGHT, &WIDTH);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            char c;
            fscanf(input, "%c", &c);
            if (c == '#')
                map[i][j] = BLOCK;
            else if (c == '.')
                map[i][j] = EMPTY;
            else if (c == '@') {
                map[i][j] = RIGHT;
                head.x = tail.x = i;
                head.y = tail.y = j;
            } else
                map[i][j] = FOOD;
        }
        if (i != HEIGHT - 1)
            fscanf(input, "\n");
    }
}

int move_x(int pos, Cell state) {
    switch (state) {
        case LEFT:
            return pos;
        case RIGHT:
            return pos;
        case UP:
            return (pos + HEIGHT - 1) % HEIGHT;
        case DOWN:
            return (pos + 1) % HEIGHT;
        default:
            assert(0);
    }
}

int move_y(int pos, Cell state) {
    switch (state) {
        case UP:
            return pos;
        case DOWN:
            return pos;
        case LEFT:
            return (pos + WIDTH - 1) % WIDTH;
        case RIGHT:
            return (pos + 1) % WIDTH;
        default:
            assert(0);
    }
}

int move_forward() {
    Pair nhead;
    nhead.x = move_x(head.x, map[head.x][head.y]);
    nhead.y = move_y(head.y, map[head.x][head.y]);

    Pair ntail;
    ntail.x = move_x(tail.x, map[tail.x][tail.y]);
    ntail.y = move_y(tail.y, map[tail.x][tail.y]);

    switch (map[nhead.x][nhead.y]) {
        case FOOD:
            map[nhead.x][nhead.y] = map[head.x][head.y];
            generate_food();

            head.x = nhead.x;
            head.y = nhead.y;
            return 0;
        case EMPTY:
            map[nhead.x][nhead.y] = map[head.x][head.y];
            map[tail.x][tail.y] = EMPTY;

            head.x = nhead.x;
            head.y = nhead.y;

            tail.x = ntail.x;
            tail.y = ntail.y;
            return 0;
        default:
            return 1;
    }
}

int main(int argc, char *argv[]) {
    load_map(argc, argv);
    while (true) {
        if (move_forward())
            break;
        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++)
                printf("%d", (int) map[i][j]);
            printf("\n");
        }
        printf("\n");
        for (int i = 0; i < 1000000000; i++);
    }
    return 0;
}