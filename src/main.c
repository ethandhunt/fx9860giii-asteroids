#include <gint/display.h>
#include <gint/keyboard.h>
#include <math.h>
#include <gint/clock.h>
#include <gint/keycodes.h>
#include <stdlib.h>

#define NELEMS(x) (sizeof(x)/sizeof(x[0]))
#define RAND_FLOAT(min, max) (((float)rand()/(float)(RAND_MAX)) * (max-min) + min)

#define ASTEROID_COUNT 100

struct vecf
{
	float x;
	float y;
};

struct vecf p_pos = {64, 32};
struct vecf p_vel = {0,  0};
float p_dir;

struct vecf thrust_vector = {0.0f, 0.05f};

struct vecf p_vertices[] = {{0.0f, 3.0f}, {2.0f, -3.0f}, {-2.0f, -3.0f}};

struct vecf asteroids_pos[ASTEROID_COUNT];
struct vecf asteroids_vel[ASTEROID_COUNT];

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
        dpixel(asteroids_pos[i].x, asteroids_pos[i].y, C_BLACK);
    }
}

int main(void)
{
    for (int i=0; i<ASTEROID_COUNT; i++)
    {
        asteroids_pos[i].x = (float) fmod(rand(), 128);
        asteroids_pos[i].y = (float) fmod(rand(), 64);

        asteroids_vel[i].x = RAND_FLOAT(-0.05, 0.05);
        asteroids_vel[i].y = RAND_FLOAT(-0.05, 0.05);
    }

    while (1)
    {
        draw();
        dupdate();
        clearevents();
        if (keydown(KEY_LEFT))
        {
            p_dir -= 0.1f;
        }
        if (keydown(KEY_RIGHT))
        {
            p_dir += 0.1f;
        }
        if (keydown(KEY_UP))
        {
            struct vecf thrust_vector_rotated = rotate(thrust_vector, p_dir);
            p_vel.x += thrust_vector_rotated.x;
            p_vel.y += thrust_vector_rotated.y;
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
        }

        sleep_ms(10);
    }
	return 1;
}
