#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "geral.h"
#include "exibe.h"
#include "audio.h"
#include "teclado.h"

//general stuff

int alien_count = 0, alien_count_posi = 0;
int rounds = 0;
int posi = 2;
long frame_count = 0, old_frame_count = 0;
bool action = false;
long frames;
long score;

//display stuff

ALLEGRO_DISPLAY* disp;
ALLEGRO_BITMAP* buffer;

//keyboard stuff
unsigned char key[ALLEGRO_KEY_MAX];


//sprites

#define SHIP_W 12
#define SHIP_H 13

#define SHIP_SHOT_W 2
#define SHIP_SHOT_H 9

#define LIFE_W 6
#define LIFE_H 6

const int ALIEN_W[] = {14, 13, 13, 25};
const int ALIEN_H[] = { 9, 10, 10, 11};

#define ALIEN_BUG_W      ALIEN_W[0]
#define ALIEN_BUG_H      ALIEN_H[0]
#define ALIEN_ARROW_W    ALIEN_W[1]
#define ALIEN_ARROW_H    ALIEN_H[1]
#define ALIEN_THICCBOI_W ALIEN_W[2]
#define ALIEN_THICCBOI_H ALIEN_H[2]
#define ALIEN_FAST_W     ALIEN_W[3]
#define ALIEN_FAST_H     ALIEN_H[3]

#define ALIEN_SHOT_W 4
#define ALIEN_SHOT_H 4

#define EXPLOSION_FRAMES 4
#define SPARKS_FRAMES    3


typedef struct SPRITES
{
    ALLEGRO_BITMAP* _sheet;

    ALLEGRO_BITMAP* ship;
    ALLEGRO_BITMAP* ship_shot[2];
    ALLEGRO_BITMAP* life;

    ALLEGRO_BITMAP* alien[4];
    ALLEGRO_BITMAP* alien_2[3];
    ALLEGRO_BITMAP* alien_shot;

    ALLEGRO_BITMAP* shield[3];

    ALLEGRO_BITMAP* explosion[EXPLOSION_FRAMES];
    ALLEGRO_BITMAP* sparks[SPARKS_FRAMES];

    ALLEGRO_BITMAP* powerup[4];
} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init()
{
    sprites._sheet = al_load_bitmap("spritesheet.png");
    must_init(sprites._sheet, "spritesheet");

    sprites.ship = sprite_grab(0, 0, SHIP_W, SHIP_H);

    sprites.life = sprite_grab(0, 14, LIFE_W, LIFE_H);

    sprites.ship_shot[0] = sprite_grab(13, 0, SHIP_SHOT_W, SHIP_SHOT_H);
    sprites.ship_shot[1] = sprite_grab(16, 0, SHIP_SHOT_W, SHIP_SHOT_H);

    sprites.alien[0] = sprite_grab(19, 0, ALIEN_BUG_W, ALIEN_BUG_H);
    sprites.alien[1] = sprite_grab(19, 10, ALIEN_ARROW_W, ALIEN_ARROW_H);
    sprites.alien[2] = sprite_grab(19, 21, ALIEN_THICCBOI_W, ALIEN_THICCBOI_H);
    sprites.alien[3] = sprite_grab(0, 37, ALIEN_FAST_W, ALIEN_FAST_H);

    sprites.alien_2[0] = sprite_grab(0, 21, ALIEN_BUG_W, ALIEN_BUG_H);
    sprites.alien_2[1] = sprite_grab(0, 64, ALIEN_ARROW_W, ALIEN_ARROW_H);
    sprites.alien_2[2] = sprite_grab(33, 21, ALIEN_THICCBOI_W, ALIEN_THICCBOI_H);

    sprites.alien_shot = sprite_grab(13, 10, ALIEN_SHOT_W, ALIEN_SHOT_H);

    sprites.shield[0] = sprite_grab(27,45,16,3);
    sprites.shield[1] = sprite_grab(27,41,16,3);
    sprites.shield[2] = sprite_grab(27,37,16,3);

    sprites.explosion[0] = sprite_grab(33, 10, 9, 9);
    sprites.explosion[1] = sprite_grab(43, 9, 11, 11);
    sprites.explosion[2] = sprite_grab(46, 21, 17, 18);
    sprites.explosion[3] = sprite_grab(46, 40, 17, 17);

    sprites.sparks[0] = sprite_grab(34, 0, 10, 8);
    sprites.sparks[1] = sprite_grab(45, 0, 7, 8);
    sprites.sparks[2] = sprite_grab(54, 0, 9, 8);

    sprites.powerup[0] = sprite_grab(0, 49, 9, 12);
    sprites.powerup[1] = sprite_grab(10, 49, 9, 12);
    sprites.powerup[2] = sprite_grab(20, 49, 9, 12);
    sprites.powerup[3] = sprite_grab(30, 49, 9, 12);
}

