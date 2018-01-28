

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "SDL2_gfxPrimitives.h"

#define CELL_SIZE 50
#define BODY_RADIUS 25
#define FOOD_RADIUS 12
#define EYE_RADIUS 5
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
    bool must_load_default_map = true || (bool) feof(input);
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

int move_y(int pos, Cell state) {
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

int move_x(int pos, Cell state) {
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

    //init sdl
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init Error: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("SDL2_gfx test", 100, 100, WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE,
                                          SDL_WINDOW_OPENGL);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s", SDL_GetError());
        SDL_Quit();
        return 2;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s", SDL_GetError());
        SDL_Quit();
        return 3;
    }

    SDL_Event e;

    int quit = 0;
    while (!quit) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = 1;
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        if (map[head.x][head.y] != DOWN)
                            map[head.x][head.y] = UP;
                        break;
                    case SDLK_DOWN:
                        if (map[head.x][head.y] != UP)
                            map[head.x][head.y] = DOWN;
                        break;
                    case SDLK_LEFT:
                        if (map[head.x][head.y] != RIGHT)
                            map[head.x][head.y] = LEFT;
                        break;
                    case SDLK_RIGHT:
                        if (map[head.x][head.y] != LEFT)
                            map[head.x][head.y] = RIGHT;
                        break;
                }
            }

        }

        if (move_forward()) {
            SDL_RenderClear(renderer);
            SDL_Delay(3000);
            break;
        }
        SDL_SetRenderDrawColor(renderer, 10, 10, 10, 0);
        SDL_RenderClear(renderer);

        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                switch (map[i][j]) {
                    case FOOD:
                        filledCircleColor(renderer, i * CELL_SIZE + CELL_SIZE / 2, j * CELL_SIZE + CELL_SIZE / 2,
                                          FOOD_RADIUS, 0xFF0000FF);
                        break;
                    case EMPTY:
                        break;
                    default:
                        filledCircleColor(renderer, i * CELL_SIZE + CELL_SIZE / 2, j * CELL_SIZE + CELL_SIZE / 2,
                                          BODY_RADIUS, 0xFF80FF00);
                }
            }
        }

        //for (int i = 0; i < 1000000000; i++);
        SDL_RenderPresent(renderer);
        SDL_Delay(100);
    }

    //quit sdl
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}