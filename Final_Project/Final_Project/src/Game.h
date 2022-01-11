#pragma once
#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "GameObject.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);

// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

class Game
{
public:
    // game state
    GameState State;
    bool Keys[1024];
    unsigned int Width, Height;

    std::vector<GameLevel> Levels;
    GLuint Level;

    // constructor/destructor
    Game(GLuint width, GLuint height);
    ~Game();

    // initialize game state (load all shaders/textures/levels)
    void Init();

    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
};

