#ifndef GAME_H
#define GAME_H

#include <libc.h>

#define HEIGHT 21
#define WIDTH 80

#define TICKS_PER_SECOND 18
#define NUM_LEVELS 3

#define SIZE (HEIGHT*WIDTH/8)+1

// 'level1', 80x21px
unsigned char level_1[SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 
    0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 
    0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 
    0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 
    0x00, 0x00, 0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x7f, 0xc0, 
    0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 
    0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 
    0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 
    0x7f, 0xc0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x01, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7f, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00
};

unsigned char level_2[SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x03, 0xff, 0x00, 0x7f, 0xc0, 0x1f, 0xfc, 
    0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x7f, 0xc0, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 
    0x7f, 0xc0, 0x1f, 0xfc, 0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x7f, 0xc0, 0x1f, 0xfc, 0x00, 0x00, 
    0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 
    0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 
    0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 
    0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0x00, 
    0x7f, 0xc0, 0x18, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0xff, 0xff, 0xff, 0xf8, 0x3c, 0x00, 0x00, 
    0x00, 0x03, 0xc3, 0xff, 0xc0, 0x7f, 0xf8, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0xff, 0xc0, 0x7f, 
    0xf8, 0x3c, 0x00, 0x00, 0x00, 0x03, 0xc3, 0xff, 0xc0, 0x7f, 0xf8, 0x3c, 0x00, 0x00, 0x00, 0x03, 
    0xc3, 0xff, 0xc0, 0x7f, 0xf8, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00
};

unsigned char level_3[SIZE] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc0, 0x1f, 0xff, 0xff, 
    0xff, 0x00, 0x7f, 0xfe, 0x7f, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xfe, 0x7f, 0xff, 
    0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xfe, 0x7f, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 
    0x7f, 0xfe, 0x7f, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xfe, 0x00, 0x01, 0xc0, 0x1f, 
    0xff, 0xff, 0xff, 0x00, 0x70, 0x00, 0x00, 0x01, 0xc0, 0x18, 0x00, 0x40, 0x03, 0x00, 0x70, 0x00, 
    0x00, 0x01, 0xc0, 0x18, 0x00, 0x40, 0x03, 0x00, 0x70, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x40, 
    0x03, 0xff, 0xf0, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x40, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x01, 
    0xff, 0xf8, 0x00, 0x40, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x01, 0xc0, 0x18, 0x00, 0x40, 0x03, 0x00, 
    0x70, 0x00, 0x00, 0x01, 0xc0, 0x18, 0x00, 0x40, 0x03, 0x00, 0x70, 0x00, 0x7f, 0xff, 0xc0, 0x1f, 
    0xff, 0xff, 0xff, 0x00, 0x7f, 0xfe, 0x7f, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xfe, 
    0x7f, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xfe, 0x7f, 0xff, 0xc0, 0x1f, 0xff, 0xff, 
    0xff, 0x00, 0x7f, 0xfe, 0x7f, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xfe, 0x7f, 0xff, 
    0xc0, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00
};

unsigned char *levels[NUM_LEVELS] = {level_1, level_2, level_3};

struct object {
    int x;
    int y;
};

struct game {
    int score;
    int level;
    int lives;
    int time_left;
    struct object player;
    int num_manzanitas;
    int manzanitas_left;
    struct object *manzanitas;
    unsigned long last_ticks;
};

struct game* game_new(int diff);

void game_loop(struct game* game);
 
// --------------------------------------------------------

void game_draw(struct game* game);

void game_update_time(struct game* game);

char game_is_wall(struct game* game, int x, int y);

int game_is_manzanita(struct game* game, int x, int y);

int game_should_exit(struct game* game);

void game_advance_level(struct game* game);

void game_process_input(struct game* game);

void game_process_collisions(struct game* game);

void game_move_player_left(struct game* game);

void game_move_player_right(struct game* game);

void game_move_player_up(struct game* game);

void game_move_player_down(struct game* game);

void game_move_player_random(struct game* game);

void game_move_manzanita_random(struct game* game, int i);

#endif // !GAME_H
