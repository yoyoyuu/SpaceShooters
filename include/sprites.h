#include "raylib.h"
#include <stdio.h>

typedef struct Asteroid
{
    Texture2D texture; // Textura del asteroide
    Rectangle rec;     // Rectángulo que define la posición y tamaño del asteroide
    Vector2 speed;     // Velocidad del asteroide
    bool active;       // Estado del asteroide (activo o no)
} Asteroid;

#define NUM_MAX_ASTEROIDS 10
static Asteroid asteroids[NUM_MAX_ASTEROIDS];

void InitAsteroids()
{
    for (int i = 0; i < NUM_MAX_ASTEROIDS; i++)
    {
        // Cargar texturas de asteroides
        asteroids[i].texture = LoadTexture((i % 2 == 0) ? "resources/asteroid1.png" : "resources/asteroid2.png");
        asteroids[i].rec.width = asteroids[i].texture.width;
        asteroids[i].rec.height = asteroids[i].texture.height;

        // Posicionar asteroides fuera de la pantalla
        asteroids[i].rec.x = GetRandomValue(screenWidth, screenWidth + 200);
        asteroids[i].rec.y = GetRandomValue(0, screenHeight - asteroids[i].rec.height);

        // Establecer velocidad
        asteroids[i].speed.x = GetRandomValue(2, 5); // Velocidad aleatoria
        asteroids[i].active = true;                  // Activar asteroide
    }
}
