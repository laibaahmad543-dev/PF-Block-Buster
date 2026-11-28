#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
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


// KEYS PRESSED ONCE (filled in main's event loop)
const int NO_KEY = 0;
const int KEY_UP = 1;
const int KEY_DOWN = 2;
const int KEY_ENTER = 3;
const int KEY_ESCAPE = 4;
const int LAUNCH = 5;

int pressedKey = NO_KEY;   // the key pressed this frame
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
    sf::Color(25, 25, 45),   // 0 no brick
    sf::Color::Green,        // 1 one hit left
    sf::Color::Yellow,       // 2 two hits left
    sf::Color::Red           // 3 three hits left
};

bool levelLoaded = false;         // true if level.txt was read correctly
string levelError = "";           // the error message if it was not
sf::Font font;                    // the font used to write text
bool fontLoaded = false;         // true if the font was found

sf::Texture backgroundTexture;
bool backgroundLoaded = false;
// POWER-UPS (bonus)
const int POWERUP_W = 24;
const int POWERUP_H = 24;
const int POWERUP_SPEED = 3;
const int WIDE_PADDLE_W = 150;
const int WIDE_FRAMES = 600;      // 10 seconds at 60 fps
const int POWER_WIDE = 0;
const int POWER_LIFE = 1;

bool powerUpActive = false;       // only one power-up at a time
float powerUpX = 0;
float powerUpY = 0;
int powerUpType = POWER_WIDE;
int paddleW = PADDLE_W;           // current paddle width
int wideTimer = 0;                // frames left of the wide paddle

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
void checkLose();
void checkWin();

// SAKAINAH'S FUNCTIONS
bool hitBricks();
void checkBricksAfterXStep();
void checkBricksAfterYStep();
void saveGame();
bool loadGame();
void clearSave();
// Maryam (bonus)
void resetPowerUps();
void movePowerUp();
void drawPowerUp(sf::RenderWindow& window);

// Laiba (bonus)
void activateWidePaddle();
void updateWideTimer();

// Sakainah (bonus)
void spawnPowerUp(int row, int col);
void checkPowerUpCatch();

// Maryam's FUNCTIONS 

//  read level.txt into the bricks array
void loadLevel()
{
    resetPowerUps();
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

    // File had fewer than 80 numbers
    if (fin.fail())
    {
        levelError = "Error: level.txt is incomplete";
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                bricks[r][c] = 0;
        return;
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
    brick.setOutlineThickness(-1.5f);
    brick.setOutlineColor(sf::Color(20, 20, 20));

    sf::RectangleShape highlight({ 54.f, 2.f });
    highlight.setFillColor(sf::Color(255, 255, 255, 130));

    sf::RectangleShape shadow({ 54.f, 2.f });
    shadow.setFillColor(sf::Color(0, 0, 0, 150));
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
            highlight.setPosition({
    (float)(c * BRICK_W + 3),
    (float)(BRICKS_Y + r * (BRICK_H + BRICK_GAP) + 2)
                });

            shadow.setPosition({
                (float)(c * BRICK_W + 3),
                (float)(BRICKS_Y + r * (BRICK_H + BRICK_GAP) + BRICK_H - 4)
                });

            window.draw(highlight);
            window.draw(shadow);
        }
    }
}

