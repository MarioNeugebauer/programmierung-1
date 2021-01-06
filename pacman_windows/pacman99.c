#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#define x_dim 80
#define y_dim 25

enum pacman_keycode {
    NONE,
    KEY_UP,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_DOWN,
    KEY_p
};

enum move_direction {
    UP,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
    DOWN,
    DOWN_LEFT,
    LEFT,
    UP_LEFT
};

typedef struct game_object {
    int x;
    int y;
    int auto_update_intervall_ms;
    long last_update_time_ms;
    bool updated;
} game_object_t;

typedef long long my_time_t;

const int x_field_min = 16;
const int x_field_max = 46;

enum pacman_keycode pressed_key;

char screen_memory[x_dim][y_dim];
char obstacles[x_dim][y_dim];
char eat_pellets[x_dim][y_dim];
int screen_updated;
int key_pressed;
int game_points=0;

void print_screen_memory_to_cli();
void clear_screen_memory(char empty_char);
void make_dot(int x, int y, char dot_char);
void init_obstacles();
void init_eat_pellets();
enum pacman_keycode get_pressed_key();

game_object_t move_game_object(enum move_direction direction, int eat_pellets, game_object_t game_object);
game_object_t update_ghost_position(game_object_t ghost);
int collision_with_obstacle(int x_obj, int y_obj);
void collect_eat_pellets(int x, int y);
bool game_over(game_object_t pacman, game_object_t ghost);

long getMillis_sinceMidnight();

int main(void) {

    // struct for the position of the pacman
    game_object_t pacman;
    // initial position of the pacman
    pacman.x = 16;
    pacman.y = 11;

    // struct for the position of the ghost
    game_object_t ghost;
    // initial position of ghost
    ghost.x = 40;
    ghost.y = 11;
    ghost.auto_update_intervall_ms = 1000;

    ghost.last_update_time_ms = getMillis_sinceMidnight();

    init_obstacles();
    init_eat_pellets();

    clear_screen_memory(' ');
    make_dot(pacman.x, pacman.y, 'O');
    print_screen_memory_to_cli();

    // printf("please press P key to pause \n ");

    pressed_key = NONE;

    while (1)
    {
        // call function to determine the pressed key
        pressed_key = get_pressed_key();

        // therefore ask the keybuffer with _getch a second time
        if (pressed_key == KEY_UP) { 
            pacman = move_game_object(UP, 1, pacman);
        }
        else if (pressed_key == KEY_DOWN) { // cursor key DOWN
            pacman = move_game_object(DOWN, 1, pacman);
        }
        else if (pressed_key == KEY_LEFT) { // cursor key LEFT
            pacman = move_game_object(LEFT, 1, pacman);
        }
        else if (pressed_key == KEY_RIGHT) { // cursor key RIGHT
            pacman = move_game_object(RIGHT, 1, pacman);
        }
        else if (pressed_key == KEY_p) {
            // leave the while loop and therewith terminate the program
            break;
        }

        ghost = update_ghost_position(ghost);

        if (game_over(pacman, ghost)) {
            printf("\n*** Game Over ***\n");
            break;
        }

        if (pressed_key!=NONE || ghost.updated) {
            clear_screen_memory(' ');
            make_dot(pacman.x, pacman.y, 'O');
            make_dot(ghost.x, ghost.y, 'A');
            print_screen_memory_to_cli();
            printf("\n.. %i\n", game_points);
            // printf(".. ghost.x=%i; ghost.y=%i\n", ghost.x, ghost.y); // for debugging
            // initialize for taking new key events or ghost update
            pressed_key= NONE;
            ghost.updated = false;
        }
    }
    return 0;
}

void print_screen_memory_to_cli() {
    // system("@cls||clear");
    system("cls"); // use this in windows
    // printf("\e[1;1H\e[2J"); // for linux use the regex

    for (int y_i = 0; y_i < y_dim; y_i++) {
        for (int x_i = 0; x_i < x_dim; x_i++) {
            if (obstacles[x_i][y_i] == 1) {
                printf("#");
            }
            else {
                if (screen_memory[x_i][y_i] == ' ') {
                    switch (eat_pellets[x_i][y_i]) {
                    case 1: printf(".");
                        break;
                    case 5: printf("+");
                        break;
                    default:printf(" ");
                    }
                }
                else {
                    printf("%c", screen_memory[x_i][y_i]);
                }
            }

        }
        printf("\n");
    }
}

void make_dot(int x, int y, char dot_char) {
    screen_memory[x][y] = dot_char;
    screen_updated = 1;
}

void clear_screen_memory(char empty_char) {
    for (int y_i = 0; y_i < y_dim; y_i++) {
        for (int x_i = 0; x_i < x_dim; x_i++) {
            screen_memory[x_i][y_i] = empty_char;
        }
    }
}

