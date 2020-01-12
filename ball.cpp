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
  size    l_ballsize = sprite_size( "ball" );
  
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
  m_balls[l_index].x = fb.bounds.h - 8 - ( ( l_ballsize.h+1 ) / 2 );
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
 * Returns any score that has been earned by the update, or -1 if the ball died.
 */

int8_t ball_update( uint8_t p_ballid, uint16_t p_batloc, uint8_t p_batwidth )
{
  uint8_t  l_score = 0;
  uint8_t  l_row, l_column;
  uint8_t *l_bricks;
  uint16_t l_newx, l_newy;
  size     l_ballsize = sprite_size( "ball" );
  
  /* Only active, valid balls need apply. */
  if ( ( p_ballid < 0 ) || ( p_ballid >= MAX_BALLS ) || ( !m_balls[p_ballid].active ) )
  {
    return 0;
  }
  
  /* If we're stuck to the bat, reflect that. */
  if ( m_balls[p_ballid].stuck )
  {
    m_balls[p_ballid].y = p_batloc;
    return 0;
  }

  /* Then we're in free flight! First, calculate the new possible location. */
  l_newx = m_balls[p_ballid].x + m_balls[p_ballid].dx;
  l_newy = m_balls[p_ballid].y + m_balls[p_ballid].dy;
  
  /* Check for hard boundaries on the play area itself. */
  if ( l_newx <= 10 ) 
  {
    m_balls[p_ballid].dx *= -1.0f;
    l_score++;
  }
  if ( ( l_newy <= 0 ) || ( l_newy >= fb.bounds.w ) )
  {
    m_balls[p_ballid].dy *= -1.0f;
    l_score++;
  }
  
  /* If we've hit the bottom, though, we have bigger problems. */
  if ( l_newx >= fb.bounds.h )
  {
    m_balls[p_ballid].active = 0;
    return -1;
  }
  
  /* Check to see if we hit the bat. */
  if ( ( m_balls[p_ballid].dx > 0.0f ) && 
       ( l_newx >= ( fb.bounds.h - 11 ) ) && 
       ( m_balls[p_ballid].x < ( fb.bounds.h - 8 - ( ( l_ballsize.h+1 ) / 2 ) ) ) )
  {
    /* So, we've crossed the baseline. Check the bat bounds. */
    if ( ( ( l_newy + ( l_ballsize.w / 2 ) ) >= ( p_batloc - ( p_batwidth / 2 ) ) ) &&
         ( ( l_newy - ( l_ballsize.w / 2 ) ) <= ( p_batloc + ( p_batwidth / 2 ) ) ) )
    {
      /* Bounce vertically, and score. */
      m_balls[p_ballid].dx *= -1.0f;
      l_score++;
      
      /* But if it was an edge shot, impart some horizontal speed too. */
      if ( ( l_newy + ( l_ballsize.w / 2 ) ) < ( p_batloc - ( p_batwidth / 2 ) + 3 ) )
      {
        m_balls[p_ballid].dy -= 0.5f;
      }
      if ( ( l_newy - ( l_ballsize.w / 2 ) ) > ( p_batloc + ( p_batwidth / 2 ) - 3 ) )
      {
        m_balls[p_ballid].dy += 0.5f;
      }
    }
  }
  
  /* Lastly, we need to consider bricks. Where are we? */
  if ( m_balls[p_ballid].dx < 0 )
  {
    l_row = ( l_newx - 10 - ( l_ballsize.h / 2 ) ) / 8;
  }
  else
  {
    l_row = ( l_newx - 10 + ( l_ballsize.h / 2 ) ) / 8;
  }
  if ( m_balls[p_ballid].dy < 0 )
  {
    l_column = ( l_newy - ( l_ballsize.w / 2 ) ) / 16;
  }
  else
  {
    l_column = ( l_newy + ( l_ballsize.w / 2 ) ) / 16;
  }
  
  /* Obviously we don't worry if we're below the bottom row. */
  if ( l_row < 10 )
  {
    /* Fetch the row details. */
    l_bricks = level_get_line( l_row );
    
    /* And look at the brick we're in. If we're are, then hit and bounce. */
    if ( l_bricks[l_column] > 0 )
    {
      /* Hit the brick, give a score. */
      level_hit_brick( l_row, l_column );
      l_score += 10;
      
      /* And work out which direction to bounce in. */
      if ( m_balls[p_ballid].dx < 0 )
      {
        if ( (uint8_t)( ( m_balls[p_ballid].x - 10 - ( l_ballsize.h / 2 ) ) / 8 ) != l_row )
        {
          /* Then it's a vertical bounce. */
          m_balls[p_ballid].dx *= -1.0f;
        }
      }
      else
      {
        if ( (uint8_t)( ( m_balls[p_ballid].x - 10 + ( l_ballsize.h / 2 ) ) / 8 ) != l_row )
        {
          /* Then it's a vertical bounce. */
          m_balls[p_ballid].dx *= -1.0f;
        }
      }
      
      if ( m_balls[p_ballid].dy < 0 )
      {
        if ( (uint8_t)( ( m_balls[p_ballid].y - ( l_ballsize.w / 2 ) ) / 16 ) != l_column )
        {
          /* Then it's a horizontal bounce. */
          m_balls[p_ballid].dy *= -1.0f;
        }
      }
      else
      {
        if ( (uint8_t)( ( m_balls[p_ballid].y + ( l_ballsize.w / 2 ) ) / 16 ) != l_column )
        {
          /* Then it's a horizontal bounce. */
          m_balls[p_ballid].dy *= -1.0f;
        }
      }
    }
  }  
  
  /* Ok, apply the new deltas then and we're done. */
  m_balls[p_ballid].x += m_balls[p_ballid].dx;
  m_balls[p_ballid].y += m_balls[p_ballid].dy;
  
  /* And return the new ball location; probably not needed for now... */
  return l_score;
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
  m_balls[p_ballid].dx = -0.75f;
  m_balls[p_ballid].dy = -0.5f + ( ( blit::random() % 100 ) / 100.0f );
  m_balls[p_ballid].stuck = false;
}


/*
 * ball_stuck - lets us know if the ball is stuck to the bat.
 * 
 * uint8_t - the ball ID being queried
 * 
 * Returns bool, true if the ball is attached to the bat.
 */

bool ball_stuck( uint8_t p_ballid ) 
{
  if ( ( p_ballid < 0 ) || ( p_ballid >= MAX_BALLS ) || ( !m_balls[p_ballid].active ) )
  {
    return false;
  }
  
  return m_balls[p_ballid].stuck;
}


/* End of ball.cpp */
