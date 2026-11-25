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

// GAME STATES
const int MENU = 0;
const int PLAYING = 1;
const int GAME_OVER = 2;

// MAIN MENU OPTIONS (in this order on screen)
const int MENU_NEW_GAME = 0;
const int MENU_CONTINUE = 1;
const int MENU_EXIT = 2;

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
bool ballLaunched = false;   // false = ball sits on the paddle, true = ball is moving

int menuChoice = MENU_NEW_GAME;   // which menu option is highlighted
string menuMessage = "";          // small message on the menu, e.g. "No saved game found"
bool exitRequested = false;       // becomes true when Exit is chosen

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

//  FUNCTION DECLARATIONS 
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
bool hitBricks();
void checkBricksAfterXStep();
void checkBricksAfterYStep();
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
    // "Just pressed" detection: one tap = one action, even though the
    // loop runs 60 times per second
    static bool enterWasDown = false;
    static bool upWasDown = false;
    static bool downWasDown = false;

    bool enterDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
    bool upDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    bool downDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);

    bool enterJustPressed = enterDown && !enterWasDown;
    bool upJustPressed = upDown && !upWasDown;
    bool downJustPressed = downDown && !downWasDown;

    enterWasDown = enterDown;
    upWasDown = upDown;
    downWasDown = downDown;

    switch (gameState)
    {
    case MENU:
        // Move the highlight up or down (wraps around)
        if (upJustPressed)
        {
            menuChoice = (menuChoice + 2) % 3;
            menuMessage = "";
        }
        if (downJustPressed)
        {
            menuChoice = (menuChoice + 1) % 3;
            menuMessage = "";
        }

        if (enterJustPressed)
        {
            switch (menuChoice)
            {
            case MENU_NEW_GAME:
                loadLevel();             // score 0, 3 lives, bricks from level.txt
                ballLaunched = false;    // ball waits on the paddle
                menuMessage = "";
                gameState = PLAYING;
                break;

            case MENU_CONTINUE:
                if (loadGame())          // true if a saved game was loaded
                {
                    ballLaunched = false;
                    menuMessage = "";
                    gameState = PLAYING;
                }
                else
                {
                    menuMessage = "No saved game found";
                }
                break;

            case MENU_EXIT:
                exitRequested = true;    // main() closes the window
                break;
            }
        }
        break;

    case PLAYING:
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            movePaddle(-1);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            movePaddle(1);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !ballLaunched)
        {
            launchBall();
            ballLaunched = true;
        }
        break;

    case GAME_OVER:
        if (enterJustPressed)
        {
            menuChoice = MENU_NEW_GAME;
            menuMessage = "";
            gameState = MENU;            // Enter goes back to the menu
        }
        break;
    }
}

void updateGame()
{
    if (gameState == PLAYING && ballLaunched)
    {
        moveBall();
        bounceOnPaddle();
    }
}

// Draws one line of text, centred left to right, at height y
void drawCenteredText(sf::RenderWindow& window, const string& words, float y,
    unsigned int size, sf::Color color = sf::Color::White)
{
    if (!fontLoaded)
        return;                          // no font found, skip the text

    sf::Text text(font, words, size);
    text.setFillColor(color);

    float textWidth = text.getLocalBounds().size.x;
    text.setPosition({ (SCREEN_W - textWidth) / 2.0f, y });
    window.draw(text);
}

