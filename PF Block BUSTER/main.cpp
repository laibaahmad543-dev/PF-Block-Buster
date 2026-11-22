#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//  CONSTANTS 
const int SCREEN_W = 600;
const int SCREEN_H = 700;

const int ROWS = 8;
const int COLS = 10;
const int BRICK_W = 60;
const int BRICK_H = 20;
const int BRICKS_Y = 100;   // where the first row of bricks starts (from the top)
const int BRICK_GAP = 5;    // small gap: row spacing = 20 + 5 = 25

const int PADDLE_W = 100;
const int PADDLE_H = 15;
const int PADDLE_SPEED = 8;

const int BALL_SIZE = 10;
const int HUD_H = 40;

//GAME STATES--const int MENU = 0;
const int MENU = 0;
const int PLAYING = 1;
const int GAME_OVER = 2;

//  SHARED VARIABLE
int bricks[ROWS][COLS] = { 0 };   // each number = hits left (0 = no brick)

float ballX = 0;
float ballY = 0;
float dx = 0;
float dy = 0;

float paddleX = 0;

int score = 0;
int lives = 3;
int gameState = MENU;

bool levelLoaded = false;         // true if level.txt was read correctly
string levelError = "";           // the error message if it was not
sf::Font font;                    // the font used to write text
bool fontLoaded = false;          // true if the font was found

// ================= FUNCTION DECLARATIONS =================
// YOUR FUNCTIONS
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

//  read level.txt into the bricks array
void loadLevel()
{
    // Start with an empty board
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            bricks[r][c] = 0;

    score = 0;
    lives = 3;
    paddleX = (SCREEN_W - PADDLE_W) / 2.0f;
    levelLoaded = false;

    // Open the file
    ifstream fin("level.txt");
    if (!fin)   // file could not be opened
    {
        levelError = "Error: level.txt not found";
        return;   // stop here, the game keeps running (no crash)
    }

    // First line of the file: number of rows and columns
    int fileRows = 0;
    int fileCols = 0;
    fin >> fileRows >> fileCols;

    if (fileRows != ROWS || fileCols != COLS)
    {
        levelError = "Error: first line must be 8 10";
        return;
    }

    // Read every brick number: two loops, one for rows, one for columns
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            fin >> bricks[r][c];
        }
    }

    levelLoaded = true;
}

// FUNCTION  draw the bricks (or the error message)
void drawBricks(sf::RenderWindow& window)
{
    // If loading failed, show a red box with the error and stop
    if (!levelLoaded)
    {
        sf::RectangleShape box(sf::Vector2f((float)SCREEN_W, 60.0f));
        box.setPosition(sf::Vector2f(0.0f, 300.0f));
        box.setFillColor(sf::Color(150, 0, 0));
        window.draw(box);

        if (fontLoaded)
        {
            sf::Text message(font, levelError, 24);
            message.setFillColor(sf::Color::White);
            message.setPosition(sf::Vector2f(20.0f, 315.0f));
            window.draw(message);
        }
        return;
    }

    // Go through every row and every column
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (bricks[r][c] == 0)
                continue;   // 0 means no brick, skip it

            // One brick: 58 wide (leaves a gap), 20 tall
            sf::RectangleShape brick(sf::Vector2f((float)(BRICK_W - 2), (float)BRICK_H));

            // Position: x = column * 60, y = 100 + row * 25
            brick.setPosition(sf::Vector2f((float)(c * BRICK_W),
                (float)(BRICKS_Y + r * (BRICK_H + BRICK_GAP))));

            // Colour depends on hits left
            if (bricks[r][c] == 1)
                brick.setFillColor(sf::Color::Green);
            else if (bricks[r][c] == 2)
                brick.setFillColor(sf::Color::Yellow);
            else
                brick.setFillColor(sf::Color::Red);

            window.draw(brick);
        }
    }
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
    window.clear(sf::Color::Black);   // wipe the old picture

    drawBricks(window);               // draw the new one
    // TODO: draw HUD, paddle, ball here

    window.display();                 // show it on the screen
}

// SAK 
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

// LAIBA 
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


int main()
{
    // Make the window: 600 wide, 700 tall, with a title
    sf::RenderWindow window(
        sf::VideoMode({ SCREEN_W, SCREEN_H }),
        "Block Buster"
    );
    window.setFramerateLimit(60);   // at most 60 turns per second

    // Try to load a font (only needed for the error message)
    fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");

    loadLevel();

    // The game loop: repeats until the window is closed
    while (window.isOpen())
    {
        
        while (const std::optional event = window.pollEvent())
        {
            // if X button is typed.
            if (event->is<sf::Event::Closed>())
            {
                window.close();   //close the window
            }
        }

        // The three jobs
        handleInput();
        updateGame();
        drawGame(window);
    }

    return 0;
}