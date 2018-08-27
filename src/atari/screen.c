/**
 * PLATOTerm64 - A PLATO Terminal for the Commodore 64
 * Based on Steve Peltz's PAD
 * 
 * Author: Thomas Cherryhomes <thom.cherryhomes at gmail dot com>
 *
 * screen.c - Display output functions
 */

#include <peekpoke.h>
#include <tgi.h>
#include <stdint.h>
#include <conio.h>
#include "../config.h"

extern uint8_t pal[2];
extern ConfigInfo config; 

/**
 * screen_init_hook()
 * Called after tgi_init to set any special features, e.g. nmi trampolines.
 */
void screen_init_hook(void)
{
  POKE(0x22F,62); // Turn on P/M DMA for mouse cursor.
  bordercolor(TGI_COLOR_BLUE);
}

/**
 * screen_load_driver()
 * Load the TGI driver
 */
void screen_load_driver(void)
{
  tgi_install(tgi_static_stddrv);
}

/**
 * screen_cycle_foreground()
 * Go to the next foreground color in palette
 */
void screen_cycle_foreground(void)
{
  ++config.color_foreground;
}

/**
 * screen_cycle_background()
 * Go to the next background color in palette
 */
void screen_cycle_background(void)
{
  ++config.color_background;
}

/**
 * screen_cycle_border()
 * Go to the next border color in palette
 */
void screen_cycle_border(void)
{
  ++config.color_border;
}

/**
 * screen_cycle_foreground_back()
 * Go to the previous foreground color in palette
 */
void screen_cycle_foreground_back(void)
{
  --config.color_foreground;
}

/**
 * screen_cycle_background_back()
 * Go to the previous background color in palette
 */
void screen_cycle_background_back(void)
{
  --config.color_background;
}

/**
 * screen_cycle_border_back()
 * Go to the previous border color in palette
 */
void screen_cycle_border_back(void)
{
  --config.color_border;
}

/**
 * Set the terminal colors
 */
void screen_update_colors(void)
{
  pal[0]=config.color_background;
  pal[1]=config.color_foreground;
  tgi_setpalette(pal);
  POKE(712,config.color_border);
}

/**
 * Wait(void) - Sleep for approx 16.67ms
 */
void screen_wait(void)
{
  // TODO: function to do one vblank wait
}

/**
 * screen_beep(void) - Beep the terminal
 */
void screen_beep(void)
{
  // My feeble attempt at a beep.
}