void sprites_deinit()
{
    al_destroy_bitmap(sprites.ship);

    al_destroy_bitmap(sprites.ship_shot[0]);
    al_destroy_bitmap(sprites.ship_shot[1]);

    al_destroy_bitmap(sprites.alien[0]);
    al_destroy_bitmap(sprites.alien[1]);
    al_destroy_bitmap(sprites.alien[2]);
    al_destroy_bitmap(sprites.alien[3]);

    al_destroy_bitmap(sprites.alien_2[0]);
    al_destroy_bitmap(sprites.alien_2[2]);

    al_destroy_bitmap(sprites.shield[0]);
    al_destroy_bitmap(sprites.shield[1]);
    al_destroy_bitmap(sprites.shield[2]);

    al_destroy_bitmap(sprites.sparks[0]);
    al_destroy_bitmap(sprites.sparks[1]);
    al_destroy_bitmap(sprites.sparks[2]);

    al_destroy_bitmap(sprites.explosion[0]);
    al_destroy_bitmap(sprites.explosion[1]);
    al_destroy_bitmap(sprites.explosion[2]);
    al_destroy_bitmap(sprites.explosion[3]);

    al_destroy_bitmap(sprites.powerup[0]);
    al_destroy_bitmap(sprites.powerup[1]);
    al_destroy_bitmap(sprites.powerup[2]);
    al_destroy_bitmap(sprites.powerup[3]);

    al_destroy_bitmap(sprites._sheet);
}

//audio

ALLEGRO_SAMPLE* sample_shot;
ALLEGRO_SAMPLE* sample_explode[2];

//vfx

typedef struct FX
{
    int x, y;
    int frame;
    bool spark;
    bool used;
} FX;

#define FX_N 128
FX fx[FX_N];

void fx_init()
{
    for(int i = 0; i < FX_N; i++)
        fx[i].used = false;
}

void fx_add(bool spark, int x, int y)
{
    if(!spark)
        al_play_sample(sample_explode[between(0, 2)], 0.75, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);

    for(int i = 0; i < FX_N; i++)
    {
        if(fx[i].used)
            continue;

        fx[i].x = x;
        fx[i].y = y;
        fx[i].frame = 0;
        fx[i].spark = spark;
        fx[i].used = true;
        return;
    }
}

void fx_update()
{
    for(int i = 0; i < FX_N; i++)
    {
        if(!fx[i].used)
            continue;

        fx[i].frame++;

        if((!fx[i].spark && (fx[i].frame == (EXPLOSION_FRAMES * 2)))
        || ( fx[i].spark && (fx[i].frame == (SPARKS_FRAMES * 2)))
        )
            fx[i].used = false;
    }
}

void fx_draw()
{
    for(int i = 0; i < FX_N; i++)
    {
        if(!fx[i].used)
            continue;

        int frame_display = fx[i].frame / 2;
        ALLEGRO_BITMAP* bmp =
            fx[i].spark
            ? sprites.sparks[frame_display]
            : sprites.explosion[frame_display]
        ;

        int x = fx[i].x - (al_get_bitmap_width(bmp) / 2);
        int y = fx[i].y - (al_get_bitmap_height(bmp) / 2);
        al_draw_bitmap(bmp, x, y, 0);
    }
}

//shot

typedef struct SHOT_POSITION
{
    int x, y;
} SHOT_POSITION;

typedef struct SHOT
{
    SHOT_POSITION position;
    int x, y, dx, dy;
    int frame;
    bool ship;
    bool used;
} SHOT;

#define SHOTS_N 128
SHOT shots[SHOTS_N];

void shots_init()
{
    for(int i = 0; i < SHOTS_N; i++)
        shots[i].used = false;
}

