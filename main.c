/**
 * File:        main.c
 * Purpose:     The main C source file for the game
 *
 * Author:      Lionel Pinkhard
 * Date:        January 29, 2019
 * Version:     1.0
 *
 */

// Include the headers
#include "main.h"

// Represents the current landscape
BITMAP * g_bLandscape;

// Represents the player
BITMAP * g_bPlayer;

// Represents the enemy
BITMAP * g_bEnemy;

// Reference to the global font used in the game
FONT * g_fFont = NULL;

SAMPLE * g_sCannon = NULL; // Cannon sound

int g_nScore = 0;          // The player's current score

int g_nPlayerHealth = 100; // The player's health
int g_nPlayerAmmo = 5;     // The player's ammo

int g_nPlayerX = 0;         // Player location X
int g_nPlayerY = 0;         // Player location Y
float g_nPlayerAngle = 0;   // Player's cannon angle
int g_nPlayerDirection = 1; // Player direction
int g_nPlayerXOld = -1;     // Old location X
int g_nPlayerYOld = -1;     // Old location Y

int g_nEnemyHealth = 100;   // The enemy's health
int g_nEnemyAmmo = 5;       // The enemy's ammo

int g_nEnemyX = 0;          // Enemy location X
int g_nEnemyY = 0;          // Enemy location Y
float g_nEnemyAngle = 0;    // Enemy's cannon angle
int g_nEnemyDirection = 1;  // Enemy direction
int g_nEnemyXOld = -1;      // Old location X
int g_nEnemyYOld = -1;      // Old location Y

int g_bProjectileActive = 0;    // Whether a projectile is in the air
int g_nProjectileX = 0;         // Projectile location x
int g_nProjectileY = 0;         // Projectile location y
int g_nProjectileDirection = 0; // Projectile direction
int g_nProjectileCount = 0;     // Iteration count
float g_nProjectileAngle = 0;   // Projectile angle

// Whether busy exiting
int g_bExiting = 0;

// Mutex
pthread_mutex_t threadsafe = PTHREAD_MUTEX_INITIALIZER;

/**
 * Draws the main game display on the screen.
 *
 * Parameters:
 * bBuffer      Display buffer
 * nSelection   Currently selected answer
 * bFullDraw    Redraw everything
 */
