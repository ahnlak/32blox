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


/* Module functions. */

static bool check_brick_hit( uint16_t p_row, uint16_t p_column, uint16_t p_newx, uint16_t p_newy )
{
  uint8_t *l_bricks;
  
  /* Sanity check the arguments. */
  if ( ( p_row >= 10 ) || ( p_column >= 10 ) )
  {
    return false;
  }
  
  /* Fetch the bricks. */
  l_bricks = level_get_line( p_row );
    
  /* No hit where there is no brick. */
  if ( l_bricks[p_column] == 0 )
  {
    return false;
  }
  
  /* Sprite collision check then. */
  return sprite_collide( level_get_bricktype( l_bricks[p_column] ),
                         p_column * 16, ( p_row * 8 ) + 10, ALIGN_TOPLEFT,
                         "ball", p_newy, p_newx, ALIGN_MIDCENTRE );
}

/* Functions. */


/*
 * ball_create - generate a new player ball, on the player bat
 *
 * bat_t - details of the player's bat
 *
 * Returns the ball ID of the new ball.
 */

uint8_t ball_create( bat_t p_bat )
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
  m_balls[l_index].x = p_bat.baseline - ( ( l_ballsize.h+1 ) / 2 );
  m_balls[l_index].y = p_bat.position;
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
 * bat_t   - the players bat details, potentially important
 * 
 * Returns any score that has been earned by the update, or -1 if the ball died.
 */

