#include "raylib.h"
#include "raymath.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include <inttypes.h>

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)
#define MAX_PARTICLES_AMOUNT (400)
#define SUBSTEPS 5
#define RANDOMIZE_RADIUS
#define SPEED_SCALE 1
#define GRID_SIZE 50
#define MIN_PARTICLE_RADIUS 5
#define PARTITIONS (SCREEN_WIDTH / GRID_SIZE) * (SCREEN_HEIGHT / GRID_SIZE)

#define WINDOW_TITLE "Verlet Integration"

// Particle
typedef struct Particle {
    Vector2 pos;
    Vector2 prv;
    Vector2 acc;
#ifdef RANDOMIZE_RADIUS
    float mass;
#endif
    Color color;
} Particle;

static inline float getRadius(struct Particle* particle)
{
#ifdef RANDOMIZE_RADIUS
    return particle->mass;
#else
    return 10;
#endif
}

static inline struct Vector2 applyForces()
{
    Vector2 acc = { 0.0, 9.81 * 50 };

    // Apply additional forces

    return acc;
}

static inline void constrainParticle(struct Particle *particle, Vector2 center, float r)
{
    Vector2 directionParticle = Vector2Subtract(particle->pos, center);
    float directionLength = Vector2Length(directionParticle);
    
    if (directionLength > r - getRadius(particle))
    {
        Vector2 normal = Vector2Scale(directionParticle, 1 / directionLength);
        particle->pos = Vector2Subtract(particle->pos, Vector2Scale(normal, directionLength + getRadius(particle) - r));

        //const Vector2 displacement = Vector2Subtract(particle->pos, particle->prv);

        //particle->prv = Vector2Add(particle->pos, Vector2Negate(Vector2Reflect(displacement, normal)));
    }
}

static inline void collideParticle(struct Particle* p1, struct Particle* p2)
{
    Vector2 directionParticle = Vector2Subtract(p1->pos, p2->pos);
    float directionLength = Vector2Length(directionParticle);

    if (directionLength < getRadius(p1) + getRadius(p2))
    {
        Vector2 normal = Vector2Scale(directionParticle, 1 / directionLength);
        p1->pos = Vector2Add(p1->pos, Vector2Scale(normal, (getRadius(p1) + getRadius(p2) - directionLength) / 2));
        p2->pos = Vector2Subtract(p2->pos, Vector2Scale(normal, (getRadius(p1) + getRadius(p2) - directionLength) / 2));

        //const Vector2 displacement = Vector2Subtract(particle->pos, particle->prv);

        //particle->prv = Vector2Add(particle->pos, Vector2Negate(Vector2Reflect(displacement, normal)));
    }
}

static inline void updateParticle(struct Particle *particle, double dt)
{
    // Compute how much we moved
    const Vector2 displacement = Vector2Subtract(particle->pos, particle->prv);
    // Update position
    particle->prv = particle->pos;
    particle->pos = Vector2Add(Vector2Add(particle->pos, displacement), Vector2Scale(particle->acc, dt * dt));
    // Reset acceleration
    particle->acc = applyForces();
}

static inline void centerParticle(struct Particle *particle)
{
    particle->pos = (Vector2) { SCREEN_WIDTH / 2      , SCREEN_HEIGHT / 2 };
    particle->prv = (Vector2) { SCREEN_WIDTH / 2 + 10  , SCREEN_HEIGHT / 2 };
}

static inline void randomParticle(struct Particle *particle)
{
#ifdef RANDOMIZE_RADIUS
    particle->mass = rand() % 5 + MIN_PARTICLE_RADIUS;
#endif
    particle->color = (Color) { rand() % 20 + 100, rand() % 20 + 100, rand() % 20 + 256 - 20, 255 };
}

static inline void updateParticles(struct Particle *particles, int liveParticles, float dt)
{
    Particle* currentParticle;

    for (int i = 0; i < liveParticles; i++)
    {
        currentParticle = &particles[i];
        updateParticle(currentParticle, dt);
    }
}

static inline void collideParticles(struct Particle *particles, int liveParticles, int windowWidth, int windowHeight)
{
    Particle* currentParticle;

    for (int i = 0; i < liveParticles; i++)
    {
        currentParticle = &particles[i];

        for (int j = 0; j < liveParticles; j++)
        {
            if (currentParticle != &particles[j])
            {
                collideParticle(currentParticle, &particles[j]);
            }
        }

        constrainParticle(currentParticle, (Vector2) { windowWidth / 2, windowHeight / 2 }, min(windowWidth / 2, windowHeight / 2) - 30);
    }
}

static inline void offsetParticles(struct Particle *particles, int liveParticles, Vector2 windowDragOffset)
{
    Particle* currentParticle;

    for (int i = 0; i < liveParticles; i++)
    {
        currentParticle = &particles[i];
        currentParticle->pos = Vector2Add(currentParticle->pos, windowDragOffset);
    }
}

static inline void drawParticles(struct Particle *particles, int liveParticles)
{
    Particle* currentParticle;

    for (int i = 0; i < liveParticles; i++)
    {
        currentParticle = &particles[i];
        DrawCircleV(currentParticle->pos, getRadius(currentParticle), currentParticle->color);
    }
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    srand(time(NULL));

    char windowTitle[sizeof(WINDOW_TITLE" FPS: %d") + 2];
    
    Particle *particles;
    int liveParticles;

    // To support dragging of the window to influence the simulation
    Vector2 oldWindowPosition;
    Vector2 windowPosition;
    Vector2 windowDragOffset;
    int windowHeight;
    int windowWidth;

    liveParticles = 0;
    particles = (Particle*) malloc(sizeof(Particle) * MAX_PARTICLES_AMOUNT);
    cumulativePartitionSizes = (int*) malloc(sizeof(int) * PARTITIONS);
    oldWindowPosition = GetWindowPosition();
    windowWidth = SCREEN_WIDTH;
    windowHeight = SCREEN_HEIGHT;

    while (!WindowShouldClose())
    {
        snprintf(windowTitle, sizeof windowTitle, WINDOW_TITLE" FPS: %d", GetFPS());
        SetWindowTitle(windowTitle);
        
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();

        if (liveParticles < MAX_PARTICLES_AMOUNT)
        {
            Particle particle = {
                {windowWidth / 2                 , windowHeight / 2},
                {windowWidth / 2 + rand() % 2 - 1, windowHeight / 2},
                {0.0                             , 0.0              },
                 10.0
            };

            randomParticle(&particle);
            particles[liveParticles++] = particle;
        }

        windowPosition = GetWindowPosition();
        windowDragOffset = Vector2Subtract(windowPosition, oldWindowPosition);
        
        offsetParticles(particles, liveParticles, windowDragOffset);

        for (int s = 0; s < SUBSTEPS; s++)
        {
            updateParticles(particles, liveParticles, (GetFrameTime() / SUBSTEPS) * SPEED_SCALE);
            collideParticles(particles, liveParticles, windowWidth, windowHeight);
        }

        BeginDrawing();

        ClearBackground(BLACK);
        DrawCircle(windowWidth / 2, windowHeight / 2, min(windowWidth / 2, windowHeight / 2) - 30, RAYWHITE);

        drawParticles(particles, liveParticles);

        EndDrawing();

        oldWindowPosition = windowPosition;
        //printf("%f\t%f\n", particle.pos.x, particle.pos.y);
    }

    CloseWindow();

    return 0;
}