void drawGame(sf::RenderWindow& window)
{
    window.clear(sf::Color::Black);   // wipe the old picture

    switch (gameState)
    {
    case MENU:
    {
        drawCenteredText(window, "BLOCK BUSTER", 180, 48);

        // The three options in order. The chosen one is yellow with arrows.
        string options[3] = { "New Game", "Continue", "Exit" };
        for (int i = 0; i < 3; i++)
        {
            if (i == menuChoice)
                drawCenteredText(window, "> " + options[i] + " <", 320.f + i * 50, 30, sf::Color::Yellow);
            else
                drawCenteredText(window, options[i], 320.f + i * 50, 30);
        }

        drawCenteredText(window, "Up / Down to choose, Enter to select", 530, 18, sf::Color(160, 160, 160));

        if (menuMessage != "")
            drawCenteredText(window, menuMessage, 580, 22, sf::Color::Red);
        break;
    }

    case PLAYING:
    {
        drawBricks(window);

        // Score and lives at the top
        drawCenteredText(window, "Score: " + to_string(score) + "     Lives: " + to_string(lives), 10, 22);

        // Paddle
        sf::RectangleShape paddle(sf::Vector2f((float)PADDLE_W, (float)PADDLE_H));
        paddle.setPosition(sf::Vector2f(paddleX, 650.f));
        paddle.setFillColor(sf::Color::Cyan);
        window.draw(paddle);

        // Ball: on the paddle until launched, then at ballX, ballY
        sf::CircleShape ball(BALL_SIZE / 2.0f);
        ball.setFillColor(sf::Color::White);
        if (ballLaunched)
            ball.setPosition(sf::Vector2f(ballX, ballY));
        else
            ball.setPosition(sf::Vector2f(paddleX + PADDLE_W / 2.0f - BALL_SIZE / 2.0f,
                650.f - BALL_SIZE));
        window.draw(ball);
        break;
    }

    case GAME_OVER:
        if (lives > 0)
            drawCenteredText(window, "YOU WIN!", 250, 48, sf::Color::Green);
        else
            drawCenteredText(window, "GAME OVER", 250, 48, sf::Color::Red);

        drawCenteredText(window, "Final score: " + to_string(score), 330, 28);
        drawCenteredText(window, "Press Enter", 400, 24);
        break;
    }

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
    ballX += dx;
    ballY += dy;


    if (ballX + BALL_SIZE >= SCREEN_W && dx > 0)
    {
        ballX = SCREEN_W - BALL_SIZE;
        dx = -dx;
    }

    // Left wall
    if (ballX <= 0 && dx < 0)
    {
        ballX = 0;
        dx = -dx;
    }

    // Top wall
    if (ballY <= HUD_H && dy < 0)
    {
        ballY = HUD_H;
        dy = -dy;
    }
}


void bounceOnPaddle()
{
    if (dy > 0 &&
        ballY + BALL_SIZE >= 650 &&
        ballY + BALL_SIZE - dy <= 650 &&
        ballX + BALL_SIZE >= paddleX &&
        ballX <= paddleX + PADDLE_W)
    {
        ballY = 650 - BALL_SIZE;
        dy = -dy;

        float hitPosition = ballX + BALL_SIZE / 2.0f - paddleX;

        if (hitPosition < PADDLE_W / 4.0f)
        {
            dx = -5.0f;
        }
        else if (hitPosition < PADDLE_W / 2.0f)
        {
            dx = -2.0f;
        }
        else if (hitPosition < 3.0f * PADDLE_W / 4.0f)
        {
            dx = 2.0f;
        }
        else
        {
            dx = 5.0f;
        }
    }
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
// checks the 4 corners of the ball and hits the bricks they touch
// returns true if any brick was hit
bool hitBricks()
{
    // x and y of the 4 corners
    float cx[4] = { ballX, ballX + BALL_SIZE - 1, ballX, ballX + BALL_SIZE - 1 };
    float cy[4] = { ballY, ballY, ballY + BALL_SIZE - 1, ballY + BALL_SIZE - 1 };

    // to remember which bricks got hit already
    int hitR[4], hitC[4];
    int count = 0;

    for (int i = 0; i < 4; i++)
    {
        int r, c;

        if (!cornerToCell(cx[i], cy[i], r, c))   // corner not on the bricks
            continue;

        if (bricks[r][c] == 0)   // empty spot
            continue;

        // if 2 corners are on the same brick only hit it once
        bool same = false;
        for (int j = 0; j < count; j++)
        {
            if (hitR[j] == r && hitC[j] == c)
                same = true;
        }
        if (same)
            continue;

        hitR[count] = r;
        hitC[count] = c;
        count++;

        damageBrick(r, c);
    }

    return count > 0;
}
// after x move - if it hit a brick go back and bounce
void checkBricksAfterXStep()
{
    if (hitBricks())
    {
        ballX -= dx;
        dx = -dx;
    }
}

// after y move - same thing but up/down
void checkBricksAfterYStep()
{
    if (hitBricks())
    {
        ballY -= dy;
        dy = -dy;
    }
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
    // Making window: 600 wide, 700 tall, with a title
    sf::RenderWindow window(
        sf::VideoMode({ SCREEN_W, SCREEN_H }),
        "Block Buster Test"
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
        if (exitRequested)
            window.close();       // Exit was chosen on the menu
        updateGame();
        drawGame(window);
    }

    return 0;
}