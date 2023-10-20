#include <gint/display.h>
#include <gint/keyboard.h>
#include <math.h>
#include <gint/clock.h>
#include <gint/keycodes.h>

struct vecf
{
	float x;
	float y;
};

struct vecf p_pos = {64, 32};
struct vecf p_vel = {0.1f, 0.0f};
float p_dir;

struct vecf thrust_vector = {0.0f, 0.05f};

struct vecf p_vertices[] = {{0.0f, 3.0f}, {2.0f, -3.0f}, {-2.0f, -3.0f}};
#define p_vertices_length (int)(sizeof(p_vertices)/sizeof(struct vecf))

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
    for (int i=0; i<p_vertices_length; i++)
    {
        struct vecf vec_a = rotate(p_vertices[i], p_dir);
        struct vecf vec_b = rotate(p_vertices[(i+1 == p_vertices_length) ? 0 : i+1], p_dir);
        dline(vec_a.x + p_pos.x, vec_a.y + p_pos.y, vec_b.x + p_pos.x, vec_b.y + p_pos.y, C_BLACK);
    }
}

int main(void)
{
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

        sleep_ms(10);
    }
	return 1;
}
