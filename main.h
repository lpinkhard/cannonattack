/**
 * File:        main.h
 * Purpose:     Header file for main.c
 *
 * Author:      Lionel Pinkhard
 * Date:        January 29, 2019
 * Version:     1.0
 *
 */

// Only include this header once
#ifndef _MAIN_H_
#define _MAIN_H_

// Include Allegro library and C stdlib
#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>

// Include pthread
#include "pthread.h"

// Include math library
#include <math.h>

// Include data file references and headers for external functions
#include "defines.h"
#include "util.h"

// Game states
#define STATE_INTRO 0
#define STATE_LOADING 1
#define STATE_GAMEPLAY 2
#define STATE_END 3
#define STATE_HELP 4

// Forward declarations for functions in main.c (not strictly necessary given the code layout)
void drawGame(BITMAP *bBuffer);
int introLoop(BITMAP *bBuffer);
int gameLoad();
int playLoop(BITMAP *bBuffer, int bHasSound);
int helpScreen(BITMAP *bBuffer);
void setupGlobal(DATAFILE *dData);
int main(void);

#endif
