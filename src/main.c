#include "raylib.h"
#include "raymath.h"
#include "stdio.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

#define WINDOW_TITLE "Verlet Integration"

// Particle
typedef struct Particle {
    Vector2 pos;
    Vector2 vel;
    Vector2 acc;
    float mass;
} Particle;

static struct Vector2 applyForces()
{
    Vector2 acc = { 0.0, 9.81 };

    // Apply additional forces

    return acc;
}

static void collideParticle(struct Particle *particle, Vector2 center, float r)
{
    float perimeterDistance = Vector2Length(Vector2Subtract(particle->pos, center)) + particle->mass - r;
    
    if (perimeterDistance > 0)
    {
        Vector2 normal = Vector2Normalize(Vector2Subtract(particle->pos, center));
        particle->pos = Vector2Subtract(particle->pos, Vector2Scale(normal, perimeterDistance));
        particle->vel = Vector2Negate(particle->vel);
    }
}

static void updateParticle(struct Particle *particle, double dt)
{
    particle->pos.x += particle->vel.x * dt + particle->acc.x * (dt * dt * 0.5);
    particle->pos.y += particle->vel.y * dt + particle->acc.y * (dt * dt * 0.5);
    Vector2 newAcc = applyForces();
    particle->vel.x += (particle->acc.x + newAcc.x) * (dt * 0.5);
    particle->vel.y += (particle->acc.y + newAcc.y) * (dt * 0.5);
    particle->acc = newAcc;

    collideParticle(particle, (Vector2) { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, 200.0);
}

static void initParticle(struct Particle *st, struct Vector2 *pos, struct Vector2 *vel, struct Vector2 *acc)
{
    st->pos = *pos;
    st->vel = *vel;
    st->acc = *acc;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Texture2D texture = LoadTexture(ASSETS_PATH"test.png"); // Check README.md for how this works
    Particle particle = {
        {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
        {25.0, 0.0},
        {0.0, 0.0},
        10.0
    };

    float speedScale = 5;

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 200, RAYWHITE);
        DrawCircleV(particle.pos, particle.mass, PURPLE);
        updateParticle(&particle, GetFrameTime() * speedScale);

        //printf("%f\t%f\n", particle.pos.x, particle.pos.y);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