void drawGame(BITMAP * bBuffer) {
    int nPlayerCannonX, nPlayerCannonY;
    int nEnemyCannonX, nEnemyCannonY;
    int rx1, rx2, ry1, ry2; // Coordinates for cannons
    int v[4 * 2]; // Vertices for drawing cannons
    float d; // Distance for line

    // Clear the whole screen
    clear_bitmap(bBuffer);

    // Draw the landscape
    blit(g_bLandscape, bBuffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    // Display the score
    textprintf_ex(bBuffer, g_fFont, SCREEN_W * 0.6, SCREEN_H * 0.05, makecol(0, 0, 0), -1, "Score: %d", g_nScore);
    // Display the health
    textprintf_ex(bBuffer, g_fFont, SCREEN_W * 0.7, SCREEN_H * 0.05, makecol(0, 0, 0), -1, "Health: %d",
        g_nPlayerHealth);
    // Display the ammo
    textprintf_ex(bBuffer, g_fFont, SCREEN_W * 0.8, SCREEN_H * 0.05, makecol(0, 0, 0), -1, "Ammo: %d", g_nPlayerAmmo);

    // Draw a projectile
    if (g_bProjectileActive) {
        circlefill(bBuffer, g_nProjectileX, g_nProjectileY, 3, makecol(0, 0, 0));
    }

    // Draw the player cannon
    nPlayerCannonX = g_nPlayerX + g_bPlayer -> w / 2;
    nPlayerCannonY = g_nPlayerY + g_bPlayer -> h / 3;
	
    // Calculate the corners of the cannon
    rx1 = nPlayerCannonX - (8 + 15 - 15 * cos(g_nPlayerAngle)) * g_nPlayerDirection;
    ry1 = nPlayerCannonY - 2 + 15 * sin(g_nPlayerAngle);
    rx2 = nPlayerCannonX + (15 * cos(g_nPlayerAngle)) * g_nPlayerDirection;
    ry2 = nPlayerCannonY - 2 - 15 * sin(g_nPlayerAngle);
	
    // Set up a polygon to draw the cannon
    d = sqrtf((rx1 - rx2) * (rx1 - rx2) + (ry1 - ry2) * (ry1 - ry2));
    v[0] = rx1 - 3 * (ry1 - ry2) / d;
    v[1] = ry1 + 3 * (rx1 - rx2) / d;
    v[2] = rx1 + 3 * (ry1 - ry2) / d;
    v[3] = ry1 - 3 * (rx1 - rx2) / d;
    v[4] = rx2 + 3 * (ry1 - ry2) / d;
    v[5] = ry2 - 3 * (rx1 - rx2) / d;
    v[6] = rx2 - 3 * (ry1 - ry2) / d;
    v[7] = ry2 + 3 * (rx1 - rx2) / d;
    polygon(bBuffer, 4, v, makecol(0, 0, 0));

    // Draw the enemy cannon
    nEnemyCannonX = g_nEnemyX + g_bEnemy -> w / 2;
    nEnemyCannonY = g_nEnemyY + g_bEnemy -> h / 3;
	
    // Calculate the corners of the cannon
    rx1 = nEnemyCannonX - (8 + 15 - 15 * cos(g_nEnemyAngle)) * g_nEnemyDirection;
    ry1 = nEnemyCannonY - 2 + 15 * sin(g_nEnemyAngle);
    rx2 = nEnemyCannonX + (15 * cos(g_nEnemyAngle)) * g_nEnemyDirection;
    ry2 = nEnemyCannonY - 2 - 15 * sin(g_nEnemyAngle);
	
    // Set up a polygon to draw the cannon
    d = sqrtf((rx1 - rx2) * (rx1 - rx2) + (ry1 - ry2) * (ry1 - ry2));
    v[0] = rx1 - 3 * (ry1 - ry2) / d;
    v[1] = ry1 + 3 * (rx1 - rx2) / d;
    v[2] = rx1 + 3 * (ry1 - ry2) / d;
    v[3] = ry1 - 3 * (rx1 - rx2) / d;
    v[4] = rx2 + 3 * (ry1 - ry2) / d;
    v[5] = ry2 - 3 * (rx1 - rx2) / d;
    v[6] = rx2 - 3 * (ry1 - ry2) / d;
    v[7] = ry2 + 3 * (rx1 - rx2) / d;
    polygon(bBuffer, 4, v, makecol(0, 0, 0));

    // Draw the player
    draw_sprite(bBuffer, g_bPlayer, g_nPlayerX, g_nPlayerY);

    // Draw the enemy
    draw_sprite(bBuffer, g_bEnemy, g_nEnemyX, g_nEnemyY);

    // Copy the buffer to the screen
    blit(bBuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

/**
 * Processes one game loop iteration while in intro state.
 * Displays the welcome message and instructions.
 *
 * Parameters:
 * bBuffer      Display buffer
 *
 * Returns:
 *   Game state at end of iteration
 */
int introLoop(BITMAP * bBuffer) {
    int nGamestate = STATE_INTRO;                // Default to remain in intro state
    static int bRedraw = 1;                      // Redraw intro screen
    int nTitleColor = makecol(0xf9, 0xef, 0x06); // Color of the title text
    int nTextColor = makecol(0xf9, 0xef, 0xf6);  // Color of the other text
    static time_t tStart;                        // Starting time in clock ticks

    // Check for redraw (should only happen once)
    if (bRedraw) {
        // Display text
        textout_centre_ex(bBuffer, font, "Cannon Attack", SCREEN_W / 2, SCREEN_H * 0.15, nTitleColor, -1);

        textout_centre_ex(bBuffer, font, "Copyright (C) 2019 Lionel Pinkhard. All rights reserved.",
            SCREEN_W / 2, SCREEN_H * 0.2, nTitleColor, -1);

        textout_centre_ex(bBuffer, font, "Welcome to Cannon Attack!", SCREEN_W / 2, SCREEN_H * 0.3, nTextColor, -1);

        textout_ex(bBuffer, font, "In this game, you need to destroy the enemy's cannon",
            SCREEN_W * 0.15, SCREEN_H * 0.35, nTextColor, -1);
        textout_ex(bBuffer, font, "before your cannon is destroyed. You can achieve this",
            SCREEN_W * 0.15, SCREEN_H * 0.375, nTextColor, -1);
        textout_ex(bBuffer, font, "goal by aiming and shooting at the enemy. You may,",
            SCREEN_W * 0.15, SCREEN_H * 0.4, nTextColor, -1);

        textout_ex(bBuffer, font, "however, not get too close to the enemy.",
            SCREEN_W * 0.15, SCREEN_H * 0.425, nTextColor, -1);

        textout_ex(bBuffer, font, "To play, use the arrow keys to adjust your cannon and.",
            SCREEN_W * 0.15, SCREEN_H * 0.475, nTextColor, -1);

        textout_ex(bBuffer, font, "press spacebar to fire. For help, press Ctrl-H.",
            SCREEN_W * 0.15, SCREEN_H * 0.525, nTextColor, -1);

        textout_centre_ex(bBuffer, font, "Press any key to continue...", SCREEN_W / 2, SCREEN_H * 0.65, nTextColor, -1);

        // Copy the buffer to the screen
        blit(bBuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        // Set start time
        time( & tStart);

        // Cancel redraw
        bRedraw = 0;
    }

    // Check for key press or 30 seconds passed
    if (keypressed() || time(NULL) > tStart + 30) {
        nGamestate++; // Change to next state (mode select)
        rest(200); // Protect from key press through to next state
    }

    return nGamestate;
}

/**
 * Prepares the playing field
 *
 * Returns:
 *   Game state at end
 */
int gameLoad() {
    int nGamestate = STATE_END; // Default to exit
    int i;                      // Index for outer loop
    int j;                      // Index for inner loop
    int y;                      // Y position
    int nMovement;              // Amount of movement
    int nPixels;
    int nMaskCol;               // Masking color

    // Create landscape bitmap and clear to brown
    pthread_mutex_lock( & threadsafe);
    g_bLandscape = create_bitmap(SCREEN_W, SCREEN_H);
    clear_to_color(g_bLandscape, COL_LANDSCAPE);

    // Sky colour
    nMaskCol = COL_SKY;

    // Clear top part with region to accomodate random movement
    rectfill(g_bLandscape, 0, 0, SCREEN_W, 32, nMaskCol);

    // Initailize random number generator
    srand(time(NULL));

    // Set starting Y position for seperation between sky and earth
    y = SCREEN_H * 2 / 5 + (rand() % (SCREEN_H / 3));

    // Handle between 5 and 15 pixels at a time
    nPixels = (rand() % 10) + 5;
    for (i = 0; i < SCREEN_W; i += nPixels) {
        // Move along Y axis a random amount (more likely a smaller amount)
        nMovement = sqrt(rand() % 1000) - sqrt(rand() % 1000);

        // Loop to Y position
        for (j = 0; j < y; j++) {
            line(g_bLandscape, i, j, i + nPixels, j + nMovement, nMaskCol);
        }

        // Update Y
        y += nMovement;
    }
    pthread_mutex_unlock( & threadsafe);

    // Random starting locations
    g_nPlayerX = rand() % (SCREEN_W - g_bPlayer -> w);
    // Enemy not too close
    do {
        g_nEnemyX = rand() % (SCREEN_W - g_bEnemy -> w);
    } while (abs(g_nEnemyX - g_nPlayerX) < SCREEN_W / 4);

    // Reset old locations
    g_nPlayerXOld = g_nPlayerX;
    g_nEnemyXOld = g_nEnemyX;

    // Recharge health and ammo
    g_nPlayerHealth = 100;
    g_nPlayerAmmo = 5;
    g_nEnemyHealth = 100;
    g_nEnemyAmmo = 5;

    return STATE_GAMEPLAY;
}

/**
 * Processes main game loop iteration.
 * Updates the main game screen.
 *
 * Parameters:
 * bBuffer      Display buffer
 * bHasSound    System has sound capability
 *
 * Returns:
 *   Game state at end of iteration
 */
int playLoop(BITMAP * bBuffer, int bHasSound) {
    static int bRedraw = 1;          // Redraw game screen
    static int bEnemyTurn = 0;       // Whether it's the enemy's turn
    static int bMusic = 1;
    static int nLastAttackX = -1;    // Enemy's last attack
    static int nLastAttackY = -1;
    static int nSteps = -100;        // Steps to take (enemy AI)
    int nDamage = 0;                 // Damage calculation
    int nGamestate = STATE_GAMEPLAY; // Default to remain in main game state
    int nPower = 5;                  // Power of the shot
    int i;                           // Index
    int j;                           // Second index
    int bCollision = 0;              // Collision detection

    // New landscape if out of ammo
    if (g_nPlayerAmmo <= 0 || g_nEnemyAmmo <= 0) {
        gameLoad();
    }

    // Check for help key or music key
    if (key_shifts & KB_CTRL_FLAG) {
        if (key[KEY_H]) {
            // Set redraw on return
            bRedraw = 1;

            // Go to help screen
            return STATE_HELP;
        } else if (key[KEY_M]) {
            if (bMusic) {
                // Pause music
                midi_pause();
                bMusic = 0;
            } else {
                // Resumse music
                midi_resume();
                bMusic = 1;
            }
            // Slight wait
            rest(10);
        }
    }

    if (!(g_bProjectileActive || bEnemyTurn)) {
        if (key[KEY_LEFT]) { // Left arrow pressed
            // Store old location
            g_nPlayerXOld = g_nPlayerX;
            g_nPlayerYOld = g_nPlayerY;

            // Move left
            g_nPlayerX--;

            // Check for out of bounds
            if (g_nPlayerX < 0)
                g_nPlayerX = 0;

            // Check too close
            if (abs(g_nPlayerX - g_nEnemyX) < SCREEN_W / 4)
                g_nPlayerX++;

            // Facing left
            g_nPlayerDirection = -1;

            // Redraw
            bRedraw = 1;
        } else if (key[KEY_RIGHT]) { // Right arrow pressed
            // Move right
            g_nPlayerX++;

            // Check for out of bounds
            if (g_nPlayerX > SCREEN_W - g_bPlayer -> w)
                g_nPlayerX = SCREEN_W - g_bPlayer -> w;

            // Check too close
            if (abs(g_nPlayerX - g_nEnemyX) < SCREEN_W / 4)
                g_nPlayerX--;

            // Facing right
            g_nPlayerDirection = 1;

            // Redraw
            bRedraw = 1;
        } else if (key[KEY_UP]) { // Up arrow pressed
            // Move cannon up
            g_nPlayerAngle += 0.1;

            // Check bounds
            if (g_nPlayerAngle > M_PI / 4) {
                g_nPlayerAngle = M_PI / 4;
            }

            // Redraw
            bRedraw = 1;
        } else if (key[KEY_DOWN]) { // Down arrow pressed
            // Move cannon down
            g_nPlayerAngle -= 0.1;

            // Check bounds
            if (g_nPlayerAngle < 0) {
                g_nPlayerAngle = 0;
            }

            // Redraw
            bRedraw = 1;
        } else if (key[KEY_SPACE] && g_nPlayerAmmo > 0) { // Spacebar pressed
            // Set up projectile
            g_bProjectileActive = 1;
            g_nProjectileX = g_nPlayerX + g_bPlayer -> w / 2;
            g_nProjectileY = g_nPlayerY + g_bPlayer -> h / 3 - 2;
            g_nProjectileAngle = g_nPlayerAngle;
            g_nProjectileDirection = g_nPlayerDirection;
            g_nProjectileCount = 0;

            // Play sound
            play_sample(g_sCannon, 250, 128, 1000, 0);

            // Update ammo
            g_nPlayerAmmo--;

            // Redraw
            bRedraw = 1;
        }
    }

    // Enemy player
    if (bEnemyTurn && (!g_bProjectileActive)) {
        // Step forward or backward randomly (fake intelligence)
        if (nSteps == -100) { // Special value to make decision
            nSteps = 0;
            nSteps += rand() % 10 - 20;
        } else if (nSteps > 0) {
            g_nEnemyX++; // Move right
            if (g_nEnemyX > SCREEN_W - g_bEnemy -> w)
                g_nEnemyX = SCREEN_W - g_bEnemy -> w;
            nSteps--;

            // Check too close
            if (abs(g_nPlayerX - g_nEnemyX) < SCREEN_W / 4)
                g_nEnemyX--;
        } else if (nSteps < 0) {
            g_nEnemyX--; // Move left
            if (g_nEnemyX < 0)
                g_nEnemyX = 0;
            nSteps++;

            // Check too close
            if (abs(g_nPlayerX - g_nEnemyX) < SCREEN_W / 4)
                g_nPlayerX++;
        } else {
            if (g_nEnemyX > g_nPlayerX) // Face the player
                g_nEnemyDirection = -1;
            else
                g_nEnemyDirection = 1;

            if (g_nEnemyDirection == -1) {
                if ((nLastAttackX > g_nPlayerX || nLastAttackY > g_nPlayerY) && (g_nEnemyAngle > 0)) {
                    // Shallower angle
                    g_nEnemyAngle -= 0.3;
                } else if ((nLastAttackX < g_nPlayerX || nLastAttackY < g_nPlayerY) && (g_nEnemyAngle < M_PI / 4)) {
                    // Steeper angle
                    g_nEnemyAngle += 0.3;
                }
            } else {
                if ((nLastAttackX < g_nPlayerX || nLastAttackY > g_nPlayerY) && (g_nEnemyAngle > 0)) {
                    // Shallower angle
                    g_nEnemyAngle -= 0.3;
                } else if ((nLastAttackX > g_nPlayerX || nLastAttackY < g_nPlayerY) && (g_nEnemyAngle < M_PI / 4)) {
                    // Steeper angle
                    g_nEnemyAngle += 0.3;
                }
            }

            // Check bounds
            if (g_nEnemyAngle < 0)
                g_nEnemyAngle = 0;
            if (g_nEnemyAngle > M_PI / 4)
                g_nEnemyAngle = M_PI / 4;

            if (g_nEnemyAmmo > 0) {
                // Set up projectile
                g_bProjectileActive = 1;
                g_nProjectileX = g_nEnemyX + g_bEnemy -> w / 2;
                g_nProjectileY = g_nEnemyY + g_bEnemy -> h / 3 - 2;
                g_nProjectileAngle = g_nEnemyAngle;
                g_nProjectileDirection = g_nEnemyDirection;
                g_nProjectileCount = 0;

                // Play sound
                play_sample(g_sCannon, 250, 128, 1000, 0);

                // Update ammo
                g_nEnemyAmmo--;
            }
        }

        // Redraw
        bRedraw = 1;
    }

    // Update projectile
    if (g_bProjectileActive) {
        // Move projectile
        nPower = 5 - 5 * abs(g_nProjectileAngle);
        g_nProjectileX += nPower * g_nProjectileDirection * cos(g_nProjectileAngle);
        g_nProjectileY -= nPower * sin(g_nProjectileAngle);

        // Check bounds
        if (g_nProjectileX < 0 || g_nProjectileX > SCREEN_W) {
            g_bProjectileActive = 0;
            if (bEnemyTurn) {
                bEnemyTurn = 0;
                nLastAttackX = g_nProjectileX;
                nLastAttackY = g_nProjectileY;
            } else {
                bEnemyTurn = 1;
            }
        } else if (g_nProjectileY < 0 || g_nProjectileY > SCREEN_H) {
            g_bProjectileActive = 0;
            if (bEnemyTurn) {
                bEnemyTurn = 0;
                nLastAttackX = g_nProjectileX;
                nLastAttackY = g_nProjectileY;
            } else {
                bEnemyTurn = 1;
            }
        }

        // Change angle
        g_nProjectileAngle -= 0.01;

        // Check collision with terrain
        if (getpixel(g_bLandscape, g_nProjectileX, g_nProjectileY) == COL_LANDSCAPE) {
            pthread_mutex_lock( & threadsafe);

            // Make a hole
            circlefill(g_bLandscape, g_nProjectileX, g_nProjectileY, 15, COL_SKY);

            // Collapse terrain above
            rectfill(g_bLandscape, g_nProjectileX - 15, 0, g_nProjectileX + 15, g_nProjectileY, COL_SKY);

            pthread_mutex_unlock( & threadsafe);

            // Stop projectile
            g_bProjectileActive = 0;
            if (bEnemyTurn) {
                bEnemyTurn = 0;
                nLastAttackX = g_nProjectileX;
                nLastAttackY = g_nProjectileY;
            } else {
                bEnemyTurn = 1;
            }
        }

        if (g_nProjectileCount > 4) {
            // Check collision with player (bounding box)
            if (g_nProjectileX > g_nPlayerX - 5 && g_nProjectileX < g_nPlayerX + g_bPlayer -> w + 5) {
                // Collision on X axis
                if (g_nProjectileY > g_nPlayerY - 5 && g_nProjectileY < g_nPlayerY + g_bPlayer -> h + 5) {
                    // Collision on both axis
                    g_nScore -= 2;
                    g_bProjectileActive = 0;
                    bEnemyTurn = bEnemyTurn ? 0 : 1;
                    // Calculate damage
                    nDamage = 100 - sqrt((g_nPlayerX + g_bPlayer -> w / 2 - g_nProjectileX) *
                        (g_nPlayerX + g_bPlayer -> w / 2 - g_nProjectileX) +
                        (g_nPlayerY + g_bPlayer -> h / 2 - g_nProjectileY) *
                        (g_nPlayerY + g_bPlayer -> h / 2 - g_nProjectileY));
                    g_nPlayerHealth -= (nDamage < 0 ? 0 : nDamage);
                    // Still alive?
                    if (g_nPlayerHealth <= 0) {
                        g_nScore -= 5;
                        nLastAttackX = -1;
                        nSteps = -100; // Reset steps

                        // New landscape
                        gameLoad();
                    }

                    // New landscape
                    gameLoad();
                }
            }

            // Check collision with enemy (bounding box)
            if (g_nProjectileX > g_nEnemyX - 5 && g_nProjectileX < g_nEnemyX + g_bEnemy -> w + 5) {
                // Collision on X axis
                if (g_nProjectileY > g_nEnemyY - 5 && g_nProjectileY < g_nEnemyY + g_bEnemy -> h + 5) {
                    // Collision on both axis
                    g_nScore += 5;
                    g_bProjectileActive = 0;
                    bEnemyTurn = bEnemyTurn ? 0 : 1;
                    // Calculate damage
                    nDamage = 100 - sqrt((g_nEnemyX + g_bEnemy -> w / 2 - g_nProjectileX) *
                        (g_nEnemyX + g_bEnemy -> w / 2 - g_nProjectileX) +
                        (g_nEnemyY + g_bEnemy -> h / 2 - g_nProjectileY) *
                        (g_nEnemyY + g_bEnemy -> h / 2 - g_nProjectileY));
                    g_nEnemyHealth -= (nDamage < 0 ? 0 : nDamage);
                    // Still alive?
                    if (g_nEnemyHealth <= 0) {
                        g_nScore += 10;
                        nLastAttackX = -1;
                        nSteps = -100; // Reset steps

                        // New landscape
                        gameLoad();
                    }
                }
            }
        }

        // Iteration count update
        g_nProjectileCount++;

        // Redraw
        bRedraw = 1;
    }

    // Calculate player Y location
    g_nPlayerY = 0;
    for (i = 0; i < SCREEN_H; i++) {
        for (j = 0; j < g_bPlayer -> w; j++) {
            // Check pixel for landscape, adjusting for height of player
            if (getpixel(g_bLandscape, g_nPlayerX + j, i + g_bPlayer -> h) == COL_LANDSCAPE) {
                bCollision = 1;
                break;
            }
        }
        if (bCollision)
            break;
        g_nPlayerY++;
    }

    // Reset collision
    bCollision = 0;

    // Calculate enemy Y location
    g_nEnemyY = 0;
    for (i = 0; i < SCREEN_H; i++) {
        for (j = 0; j < g_bEnemy -> w; j++) {
            // Check pixel for landscape, adjusting for height of enemy
            if (getpixel(g_bLandscape, g_nEnemyX + j, i + g_bEnemy -> h) == COL_LANDSCAPE) {
                bCollision = 1;
                break;
            }
        }
        if (bCollision)
            break;
        g_nEnemyY++;
    }

    // Draw the game screen
    if (bRedraw) {
        drawGame(bBuffer);

        rest(10);
        bRedraw = 0;
    }

    return nGamestate;
}

/**
 * Sets up global data
 *
 * Parameters:
 * dData        Reference to game data
 */
void setupGlobal(DATAFILE * dData) {
    // Load global font
    g_fFont = (FONT * ) dData[FONT_PCX].dat;

    // Load player sprite
    g_bPlayer = (BITMAP * ) dData[PLAYER_BMP].dat;

    // Load enemy sprite
    g_bEnemy = (BITMAP * ) dData[ENEMY_BMP].dat;

    // Load cannon sample	
    g_sCannon = (SAMPLE * ) dData[CANNON_WAV].dat;
}

/**
 * Displays the help screen.
 *
 * Parameters:
 * bBuffer      Display buffer
 *
 * Returns:
 *   Game state at end of iteration
 */
int helpScreen(BITMAP * bBuffer) {
    int nGamestate = STATE_HELP;                 // Default to remain in help state
    int nTitleColor = makecol(0xf9, 0xef, 0x06); // Color of the title text
    int nTextColor = makecol(0xf9, 0xef, 0xf6);  // Color of the other text

    // Clear buffer
    clear_bitmap(bBuffer);

    // Display text
    textout_centre_ex(bBuffer, font, "HELP", SCREEN_W / 2, SCREEN_H * 0.15, nTitleColor, -1);

    // Display help
    textout_centre_ex(bBuffer, font, "CONTROLS", SCREEN_W / 2, SCREEN_H * 0.25, nTextColor, -1);
    textout_ex(bBuffer, font, "Left arrow    Move to the left", SCREEN_W / 3 - 20, SCREEN_H * 0.3, nTextColor, -1);
    textout_ex(bBuffer, font, "Right arrow   Move to the left", SCREEN_W / 3 - 20, SCREEN_H * 0.35, nTextColor, -1);
    textout_ex(bBuffer, font, "Up arrow      Move cannon up", SCREEN_W / 3 - 20, SCREEN_H * 0.4, nTextColor, -1);
    textout_ex(bBuffer, font, "Down arrow    Move cannon down", SCREEN_W / 3 - 20, SCREEN_H * 0.45, nTextColor, -1);
    textout_ex(bBuffer, font, "Spacebar      Fire!", SCREEN_W / 3 - 20, SCREEN_H * 0.5, nTextColor, -1);

    textout_centre_ex(bBuffer, font, "SCORING", SCREEN_W / 2, SCREEN_H * 0.6, nTextColor, -1);
    textout_centre_ex(bBuffer, font, "You gain points for hitting the enemy and lose points when hit",
        SCREEN_W / 2, SCREEN_H * 0.65, nTextColor, -1);

    textout_centre_ex(bBuffer, font, "Press Enter to continue playing...",
        SCREEN_W / 2, SCREEN_H * 0.75, nTextColor, -1);

    // Copy the buffer to the screen
    blit(bBuffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    // Check for key press
    if (key[KEY_ENTER]) {
        nGamestate = STATE_GAMEPLAY; // Return to game
    }

    return nGamestate;
}

/**
 * Reduces the height of the terrain every 5 seconds
 */
void * terrainThread(void * threadid) {
    static int counter = 0;
    int i, j;

    while (!g_bExiting) {
        // Wait 5 seconds while looping back to check for exit
        if (counter++ < 50) {
            rest(100);
            continue;
        }
        counter = 0;

        pthread_mutex_lock( & threadsafe);

        // If there is active terrain
        if (g_bLandscape != NULL) {
            for (i = 0; i < SCREEN_W; i++) {
                for (j = 0; j < SCREEN_H; j++) {
                    // Strip the top pixel
                    if (getpixel(g_bLandscape, i, j) != COL_SKY) {
                        putpixel(g_bLandscape, i, j, COL_SKY);
                        break;
                    }
                }
            }
        }

        pthread_mutex_unlock( & threadsafe);
    }
}

/**
 * Main entry point.
 * Initializes everything, loads the data and prepares to start the game.
 * Handles the main loop, calling other functions as necessary.
 * Cleans up when done.
 */
int main(void) {
    DATAFILE * dData;             // Reference to game data

    BITMAP * bBuffer;             // Display buffer

    // Audio data
    MIDI * mMusic = NULL;         // Background music

    int bHasSound = 0;            // Sound enabled
    int nGamestate = STATE_INTRO; // Current game state

    // Threading for terrain
    pthread_t pthread0;
    int threadid0 = 0;

    // Initialize required systems
    bHasSound = init();

    // Create a buffer for double-buffering
    bBuffer = create_bitmap(SCREEN_W, SCREEN_H);

    // Load in game data, exit on failure
    dData = load_datafile(GAME_DATAFILE);
    if (!dData) {
        allegro_message("Error: %s is missing", GAME_DATAFILE);
        allegro_exit();
        return -1;
    }

    // Set up font
    setupGlobal(dData);

    // Load audio resources if needed
    if (bHasSound) {
        if (mMusic = (MIDI * ) dData[MUSIC_MID].dat) {
            play_midi(mMusic, 1);
        }
    }

    // Start terrain thread
    pthread_create( & pthread0, NULL, terrainThread, (void * ) & threadid0);

    // Main loop
    while (!key[KEY_ESC] && nGamestate != STATE_END) {
        // Check game state and pass iteration to appropriate function
        switch (nGamestate) {
        case STATE_INTRO:
            nGamestate = introLoop(bBuffer);
            break;
        case STATE_LOADING:
            nGamestate = gameLoad();
        case STATE_GAMEPLAY:
            nGamestate = playLoop(bBuffer, bHasSound);
            break;
        case STATE_HELP:
            nGamestate = helpScreen(bBuffer);
            break;
        }
    }

    // Close thread
    g_bExiting = 1;
    pthread_mutex_destroy( & threadsafe);
    rest(200);

    // Stop music
    if (mMusic) {
        stop_midi();
    }

    // Destroy landcsape
    destroy_bitmap(g_bLandscape);

    // Shutdown used systems
    shutdown(dData, bHasSound);

    return 0;
}
END_OF_MAIN()