bool shots_add(bool ship, bool straight, int x, int y)
{
    for (int i = 0; i < SHOTS_N; i++)
    {
        if (shots[i].used && shots[i].position.x == x && shots[i].position.y == y)
            return false;
    }

    al_play_sample(
        sample_shot,
        0.3,
        0,
        ship ? 1.0 : between_f(1.5, 1.6),
        ALLEGRO_PLAYMODE_ONCE,
        NULL
    );

    for(int i = 0; i < SHOTS_N; i++)
    {
        if(shots[i].used)
            continue;

        shots[i].position.x = x;
        shots[i].position.y = y;
        shots[i].ship = ship;

        if(ship)
        {
            shots[i].x = x - (SHIP_SHOT_W / 2);
            shots[i].y = y;
        }
        else // alien
        {
            shots[i].x = x - (ALIEN_SHOT_W / 2);
            shots[i].y = y - (ALIEN_SHOT_H / 2);

            if(straight)
            {
                shots[i].dx = 0;
                shots[i].dy = 2;
            }
            else
            {

                shots[i].dx = between(-2, 2);
                shots[i].dy = between(-2, 2);
            }
            if(!shots[i].dx && !shots[i].dy)
                return true;

            shots[i].frame = 0;
        }

        shots[i].frame = 0;
        shots[i].used = true;

        return true;
    }
    return false;
}

void shots_update()
{
    for(int i = 0; i < SHOTS_N; i++)
    {
        if(!shots[i].used)
            continue;

        if(shots[i].ship)
        {
            shots[i].y -= 5;

            if(shots[i].y < -SHIP_SHOT_H)
            {
                shots[i].used = false;
                continue;
            }
        }
        else // alien
        {
            shots[i].x += shots[i].dx;
            shots[i].y += shots[i].dy;

            if((shots[i].x < -ALIEN_SHOT_W)
            || (shots[i].x > BUFFER_W)
            || (shots[i].y < -ALIEN_SHOT_H)
            || (shots[i].y > BUFFER_H)
            ) {
                shots[i].used = false;
                continue;
            }
        }

        shots[i].frame++;
    }
}

bool shots_collide(bool ship, int x, int y, int w, int h)
{
    for(int i = 0; i < SHOTS_N; i++)
    {
        if(!shots[i].used)
            continue;

        if(shots[i].ship == ship) 
            continue;

        int sw, sh;
        if(ship)
        {
            sw = ALIEN_SHOT_W;
            sh = ALIEN_SHOT_H;
        }
        else
        {
            sw = SHIP_SHOT_W;
            sh = SHIP_SHOT_H;
        }

        if(collide(x, y, x+w, y+h, shots[i].x, shots[i].y, shots[i].x+sw, shots[i].y+sh))
        {
            fx_add(true, shots[i].x + (sw / 2), shots[i].y + (sh / 2));
            shots[i].used = false;

            return true;
        }
    }

    return false;
}

void shots_draw()
{
    for(int i = 0; i < SHOTS_N; i++)
    {
        if(!shots[i].used)
            continue;

        int frame_display = (shots[i].frame / 2) % 2;

        if(shots[i].ship)
            al_draw_bitmap(sprites.ship_shot[frame_display], shots[i].x, shots[i].y, 0);
        else // alien
        {
            ALLEGRO_COLOR tint =
                frame_display
                ? al_map_rgb_f(1, 1, 1)
                : al_map_rgb_f(0.5, 0.5, 0.5)
            ;
            al_draw_tinted_bitmap(sprites.alien_shot, tint, shots[i].x, shots[i].y, 0);
        }
    }
}

//shield

typedef struct SHIELD {
    int x, y;
    int life;
    bool active;
} SHIELD;

#define SHIELDS_N 4
SHIELD shields[SHIELDS_N];

void shields_init() {
    int shield_spacing = 80;
    int shield_start_x = (BUFFER_W - (shield_spacing * (SHIELDS_N - 1) + 16)) / 2;

    for(int i = 0; i < SHIELDS_N; i++) {
        shields[i].x = shield_start_x + i * shield_spacing;
        shields[i].y = BUFFER_H - 40;
        shields[i].life = 10;
        shields[i].active = true;
    }
}

