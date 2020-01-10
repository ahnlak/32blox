/*
 * game.cpp - part of 32Blox, a breakout game for the 32blit built to
 * explore the API.
 *
 * The main game logic; if the user is bouncing balls, she's here.
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



/* Functions. */

using namespace blit;

/*
 * game_init - called at the start of a new game, to blank scores and
 *             configure suitable levels.
 */

void game_init( void )
{
}

/*
 * game_update - handle playing the game.
 *
 * uint32_t - the gametick so that we animate at a known rate.
 * 
 * Returns gamestate_t, the state to continue in. Should either be GAME, 
 * or DEATH if the player fails.
 */

gamestate_t game_update( uint32_t p_time )
{
  
  /* Default to the status quo, then. */
  return STATE_GAME;
}


/* 
 * game_render - draw the current splash screen state onto the screen.
 */

void game_render( void )
{
  /* Clear the screen back to something sensible. */
  fb.pen( rgba( 0, 0, 0, 255 ) );
  fb.clear();
  
  
  fb.pen( rgba( 255, 255, 255, 255 ) );
  fb.text( "GGGAAAMMMEEE   OOOVVVEEERRR!!!!!!", &outline_font[0][0], point( 94, 180 ), true );
}


/* End of game.cpp */
