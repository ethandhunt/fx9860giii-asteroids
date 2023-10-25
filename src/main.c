#include <gint/display.h>
#include <gint/keyboard.h>
#include <math.h>
#include <gint/clock.h>
#include <gint/keycodes.h>
#include <stdlib.h>

// from openlibm_math.h
#define	M_PI		3.14159265358979323846	/* pi */

#define NELEMS(x) (sizeof(x)/sizeof(x[0]))
#define RAND_FLOAT(min, max) (((float)rand()/(float)(RAND_MAX)) * (max-min) + min)
#define DISTF(x1, y1, x2, y2) sqrtf(powf((x1-x2), 2) + powf((y1-y2), 2))

#define ASTEROID_COUNT 10
#define ASTEROID_MAX_RAD 5

#define P_TURN_SPEED 0.1f
#define P_COLLISION_RAD 2

#define MAX_BULLETS 20

struct vecf
{
	float x;
	float y;
};

struct vecf p_pos = {64, 32};
struct vecf p_vel = {0,  0};
float p_dir;

struct vecf thrust_vector = {0.0f, 0.05f};
struct vecf bullet_vector = {0.0f, 0.2};
struct vecf knockback_vector = {0.0f, -0.1};

struct vecf p_vertices[] = {{0.0f, 3.0f}, {2.0f, -3.0f}, {-2.0f, -3.0f}};

struct vecf asteroids_pos[ASTEROID_COUNT];
struct vecf asteroids_vel[ASTEROID_COUNT];
float asteroids_rad[ASTEROID_COUNT];

struct vecf bullets_pos[MAX_BULLETS];
struct vecf bullets_vel[MAX_BULLETS];
int bullets_life[MAX_BULLETS];
int bullet_count = 0;
int bullet_cooldown = 0;


/*
Rotate point about origin by angle
*/
struct vecf rotate(struct vecf point, float angle)
{
    return (struct vecf) {point.x*cosf(angle) - point.y*sinf(angle), point.x*sinf(angle) + point.y*cosf(angle)};
}

float pos_fmodf(float a, float b)
{
    return fmodf(a, b) > 0 ? fmodf(a, b) : b - fmodf(a, b);
}

// algorithm from https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void draw_circle(float x, float y, float radius)
{
    float t1 = radius/16;
    float dx = radius;
    float dy = 0;
    while (dx >= dy)
    {
        dpixel( dx + x,  dy + y, C_BLACK);
        dpixel( dy + x,  dx + y, C_BLACK);
        dpixel(-dx + x,  dy + y, C_BLACK);
        dpixel( dx + x, -dy + y, C_BLACK);
        dpixel(-dy + x,  dx + y, C_BLACK);
        dpixel( dy + x, -dx + y, C_BLACK);
        dpixel(-dx + x, -dy + y, C_BLACK);
        dpixel(-dy + x, -dx + y, C_BLACK);

        dy++;

        t1 += dy;
        if (t1 - dx >= 0)
        {
            t1 -= dx;
            dx--;
        }
    }
}

int frames = 0;
void draw()
{
	dclear(C_WHITE);
    for (int i=0; i<NELEMS(p_vertices); i++)
    {
        struct vecf vec_a = rotate(p_vertices[i], p_dir);
        struct vecf vec_b = rotate(p_vertices[(i+1 == NELEMS(p_vertices)) ? 0 : i+1], p_dir);
        dline(vec_a.x + p_pos.x, vec_a.y + p_pos.y, vec_b.x + p_pos.x, vec_b.y + p_pos.y, C_BLACK);
    }

    for (int i=0; i<ASTEROID_COUNT; i++)
    {
        draw_circle(asteroids_pos[i].x, asteroids_pos[i].y, asteroids_rad[i]);
    }

    for (int i=0; i<bullet_count; i++)
    {
        dpixel(bullets_pos[i].x, bullets_pos[i].y, C_BLACK);
    }

    frames++;
}

int main(void)
{
    for (int i=0; i<ASTEROID_COUNT; i++)
    {
        asteroids_pos[i].x = (float) fmod(rand(), 128);
        asteroids_pos[i].y = (float) fmod(rand(), 64);

        asteroids_vel[i].x = RAND_FLOAT(-0.05, 0.05);
        asteroids_vel[i].y = RAND_FLOAT(-0.05, 0.05);

        asteroids_rad[i] = RAND_FLOAT(1, ASTEROID_MAX_RAD);
    }

    while (1)
    {
        draw();
        dupdate();
        clearevents();
        if (keydown(KEY_LEFT) || keydown(KEY_4))
        {
            p_dir -= P_TURN_SPEED;
        }
        if (keydown(KEY_RIGHT) || keydown(KEY_6))
        {
            p_dir += P_TURN_SPEED;
        }
        if (keydown(KEY_UP) || keydown(KEY_8))
        {
            struct vecf thrust_vector_rotated = rotate(thrust_vector, p_dir);
            p_vel.x += thrust_vector_rotated.x;
            p_vel.y += thrust_vector_rotated.y;
        }
        if (keydown(KEY_EXE) || keydown(KEY_7) || keydown(KEY_9) && bullet_cooldown < 0 && bullet_count <= MAX_BULLETS)
        {
            bullets_pos[bullet_count] = p_pos;
            
            struct vecf bullet_vector_rotated = rotate(bullet_vector, p_dir);
            bullets_vel[bullet_count] = (struct vecf) {p_vel.x + bullet_vector_rotated.x,
                                                       p_vel.y + bullet_vector_rotated.y};
            
            bullet_count++;
            bullet_cooldown = 100;
            
            struct vecf knockback_vector_rotated = rotate(knockback_vector, p_dir);
            p_vel.x += knockback_vector_rotated.x;
            p_vel.y += knockback_vector_rotated.y;
        }

        if (keydown(KEY_EXIT))
        {
            break;
        }

        p_pos.x += p_vel.x;
        p_pos.y += p_vel.y;

        p_pos.x = pos_fmodf(p_pos.x, 128);
        p_pos.y = pos_fmodf(p_pos.y, 64);

        for (int i=0; i<ASTEROID_COUNT; i++)
        {
            asteroids_pos[i].x += asteroids_vel[i].x;
            asteroids_pos[i].y += asteroids_vel[i].y;

            asteroids_pos[i].x = pos_fmodf(asteroids_pos[i].x, 128);
            asteroids_pos[i].y = pos_fmodf(asteroids_pos[i].y, 64);

            if (DISTF(p_pos.x, p_pos.y, asteroids_pos[i].x, asteroids_pos[i].y) < P_COLLISION_RAD + asteroids_rad[i])
            {
                dprint(0, 1, C_BLACK, "LOSE");
                getkey();

                bullet_count = 0;
                bullet_cooldown = 0;
                return main(); // don't properly reset, just recurse
            }
        }

        for (int i=0; i<bullet_count; i++)
        {
            bullets_pos[i].x += bullets_vel[i].x;
            bullets_pos[i].y += bullets_vel[i].y;

            bullets_pos[i].x = pos_fmodf(bullets_pos[i].x, 128);
            bullets_pos[i].y = pos_fmodf(bullets_pos[i].y, 64);

            for (int j=0; j<ASTEROID_COUNT; j++)
            {
                if (DISTF(bullets_pos[i].x, bullets_pos[i].y, asteroids_pos[j].x, asteroids_pos[j].y) < asteroids_rad[j])
                {
                    
                }
            }
        }

        bullet_cooldown--;

        sleep_ms(10);
    }
	return 1;
}