void shields_update() {

    for(int i = 0; i < SHIELDS_N; i++) {

        if(!shields[i].active)
            continue;

        if (shots_collide(true, shields[i].x, shields[i].y, 16, 3)) 
            shields[i].life--;

        if (shots_collide(false, shields[i].x, shields[i].y, 16, 3)) 
            shields[i].life--;
        
        if(shields[i].life <= 0)
            shields[i].active = false;

    }

}

void shields_draw() {
    for (int i = 0; i < SHIELDS_N; i++)
    {
        if (shields[i].active)
        {
            int shield_index = 0;

            if (shields[i].life <= 7 && shields[i].life > 4)
                shield_index = 1;
            else if (shields[i].life <= 4)
                shield_index = 2;

            al_draw_bitmap(sprites.shield[shield_index], shields[i].x, shields[i].y, 0);
        }
    }
}

//player 1

#define SHIP_SPEED 3
#define SHIP_MAX_X (BUFFER_W - SHIP_W)
#define SHIP_MAX_Y (BUFFER_H - SHIP_H)

typedef struct SHIP
{
    int x, y;
    int shot_timer;
    int lives;
    int respawn_timer;
    int invincible_timer;
} SHIP;
SHIP ship;

void ship_init()
{
    ship.x = (BUFFER_W / 2) - (SHIP_W / 2);
    ship.y = (BUFFER_H+10) - (SHIP_H+10);
    ship.shot_timer = 0;
    ship.lives = 3;
    ship.respawn_timer = 0;
    ship.invincible_timer = 120;
}

void ship_update()
{
    if(ship.lives < 0)
        return;

    if(ship.respawn_timer)
    {
        ship.respawn_timer--;
        return;
    }

    if(key[ALLEGRO_KEY_LEFT])
        ship.x -= SHIP_SPEED;
    if(key[ALLEGRO_KEY_RIGHT])
        ship.x += SHIP_SPEED;

    if(ship.x < 0)
        ship.x = 0;

    if(ship.x > SHIP_MAX_X)
        ship.x = SHIP_MAX_X;

    if(ship.invincible_timer)
        ship.invincible_timer--;
    else
    {
        if(shots_collide(true, ship.x, ship.y, SHIP_W, SHIP_H))
        {
            int x = ship.x + (SHIP_W / 2);
            int y = ship.y + (SHIP_H / 2);
            fx_add(false, x, y);
            fx_add(false, x+4, y+2);
            fx_add(false, x-2, y-4);
            fx_add(false, x+1, y-5);

            ship.lives--;
            ship.respawn_timer = 90;
            ship.invincible_timer = 180;
        }
    }

    if(ship.shot_timer)
        ship.shot_timer--;
    else if(key[ALLEGRO_KEY_X])
    {
        int x = ship.x + (SHIP_W / 2);
        if(shots_add(true, false, x, ship.y)) {
            ship.shot_timer = 5;
        }
    }
}

void ship_draw()
{
    if(ship.lives == 0)
        return;
    if(ship.respawn_timer)
        return;
    if(((ship.invincible_timer / 2) % 3) == 1)
        return;

    al_draw_bitmap(sprites.ship, ship.x, ship.y, 0);
}

//fast ship fast
typedef struct SHIP_F
{
    int x, y;
    int life;
    bool alive;
} SHIP_F;
SHIP_F ship_f;

void ship_f_init()
{
    ship_f.x = (BUFFER_W);
    ship_f.y = (BUFFER_H) - 220;
    ship_f.alive = true;
    ship_f.life = 1;
}

void ship_f_update()
{
    if(shots_collide(false, ship_f.x, ship_f.y, ALIEN_FAST_W, ALIEN_FAST_H))
    {
        ship_f.life--;
        score += 100*(rounds+1);
    }

    if(ship_f.life <= 0) {
        ship_f.x = (BUFFER_W);
        ship_f.y = (BUFFER_H) - 220;
        ship_f.alive = false;
    }

    if(!(ship_f.alive))
        return;

    if(!(frames % 3)) {
        ship_f.x--;
    }

    if (ship_f.x < -25) {
        ship_f.alive = false;
    }
}

void ship_f_draw()
{
    if(ship_f.life == 0)
        return;

    al_draw_bitmap(sprites.alien[3], ship_f.x, ship_f.y, 0);
}

//villans

