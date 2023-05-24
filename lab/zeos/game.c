#include <game.h>

struct game* game_new(int diff) {
    struct game* game = (struct game*)dyn_mem(sizeof(struct game));

    game->score = 0;
    game->level = 0;
    game->time_left = 30;
    game->last_ticks = 0;

    // bigger diff, less lives.
    game->lives = 6 - (diff/2);
    if (game->lives < 1) game->lives = 1;

    // bigger diff, more manzanitas.
    int num_manzanitas = 3 + (diff * 2);
    game->num_manzanitas = num_manzanitas;
    game->manzanitas_left = num_manzanitas;
    game->manzanitas = (struct object*)dyn_mem(sizeof(struct object) * num_manzanitas);

    // init manzanitas position
    for (int i = 0; i < num_manzanitas; i++) game_move_manzanita_random(game, i);

    // init player position
    game_move_player_random(game);

    return game;
}

void game_loop(struct game* game) {
    // clear_screen();

    char should_exit = 0;

    while (!should_exit) {
        game_draw(game);
        game_update_time(game);
        // TODO esto hay que hacerlo en otro thread
        game_process_input(game);
        game_process_collisions(game);
        if (game->manzanitas_left <= 0) game_advance_level(game);
        should_exit = game_should_exit(game);
    }

    // TODO mostrar score y game over title??? 
}

// --------------------------------------------------------

void game_draw(struct game *game) {
    // pintar mapa
    // pintar score
    // pintar tiempo
    // pintar vidas
    // pintar player
    // pintar manzanitas
        // si la posición de la manzanita es < 0 ignora manzanita
}

int game_should_exit(struct game* game) {
    if (game->time_left <= 0) return 1;
    if (game->lives <= 0) return 1;
    if (game->level == NUM_LEVELS) return 1;

    return 0;
}

void game_advance_level(struct game* game) {
    game->level++;
    game->time_left = 30;
    game->lives++;
    game->manzanitas_left = game->num_manzanitas;
    for (int i = 0; i < game->num_manzanitas; i++) game_move_manzanita_random(game, i);
}

void game_update_time(struct game *game) {
    unsigned long ticks = gettime();

    if (ticks > (game->last_ticks + TICKS_PER_SECOND)) {
        game->time_left--;
    }
}

char game_is_wall(struct game* game, int x, int y) {
    // bithack to raro
    return 0;
}

int game_is_manzanita(struct game* game, int x, int y) {
    for (int i = 0; i < game->num_manzanitas; i++) {
        if (game->manzanitas[i].x == x && game->manzanitas[i].y == y) {
            return i;
        }
    }

    return -1;
}

void game_process_input(struct game *game) {
    // TODO handling de threads y tal
        // mutexes, etc.

    char c;

    read(&c, 1);

    // delta time?
    switch (c) {
        case 'w':
            game_move_player_up(game);
            break;
        case 'a':
            game_move_player_left(game);
            break;
        case 's':
            game_move_player_down(game);
            break;
        case 'd':
            game_move_player_right(game);
            break;
        case 'q':
            game->lives = 0;
            break;
        default:
            break;
    }
}

void game_process_collisions(struct game* game) {
    if (game_is_wall(game, game->player.x, game->player.y)) {
        game->lives--;
        game_move_player_random(game);
        return;
    }

    int manzanita = game_is_manzanita(game, game->player.x, game->player.y);
    if (manzanita == -1) return;

    game->score++;
    game->manzanitas_left--;
    game->manzanitas[manzanita].x = -1;
    game->manzanitas[manzanita].y = -1;
}

void game_move_player_left(struct game* game) {
    if (game->player.x > 0 && !game_is_wall(game, game->player.x - 1, game->player.y)) {
        game->player.x--;
    }
}

void game_move_player_right(struct game* game) {
    if (game->player.x < WIDTH-1 && !game_is_wall(game, game->player.x + 1, game->player.y)) {
        game->player.x++;
    }
}

void game_move_player_up(struct game* game) {
    if (game->player.y > 0 && !game_is_wall(game, game->player.x, game->player.y - 1)) {
        game->player.y--;
    }
}

void game_move_player_down(struct game* game) {
    if (game->player.y < HEIGHT-1 && !game_is_wall(game, game->player.x, game->player.y + 1)) {
        game->player.y++;
    }
}

void game_move_player_random(struct game* game) {
    int x = rand() % WIDTH;
    int y = rand() % HEIGHT;

    if (!game_is_wall(game, x, y) && (game_is_manzanita(game, x, y) < 0)) {
        game->player.x = x;
        game->player.y = y;
    } else {
        game_move_player_random(game);
    }
}

void game_move_manzanita_random(struct game* game, int i) {
    int x = rand() % WIDTH;
    int y = rand() % HEIGHT;

    if (!game_is_wall(game, x, y) && !game_is_manzanita(game, x, y)) {
        game->manzanitas[i].x = x;
        game->manzanitas[i].y = y;
    } else {
        game_move_manzanita_random(game, i);
    }
}