int8_t ball_update( uint8_t p_ballid, bat_t p_bat )
{
  uint8_t  l_score = 0;
  uint8_t  l_row, l_column;
  uint16_t l_newx, l_newy;
  float    l_edge, l_speed;
  size     l_ballsize = sprite_size( "ball" );
  bool     l_bounced;
  
  /* Only active, valid balls need apply. */
  if ( ( p_ballid < 0 ) || ( p_ballid >= MAX_BALLS ) || ( !m_balls[p_ballid].active ) )
  {
    return 0;
  }
  
  /* If we're stuck to the bat, reflect that. */
  if ( m_balls[p_ballid].stuck )
  {
    m_balls[p_ballid].y = p_bat.position;
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
  if ( ( l_newy <= 0 ) || ( l_newy >= blit::fb.bounds.w ) )
  {
    m_balls[p_ballid].dy *= -1.0f;
    l_score++;
  }
  
  /* If we've hit the bottom, though, we have bigger problems. */
  if ( l_newx >= blit::fb.bounds.h )
  {
    m_balls[p_ballid].active = 0;
    return -1;
  }
  
  /* See if we've dropped below the bat baseline. */
  if ( ( ( l_newx + ( l_ballsize.h / 2 ) ) >= p_bat.baseline ) && 
       ( ( m_balls[p_ballid].x + ( l_ballsize.h / 2 ) ) < p_bat.baseline ) )
  {
    /* Check to see if we hit the bat. */
    if ( sprite_collide( "bat_normal", p_bat.position, p_bat.baseline, ALIGN_TOPCENTRE,
                        "ball", l_newy, l_newx, ALIGN_MIDCENTRE ) )
    {
      /* Bounce vertically, and score. */
      m_balls[p_ballid].dx *= -1.0f;
      l_score++;
      
      /* Take into account edge shots, somehow... */
      l_edge = ( l_newy + ( l_ballsize.w / 2 ) ) - ( p_bat.position - ( p_bat.width / 2 ) );
      if ( l_edge < 5.0f )
      {
        m_balls[p_ballid].dy -= ( ( 5.0f - l_edge ) / 10.0f );
      }

      l_edge = ( p_bat.position + ( p_bat.width / 2 ) ) - ( l_newy - ( l_ballsize.w / 2 ) );
      if ( l_edge < 5.0f )
      {
        m_balls[p_ballid].dy += ( ( 5.0f - l_edge ) / 10.0f );
      }
    }
  }
  
  /* Lastly, bricks. Nothing to do if we're below the play space. */
  if ( ( ( l_newx - 10 ) / 8 ) < 10 )
  {
    /* We need to know roughly where we are now. */
    l_row = ( m_balls[p_ballid].x - 10 ) / 8;
    l_column = m_balls[p_ballid].y / 16;
    
    /* First, consider the row above us if we're moving up and not at the top. */
    if ( ( m_balls[p_ballid].dx < 0 ) && ( l_row > 0 ) )
    {
      /* Check the three bricks above us. */
      l_bounced = false;
      if ( check_brick_hit( l_row - 1, l_column - 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row - 1, l_column - 1 );
      }
      else if ( check_brick_hit( l_row - 1, l_column, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row - 1, l_column );
      }
      else if ( check_brick_hit( l_row - 1, l_column + 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row - 1, l_column + 1 );
      }
      
      /* Lastly, bounce and score if we, well, bounced. */
      if ( l_bounced )
      {
        l_score += 10;
        m_balls[p_ballid].dx *= -1.0f;
      }
    }
    
    /* Then the row below us, if we're moving down (and not off the bottom) */
    if ( ( m_balls[p_ballid].dx > 0 ) && ( l_row < 9 ) )
    {
      /* Check the three bricks below us. */
      l_bounced = false;
      if ( check_brick_hit( l_row + 1, l_column - 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row + 1, l_column - 1 );
      }
      else if ( check_brick_hit( l_row + 1, l_column, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row + 1, l_column );
      }
      else if ( check_brick_hit( l_row + 1, l_column + 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row + 1, l_column + 1 );
      }
      
      /* Lastly, bounce and score if we, well, bounced. */
      if ( l_bounced )
      {
        l_score += 10;
        m_balls[p_ballid].dx *= -1.0f;
      }
    }
    
    /* Now leftward collisions. */
    if ( ( m_balls[p_ballid].dy < 0 ) && ( l_column > 0 ) )
    {
      /* Check the three bricks left us. */
      l_bounced = false;
      if ( check_brick_hit( l_row - 1, l_column - 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row - 1, l_column - 1 );
      }
      else if ( check_brick_hit( l_row, l_column - 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row, l_column - 1 );
      }
      else if ( check_brick_hit( l_row + 1, l_column - 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row + 1, l_column - 1 );
      }
      
      /* Lastly, bounce and score if we, well, bounced. */
      if ( l_bounced )
      {
        l_score += 10;
        m_balls[p_ballid].dy *= -1.0f;
      }
    }
    
    /* And finally, rightward collisions. */
    if ( ( m_balls[p_ballid].dy > 0 ) && ( l_column < 15 ) )
    {
      /* Check the three bricks right us. */
      l_bounced = false;
      if ( check_brick_hit( l_row - 1, l_column + 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row - 1, l_column + 1 );
      }
      else if ( check_brick_hit( l_row, l_column + 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row, l_column + 1 );
      }
      else if ( check_brick_hit( l_row + 1, l_column + 1, l_newx, l_newy ) )
      {
        l_bounced = true;
        level_hit_brick( l_row + 1, l_column + 1 );
      }
      
      /* Lastly, bounce and score if we, well, bounced. */
      if ( l_bounced )
      {
        l_score += 10;
        m_balls[p_ballid].dy *= -1.0f;
      }
    }
  }

  /* Lastly, check that the deltas haven't got *too* out of hand. */
  l_speed = ( ( m_balls[p_ballid].dx * m_balls[p_ballid].dx ) +
              ( m_balls[p_ballid].dy * m_balls[p_ballid].dy ) );
  if ( l_speed > 0.9f )
  {
    /* Just nudge everything down a little. */
    m_balls[p_ballid].dx *= 0.95f;
    m_balls[p_ballid].dy *= 0.95f;
  }
  if ( l_speed < 0.6f )
  {
    /* Just nudge everything up a little. */
    m_balls[p_ballid].dx *= 1.05f;
    m_balls[p_ballid].dy *= 1.05f;
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
