/**
 * File:        util.h
 * Purpose:     Header file for util.c
 *
 * Author:      Lionel Pinkhard
 * Date:        January 12, 2019
 * Version:     1.0
 *
 */

// Only include this header once
#ifndef _UTIL_H_
#define _UTIL_H_

// Include Allegro library
#include <allegro.h>

// Include configuration (allows for easily changing settings)
#include "config.h"

// Reference to global font from main.c
extern FONT *g_fFont;

// Declarations for functions in util.c (needed by main.c)
int init();
void shutdown(DATAFILE *dData, int bHasSound);

#endif