typedef enum ALIEN_TYPE
{
    ALIEN_TYPE_BUG = 0,
    ALIEN_TYPE_ARROW,
    ALIEN_TYPE_THICCBOI,
    ALIEN_TYPE_N
} ALIEN_TYPE;

typedef struct ALIEN
{
    int x, y;
    ALIEN_TYPE type;
    int shot_timer;
    int blink;
    int life;
    bool used;
} ALIEN;

#define ALIENS_N 16
ALIEN aliens[5][10];

void aliens_init()
{
    int aliens_per_row = 10;
    int alien_width = ALIEN_BUG_W + 10;  
    int alien_height = ALIEN_BUG_H + 10; 

    int start_x = (BUFFER_W - aliens_per_row * alien_width) / 2; 
    int start_y = 50;  
    alien_count = 50;

    int round_difficulty = 700*rounds;
    if(10000 - round_difficulty <= 6500)
        round_difficulty = 6500;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < aliens_per_row; j++)
        {
            aliens[i][j].x = start_x + j * alien_width;
            aliens[i][j].y = start_y + i * alien_height;
            if(i <= 0)
                aliens[i][j].type = 2;
            else if(i > 0  && i <= 2)
                aliens[i][j].type = 1;
            else
                aliens[i][j].type = 0;
            aliens[i][j].shot_timer = between(1, 10000 - round_difficulty);
            aliens[i][j].blink = 0;
            aliens[i][j].life = 1;
            aliens[i][j].used = true;
        }
    }
}

bool check_alien_edge() {
    for (int i = 0; i < 5; i++) {
        for (int e = 0; e < 10; e++) {
            if (!aliens[i][e].used)
                continue;

            if (aliens[i][e].x <= 0 || aliens[i][e].x >= (BUFFER_W - ALIEN_W[aliens[i][e].type])) {
                return true;
            }
        }
    }

    return false;
}


void aliens_update()
{
    if(check_alien_edge() && alien_count_posi == 0) {
        alien_count_posi = 1;
        posi++;
    }

    int round_difficulty = 5*rounds;
    if(20 - round_difficulty <= 0)
        round_difficulty = 19;


    for(int i = 0; i < 5; i++) {
        for (int e = 0; e < 10; e++) {

            if(!aliens[i][e].used)
                continue;

            switch(aliens[i][e].type)
            {
                case ALIEN_TYPE_BUG:
                    if(!(frames % (20 - round_difficulty))){
                        if(posi % 2 == 0) {
                            alien_count_posi = 0;
                            aliens[i][e].x--;
                        }else {
                            alien_count_posi = 0;
                            aliens[i][e].x++;
                        }
                    }
                    break;

                case ALIEN_TYPE_ARROW:
                    if(!(frames % (20 - round_difficulty))) {
                        if(posi % 2 == 0) {
                            alien_count_posi = 0;
                            aliens[i][e].x--;
                        }else {
                            alien_count_posi = 0;
                            aliens[i][e].x++;
                        }
                    }
                    break;

                case ALIEN_TYPE_THICCBOI:
                    if(!(frames % (20 - round_difficulty))) {
                        if(posi % 2 == 0) {
                            alien_count_posi = 0;
                            aliens[i][e].x--;
                        }else {
                            alien_count_posi = 0;
                            aliens[i][e].x++;
                        }
                    }
                    break;
            }

            if(aliens[i][e].y >= BUFFER_H)
            {
                aliens[i][e].used = false;
                continue;
            }

            if(aliens[i][e].blink)
                aliens[i][e].blink--;

            if(shots_collide(false, aliens[i][e].x, aliens[i][e].y, ALIEN_W[aliens[i][e].type], ALIEN_H[aliens[i][e].type]))
            {
                aliens[i][e].life--;
                aliens[i][e].blink = 4;
                alien_count --;
            }

            int cx = aliens[i][e].x + (ALIEN_W[aliens[i][e].type] / 2);
            int cy = aliens[i][e].y + (ALIEN_H[aliens[i][e].type] / 2);

            if(aliens[i][e].life <= 0)
            {
                fx_add(false, cx, cy);

                switch(aliens[i][e].type)
                {
                    case ALIEN_TYPE_BUG:
                        score += 10;
                        break;

                    case ALIEN_TYPE_ARROW:
                        score += 20;
                        break;

                    case ALIEN_TYPE_THICCBOI:
                        score += 40;
                        break;
                }

                aliens[i][e].used = false;
                continue;
            }

            aliens[i][e].shot_timer--;
            if(aliens[i][e].shot_timer == 0)
            {
                switch(aliens[i][e].type)
                {
                    case ALIEN_TYPE_BUG:
                        shots_add(false, false, cx, cy);
                        aliens[i][e].shot_timer = 150;
                        break;
                    case ALIEN_TYPE_ARROW:
                        shots_add(false, true, cx, aliens[i][e].y);
                        aliens[i][e].shot_timer = 80;
                        break;
                    case ALIEN_TYPE_THICCBOI:
                        shots_add(false, true, cx, aliens[i][e].y);
                        aliens[i][e].shot_timer = 150;
                        break;
                }
            }
        }
    }
}