void handleInput()
{
    switch (gameState)
    {
    case MENU:
        // Move the highlight up or down (wraps around)
        if (pressedKey == KEY_UP)
        {
            menuChoice = (menuChoice + 2) % 3;
            menuMessage = "";
        }
        if (pressedKey == KEY_DOWN)
        {
            menuChoice = (menuChoice + 1) % 3;
            menuMessage = "";
        }

        if (pressedKey == KEY_ENTER)
        {
            switch (menuChoice)
            {
            case MENU_NEW_GAME:
                loadLevel();             // score 0, 3 lives, bricks from level.txt
                if (levelLoaded)
                {
                    ballLaunched = false;    // ball waits on the paddle
                    menuMessage = "";
                    gameState = PLAYING;
                }
                else
                {
                    menuMessage = levelError;   // stay on menu, show the error
                }
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
        // Escape: save and go back to the menu
        if (pressedKey == KEY_ESCAPE)
        {
            saveGame();
            menuChoice = MENU_NEW_GAME;
            menuMessage = "";
            gameState = MENU;
            break;
        }

        // Held keys: these stay as isKeyPressed
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

        // Once-per-press key
        if (pressedKey == LAUNCH && !ballLaunched)
        {
            launchBall();
            ballLaunched = true;
        }
        break;

    case GAME_OVER:
        if (pressedKey == KEY_ENTER)
        {
            menuChoice = MENU_NEW_GAME;
            menuMessage = "";
            gameState = MENU;            // Enter goes back to the menu
        }
        break;
    }

    pressedKey = NO_KEY;   // clear it so one press = one action
}

void updateGame()
{
    // Only run game logic based on the current state
    switch (gameState)
    {
    case PLAYING:
        // Only move the ball and check collisions after it has been launched
        if (ballLaunched)
        {
            moveBall();         // Update the ball's position
            bounceOnPaddle();   // Bounce the ball if it hits the paddle
            checkLose();        
        }

        
        if (gameState == PLAYING)
        {
            movePowerUp();          // Move any falling power-up
            checkPowerUpCatch();    // Check if the paddle caught the power-up
            updateWideTimer();      // Count down the widepaddle power-up timer
            checkWin();             // Check if all the bricks are destroyed
        }
        // The player lost this frame, so the save is cleared once
        else if (gameState == GAME_OVER)
        {
            clearSave();            // Delete the saved game data
        }
        break;

       
    default:
        break;
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
    if (backgroundLoaded)
    {
        sf::Sprite background(backgroundTexture);

        background.setScale({
            (float)SCREEN_W / backgroundTexture.getSize().x,
            (float)SCREEN_H / backgroundTexture.getSize().y
            });

        window.draw(background);
    }
    switch (gameState)
    {
    case MENU:
    {
        drawCenteredText(window, "Block Buster", 180, 56,
            sf::Color(255, 245, 230));

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
        sf::Color silver(210, 205, 210);

        sf::RectangleShape paddle({
            (float)(paddleW - PADDLE_H),
            (float)PADDLE_H
            });
        paddle.setPosition({ paddleX + 7.f, 650.f });
        paddle.setFillColor(silver);

        sf::CircleShape leftEnd(7.f);
        leftEnd.setPosition({ paddleX, 650.f });
        leftEnd.setFillColor(silver);

        sf::CircleShape rightEnd(7.f);
        rightEnd.setPosition({ paddleX + paddleW - 14.f, 650.f });
        rightEnd.setFillColor(silver);

        window.draw(paddle);
        window.draw(leftEnd);
        window.draw(rightEnd);

        sf::RectangleShape shine({ (float)(paddleW - 20), 2.f });
        shine.setPosition({ paddleX + 10.f, 652.f });
        shine.setFillColor(sf::Color::White);
        window.draw(shine);

        drawPowerUp(window);

        // Ball: on the paddle until launched, then at ballX, ballY
        sf::CircleShape ball(BALL_SIZE / 2.0f);
        ball.setFillColor(sf::Color::White);
        if (ballLaunched)
            ball.setPosition(sf::Vector2f(ballX, ballY));
        else
            ball.setPosition(sf::Vector2f(paddleX + paddleW / 2.0f - BALL_SIZE / 2.0f,
                650.f - BALL_SIZE));
        // Outer glow
        sf::CircleShape glow(12.f);
        glow.setPosition({
            ball.getPosition().x - 6.f,
            ball.getPosition().y - 6.f
            });

        glow.setFillColor(sf::Color(255, 220, 200, 30));
        window.draw(glow);

        // Inner glow
        sf::CircleShape innerGlow(9.f);
        innerGlow.setPosition({
            ball.getPosition().x - 3.f,
            ball.getPosition().y - 3.f
            });

        innerGlow.setFillColor(sf::Color(255, 220, 200, 70));
        window.draw(innerGlow);

        // Actual ball
        ball.setFillColor(sf::Color::White);
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

    if (paddleX > SCREEN_W - paddleW)
    {
        paddleX = SCREEN_W - paddleW;
    }

    if (paddleX < 0)
    {
        paddleX = 0;
    }
}

void launchBall()
{
    ballX = paddleX + (paddleW - BALL_SIZE) / 2.0f;
    ballY = 638;

    dy = -5;

    if (rand() % 2 == 0)
    {
        dx = 3;
    }
    else
    {
        dx = -3;
    }
}
void moveBall()
{
    // Horizontal movement
    ballX += dx;

    // Left and right wall collision
    if (ballX < 0 || ballX + BALL_SIZE > SCREEN_W)
    {
        ballX -= dx;
        dx = -dx;
    }
    else
    {
        checkBricksAfterXStep();
    }

    // Vertical movement
    ballY += dy;

    // Top wall collision
    if (ballY < HUD_H)
    {
        ballY -= dy;
        dy = -dy;
    }
    else
    {
        checkBricksAfterYStep();
    }
}


void bounceOnPaddle()
{
    if (dy > 0 &&
        ballX + BALL_SIZE > paddleX &&
        ballX < paddleX + paddleW &&
        ballY + BALL_SIZE > 650 &&
        ballY < 650 + PADDLE_H)
    {
        ballY = 638;
        dy = -5;

        int part = (int)((ballX + 6 - paddleX) * 4 / paddleW);

        if (part < 0)
        {
            part = 0;
        }

        if (part > 3)
        {
            part = 3;
        }

        if (part == 0)
        {
            dx = -6;
        }
        else if (part == 1)
        {
            dx = -3;
        }
        else if (part == 2)
        {
            dx = 3;
        }
        else
        {
            dx = 6;
        }
    }
}


void checkWin()
{
    int count = 0;

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (bricks[i][j] != 0)
            {
                count++;
            }
        }
    }

    if (count == 0 && levelLoaded)
    {
        gameState = GAME_OVER;
        ballLaunched = false;
    }
}

void checkLose()
{
    if (ballLaunched && ballY > SCREEN_H)
    {
        lives--;

        ballLaunched = false;

        ballX = paddleX + (paddleW - BALL_SIZE) / 2.0f;
        ballY = 638;

        dx = 0;
        dy = 0;

        if (lives == 0)
        {
            gameState = GAME_OVER;
        }
    }
}

// Laiba (bonus)
void activateWidePaddle()
{
    paddleW = WIDE_PADDLE_W;
    wideTimer = WIDE_FRAMES;

    if (paddleX + paddleW > SCREEN_W)
    {
        paddleX = SCREEN_W - paddleW;
    }
}

void updateWideTimer()
{
    if (wideTimer > 0)
    {
        wideTimer--;

        if (wideTimer == 0)
        {
            paddleW = PADDLE_W;
        }
    }
}

void checkPowerUpCatch()
{
    if (powerUpActive)
    {
        if (powerUpX + POWERUP_W > paddleX &&
            powerUpX < paddleX + paddleW &&
            powerUpY + POWERUP_H > 650 &&
            powerUpY < 650 + PADDLE_H)
        {
            if (powerUpType == POWER_WIDE)
            {
                activateWidePaddle();
            }
            else if (powerUpType == POWER_LIFE)
            {
                lives++;
            }

            powerUpActive = false;
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
    {
        score = score + POINTS_BREAK;   // this hit broke the brick
        spawnPowerUp(r, c);             // maybe drop a power-up
    }
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

// FEATURE 8: SAVE AND CONTINUE

const char SAVE_FILE[] = "savegame.txt";

// saves score, lives and all the bricks to savegame.txt
// paddle and ball dont get saved

void saveGame()
{
    ofstream fout(SAVE_FILE);

    if (!fout)
    {
        return;   // could not open the file, but no crash
    }

    fout << score << " " << lives << "\n";   // first line

    // after, 8 rows of 10 bricks
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            fout << bricks[r][c];

            if (c < COLS - 1)
            {
                fout << " ";   // no space after the last one
            }
        }

        fout << "\n";
    }

    fout.close();
}

// loads savegame.txt back into the game
// returns false if theres no saved game (no file, empty, or broken)
bool loadGame()
{
    ifstream fin(SAVE_FILE);
    if (!fin)
        return false;   // no file

    // read into temp variables first so a broken file doesnt mess up the game
    int newScore, newLives;
    int newBricks[ROWS][COLS];

    fin >> newScore >> newLives;
    if (fin.fail())
        return false;   // file is empty (last game ended)

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            fin >> newBricks[r][c];
            if (fin.fail())
                return false;   // file cut short
        }
    }
    fin.close();

    // file was fine so now copy it into the real game
    score = newScore;
    lives = newLives;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            bricks[r][c] = newBricks[r][c];

    levelLoaded = true;   // so the bricks get drawn
    resetPowerUps();   // normal paddle, no power-up falling
    // paddle back in the middle, ball waits on it for space
    paddleX = (SCREEN_W - PADDLE_W) / 2.0f;
    ballLaunched = false;
    dx = 0;
    dy = 0;

    return true;
}

// empties the save file when a game ends so it cant be continued
// (opening with ofstream and closing it straight away makes it empty)
void clearSave()
{
    ofstream fout(SAVE_FILE);
    fout.close();
}
// Maryam (bonus)
// back to normal: no power-up, normal paddle
void resetPowerUps()
{
    powerUpActive = false;
    paddleW = PADDLE_W;
    wideTimer = 0;
}
// the power-up falls 3 pixels per frame, gone if below the window
void movePowerUp()
{
    if (!powerUpActive)
        return;

    powerUpY += POWERUP_SPEED;

    if (powerUpY > SCREEN_H)
        powerUpActive = false;
}
// draws a coloured box with W or +
void drawPowerUp(sf::RenderWindow& window)
{
    if (!powerUpActive)
        return;

    sf::RectangleShape box({ (float)POWERUP_W, (float)POWERUP_H });
    box.setPosition({ powerUpX, powerUpY });
    box.setOutlineThickness(-2.f);
    box.setOutlineColor(sf::Color::White);

    if (powerUpType == POWER_WIDE)
        box.setFillColor(sf::Color(0, 150, 255));
    else
        box.setFillColor(sf::Color(0, 180, 60));

    window.draw(box);

    if (fontLoaded)
    {
        string letter = (powerUpType == POWER_WIDE) ? "W" : "+";
        sf::Text text(font, letter, 18);
        text.setFillColor(sf::Color::White);
        float w = text.getLocalBounds().size.x;
        text.setPosition({ powerUpX + (POWERUP_W - w) / 2.0f, powerUpY });
        window.draw(text);
    }

}

// Sakainah (bonus)
// when a brick breaks theres a 1 in 5 chance a power-up falls from it
// only one can be on the screen at a time
void spawnPowerUp(int row, int col)
{
    if (powerUpActive)
        return;   // one is already falling

    if (rand() % 5 != 0)
        return;   // no luck this time

    powerUpActive = true;

    // start in the middle of the brick that broke
    powerUpX = col * BRICK_W + (BRICK_W - POWERUP_W) / 2.0f;
    powerUpY = (float)(BRICKS_Y + row * ROW_SPACING);

    // random type: wide paddle or extra life
    if (rand() % 2 == 0)
        powerUpType = POWER_WIDE;
    else
        powerUpType = POWER_LIFE;
}

int main()
{
    srand((unsigned int)time(0));

    sf::RenderWindow window(
        sf::VideoMode({ SCREEN_W, SCREEN_H }),
        "Block Buster"
    );

    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);   // a held key counts as one press

    backgroundLoaded = backgroundTexture.loadFromFile("background.png");

    fontLoaded = font.openFromFile("gamefont.ttf");

    loadLevel();

    // The game loop: repeats until the window is closed
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            // if X button is clicked
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Space)
                    pressedKey = LAUNCH;
                else if (key->code == sf::Keyboard::Key::Enter)
                    pressedKey = KEY_ENTER;
                else if (key->code == sf::Keyboard::Key::Escape)
                    pressedKey = KEY_ESCAPE;
                else if (key->code == sf::Keyboard::Key::Up)
                    pressedKey = KEY_UP;
                else if (key->code == sf::Keyboard::Key::Down)
                    pressedKey = KEY_DOWN;
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