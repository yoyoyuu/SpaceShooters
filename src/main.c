#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

//----------------------------------------------------------------------------------
//  Algunos defines
//----------------------------------------------------------------------------------
#define NUM_DISPAROS 50
#define NUM_MAX_ASTEROIDES 10
#define OBJETIVOS_WAVE 5
#define MAX_WAVE 3
#define PUNTOS_OBJETIVO 100
#define PUNTOS_ERROR -10
#define ASTE_MIN_DISTANCIA 100

#define EQUIVALENCIA 1
#define IDENTIFICACION 0

#define ANCHO_BOTON 450
#define ALTO_BOTON 60

#define TAM_TEXT_BOTON 35

//----------------------------------------------------------------------------------
// Definición de tipos y estructuras
//----------------------------------------------------------------------------------
typedef struct _fraccion
{
    int numerador;
    int denominador;
} Ts_fraccion;

typedef struct _fraccionIdent
{
    Ts_fraccion fraccion;
    const char *rutaImagen; // Ruta de la imagen del asteroide
} Ts_fraccionIdent;

typedef struct _fraccionEquiv
{
    Ts_fraccion objetivo;
    Ts_fraccion equivalentes[3];
} Ts_fraccionEquiv;

typedef struct Ts_nave
{
    Rectangle rec;
    Vector2 velocidad;
    Color color;
    Texture2D texture;
} Ts_nave;

typedef struct _fondo
{
    Texture2D texture;
    float x;
    float velocidad;
} Ts_fondo;

typedef struct _circulo
{
    float x;
    float y;
    float radio;
} Ts_circulo;

typedef struct _asteroide
{
    Ts_circulo circulo;
    Vector2 velocidad;
    bool active;
    Color color;
    Ts_fraccion valor;
    char textoFrac[10];
    bool esObjetivo;
    Texture2D asteroideTextura; // Texturas para el asteroide
} Ts_asteroide;

typedef struct _disparo
{
    Rectangle rec;
    Vector2 velocidad;
    bool active;
    Color color;
} Ts_disparo;

typedef struct _boton
{
    Rectangle rec;
    const char *text;
    Color color;
} Ts_boton;

typedef struct _sonidos
{
    Sound disparoSound;
    Sound choqueSound;
    Sound correctoSound;
    Sound incorrectoSound;
    Sound victoriaSound;
    Sound gameOverSound;
    Sound fondoSound;
} Ts_sonidos;

typedef struct _estadoJuego
{
    int currentWave;
    int score;
    int targetsHit;
    bool gameOver;
    bool pause;
    bool victory;
    Ts_fraccion targetFraction;
    int currentDifficulty;
    char waveMessage[50];
    float messageAlpha;
    bool shouldFadeMessage;
    Ts_fondo background;
    int playerLives;
    bool lostLife;
    float lostLifeMessageTime;
    bool AudioReproducido;
} Ts_estadoJuego;

typedef struct _puntos
{
    char username[10];
    int score;
    int nivel; // nivel EQUIVALENCIA o IDENTIFICACION
} Ts_puntos;

// Variables globales
//----------------------------------------------------------------------------------
// Ancho de la ventana del juego
static const int screenWidth = 1000;

// Alto de la ventana del juego
static const int screenHeight = 800;

// Estructura que representa al jugador
static Ts_nave player = {0};

// Arreglo que contiene los asteroides en el juego
static Ts_asteroide asteroide[NUM_MAX_ASTEROIDES] = {0};

// Arreglo que contiene los disparos realizados por el jugador
static Ts_disparo disparo[NUM_DISPAROS] = {0};

// Estructura que mantiene el estado del juego (puntuación, waves, etc.)
static Ts_estadoJuego gameState = {0};

// Contador para la tasa de disparo
static int shootRate = 0;

// Arreglo que almacena las puntuaciones de los jugadores
static Ts_puntos scores[2][20];

// Estructura que contiene los sonidos del juego
Ts_sonidos sonido;

// Bandera que indica si se ha reproducido un sonido (ESCENCIAL EN LA PANTALLA GAME OVER)
bool sonidoReproducido = false;

// Prototipos
//----------------------------------------------------------------------------------