void aliens_draw()
{
    frame_count = frames;
    if(old_frame_count+100 <= frame_count) {
        old_frame_count = frame_count;
        action = !action;
    }

    for(int i = 0; i < ALIENS_N; i++) {
        for(int e = 0; e < ALIENS_N; e++) {
            if(!aliens[i][e].used)
                continue;
            if(aliens[i][e].blink > 2)
                continue;

            switch(aliens[i][e].type)
            {
                case ALIEN_TYPE_BUG:
                    if(action)
                        al_draw_bitmap(sprites.alien_2[aliens[i][e].type], aliens[i][e].x, aliens[i][e].y, 0);
                    else
                        al_draw_bitmap(sprites.alien[aliens[i][e].type], aliens[i][e].x, aliens[i][e].y, 0);
                    break;

                case ALIEN_TYPE_ARROW:
                    if(action)
                        al_draw_bitmap(sprites.alien[aliens[i][e].type], aliens[i][e].x, aliens[i][e].y, 0);
                    else
                        al_draw_bitmap(sprites.alien_2[aliens[i][e].type], aliens[i][e].x, aliens[i][e].y, 0);
                    break;

                case ALIEN_TYPE_THICCBOI:
                        if(action)
                            al_draw_bitmap(sprites.alien_2[aliens[i][e].type], aliens[i][e].x, aliens[i][e].y, 0);
                        else
                            al_draw_bitmap(sprites.alien[aliens[i][e].type], aliens[i][e].x, aliens[i][e].y, 0);
                        break;
            }
        }
    }
}

//stars background

typedef struct STAR
{
    float y;
    float speed;
} STAR;

#define STARS_N ((BUFFER_W / 2) - 1)
STAR stars[STARS_N];

void stars_init()
{
    for(int i = 0; i < STARS_N; i++)
    {
        stars[i].y = between_f(0, BUFFER_H);
        stars[i].speed = between_f(0.1, 1);
    }
}

void stars_update()
{
    for(int i = 0; i < STARS_N; i++)
    {
        stars[i].y += stars[i].speed;
        if(stars[i].y >= BUFFER_H)
        {
            stars[i].y = 0;
            stars[i].speed = between_f(0.1, 1);
        }
    }
}

void stars_draw()
{
    float star_x = 1.5;
    for(int i = 0; i < STARS_N; i++)
    {
        float l = stars[i].speed * 0.8;
        al_draw_pixel(star_x, stars[i].y, al_map_rgb_f(l,l,l));
        star_x += 2;
    }
}

//points

ALLEGRO_FONT* font;
long score_display;

void hud_init()
{
    font = al_create_builtin_font();
    must_init(font, "font");

    score_display = 0;
}

void hud_deinit()
{
    al_destroy_font(font);
}

void hud_update()
{
    if(frames % 2)
        return;

    for(long i = 5; i > 0; i--)
    {
        long diff = 1 << i;
        if(score_display <= (score - diff))
            score_display += diff;
    }
}

void hud_draw()
{
    al_draw_textf(
        font,
        al_map_rgb_f(1,1,1),
        1, 1,
        0,
        "%06ld",
        score_display
    );

    int spacing = LIFE_W + 1;
    for(int i = 0; i < ship.lives; i++)
        al_draw_bitmap(sprites.life, 1 + (i * spacing), 10, 0);

    if(ship.lives == 0) {
        al_draw_text(
            font,
            al_map_rgb_f(1,1,1),
            BUFFER_W / 2, BUFFER_H / 2 + 40,
            ALLEGRO_ALIGN_CENTER,
            "G A M E  O V E R"
        );

        al_draw_text(font, al_map_rgb(255, 255, 255),
                     BUFFER_W / 2, BUFFER_H / 2 + 50,
                     ALLEGRO_ALIGN_CENTRE, "Pressione 'esc' para sair");
    }
}

