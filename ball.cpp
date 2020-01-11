/*
 * ball.cpp - part of 32Blox, a breakout game for the 32blit built to
 * explore the API.
 *
 * Handles the behaviour of the balls; the location, the speed and computations
 * of bounce. This is all kept separate from the game logic to cope with
 * multiball scenarios later on.
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

static struct { 
  float     x;
  float     y;
  float     dx;
  float     dy;
  bool      stuck;
  bool      active;
} m_balls[5];


/* Functions. */

using namespace blit;


/*
 * ball_create - generate a new player ball, on the player bat
 *
 * uint16_t - the location of the player bat
 *
 * Returns the ball ID of the new ball.
 */

uint8_t ball_create( uint16_t p_batloc )
{
  uint8_t l_index;
  
  /* Find an empty slot in the ball array. */
  for ( l_index = 0; l_index < MAX_BALLS; l_index++ )
  {
    if ( !m_balls[l_index].active )
    {
      break;
    }
  }
  
  /* If we didn't find a slot, we can't really do this. */
  if ( l_index == MAX_BALLS )
  {
    return MAX_BALLS;
  }
  
  /* Good, so initiate the ball as stuck to the bat. */
  m_balls[l_index].x = fb.bounds.h - 11;
  m_balls[l_index].y = p_batloc;
  m_balls[l_index].dx = m_balls[l_index].dy = 0;
  m_balls[l_index].stuck = m_balls[l_index].active = true;
  
  /* And return the new ball. */
  return l_index;
}

uint8_t ball_spawn( uint8_t )
{
  /* __RETURN__ */
  return 0;
}


/*
 * ball_update - updates the location of the ball, taking into account any
 *               potential bounces.
 * 
 * uint8_t - the ball ID being updated
 * uint16_t - the location of the player bat, potentially relavent
 * 
 * Returns a point indicating the new location of the ball.
 */

point ball_update( uint8_t p_ballid, uint16_t p_batloc )
{
  uint16_t l_newx, l_newy;
  
  /* Only active, valid balls need apply. */
  if ( ( p_ballid < 0 ) || ( p_ballid >= MAX_BALLS ) || ( !m_balls[p_ballid].active ) )
  {
    return point( 0, 0 );
  }
  
  /* If we're stuck to the bat, reflect that. */
  if ( m_balls[p_ballid].stuck )
  {
    m_balls[p_ballid].y = p_batloc;
    return point( m_balls[p_ballid].y, m_balls[p_ballid].x );
  }

  /* Then we're in free flight! First, calculate the new possible location. */
  l_newx = m_balls[p_ballid].x + m_balls[p_ballid].dx;
  l_newy = m_balls[p_ballid].y + m_balls[p_ballid].dy;
  
  /* Check for hard boundaries on the play area itself. */
  if ( ( l_newx <= 10 ) || ( l_newx >= fb.bounds.h ) )
  {
    m_balls[p_ballid].dx *= -1.0f;
  }
  if ( ( l_newy <= 0 ) || ( l_newy >= fb.bounds.w ) )
  {
    m_balls[p_ballid].dy *= -1.0f;
  }
  
  /* Ok, apply the new deltas then and we're done. */
  m_balls[p_ballid].x += m_balls[p_ballid].dx;
  m_balls[p_ballid].y += m_balls[p_ballid].dy;
  
  /* And return the new ball location; probably not needed for now... */
  return point( m_balls[p_ballid].y, m_balls[p_ballid].x );
}


/*
 * ball_render - draws the specified ball
 * 
 * uint8_t - the ball ID to be rendered
 */

void ball_render( uint8_t p_ballid )
{
  /* Obviously only render active balls. */
  if ( ( p_ballid < 0 ) || ( p_ballid >= MAX_BALLS ) || ( !m_balls[p_ballid].active ) )
  {
    return;
  }
  
  /* So simply draw the ball sprite in. */
  sprite_render( "ball", m_balls[p_ballid].y, m_balls[p_ballid].x, ALIGN_MIDCENTRE );
}


/*
 * ball_launch - releases a ball currently stuck to the player's bat.
 * 
 * uint8_t - the ball ID being released
 */

void ball_launch( uint8_t p_ballid )
{
  /* Only meaningful for real, active, stuck balls. */
  if ( ( p_ballid < 0 ) || ( p_ballid >= MAX_BALLS ) || 
       ( !m_balls[p_ballid].active ) || ( !m_balls[p_ballid].stuck ) )
  {
    return;
  }
  
  /* So, all we really do is create a slightly random vector to release on. */
  m_balls[p_ballid].dx = -0.5;
  m_balls[p_ballid].dy = -0.5;
  m_balls[p_ballid].stuck = false;
}


/* End of ball.cpp */
