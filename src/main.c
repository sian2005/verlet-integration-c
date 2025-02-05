#include "raylib.h"
#include "raymath.h"
#include "stdio.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

#define WINDOW_TITLE "Verlet Integration"

// Particle
typedef struct Particle {
    Vector2 pos;
    Vector2 prv;
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
    Vector2 directionParticle = Vector2Subtract(particle->pos, center);
    float directionLength = Vector2Length(directionParticle);
    
    if (directionLength > r - particle->mass)
    {
        Vector2 normal = Vector2Scale(directionParticle, 1 / directionLength);
        particle->pos = Vector2Subtract(particle->pos, Vector2Scale(normal, directionLength + particle->mass - r));

        const Vector2 displacement = Vector2Subtract(particle->pos, particle->prv);

        particle->prv = Vector2Add(particle->pos, Vector2Negate(Vector2Reflect(displacement, normal)));
    }
}

static void updateParticle(struct Particle *particle, double dt)
{
    // Compute how much we moved
    const Vector2 displacement = Vector2Subtract(particle->pos, particle->prv);
    // Update position
    particle->prv = particle->pos;
    particle->pos = Vector2Add(Vector2Add(particle->pos, displacement), Vector2Scale(particle->acc, dt * dt));
    // Reset acceleration
    particle->acc = applyForces();

    collideParticle(particle, (Vector2) { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }, 200.0);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Particle particle = {
        {SCREEN_WIDTH / 2      , SCREEN_HEIGHT / 2},
        {SCREEN_WIDTH / 2 - 2  , SCREEN_HEIGHT / 2},
        {0.0                   , 0.0              },
         10.0
    };

    const float speedScale = 5;

    while (!WindowShouldClose())
    {
        updateParticle(&particle, GetFrameTime() * speedScale);
        
        BeginDrawing();

        ClearBackground(BLACK);

        DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 200, RAYWHITE);
        DrawCircleV(particle.pos, particle.mass, PURPLE);

        EndDrawing();

        //printf("%f\t%f\n", particle.pos.x, particle.pos.y);
    }

    CloseWindow();

    return 0;
}