//main func

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_init_font_addon();

    ALLEGRO_DISPLAY *display = al_create_display(960, 720);
    al_set_window_position(display, 200, 100);
    al_set_window_title(display, "Jogo - Menu");

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    ALLEGRO_FONT *fonte = al_create_builtin_font();

    if (!fonte) {
        fprintf(stderr, "Erro ao criar a fonte interna do Allegro.\n");
        return -1;
    }

    int sair = 0;
    int jogar = false;

    // Loop menu
    while (!sair) {
        al_clear_to_color(al_map_rgb(0, 0, 0));  // Limpa a tela para preto

        int largura_tela = al_get_display_width(display);
        int altura_tela = al_get_display_height(display);

        al_draw_text(fonte, al_map_rgb(255, 255, 255),
                     largura_tela/2, altura_tela/2,
                     ALLEGRO_ALIGN_CENTRE, "SPACE INVADERS");

        al_draw_text(fonte, al_map_rgb(255, 255, 255),
                     largura_tela / 2, altura_tela / 2 - 50,
                     ALLEGRO_ALIGN_CENTRE, "Aperte E para jogar");

        al_draw_text(fonte, al_map_rgb(255, 255, 255),
                     largura_tela / 2, altura_tela / 2 + 50,
                     ALLEGRO_ALIGN_CENTRE, "Aperte S para sair");

        al_flip_display();  // Atualiza a tela

        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        // Verifica se uma tecla foi pressionada
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_E:
                    sair = 1;
                    jogar = true;
                    break;
                case ALLEGRO_KEY_S:
                    sair = 1;
                    break;
            }
        }
    }

    // Finalização do menu
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    if(jogar) {
        disp_init();

        audio_init();

        must_init(al_init_image_addon(), "image");
        sprites_init();

        hud_init();

        must_init(al_init_primitives_addon(), "primitives");

        must_init(al_install_audio(), "audio");
        must_init(al_init_acodec_addon(), "audio codecs");
        must_init(al_reserve_samples(16), "reserve samples");

        al_register_event_source(queue, al_get_keyboard_event_source());
        al_register_event_source(queue, al_get_display_event_source(disp));
        al_register_event_source(queue, al_get_timer_event_source(timer));

        keyboard_init();
        fx_init();
        shots_init();
        ship_init();
        aliens_init();
        shields_init();
        stars_init();

        frames = 0;
        score = 0;

        bool done = false;
        bool redraw = true;
        ALLEGRO_EVENT event;

        al_start_timer(timer);

        long frame_permission = 500;

        while(1)
        {
            al_wait_for_event(queue, &event);

            switch(event.type)
            {
                case ALLEGRO_EVENT_TIMER:
                    fx_update();
                    shots_update();
                    stars_update();
                    ship_update();
                    ship_f_update();
                    aliens_update();
                    shields_update();
                    hud_update();

                    if(frame_permission == frames) {
                        frame_permission += 1700;
                        ship_f_init();
                    }

                    if(key[ALLEGRO_KEY_ESCAPE])
                        done = true;
                    
                    if(alien_count == 0) {
                        aliens_init();
                        if (ship.lives < 5)
                        {
                            ship.lives++;
                        }
                        rounds ++;
                    }

                    redraw = true;
                    frames++;
                    break;

                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    done = true;
                    break;
            }

            if(done)
                break;

            keyboard_update(&event);

            if(redraw && al_is_event_queue_empty(queue))
            {
                disp_pre_draw();
                al_clear_to_color(al_map_rgb(0,0,0));

                stars_draw();
                ship_f_draw();
                aliens_draw();
                shields_draw();
                shots_draw();
                fx_draw();
                ship_draw();

                hud_draw();

                disp_post_draw();
                redraw = false;
            }
        }

        sprites_deinit();
        hud_deinit();
        audio_deinit();
        disp_deinit();
        al_destroy_timer(timer);
        al_destroy_event_queue(queue);
    }

    return 0;
}