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
const int PADDLE_H = 14;
const int PADDLE_SPEED = 8;

const int BALL_SIZE = 12;
const int HUD_H = 50;

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
sf::Color colors[4] = {
    sf::Color(25, 25, 45),   // 0: no brick
    sf::Color::Green,        // 1: one hit left
    sf::Color::Yellow,       // 2: two hits left
    sf::Color::Red           // 3: three hits left
};

bool levelLoaded = false;         // true if level.txt was read correctly
string levelError = "";           // the error message if it was not
sf::Font font;                    // the font used to write text
bool fontLoaded = false;          // true if the font was found

// ================= FUNCTION DECLARATIONS =================
// Maryam's FUNCTIONS
void loadLevel();
void drawBricks(sf::RenderWindow& window);
void handleInput();
void updateGame();
void drawGame(sf::RenderWindow& window);

// LAIBA'S FUNCTIONS
void movePaddle(int direction);
void launchBall();
void moveBall();
void bounceOnPaddle();

// SAKAINAH'S FUNCTIONS
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
            if (bricks[r][c] < 0 || bricks[r][c] > 3)  
                bricks[r][c] = 0;
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
        sf::RectangleShape box({ (float)SCREEN_W, 60.f });
        box.setPosition({ 0.f, 300.f });
        box.setFillColor(sf::Color(150, 0, 0));
        window.draw(box);

        if (fontLoaded)
        {
            sf::Text message(font, levelError, 24);
            message.setFillColor(sf::Color::White);
            message.setPosition({ 20.f, 315.f });
            window.draw(message);
        }
        return;
    }

    // Smaller than the cell, so a gap shows between bricks
    sf::RectangleShape brick(sf::Vector2f((float)(BRICK_W - 2), (float)BRICK_H));

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            if (bricks[r][c] == 0)
                continue;   // 0 means no brick, skip it

            brick.setFillColor(colors[bricks[r][c]]);
            brick.setPosition(sf::Vector2f((float)(c * BRICK_W + 1),
                (float)(BRICKS_Y + r * (BRICK_H + BRICK_GAP))));
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

    sf::RectangleShape paddle(sf::Vector2f((float)PADDLE_W, (float)PADDLE_H)); // setting shape as rectangle 

    paddle.setPosition(sf::Vector2f(paddleX, 650.f)); // setting position to be in the middle 

    paddle.setFillColor(sf::Color::Cyan); // setting colour as cyan

    window.draw(paddle);

    sf::CircleShape ball(BALL_SIZE / 2.0f); // balll shaped as a circle 

    ball.setPosition(sf::Vector2f(paddleX + PADDLE_W / 2.0f - BALL_SIZE / 2.0f, 650.f - BALL_SIZE
    )); // setting position on top of paddle in the middle 

    ball.setFillColor(sf::Color::White); // setting colour as white 

    window.draw(ball);
    window.display();                 // show it on the screen
}

// LAIBA 
void movePaddle(int direction)
{
    if (direction == 1)
    {
        paddleX += PADDLE_SPEED;
    }
    else if (direction == -1)
    {
        paddleX -= PADDLE_SPEED;
    }

    if (paddleX > SCREEN_W - PADDLE_W)
    {
        paddleX = SCREEN_W - PADDLE_W;
    }

    if (paddleX < 0)
    {
        paddleX = 0;
    }
}

void launchBall()
{
    ballX = paddleX + PADDLE_W / 2.0f - BALL_SIZE / 2.0f;
    ballY = 650.0f - BALL_SIZE;

    dx = 3.0f;
    dy = -4.0f;
}

void moveBall()
{
    // TODO
}

void bounceOnPaddle()
{
    // TODO
}

// SAKAINAH
// FEATURE 5: BREAKING BRICKS 
const int ROW_SPACING = BRICK_H + BRICK_GAP;               // 20 + 5 = 25 pixels per row
const int BRICKS_BOTTOM = BRICKS_Y + ROWS * ROW_SPACING;   // brick area ends at y = 300
const int POINTS_HIT = 10;                                 // hit that does not break the brick
const int POINTS_BREAK = 50;                               // hit that breaks the brick

/*Takes one corner(cx, cy) of the ball.If it is inside the brick area,
finds the row and column under it and returns true. Otherwise returns
false, because the division would give a wrong row.*/ 
bool cornerToCell(float cx, float cy, int& row, int& col)
{
    if (cy < BRICKS_Y || cy >= BRICKS_BOTTOM || cx < 0 || cx >= SCREEN_W)
        return false;

    row = ((int)cy - BRICKS_Y) / ROW_SPACING;   // integer division
    col = (int)cx / BRICK_W;
    return true;
}

/*Takes one hit off brick[r][c] and gives the points.
 drawBricks() picks the colour from the number, so the colour changes
 by itself, and at 0 the brick is not drawn.*/ 
void damageBrick(int r, int c)
{
    bricks[r][c] = bricks[r][c] - 1;

    if (bricks[r][c] == 0)
        score = score + POINTS_BREAK;   // this hit broke the brick
    else
        score = score + POINTS_HIT;     // brick is still there
}
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