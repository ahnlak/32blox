/*
 * level.cpp - part of 32Blox, a breakout game for the 32blit built to
 * explore the API.
 *
 * Defines the game levels, and maintains the current level state as well.
 * Levels are grids of up to 20x20 blocks.
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

#include <string.h>


/* Local headers. */

#include "32blit.hpp"
#include "32blox.hpp"


/* Module variables. */

static uint8_t m_current_level[10][10];


/* Raw level data. */

#include "levels.h"


/* Functions. */

using namespace blit;


/*
 * level_init - initialises the current level state to the provided level.
 *
 * uint8_t - the current level the player is on.
 */

void level_init( uint8_t p_level )
{
  /* Quite easy really, we just copy the whole block of level data. */
  memcpy( m_current_level, &m_levels[ p_level ], sizeof( uint8_t ) * 100 );
}


/*
 * level_get_line - returns the current bricks in the requested line. 
 *
 * uint8_t - the line of bricks to fetch, between 0 and 10
 * 
 * Returns a 10 byte array of brick types.
 */
 
uint8_t *level_get_line( uint8_t p_line )
{
  /* Simple return of the required line. */
  return m_current_level[ p_line ];
}

const char *level_get_bricktype( uint8_t p_bricktype )
{
  return "brick_yellow";
}

uint16_t level_get_bricks( void )
{
  return 0;
}

/* End of level.cpp */
