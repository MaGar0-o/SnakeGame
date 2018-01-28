

#define WIDTH 720
#define HEIGHT 480
#define CELL_SIZE 10
#define BODY_RADIUS 9
#define FOOD_RADIUS 4
#define EYE_RADIUS 1

typedef enum {
    UP,
    RIGHT,
    DOWN,
    LEFT,
    EMPTY,
    FOOD
} Cell;

Cell map[HEIGHT / CELL_SIZE][WIDTH / CELL_SIZE];

int main(int argc, char *argv[]) {

    return 0;
}