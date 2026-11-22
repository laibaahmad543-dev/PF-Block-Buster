#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
using namespace std;

//  CONSTANTS 

const int SCREEN_W = 600;
const int SCREEN_H = 700;

const int ROWS = 8;
const int COLS = 10;
const int BRICK_W = 60;
const int BRICK_H = 20;
const int BRICKS_Y = 60;

const int PADDLE_W = 100;
const int PADDLE_H = 15;
const int PADDLE_SPEED = 8;

const int BALL_SIZE = 10;
const int HUD_H = 40;

//  GAME STATES

const int MENU = 0;
const int PLAYING = 1;
const int GAME_OVER = 2;

// SHARED VARIABLES 

int bricks[ROWS][COLS] = { 0 };

float ballX = 0;
float ballY = 0;
float dx = 0;
float dy = 0;

float paddleX = 0;

int score = 0;
int lives = 3;
int gameState = MENU;

// FUNCTION DECLARATIONS 

// Maryam's FUNCTIONS
void loadLevel();
void drawBricks(sf::RenderWindow& window);
void handleInput();
void updateGame();
void drawGame(sf::RenderWindow& window);

// SAK'S FUNCTIONS
void movePaddle(int direction);
void launchBall();
void moveBall();
void bounceOnPaddle();

// LAIBA'S FUNCTIONS
void hitBricks();
void saveGame();
bool loadGame();
void clearSave();

// Maryam's FUNCTIONS

void loadLevel()
{
    // TODO
}

void drawBricks(sf::RenderWindow& window)
{
    // TODO
}

void handleInput()
{
    // TODO
}

void updateGame()
{
    // TODO
}

void drawGame(sf::RenderWindow& window)
{
    // TODO
}

// ================= SAK =================

void movePaddle(int direction)
{
    (void)direction;
    // TODO
}

void launchBall()
{
    // TODO
}

void moveBall()
{
    // TODO
}

void bounceOnPaddle()
{
    // TODO
}

// ================= LAIBA =================

void hitBricks()
{
    // TODO
}

void saveGame()
{
    // TODO
}

bool loadGame()
{
    // TODO
    return false;
}

void clearSave()
{
    // TODO
}

// MAIN 

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({ SCREEN_W, SCREEN_H }),
        "Blockbuster"
    );

    loadLevel();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        handleInput();
        updateGame();

        window.clear(sf::Color::Black);

        drawGame(window);

        window.display();
    }

    return 0;
}