/*
 * 32blox.cpp - part of 32Blox, a breakout game for the 32blit 
 *              built to explore the API.
 *
 * This file contains the main entry functions to the 32blit API; essentially 
 * the 'main' of the game.
 *
 * Please note that this is a first attempt at understanding a somewhat fluid
 * API on a shiny new bit of kit, so it probably is not full of 'best practice'.
 * It will hopefully serve as some sort of starting point, however.
 *
 * Coyright (C) 2020 Pete Favelle <pete@fsquared.co.uk>
 *
 * This software is provided under the MIT License. See LICENSE.txt for details.
 */

/* System headers. */

/* Local headers. */

#include "32blit.hpp"
#include "32blox.hpp"


/* Module variables. */

gamestate_t m_gamestate = STATE_SPLASH;


/* Functions. */

using namespace blit;

/*
 * init - called once on startup to initialise the game.
 */

void init( void )
{

  /* Set the screen into hires (320x240) mode. */
  blit::set_screen_mode( screen_mode::hires );

  /* And blank the screen. */
  fb.pen( rgba( 100, 0, 0, 255 ) );
  fb.clear();

}


/*
 * update - called every tick, to update the state of the game.
 *
 * uint32_t - the elapsed time (in ms) since the application launched.
 */

void update( uint32_t p_time )
{

  /* What we're updating depends rather a lot on our current state. */
  switch( m_gamestate ) {

    case STATE_SPLASH:      /* Show the user a simple splash screen. */
      splash_update( p_time );
      break;

    default:                /* Erk! This should Not Be Possible. */
      break;

  }
}


/*
 * render - called every time, to update the screen.
 *
 * uint32_t - the elapsed time (in ms) since the application launched.
 */

void render( uint32_t p_time )
{

  /* As with updates, what we render depends on our current gamestate. */
  switch( m_gamestate ) { 

    case STATE_SPLASH:      /* Show the user a simple splash screen. */
      splash_render();
      break;

    default:                /* Erk! This should Not Be Possible. */
      break;

  }
}


/* End of 32blox.cpp */
