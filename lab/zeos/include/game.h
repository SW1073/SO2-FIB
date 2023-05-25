#ifndef GAME_H
#define GAME_H

#include <libc.h>
#include <map.h>

#define TICKS_PER_SECOND 18
#define NUM_LEVELS 3

extern Map level_1;
extern Map level_2;
extern Map level_3;

extern unsigned char *levels[NUM_LEVELS];

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

int game_is_manzanita(struct game* game, int x, int y, int manzanita);

int game_should_exit(struct game* game);

void game_advance_level(struct game* game);

void game_process_input(struct game* game);

void game_process_collisions(struct game* game);

void game_move_player_left(struct game* game);

void game_move_player_right(struct game* game);

void game_move_player_up(struct game* game);

void game_move_player_down(struct game* game);

void game_move_player_random(struct game* game);

void game_move_manzanita_random(struct game* game, int manzanita);

#endif // !GAME_H
