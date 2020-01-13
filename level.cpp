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

static uint8_t m_current_level[BOARD_HEIGHT][BOARD_WIDTH];


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
  memcpy( m_current_level, &m_levels[ p_level ], 
          sizeof( uint8_t ) * ( BOARD_HEIGHT * BOARD_WIDTH ) );
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


/*
 * level_hit_brick - hits the specified brick with a ball. Depending on the
 *                   type of brick, we might modify or remove it.
 * 
 * uint8_t - the row the brick is on
 * uint8_t - the column the brick is in
*/

void level_hit_brick( uint8_t p_row, uint8_t p_column )
{
  /* Sanity check the location. */
  if ( ( p_row >= BOARD_HEIGHT ) || ( p_column >= BOARD_WIDTH ) )
  {
    return;
  }
  
  /* And only act if a brick is there. */
  if ( m_current_level[p_row][p_column] == 0 )
  {
    return;
  }
  
  /* For now, we'll just decrement the brick type. */
  m_current_level[p_row][p_column]--;
}

const char *level_get_bricktype( uint8_t p_bricktype )
{
  switch( p_bricktype )
  {
    case 3:
      return "brick_yellow";
    case 2:
      return "brick_orange";
    case 1:
      return "brick_red";
  }
  
  /* Default to a red brick, should never be reached though... */
  return "brick_red";
}


/*
 * level_get_bricks - returns the number of bricks remaining in the level
 * 
 * Returns a brick count.
 */

uint16_t level_get_bricks( void )
{
  uint8_t  l_row, l_column;
  uint16_t l_bricks = 0;
  
  /* Simply count the non-zero entries in the level. */
  for ( l_row = 0; l_row < BOARD_HEIGHT; l_row++ )
  {
    for ( l_column = 0; l_column < BOARD_WIDTH; l_column++ )
    {
      if ( m_current_level[l_row][l_column] > 0 )
      {
        l_bricks++;
      }
    }
  }
  return l_bricks;
}

/* End of level.cpp */
