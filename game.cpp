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

#include <string.h>


/* Local headers. */

#include "32blit.hpp"
#include "32blox.hpp"


/* Module variables. */

static const struct { const char *name; uint8_t width; } m_bats[] =
{
  { "bat_normal", 16 },
  { NULL, 0 }
};

static rgba      m_text_colour;
static uint32_t  m_hiscore;
static uint32_t  m_score;
static uint8_t   m_lives;
static uint8_t   m_level;
static float     m_player;
static float     m_speed;
static uint8_t   m_battype;
static bool      m_flash;
static int8_t    m_balls[MAX_BALLS];


/* Functions. */

using namespace blit;

/*
 * game_init - called at the start of a new game, to blank scores and
 *             configure suitable levels.
 */

void game_init( void )
{
  /* Set the player stats to an opening value. */
  m_score = 0;
  m_lives = 3;
  m_level = 1;
  m_player = fb.bounds.w / 2;
  m_speed = 0.9;
  m_battype = 0;
  m_flash = false;
  
  /* Initialise that level. */
  level_init( m_level );
  
  /* Fetch the current high score from long term storage. */
  /* __RETURN__ */
  m_hiscore = 9999;
  
  /* Spawn a ball on the player's bat. */
  memset( m_balls, -1, MAX_BALLS );
  m_balls[0] = ball_create( m_player );
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
  int8_t  l_score;
  uint8_t l_index;
  
  /* Update the flickering prompt text. */
  m_text_colour = rgba( p_time % 255, ( p_time % 512 ) / 2, 255 - (p_time % 255), 255 );
  
  /* See if the player is moving left. */
  if ( ( blit::pressed( blit::button::DPAD_LEFT ) ) || ( blit::joystick.x < -0.1f ) )
  {
    /* Don't let them go outside of bounds. */
    if ( ( m_player -= m_speed ) < ( m_bats[m_battype].width / 2 ) )
    {
      m_player += m_speed;
    }
  }
  
  /* Or right, come to that! */
  if ( ( blit::pressed( blit::button::DPAD_RIGHT ) ) || ( blit::joystick.x > 0.1f ) )
  {
    /* Don't let them go outside of bounds. */
    if ( ( m_player += m_speed ) > ( fb.bounds.w - ( m_bats[m_battype].width / 2 ) ) )
    {
      m_player -= m_speed;
    }
  }
  
  /* If they press the Y button, launch any balls we're currently holding. */
  if ( blit::pressed( blit::button::Y ) )
  {
    for ( l_index = 0; l_index < MAX_BALLS; l_index++ )
    {
      if ( m_balls[l_index] >= 0 )
      {
        ball_launch( m_balls[l_index] );
      }
    }
  }
  
  /* Update the location of the ball(s). */
  for ( l_index = 0; l_index < MAX_BALLS; l_index++ )
  {
    if ( m_balls[l_index] >= 0 )
    {
      /* The ball update will tell us if there's a score to be earned. */
      l_score = ball_update( m_balls[l_index], m_player, m_bats[m_battype].width );
      if ( l_score > 0 )
      {
        m_score += l_score;
      }
      
      /* A negative score means it's dipped below the board, and despawned. */
      if ( l_score < 0 )
      {
        m_flash = true;
        m_balls[l_index] = -1;
      }
    }
  }
  
  /* If no balls are left in play, a life it lost. If there are more lives, */
  /* then a fresh ball is spawned. If not, it's game over (man).            */
  for ( l_index = 0; l_index < MAX_BALLS; l_index++ )
  {
    if ( m_balls[l_index] >= 0 )
    {
      break;
    }
  }
  if ( l_index == MAX_BALLS )
  {
    if ( --m_lives <=0 )
    {
      return STATE_DEATH;
    }
    m_balls[0] = ball_create( m_player );
  }
  
  /* Default to the status quo, then. */
  return STATE_GAME;
}


/* 
 * game_render - draw the current splash screen state onto the screen.
 */

void game_render( void )
{
  uint8_t   l_index, l_brick;
  char      l_buffer[32];
  uint8_t  *l_line;
  
  /* Clear the screen back to something sensible. */
  if ( m_flash )
  {
    fb.pen( rgba( 240, 0, 0, 255 ) );
    m_flash = false;
  }
  else
  {
    fb.pen( rgba( 0, 0, 0, 255 ) );
  }
  fb.clear();
  
  /* Render the top status line. */
#pragma GCC diagnostic ignored "-Wformat"
  fb.pen( rgba( 255, 255, 255, 255 ) );
  sprintf( l_buffer, "HI: %05lu", m_hiscore );
  fb.text( l_buffer, &minimal_font[0][0], point( 1, 1 ), true );
  sprintf( l_buffer, "SC: %05lu", m_score );
  fb.text( l_buffer, &minimal_font[0][0], point( 115, 1 ), true );
#pragma GCC diagnostic pop
  
  /* Lives are tricky, we can run out of space... */
  if ( m_lives < 4 )
  {
    for ( l_index = 0; l_index < m_lives; l_index++ )
    {
      sprite_render( "bat_normal", 72 - ( ( m_lives - 1 ) * 10 ) + ( l_index * 20 ), 3 );
    }
  }
  
  /* Underline that, to form a hard border to bounce off at the top. */
  fb.pen( rgba( 255, 255, 255, 255 ) );
  fb.line( point( 0, 9 ), point( fb.bounds.w, 9 ) );
  
  /* Now we draw up the surviving bricks in the level. */
  for ( l_index = 0; l_index < 10; l_index++ )
  {
    /* Fetch the current state, a line at a time. */
    l_line = level_get_line( l_index );
    
    /* And work through a brick at a time. */
    for ( l_brick = 0; l_brick < 10; l_brick++ )
    {
      /* Only try and draw bricks which are actually there... */
      if ( l_line[ l_brick ] > 0 )
      {
        sprite_render( level_get_bricktype( l_line[ l_brick ] ),
                       l_brick * 16, 10 + ( l_index * 8 ) );
      }
    }
  }
  
  /* Add in the current bat. */
  sprite_render( "bat_normal", m_player, fb.bounds.h - 8, ALIGN_TOPCENTRE );
  
  /* And the ball(s), obviously. */
  for ( l_index = 0; l_index < MAX_BALLS; l_index++ )
  {
    /* Only deal with balls we know about. */
    if ( m_balls[l_index] >= 0 )
    {
      ball_render( m_balls[l_index] );
      if ( ball_stuck( m_balls[l_index] ) )
      {
        fb.pen( m_text_colour );
        fb.text( "PRESS 'Y' TO LAUNCH", &outline_font[0][0], point( 32, 100 ), true );
      }
    }
  }
  
  /* Any falling debris, specials or effects. */
}


/* End of game.cpp */