enum pacman_keycode get_pressed_key() {
    int key = 0;
    if (_kbhit())
    {
        key = _getch();

        // cursor key pressed writes 0 or 0xE0 into buffer first, then 72, 75, 77, 80 second
        if (key == 0 || key == 0xE0) {
            // therefore ask the keybuffer with _getch a second time
            key = _getch();
            if (key == 72) { // cursor key UP
                return KEY_UP;
            }
            else if (key == 80) { // cursor key DOWN
                return KEY_DOWN;
            }
            else if (key == 75) { // cursor key LEFT
                return KEY_LEFT;
            }
            else if (key == 77) { // cursor key RIGHT
                return KEY_RIGHT;
            }
        }
        else if (key == 'q') {
            return KEY_p;
        }
    }
    return NONE;
}

game_object_t move_game_object(enum move_direction direction, int eat_pellets, game_object_t game_object) {
    int y_new, x_new;
    switch (direction) {
    case UP:
        y_new = (game_object.y - 1);
        if (y_new == -1) y_new = y_dim - 1;
        if (!collision_with_obstacle(game_object.x, y_new)) {
            game_object.y = y_new;
        }
        break;
    case RIGHT:
        x_new = (game_object.x + 1) % x_dim;
        if (!collision_with_obstacle(x_new, game_object.y)) {
            if (x_new > x_field_max) {
                game_object.x = x_field_min;
            }
            else {
                game_object.x = x_new;
            }
        }
        break;
    case DOWN:
        y_new = (game_object.y + 1) % y_dim;
        if (!collision_with_obstacle(game_object.x, y_new)) {
            game_object.y = y_new;
        }
        break;
    case LEFT:
        x_new = (game_object.x - 1);
        if (game_object.x == -1) x_new = x_dim - 1;
        if (!collision_with_obstacle(x_new, game_object.y)) {
            if (x_new < x_field_min) {
                game_object.x = x_field_max;
            }
            else {
                game_object.x = x_new;
            }
        }
        break;
    }
    if (eat_pellets)
        collect_eat_pellets(game_object.x, game_object.y);
    return game_object;
}

game_object_t update_ghost_position(game_object_t ghost) {
    long time_now = getMillis_sinceMidnight();
    if (time_now - ghost.last_update_time_ms > ghost.auto_update_intervall_ms) {
        // do the update and make new random position
        int random_new_position = rand() % 4;
        switch (random_new_position) {
        case 0:
            ghost = move_game_object(UP, 0, ghost);
            break;
        case 1:
            ghost = move_game_object(RIGHT, 0, ghost);
            break;
        case 2:
            ghost = move_game_object(DOWN, 0, ghost);
            break;
        case 3:
            ghost = move_game_object(LEFT, 0, ghost);
            break;
        }
        ghost.updated = true;
        ghost.last_update_time_ms = getMillis_sinceMidnight();
        return ghost;
    }
    else {
        return ghost;
    }
}

int collision_with_obstacle(int x_obj, int y_obj) {
    if (obstacles[x_obj][y_obj]) {
        return true;
    }
    return false;
}

void collect_eat_pellets(int x, int y) {
    game_points = game_points + eat_pellets[x][y];
    if (eat_pellets[x][y] > 0) {
        eat_pellets[x][y] = 0;
    }
}

void init_obstacles() {
    FILE* fp = fopen("obstacles.txt", "r");
    char c;
    for (int row = 0; row < y_dim; row++) {
        for (int col = 0; col < x_dim; col++) {
            c = fgetc(fp);
            if (c != EOF) {
                if (c == '#') {
                    obstacles[col][row] = 1;
                }
                else {
                    obstacles[col][row] = 0;
                }
            }
        }
        // fgetc(fp); // get the CR out of the file, in linux also CR as one of two line ending chars
        c=fgetc(fp); // get the LF out of the file, in windows only this one line ending in text mode
    }
    fclose(fp);
}

void init_eat_pellets() {

    FILE* fp = fopen("eat_pellets.txt", "r");
    char c;
    for (int row = 0; row < y_dim; row++) {
        for (int col = 0; col < x_dim; col++) {
            c = fgetc(fp);
            if (c != EOF) {
                if (c == '1') {
                    eat_pellets[col][row] = 1;
                }
                else if (c == '5') {
                    eat_pellets[col][row] = 5;
                }
                else {
                    eat_pellets[col][row] = 0;
                }
            }
        }
        // fgetc(fp); // get the CR out of the file, in linux also CR as one of two line ending chars
        c = fgetc(fp); // get the LF out of the file, in windows only this one line ending in text mode
    }
    fclose(fp);
}

bool game_over(game_object_t pacman, game_object_t ghost) {
    if (pacman.x == ghost.x && pacman.y == ghost.y) {
        return true;
    }
    else {
        return false;
    }
}

// works on windows only
long getMillis_sinceMidnight() {
    SYSTEMTIME time;
    GetSystemTime(&time);
    long time_ms = time.wHour * 3600 * 1000 + time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;
    return time_ms;
}


// Programm ausf�hren: STRG+F5 oder "Debuggen" > Men� "Ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Men� "Debuggen starten"

// Tipps f�r den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzuf�gen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzuf�gen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzuf�gen", um dem Projekt vorhandene Codedateien hinzuzuf�gen.
//   6. Um dieses Projekt sp�ter erneut zu �ffnen, wechseln Sie zu "Datei" > "�ffnen" > "Projekt", und w�hlen Sie die SLN-Datei aus.