// Muestra la pantalla de menú principal----------------------------------------------------------------------------------------------------------------------------------------
void pantallaInicio(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Muestra la pantalla de puntuaciones------------------------------------------------------------------------------------------------------------------------------------------
void pantallaPuntuaciones(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Muestra la pantalla de controles---------------------------------------------------------------------------------------------------------------------------------------------
void pantallaControles(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Muestra la pantalla de selección de dificultad-------------------------------------------------------------------------------------------------------------------------------
void pantallaDificultad(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Inicializa el estado del juego y los recursos--------------------------------------------------------------------------------------------------------------------------------
void inicializarJuego(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Inicializa el estado de los asteroides y los recursos------------------------------------------------------------------------------------------------------------------------
void inicializarAsteroides(Ts_asteroide asteroide[], Texture2D asteroideTextura[], Texture2D identificacionTextures[], int numasteroides, int Width, int screenHeight);
// Actualiza el estado del juego------------------------------------------------------------------------------------------------------------------------------------------------
void actualiJuego(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Actualiza el estado de los asteroides----------------------------------------------------------------------------------------------------------------------------------------
void actualiAsteroides(Ts_asteroide asteroide[], int numasteroides, Rectangle playerRec, Texture2D asteroideTextura[], Texture2D identificacionTextures[]);
// Actualiza el estado de los disparos------------------------------------------------------------------------------------------------------------------------------------------
void actualiDisparos(Ts_disparo disparo[], int numShots, Ts_asteroide asteroide[], int numasteroides);
// Actualiza el estado del frame------------------------------------------------------------------------------------------------------------------------------------------------
void actualiFramePantalla(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Dibuja el estado del juego---------------------------------------------------------------------------------------------------------------------------------------------------
void dibujarJuego(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Dibuja el estado de los asteroides-------------------------------------------------------------------------------------------------------------------------------------------
void dibujarAsteroides(Ts_asteroide asteroide[], int numasteroides, Texture2D asteroideTextura[], Texture2D identificacionTextures[]);
// Dibuja el estado de los disparos---------------------------------------------------------------------------------------------------------------------------------------------
void dibujarDisparos(Ts_disparo disparo[], int numShots);
// Verifica si dos fracciones son iguales---------------------------------------------------------------------------------------------------------------------------------------
bool sonFraccionesIguales(Ts_fraccion f1, Ts_fraccion f2);
// Verifica si una fracción es equivalente a una fracción objetivo--------------------------------------------------------------------------------------------------------------
bool sonFraccionesEquiv(Ts_fraccion fraccion1, Ts_fraccion fraccion2);
// Devuelve una fracción equivalente aleatoria----------------------------------------------------------------------------------------------------------------------------------
Ts_fraccion fraccionEquivRandom();
// Devuelve una fracción de identificación aleatoria----------------------------------------------------------------------------------------------------------------------------
Ts_fraccion fraccionIdentRandom();
// Inicia una nueva ola de asteroides-------------------------------------------------------------------------------------------------------------------------------------------
void iniciaWave(Texture2D asteroideTextura[], Texture2D identificacionTextures[], int numIdentificacionTextures);
// Verifica si un asteroide está cerca del jugador------------------------------------------------------------------------------------------------------------------------------
bool asteroideCerca(int index);
// Muestra la pantalla de juego terminado---------------------------------------------------------------------------------------------------------------------------------------
void pantallaGameOver(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Maneja eventos de clic en botones--------------------------------------------------------------------------------------------------------------------------------------------
bool botonClick(Ts_boton button);
// Dibuja un botón--------------------------------------------------------------------------------------------------------------------------------------------------------------
void dibujarBoton(Ts_boton button);
// Descarga las texturas del juego----------------------------------------------------------------------------------------------------------------------------------------------
void UnloadGame(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[]);
// Mueve el fondo---------------------------------------------------------------------------------------------------------------------------------------------------------------
void MoverFondo();
// Guarda las puntuaciones------------------------------------------------------------------------------------------------------------------------------------------------------
void guardarPuntos(Ts_puntos score);
// Muestra la pantalla de guardar puntuaciones----------------------------------------------------------------------------------------------------------------------------------
void pantallaGuardarPuntos();

//------------------------------------------------------------------------------------
// Main game code
//------------------------------------------------------------------------------------
int main()
{
    InitAudioDevice();
    InitWindow(screenWidth, screenHeight, "Math Shooter: fracciones espaciales");

    if (access("scores.dll", F_OK) == -1)
    {
        // El archivo no existe, lo creamos
        FILE *scoresFile = fopen("scores.dll", "w");
        if (scoresFile != NULL)
        {
            // El archivo se creó correctamente
            fclose(scoresFile);
        }
        else
        {
            // Error al crear el archivo
            printf("Error al crear el archivo scores.dll\n");
        }
    }

    // Cargar texturas
    Texture2D playerTexture = LoadTexture("resources/nave.png");
    Texture2D asteroideTextura[1];
    asteroideTextura[0] = LoadTexture("resources/asteroidesEq/asteroide.png");
    Texture2D fondoTexture = LoadTexture("resources/fondo.png");

    Texture2D identificacionTextures[] = {
        LoadTexture("resources/asteroidesIdent/asteroide_1_2.png"),
        LoadTexture("resources/asteroidesIdent/asteroide_1_3.png"),
        LoadTexture("resources/asteroidesIdent/asteroide_1_4.png"),
        LoadTexture("resources/asteroidesIdent/asteroide_1_8.png"),
        LoadTexture("resources/asteroidesIdent/asteroide_2_3.png"),
        LoadTexture("resources/asteroidesIdent/asteroide_3_4.png"),
        LoadTexture("resources/asteroidesIdent/asteroide_1_1.png")};

    sonido.fondoSound = LoadSound("resources/sonidos/fondo.wav");

    sonido.gameOverSound = LoadSound("resources/sonidos/GameOver.wav");
    sonido.victoriaSound = LoadSound("resources/sonidos/victoria.mp3");
    sonido.disparoSound = LoadSound("resources/sonidos/disparo.wav");
    sonido.choqueSound = LoadSound("resources/sonidos/choque.mp3");
    sonido.correctoSound = LoadSound("resources/sonidos/correcto.mp3");
    sonido.incorrectoSound = LoadSound("resources/sonidos/incorrecto.wav");

    SetTargetFPS(60);

    // Reproducir sonido de fondo
    PlaySound(sonido.fondoSound);

    inicializarJuego(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);

    pantallaInicio(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);

    while (!WindowShouldClose())
    {
        // Actualizar el juego
        actualiFramePantalla(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
    }

    // Descargar texturas
    UnloadGame(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
    UnloadSound(sonido.fondoSound);
    UnloadSound(sonido.gameOverSound);
    UnloadSound(sonido.victoriaSound);
    UnloadSound(sonido.disparoSound);
    UnloadSound(sonido.choqueSound);
    UnloadSound(sonido.correctoSound);
    UnloadSound(sonido.incorrectoSound);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}

void pantallaInicio(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    Texture2D titulo = LoadTexture("resources/titulo.png");

    // Definir el ancho de los botones
    const int buttonWidth = ANCHO_BOTON; // Ancho de los botones
    const int buttonHeight = ALTO_BOTON; // Alto de los botones

    // Calcular la posición X centrada
    int buttonX = (screenWidth - buttonWidth) / 2;

    // Crear botones
    Ts_boton difficultyButton = {
        .rec = {buttonX, 300, buttonWidth, buttonHeight},
        .text = "JUGAR",
        .color = PURPLE};

    Ts_boton scoresButton = {
        .rec = {buttonX, 390, buttonWidth, buttonHeight},
        .text = "Puntuaciones",
        .color = PURPLE};

    Ts_boton controlsButton = {
        .rec = {buttonX, 480, buttonWidth, buttonHeight},
        .text = "Controles",
        .color = PURPLE};

    Ts_boton exitButton = {
        .rec = {buttonX, 570, buttonWidth, buttonHeight},
        .text = "Salir",
        .color = PURPLE};

    bool salir = false;

    while (true)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Dibujar y mover fondo
        MoverFondo();

        // Dibujar título del menú
        DrawTexture(titulo, (screenWidth - titulo.width) / 2, 100, WHITE);
        // Dibujar botones
        dibujarBoton(difficultyButton);
        dibujarBoton(scoresButton);
        dibujarBoton(controlsButton);
        dibujarBoton(exitButton);

        const char *nombre1 = "Universidad Autónoma de Baja California";
        int textoIzquierdaX = 10;
        int textoIzquierdaY = screenHeight - 30;
        DrawText(nombre1, textoIzquierdaX, textoIzquierdaY, 20, DARKGRAY);

        const char *nombre2 = "Programación estructurada";
        int textoIzquierdaY2 = textoIzquierdaY - 25;
        DrawText(nombre2, textoIzquierdaX, textoIzquierdaY2, 20, DARKGRAY);

        const char *nombre3 = "Mykytuk Ayvar Fanny Lillian";
        int textoDerechaX = screenWidth - MeasureText(nombre3, 20) - 10;
        int textoDerechaY = screenHeight - 30;
        DrawText(nombre3, textoDerechaX, textoDerechaY, 20, DARKGRAY);

        const char *nombre4 = "Lopez Cisneros Isis Vanesa";
        int textoDerechaY2 = textoDerechaY - 25;
        DrawText(nombre4, textoDerechaX, textoDerechaY2, 20, DARKGRAY);

        EndDrawing();

        // Comprobar si se hace clic en los botones
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePosition = GetMousePosition();

            // Botón "Selección de dificultad"
            if (botonClick(difficultyButton))
            {
                pantallaDificultad(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                break;
            }
            else
            {
                if (botonClick(scoresButton))
                {
                    pantallaPuntuaciones(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                    break;
                }
                else
                {
                    if (botonClick(controlsButton))
                    {
                        pantallaControles(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                        break;
                    }
                    else
                    {
                        if (botonClick(exitButton))
                        {
                            CloseWindow();
                            exit(0);
                        }
                    }
                }
            }
        }
    }
    UnloadTexture(titulo);
}

void pantallaPuntuaciones(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    Texture2D titulo = LoadTexture("resources/titulopuntuaciones.png");

    const int buttonWidth = ANCHO_BOTON; // Ancho de los botones
    const int buttonHeight = ALTO_BOTON; // Alto de los botones

    // Calcular la posición X centrada
    int buttonX = (screenWidth - buttonWidth) / 2;

    // Crear botón
    Ts_boton backButton = {
        .rec = {buttonX, 650, buttonWidth, buttonHeight},
        .text = "Volver al menú",
        .color = PURPLE};

    // Abrir el archivo binario
    FILE *file = fopen("scores.dll", "r+b");
    if (file == NULL)
    {
        printf("Error al abrir el archivo\n");
        return;
    }

    // Leer las puntuaciones del archivo
    Ts_puntos scores[20];
    int numScores = 0;
    for (int i = 0; i < 20; i++)
    {
        if (fread(&scores[i], sizeof(Ts_puntos), 1, file) != 1)
        {
            break;
        }
        numScores++;
    }
    fclose(file);

    // Ordenar las puntuaciones
    for (int i = 0; i < numScores; i++)
    {
        for (int j = i + 1; j < numScores; j++)
        {
            if (scores[i].score < scores[j].score)
            {
                Ts_puntos temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    while (true)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        MoverFondo();

        // Dibujar título
        DrawTexture(titulo, (screenWidth - titulo.width) / 2, 100, WHITE);

        // Dibujar encabezado de la tabla
        DrawText("NIVEL", 200, 250, 20, PINK);
        DrawText("NOMBRE", 400, 250, 20, PINK);
        DrawText("PUNTUACION", 600, 250, 20, PINK);

        // Dibujar puntuaciones
        if (numScores > 0)
        {
            for (int i = 0; i < numScores; i++)
            {
                char *nivelStr = (scores[i].nivel == 0) ? "Identificación" : "Equivalencia";

                DrawText(nivelStr, 200, 300 + (i * 20), 20, PINK);
                DrawText(scores[i].username, 400, 300 + (i * 20), 20, PINK);
                DrawText(TextFormat("%d", scores[i].score), 600, 300 + (i * 20), 20, PINK);
            }
        }
        else
        {
            DrawText("Sin puntuaciones", 300, 300, 20, PINK);
        }

        dibujarBoton(backButton);

        EndDrawing();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePosition = GetMousePosition();

            if (botonClick(backButton))
            {
                pantallaInicio(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                break;
            }
        }
    }
    UnloadTexture(titulo);
}

void pantallaControles(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    Texture2D titulo = LoadTexture("resources/titulocontroles.png");

    const int buttonWidth = ANCHO_BOTON;
    const int buttonHeight = ALTO_BOTON;

    int buttonX = (screenWidth - buttonWidth) / 2;

    Ts_boton backButton = {
        .rec = {buttonX, 650, buttonWidth, buttonHeight},
        .text = "Volver al menú",
        .color = PURPLE};

    // Cargar imágenes de flecha
    Texture2D flechaAbajo = LoadTexture("resources/teclas/abajo.png");
    Texture2D flechaArriba = LoadTexture("resources/teclas/arriba.png");
    Texture2D flechaIzquierda = LoadTexture("resources/teclas/izquierda.png");
    Texture2D flechaDerecha = LoadTexture("resources/teclas/derecha.png");

    while (true)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        MoverFondo();

        // Dibujar título de la pantalla de controles
        DrawTexture(titulo, (screenWidth - titulo.width) / 2, 100, WHITE);

        // Dibujar controles
        DrawText("Mover la nave:", 100, 300, 30, PINK);
        DrawText("Teclas de control", 400, 300, 30, PURPLE);
        DrawTexture(flechaDerecha, 750, 300, WHITE);
        DrawTexture(flechaIzquierda, 700, 300, WHITE);
        DrawTexture(flechaArriba, 725, 270, WHITE);
        DrawTexture(flechaAbajo, 725, 330, WHITE);

        DrawText("Disparar:", 100, 430, 30, PINK);
        DrawText("Tecla spacio", 400, 430, 30, PURPLE);

        DrawText("Pausa y reanudar:", 100, 530, 30, PINK);
        DrawText("Tecla 'P'", 400, 530, 30, PURPLE);

        dibujarBoton(backButton);

        EndDrawing();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePosition = GetMousePosition();

            if (botonClick(backButton))
            {
                // Descargar imágenes de flecha
                UnloadTexture(flechaAbajo);
                UnloadTexture(flechaArriba);
                UnloadTexture(flechaIzquierda);
                UnloadTexture(flechaDerecha);

                pantallaInicio(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                break;
            }
        }
    }
    UnloadTexture(titulo);
}

void pantallaDificultad(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    Texture2D titulo = LoadTexture("resources/titulodificultades.png");

    const int buttonWidth = ANCHO_BOTON;
    const int buttonHeight = ALTO_BOTON;

    int buttonX = (screenWidth - buttonWidth) / 2;

    Ts_boton identificationButton = {
        .rec = {buttonX, 300, buttonWidth, buttonHeight},
        .text = "Identificación",
        .color = PURPLE};

    Ts_boton equivalenceButton = {
        .rec = {buttonX, 390, buttonWidth, buttonHeight},
        .text = "Equivalencia",
        .color = PURPLE};

    Ts_boton backButton = {
        .rec = {buttonX, 480, buttonWidth, buttonHeight},
        .text = "Inicio",
        .color = PURPLE};

    while (true)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        MoverFondo();

        DrawTexture(titulo, (screenWidth - titulo.width) / 2, 100, WHITE);

        dibujarBoton(identificationButton);
        dibujarBoton(equivalenceButton);
        dibujarBoton(backButton);

        EndDrawing();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mousePosition = GetMousePosition();

            // Botón "Identificación"
            if (botonClick(identificationButton))
            {
                gameState.currentDifficulty = IDENTIFICACION;
                inicializarJuego(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                break;
            }
            else
            {
                // Botón "Equivalencia"
                if (botonClick(equivalenceButton))
                {
                    gameState.currentDifficulty = EQUIVALENCIA;
                    inicializarJuego(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                    break;
                }
                else
                {
                    // Botón "Volver al menú principal"
                    if (botonClick(backButton))
                    {
                        pantallaInicio(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                        break;
                    }
                }
            }
        }
    }
    UnloadTexture(titulo);
}

void inicializarJuego(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    // Inicializar variables del juego
    gameState.currentWave = 1;
    gameState.score = 0;
    gameState.targetsHit = 0;
    gameState.gameOver = false;
    gameState.pause = false;
    gameState.victory = false;

    // Inicializar nave
    player.rec.x = screenWidth / 2;
    player.rec.y = screenHeight - 50;
    player.rec.width = 50;
    player.rec.height = 50;
    player.velocidad.x = 5; // Velocidad horizontal
    player.velocidad.y = 5; // Velocidad vertical
    player.color = WHITE;
    player.texture = playerTexture;

    // Inicializar vidas
    gameState.playerLives = 5;            // agregue esto nuevo******************
    gameState.lostLife = false;           // inicializar la bandera
    gameState.lostLifeMessageTime = 0.0f; // inicializar el temporizador

    // Inicializar asteroides

    inicializarAsteroides(asteroide, asteroideTextura, identificacionTextures, NUM_MAX_ASTEROIDES, screenWidth, screenHeight);

    // Inicializar disparos
    for (int i = 0; i < NUM_DISPAROS; i++)
    {
        disparo[i].rec.x = 0;
        disparo[i].rec.y = 0;
        disparo[i].rec.width = 5;   // Ancho del disparo
        disparo[i].rec.height = 10; // Alto del disparo
        disparo[i].velocidad.x = 5; // Velocidad horizontal del disparo
        disparo[i].velocidad.y = 0; // Velocidad vertical del disparo
        disparo[i].active = false;  // El disparo no está activo al principio
        disparo[i].color = MAROON;  // Color del disparo
    }

    // Inicializar fondo
    gameState.background.texture = fondoTexture;
    gameState.background.x = 0;
    gameState.background.velocidad = 100.0f;

    gameState.messageAlpha = 1.0f;
    gameState.shouldFadeMessage = false;

    iniciaWave(asteroideTextura, identificacionTextures, NUM_MAX_ASTEROIDES);
}

void inicializarAsteroides(Ts_asteroide asteroide[], Texture2D asteroideTextura[], Texture2D identificacionTextures[], int numasteroides, int Width, int screenHeight)
{

    for (int i = 0; i < numasteroides; i++)
    {
        asteroide[i].circulo.radio = 20; // Radio para los asteroides
        do
        {
            asteroide[i].circulo.x = GetRandomValue(screenWidth, screenWidth + 2000);
            asteroide[i].circulo.y = GetRandomValue(asteroide[i].circulo.radio, screenHeight - asteroide[i].circulo.radio);
        } while (asteroideCerca(i));

        asteroide[i].velocidad.x = GetRandomValue(2, 4);
        asteroide[i].velocidad.y = 0;
        asteroide[i].active = true;
        asteroide[i].color = GRAY; // Color de los asteroides

        // Asignar valor a la fracción según la dificultad
        if (gameState.currentDifficulty == EQUIVALENCIA)
        {
            asteroide[i].valor = fraccionEquivRandom(); // Obtener una fracción equivalente
        }
        else
        {
            if (gameState.currentDifficulty == IDENTIFICACION)
            {
                asteroide[i].valor = fraccionIdentRandom(); // Obtener una fracción de identificación
            }
        }

        sprintf(asteroide[i].textoFrac, "%d/%d", asteroide[i].valor.numerador, asteroide[i].valor.denominador);
    }
    TraceLog(LOG_INFO, "Asteroides inicializados.");
}

void actualiJuego(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    if (!gameState.gameOver && !gameState.victory)
    {
        // Mueve el fondo
        MoverFondo();

        if (IsKeyPressed('P'))
            gameState.pause = !gameState.pause;

        if (!gameState.pause)
        {
            if (gameState.lostLife)
            {
                gameState.lostLifeMessageTime -= GetFrameTime(); // Reducir el temporizador
                if (gameState.lostLifeMessageTime <= 0.0f)
                {
                    gameState.lostLife = false; // Ocultar el mensaje cuando se termine el tiempo
                }
            }

            // Update message alpha
            if (gameState.shouldFadeMessage)
            {
                gameState.messageAlpha -= 0.02f;
                if (gameState.messageAlpha <= 0.0f)
                {
                    gameState.messageAlpha = 0.0f;
                    gameState.shouldFadeMessage = false;
                }
            }

            // Movimiento de la nave
            if (IsKeyDown(KEY_RIGHT))
            {
                player.rec.x += player.velocidad.x;
            }
            if (IsKeyDown(KEY_LEFT))
            {
                player.rec.x -= player.velocidad.x;
            }
            if (IsKeyDown(KEY_UP))
            {
                player.rec.y -= player.velocidad.y;
            }
            if (IsKeyDown(KEY_DOWN))
            {
                player.rec.y += player.velocidad.y;
            }

            // Agregar estas líneas para evitar que la nave se vaya más allá de la ventana
            if (player.rec.x < 0)
            {
                player.rec.x = 0;
            }
            if (player.rec.x + player.rec.width > screenWidth)
            {
                player.rec.x = screenWidth - player.rec.width;
            }
            if (player.rec.y < 0)
            {
                player.rec.y = 0;
            }
            if (player.rec.y + player.rec.height > screenHeight)
            {
                player.rec.y = screenHeight - player.rec.height;
            }

            // Disparos
            if (IsKeyDown(KEY_SPACE))
            {
                PlaySound(sonido.disparoSound);
                shootRate += 5;
                for (int i = 0; i < NUM_DISPAROS; i++)
                {
                    if (!disparo[i].active && shootRate % 20 == 0)
                    {
                        disparo[i].rec.x = player.rec.x + player.rec.width;
                        disparo[i].rec.y = player.rec.y + player.rec.height / 2;
                        disparo[i].active = true;
                        break;
                    }
                }
            }

            // Actualizar asteroides
            if (gameState.currentDifficulty == EQUIVALENCIA)
            {
                actualiAsteroides(asteroide, NUM_MAX_ASTEROIDES, player.rec, asteroideTextura, identificacionTextures);
            }
            else
            {
                if (gameState.currentDifficulty == IDENTIFICACION)
                {
                    actualiAsteroides(asteroide, NUM_MAX_ASTEROIDES, player.rec, asteroideTextura, identificacionTextures);
                }
            }

            // Actualizar disparos y checar colisiones
            actualiDisparos(disparo, NUM_DISPAROS, asteroide, NUM_MAX_ASTEROIDES);

            // Verificar si el wave ha terminado
            if (gameState.targetsHit >= OBJETIVOS_WAVE)
            {
                if (gameState.currentWave < MAX_WAVE)
                {
                    gameState.currentWave++;
                    gameState.targetsHit = 0;
                    // Llamar a StartNewWave en el momento adecuado, por ejemplo, al iniciar una nueva ola
                    if (gameState.currentDifficulty == EQUIVALENCIA)
                    {
                        iniciaWave(asteroideTextura, asteroideTextura, NUM_MAX_ASTEROIDES);
                    }
                    else
                    {
                        if (gameState.currentDifficulty == IDENTIFICACION)
                        {
                            iniciaWave(asteroideTextura, identificacionTextures, NUM_MAX_ASTEROIDES);
                        }
                    }
                }
                else
                {
                    gameState.victory = true;
                }
            }
        }
        else
        {
            if (gameState.gameOver || gameState.victory)
            {
                pantallaGameOver(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
            }
        }
    }
}

void actualiAsteroides(Ts_asteroide asteroide[], int numasteroides, Rectangle playerRec, Texture2D asteroideTextura[], Texture2D identificacionTextures[])
{
    for (int i = 0; i < numasteroides; i++)
    {
        if (asteroide[i].active)
        {
            // Mueve el asteroide hacia la izquierda
            asteroide[i].circulo.x -= asteroide[i].velocidad.x;

            // Si la nave choca contra un asteroide
            if (CheckCollisionCircleRec((Vector2){asteroide[i].circulo.x, asteroide[i].circulo.y},
                                        asteroide[i].circulo.radio, playerRec))
            {
                PlaySound(sonido.choqueSound);
                gameState.gameOver = true; // Establece el estado de Game Over
                break;
            }

            // Reinicia el asteroide si sale de la pantalla
            if (asteroide[i].circulo.x + asteroide[i].circulo.radio < 0)
            {
                asteroide[i].circulo.x = GetRandomValue(screenWidth, screenWidth + 500);
                asteroide[i].circulo.y = GetRandomValue(asteroide[i].circulo.radio, screenHeight - asteroide[i].circulo.radio);
                if (gameState.currentDifficulty == EQUIVALENCIA)
                {
                    asteroide[i].valor = fraccionEquivRandom();          // Obtener una nueva fracción equivalente
                    asteroide[i].asteroideTextura = asteroideTextura[0]; // Utilizar la textura de asteroide estándar
                }
                else
                {
                    if (gameState.currentDifficulty == IDENTIFICACION)
                    {
                        asteroide[i].valor = fraccionIdentRandom();                // Obtener una nueva fracción de identificación
                        asteroide[i].asteroideTextura = identificacionTextures[i]; // Utilizar una textura de asteroide aleatoria de la lista
                    }
                }
                sprintf(asteroide[i].textoFrac, "%d/%d", asteroide[i].valor.numerador, asteroide[i].valor.denominador);
            }
        }
    }
}

void actualiDisparos(Ts_disparo disparo[], int numShots, Ts_asteroide asteroide[], int numasteroides)
{
    for (int i = 0; i < numShots; i++)
    {
        if (disparo[i].active)
        {
            disparo[i].rec.x += disparo[i].velocidad.x;

            if (disparo[i].rec.x > screenWidth)
            {
                disparo[i].active = false;
            }

            for (int j = 0; j < numasteroides; j++)
            {
                if (asteroide[j].active &&
                    CheckCollisionCircleRec((Vector2){asteroide[j].circulo.x, asteroide[j].circulo.y},
                                            asteroide[j].circulo.radio, disparo[i].rec))
                {
                    disparo[i].active = false;

                    // Lógica de puntuación basada en la dificultad
                    if (gameState.currentDifficulty == EQUIVALENCIA)
                    {
                        if (sonFraccionesEquiv(asteroide[j].valor, gameState.targetFraction))
                        {
                            PlaySound(sonido.correctoSound);
                            gameState.score += PUNTOS_OBJETIVO;
                            gameState.targetsHit++;
                        }
                        else
                        {
                            PlaySound(sonido.incorrectoSound);
                            gameState.score += PUNTOS_ERROR;
                            gameState.playerLives--;
                            gameState.lostLife = true;
                            gameState.lostLifeMessageTime = 2.f;
                            if (gameState.playerLives <= 0)
                            {
                                gameState.gameOver = true; // se acaba el juego :c
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (gameState.currentDifficulty == IDENTIFICACION)
                        {

                            if (sonFraccionesIguales(asteroide[j].valor, gameState.targetFraction))
                            {
                                PlaySound(sonido.correctoSound);
                                gameState.score += PUNTOS_OBJETIVO;
                                gameState.targetsHit++;
                            }
                            else
                            {
                                PlaySound(sonido.incorrectoSound);
                                gameState.score += PUNTOS_ERROR;
                                gameState.playerLives--;
                                gameState.lostLife = true;
                                gameState.lostLifeMessageTime = 2.f;

                                if (gameState.playerLives <= 0)
                                {
                                    gameState.gameOver = true; // se acaba el juego :c
                                    break;
                                }
                            }
                        }
                    }

                    // Reinicia el asteroide
                    asteroide[j].circulo.x = GetRandomValue(screenWidth, screenWidth + 500);
                    asteroide[j].circulo.y = GetRandomValue(asteroide[j].circulo.radio, screenHeight - asteroide[j].circulo.radio);
                    asteroide[j].valor = (gameState.currentDifficulty == EQUIVALENCIA) ? fraccionEquivRandom() : fraccionIdentRandom();
                    sprintf(asteroide[j].textoFrac, "%d/%d", asteroide[j].valor.numerador, asteroide[j].valor.denominador);
                    break;
                }
            }
        }
    }
}

void actualiFramePantalla(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    actualiJuego(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
    dibujarJuego(playerTexture, asteroideTextura, fondoTexture, identificacionTextures); // Pasar la textura de la nave a DrawGame

    /*// Si el juego está en estado de Game Over o victoria, dibuja la pantalla de Game Over
    if (gameState.gameOver || gameState.victory)
    {
        pantallaGameOver(playerTexture, asteroideTextura, fondoTexture, identificacionTextures); // Pasar la textura de la nave
    }*/
}

void dibujarJuego(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (!gameState.gameOver && !gameState.victory)
    {
        MoverFondo();
        // Draw player
        DrawTexture(playerTexture, player.rec.x, player.rec.y, WHITE);

        // Draw enemies
        dibujarAsteroides(asteroide, NUM_MAX_ASTEROIDES, asteroideTextura, identificacionTextures); // Pasa las texturas de los asteroides

        // Draw shoots
        dibujarDisparos(disparo, NUM_DISPAROS);

        // Draw HUD
        DrawText(TextFormat("Puntuación: %04i", gameState.score), 20, 20, 20, GRAY);
        DrawText(TextFormat("Wave: %d", gameState.currentWave), 20, 50, 20, GRAY);
        DrawText(TextFormat("Objetivos disparados: %d/%d", gameState.targetsHit, OBJETIVOS_WAVE), 20, 110, 20, GRAY);
        // agregue esto nuevo, es para que muestre las vidas
        DrawText(TextFormat("Vidas: %d", gameState.playerLives), 20, 140, 20, GRAY); // cambios***************

        // AGREGUE ESTO, mostrar el mensaje de te equivocaste
        if (gameState.lostLife)
        {
            DrawText("¡Te equivocaste!", screenWidth / 2 - MeasureText("¡Te equivocaste!", 20) / 2,
                     screenHeight / 2, 20, RED);
        }

        // Draw objetivo Ts_fraccion message
        if (gameState.messageAlpha > 0.0f)
        {
            DrawText(gameState.waveMessage,
                     screenWidth / 2 - MeasureText(gameState.waveMessage, 30) / 2,
                     50, 30, Fade(RAYWHITE, gameState.messageAlpha));
        }

        if (gameState.pause)
        {
            DrawText("PAUSA",
                     screenWidth / 2 - MeasureText("PAUSA", 40) / 2,
                     screenHeight / 2 - 40, 40, GRAY);
        }
    }
    else
    {
        pantallaGameOver(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
    }

    EndDrawing();
}

void dibujarAsteroides(Ts_asteroide asteroide[], int numasteroides, Texture2D asteroideTextura[], Texture2D identificacionTextures[])
{
    for (int i = 0; i < numasteroides; i++)
    {
        if (asteroide[i].active)
        {
            Texture2D texturaAsteroide;

            if (gameState.currentDifficulty == EQUIVALENCIA)
            {
                texturaAsteroide = asteroideTextura[0]; // Utilizar la textura de asteroide estándar
            }
            else
            {
                if (gameState.currentDifficulty == IDENTIFICACION)
                {
                    // Utilizar la textura correspondiente según el valor de la fracción
                    switch (asteroide[i].valor.numerador)
                    {
                    case 1:
                        switch (asteroide[i].valor.denominador)
                        {
                        case 2:
                            texturaAsteroide = identificacionTextures[0];
                            break; // 1/2
                        case 3:
                            texturaAsteroide = identificacionTextures[1];
                            break; // 1/3
                        case 4:
                            texturaAsteroide = identificacionTextures[2];
                            break; // 1/4
                        case 8:
                            texturaAsteroide = identificacionTextures[3];
                            break; // 1/8
                        case 1:
                            texturaAsteroide = identificacionTextures[6];
                            break; // 1/1
                        }
                        break;
                    case 2:
                        switch (asteroide[i].valor.denominador)
                        {
                        case 3:
                            texturaAsteroide = identificacionTextures[4];
                            break; // 2/3
                        }
                        break;
                    case 3:
                        switch (asteroide[i].valor.denominador)
                        {
                        case 4:
                            texturaAsteroide = identificacionTextures[5];
                            break; // 3/4
                        }
                        break;
                    }
                }
            }

            // Dibuja la textura del asteroide centrada
            DrawTexture(texturaAsteroide,
                        asteroide[i].circulo.x - texturaAsteroide.width / 2,
                        asteroide[i].circulo.y - texturaAsteroide.height / 2,
                        WHITE);

            // Dibuja el texto centrado sobre la textura
            if (gameState.currentDifficulty == EQUIVALENCIA)
            {
                DrawText(asteroide[i].textoFrac,
                         asteroide[i].circulo.x - MeasureText(asteroide[i].textoFrac, 20) / 2,
                         asteroide[i].circulo.y - texturaAsteroide.height / 2 + 10, // Ajusta la posición vertical
                         18, WHITE);
            }
        }
    }
}

void dibujarDisparos(Ts_disparo disparo[], int numShots)
{
    for (int i = 0; i < numShots; i++)
    {
        if (disparo[i].active)
        {
            DrawRectangle(disparo[i].rec.x, disparo[i].rec.y, disparo[i].rec.width, disparo[i].rec.height, disparo[i].color);
        }
    }
}

bool sonFraccionesIguales(Ts_fraccion f1, Ts_fraccion f2)
{
    return (f1.numerador * f2.denominador) == (f2.numerador * f1.denominador);
}

bool sonFraccionesEquiv(Ts_fraccion fraccion1, Ts_fraccion fraccion2)
{
    return (fraccion1.numerador * fraccion2.denominador == fraccion2.numerador * fraccion1.denominador);
}

Ts_fraccion fraccionEquivRandom()
{
    static const Ts_fraccionEquiv fract_equivalentes[] = {
        {1, 2},
        {2, 4},
        {3, 6},
        {4, 8},
        {2, 3},
        {4, 6},
        {6, 9},
        {8, 12},
        {1, 4},
        {2, 8},
        {3, 12},
        {4, 16},
        {3, 4},
        {6, 8},
        {9, 12},
        {12, 16},
        {1, 3},
        {2, 6},
        {3, 9},
        {4, 12},
        {2, 5},
        {4, 10},
        {6, 15},
        {8, 20},
    };

    int index = rand() % 24; // Selecciona un índice aleatorio

    return fract_equivalentes[index].objetivo; // Devuelve una de las fracciones objetivo de manera aleatoria
}

Ts_fraccion fraccionIdentRandom()
{
    static const Ts_fraccionIdent availableFractions[] = {
        {{1, 2}},
        {{1, 3}},
        {{1, 4}},
        {{1, 8}},
        {{2, 3}},
        {{3, 4}},
        {{1, 1}}};

    int index = rand() % 7;                    // Selecciona un índice aleatorio
    return availableFractions[index].fraccion; // Devuelve la fracción del asteroide
}

void iniciaWave(Texture2D asteroideTextura[], Texture2D identificacionTextures[], int numIdentificacionTextures)
{
    if (gameState.currentDifficulty == EQUIVALENCIA)
    {
        // Nivel de Equivalencia: Usa fracciones equivalentes
        gameState.targetFraction = fraccionEquivRandom();

        sprintf(gameState.waveMessage, "Encuentra fracciones equivalentes a %d/%d! ",
                gameState.targetFraction.numerador, gameState.targetFraction.denominador);

        // Generar asteroides para la dificultad de equivalencia
        for (int i = 0; i < NUM_MAX_ASTEROIDES; i++)
        {
            asteroide[i].active = true;
            asteroide[i].circulo.radio = 20;
            do
            {
                asteroide[i].circulo.x = GetRandomValue(screenWidth, screenWidth + 2000);
                asteroide[i].circulo.y = GetRandomValue(asteroide[i].circulo.radio, screenHeight - asteroide[i].circulo.radio);
            } while (asteroideCerca(i));

            asteroide[i].velocidad.x = GetRandomValue(2, 4);
            asteroide[i].velocidad.y = 0;

            // Asignar fracciones equivalentes a los objetivos
            if (i < OBJETIVOS_WAVE)
            {
                asteroide[i].valor = fraccionEquivRandom();
                asteroide[i].esObjetivo = sonFraccionesEquiv(asteroide[i].valor, gameState.targetFraction) || sonFraccionesIguales(asteroide[i].valor, gameState.targetFraction);
                asteroide[i].color = asteroide[i].esObjetivo ? GREEN : RED; // Marcar fracciones correctas en verde
            }

            sprintf(asteroide[i].textoFrac, "%d/%d", asteroide[i].valor.numerador, asteroide[i].valor.denominador);
        }

        inicializarAsteroides(asteroide, asteroideTextura, identificacionTextures, NUM_MAX_ASTEROIDES, screenWidth, screenHeight);
    }
    else
    {
        if (gameState.currentDifficulty == IDENTIFICACION)
        {

            // Nivel de Identificación: Encuentra la fracción específica
            gameState.targetFraction = fraccionIdentRandom();

            sprintf(gameState.waveMessage, "Dispara a fracciones que representen %d/%d! ",
                    gameState.targetFraction.numerador, gameState.targetFraction.denominador);

            // Generar asteroides para la dificultad de identificación
            for (int i = 0; i < NUM_MAX_ASTEROIDES; i++)
            {
                asteroide[i].active = true;
                asteroide[i].circulo.radio = 20;
                do
                {
                    asteroide[i].circulo.x = GetRandomValue(screenWidth, screenWidth + 2000);
                    asteroide[i].circulo.y = GetRandomValue(asteroide[i].circulo.radio, screenHeight - asteroide[i].circulo.radio);
                } while (asteroideCerca(i));

                asteroide[i].velocidad.x = GetRandomValue(2, 4);
                asteroide[i].velocidad.y = 0;

                // Generar un valor aleatorio para el asteroide
                asteroide[i].valor = fraccionIdentRandom();
                // Determinar si este asteroide es una fracción objetivo
                asteroide[i].esObjetivo = sonFraccionesIguales(asteroide[i].valor, gameState.targetFraction);
                asteroide[i].color = asteroide[i].esObjetivo ? GREEN : RED; // Marcar fracciones correctas en verde

                sprintf(asteroide[i].textoFrac, "%d/%d", asteroide[i].valor.numerador, asteroide[i].valor.denominador);
            }

            inicializarAsteroides(asteroide, asteroideTextura, identificacionTextures, NUM_MAX_ASTEROIDES, screenWidth, screenHeight);
        }
    }
    TraceLog(LOG_INFO, "Wave started.");
}

bool asteroideCerca(int index)
{
    for (int j = 0; j < index; j++)
    {
        float dx = asteroide[index].circulo.x - asteroide[j].circulo.x;
        float dy = asteroide[index].circulo.y - asteroide[j].circulo.y;
        float distance = sqrtf(dx * dx + dy * dy);
        if (distance < ASTE_MIN_DISTANCIA)
            return true;
    }
    return false;
}

void pantallaGameOver(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    const int buttonWidth = ANCHO_BOTON;
    const int buttonHeight = ALTO_BOTON;

    // Calcular la posición X centrada
    int buttonX = (screenWidth - buttonWidth) / 2;

    // Crear botones
    Ts_boton playAgainButton = {
        .rec = {buttonX, 400, buttonWidth, buttonHeight},
        .text = "JUGAR DENUEVO",
        .color = PURPLE};

    Ts_boton difficultyMenuButton = {
        .rec = {buttonX, 490, buttonWidth, buttonHeight},
        .text = "Menú de Dificultad",
        .color = PURPLE};

    Ts_boton saveScoreButton = {
        .rec = {buttonX, 580, buttonWidth, buttonHeight},
        .text = "Guardar Puntuación",
        .color = PURPLE};

    if (!sonidoReproducido)
    {
        if (gameState.gameOver)
        {
            PlaySound(sonido.gameOverSound);
            sonidoReproducido = true;
        }
        else
        {
            if (gameState.victory)
            {
                PlaySound(sonido.victoriaSound);
                sonidoReproducido = true;
            }
        }
    }

    while (true)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(gameState.background.texture, gameState.background.x, 0, WHITE);
        DrawTexture(gameState.background.texture, (gameState.background.x + gameState.background.texture.width), 0, WHITE);

        DrawText(gameState.victory ? "¡GANASTE!" : "GAME OVER", (screenWidth - MeasureText(gameState.victory ? "¡GANASTE!" : "GAME OVER", 40)) / 2, 200, 40, gameState.victory ? GREEN : RED);

        DrawText(TextFormat("Puntuación Final: %04i", gameState.score), (screenWidth - MeasureText("Puntuación Final: 0000", 30)) / 2, 300, 30, GRAY);

        dibujarBoton(playAgainButton);
        dibujarBoton(difficultyMenuButton);
        dibujarBoton(saveScoreButton);

        EndDrawing();

        // Comprobar si se hace clic en los botones
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            sonidoReproducido = false;
            Vector2 mousePosition = GetMousePosition();

            // Botón "Jugar de Nuevo"
            if (botonClick(playAgainButton))
            {
                inicializarJuego(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                break;
            }
            else
            {
                if (botonClick(difficultyMenuButton))
                {
                    pantallaDificultad(playerTexture, asteroideTextura, fondoTexture, identificacionTextures);
                    break;
                }
                else
                {
                    if (botonClick(saveScoreButton))
                    {
                        pantallaGuardarPuntos();
                    }
                }
            }
        }
    }
}

bool botonClick(Ts_boton button)
{
    return CheckCollisionPointRec(GetMousePosition(), button.rec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void dibujarBoton(Ts_boton button)
{
    DrawRectangleRec(button.rec, button.color);
    DrawText(button.text,
             button.rec.x + (button.rec.width - MeasureText(button.text, TAM_TEXT_BOTON)) / 2,
             button.rec.y + (button.rec.height - TAM_TEXT_BOTON) / 2,
             TAM_TEXT_BOTON, PINK);
}

void UnloadGame(Texture2D playerTexture, Texture2D asteroideTextura[], Texture2D fondoTexture, Texture2D identificacionTextures[])
{
    UnloadTexture(playerTexture);
    UnloadTexture(asteroideTextura[0]);
    UnloadTexture(fondoTexture);
    for (int i = 0; i < 7; i++)
    {
        UnloadTexture(identificacionTextures[i]);
    }
}

void MoverFondo()
{
    gameState.background.x -= gameState.background.velocidad * GetFrameTime(); // Mueve el fondo hacia la izquierda

    // Reinicia la posición del fondo si se ha movido fuera de la pantalla
    if (gameState.background.x <= -gameState.background.texture.width)
    {
        gameState.background.x = 0; // Reinicia la posición
    }

    DrawTexture(gameState.background.texture, gameState.background.x, 0, WHITE);
    DrawTexture(gameState.background.texture, (gameState.background.x + gameState.background.texture.width), 0, WHITE);
}

void guardarPuntos(Ts_puntos score)
{
    FILE *file = fopen("scores.dll", "ab"); // Abrir el archivo
    if (file == NULL)
    {
        printf("Error al abrir el archivo\n");
        return;
    }
    fwrite(&score, sizeof(Ts_puntos), 1, file); // Escribir la estructura en el archivo
    fclose(file);
}

void pantallaGuardarPuntos()
{
    // Pedir el nombre del usuario
    Ts_puntos score;
    char inputTexto[10];
    int inputTextoLargo = 0;
    bool textoVacio = true; // Variable para verificar si el campo está vacío

    while (true)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexture(gameState.background.texture, gameState.background.x, 0, WHITE);
        DrawTexture(gameState.background.texture, (gameState.background.x + gameState.background.texture.width), 0, WHITE);

        DrawText("Ingrese su nombre:", screenWidth / 2 - 100, screenHeight / 2 - 50, 20, PINK);

        // Mostrar '?' si el campo está vacío y el usuario no ha escrito nada
        if (textoVacio)
        {
            DrawText("?", screenWidth / 2 - 100, screenHeight / 2 - 20, 30, PURPLE);
        }
        else
        {
            DrawText(inputTexto, screenWidth / 2 - 100, screenHeight / 2 - 20, 30, PURPLE); // Mostrar el texto ingresado
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            break;
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (inputTextoLargo > 0)
            {
                inputTextoLargo--;
                inputTexto[inputTextoLargo] = '\0';
                if (inputTextoLargo == 0) // Si se borra todo, marcar como vacío
                {
                    textoVacio = true;
                }
            }
        }
        else
        {
            if (IsKeyPressed(KEY_SPACE))
            {
                if (inputTextoLargo < 9)
                {
                    inputTexto[inputTextoLargo] = ' ';
                    inputTextoLargo++;
                    inputTexto[inputTextoLargo] = '\0';
                    textoVacio = false; // Cambiar a no vacío
                }
            }
            else
            {
                char c = GetCharPressed();
                if (c != '\0')
                {
                    if (inputTextoLargo < 9)
                    {
                        inputTexto[inputTextoLargo] = c;
                        inputTextoLargo++;
                        inputTexto[inputTextoLargo] = '\0';
                        textoVacio = false; // Cambiar a no vacío
                    }
                }
            }
        }

        EndDrawing();
    }

    strcpy(score.username, inputTexto);

    // Guardar la puntuación con el nivel
    score.score = gameState.score;
    score.nivel = gameState.currentDifficulty; // Agregamos el nivel

    // Buscamos la posición para guardar la puntuación
    int pos = 0;
    for (int i = 0; i < 10; i++)
    {
        if (scores[score.nivel][i].score < score.score)
        {
            pos = i;
            break;
        }
    }

    // Guardamos la puntuación
    scores[score.nivel][pos] = score;

    guardarPuntos(score);
}