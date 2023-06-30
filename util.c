/**
 * File:        util.c
 * Purpose:     Utility functions that abstract some functionality
 *
 * Author:      Lionel Pinkhard
 * Date:        January 12, 2019
 * Version:     1.0
 *
 */

// Include the headers
#include "util.h"

/**
 * Initializes required systems (Allegro, random number generator, and devices)
 *
 * Returns:
 *   1 if sound is available, 0 otherwise
 */
int init() {
	// Initialize Allegro
    allegro_init();      
	set_color_depth(16);
    set_gfx_mode(GFX_MODE, GFX_HEIGHT, GFX_WIDTH, 0, 0);

	// Initialize random number generator
	srand(time(NULL));

	// Install devices
    install_keyboard();
    install_mouse();
    install_timer();
	
    // Install sound
    if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) == 0) {
    	return 1;   // Sound available
	}
	
    // No sound available
	return 0;
}

/**
 * Shuts down used systems.
 * Removes sound, unloads data file and restores mouse cursor.
 * Shuts down Allegro.
 */
void shutdown(DATAFILE *dData, int bHasSound) {
    // Remove sound if needed
	if (bHasSound) {
		remove_sound();
	}
	
    // Unload data
	unload_datafile(dData);
    
    // Restore mouse cursor
	set_mouse_sprite(NULL);
	
    // Shut down Allegro
	allegro_exit();
}
