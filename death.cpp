/*
 * death.cpp - part of 32Blox, a breakout game for the 32blit built to
 * explore the API.
 *
 * If the player has reached a new high score, this is where we will ask them
 * to enter their initials so that their triumph is preserved for eternity.
 * Or at least until they turn their blit off.
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

#include "32bee.h"


/* Module variables. */

static blit::rgba   m_text_colour;
static uint16_t     m_gradient_row;
static uint32_t     m_score;
static char         m_player[3];
static uint8_t      m_cursor;
static uint32_t     m_waiting;
static blit::timer  m_wait_timer, m_flicker_timer;


/* Module functions. */

/*
 * _death_wait_timer_update - a callback simply to set the waiting flag to false
 */

void _death_wait_timer_update( blit::timer &p_timer )
{
    m_waiting = false;
    p_timer.stop();
}


/*
 * _death_flicker_timer_update - callback for the font flicker and background
 */

void _death_flicker_timer_update( blit::timer &p_timer )
{
  static uint16_t ls_loopcount = 0;
  
  /* Update the text colour used for flickeringness. */
  if ( ( ls_loopcount += 25 ) > 1200 ) 
  {
    ls_loopcount = 0;
  }
  m_text_colour = blit::rgba( 
                              ls_loopcount % 255, 
                              ( ls_loopcount % 512 ) / 2, 
                              255 - ( ls_loopcount % 255 ),
                              255
                            );
  m_gradient_row = ( ls_loopcount / 10 ) % 120;
}


/* Functions. */


/*
 * death_check_score - saves the new score, so that we know what it is later.
 * 
 * Returns a boolean flag, true if it qualifies for the hiscore table or false
 * if the player has fallen short.
 */

bool death_check_score( uint32_t p_score )
{
  /* Check this against the hiscore. */
  if ( hiscore_get_score( MAX_SCORES-1 ) < p_score )
  {
    m_wait_timer.init( _death_wait_timer_update, 250, 0 );
    m_score = p_score;
    m_player[0] = m_player[1] = m_player[2] = 'A';
    m_cursor = 0;
    return true;
  }
  
  /* Nothing doing, then. */
  return false;
}


/*
 * death_update - process the used inputting their name.
 *
 * Returns gamestate_t, the state to continue in. Should either be DEATH, 
 * or HISCORE when the user is ready to play.
 */

gamestate_t death_update( void )
{
  bool l_moving = false;
  
  /* If it's not running, we need to set up the flicker timer. */
  if ( !m_flicker_timer.started )
  {
    m_flicker_timer.init( _death_flicker_timer_update, 20, -1 );
    m_flicker_timer.start();
  }
  
  /* Move the cursor left. */
  if ( ( blit::pressed( blit::button::DPAD_LEFT ) ) || ( blit::joystick.x < -0.1f ) )
  {
    /* Remember that we're moving somewhere. */
    l_moving = true;
    
    /* But only act on it if we're not waiting for a timeout. */
    if ( !m_waiting )
    {
      /* Keep the cursor in bounds, obviously. */
      if ( m_cursor > 0 )
      {
        m_waiting = true;
        m_wait_timer.start();
        m_cursor--;
      }
    }
  }
  
  /* Or right, come to that! */
  if ( ( blit::pressed( blit::button::DPAD_RIGHT ) ) || ( blit::joystick.x > 0.1f ) )
  {
    /* Remember that we're moving somewhere. */
    l_moving = true;
    
    /* But only act on it if we're not waiting for a timeout. */
    if ( !m_waiting )
    {
      /* Keep the cursor in bounds, obviously. */
      if ( m_cursor < 2 )
      {
        m_waiting = true;
        m_wait_timer.start();
        m_cursor++;
      }
    }
  }
  
  /* Up means moving up through the alphabet. */
  if ( ( blit::pressed( blit::button::DPAD_UP ) ) || ( blit::joystick.y < -0.1f ) )
  {
    /* Remember that we're moving somewhere. */
    l_moving = true;
    
    /* But only act on it if we're not waiting for a timeout. */
    if ( !m_waiting )
    {
      /* Increment the appropriate letter, within bounds. */
      if ( m_player[m_cursor] < 'Z' )
      {
        m_waiting = true;
        m_wait_timer.start();
        m_player[m_cursor]++;
      }
    }
  }
  
  /* And down means, well, moving down through the alphabet. */
  if ( ( blit::pressed( blit::button::DPAD_DOWN ) ) || ( blit::joystick.y > 0.1f ) )
  {
    /* Remember that we're moving somewhere. */
    l_moving = true;
    
    /* But only act on it if we're not waiting for a timeout. */
    if ( !m_waiting )
    {
      /* Increment the appropriate letter, within bounds. */
      if ( m_player[m_cursor] > ' ' )
      {
        m_waiting = true;
        m_wait_timer.start();
        m_player[m_cursor]--;
      }
    }
  }
   
  /* If there's no user movement, reset the input timer. */
  if ( !l_moving )
  {
    m_waiting = false;
  }
  
  /* Check to see if the player has pressed the save button. */
  if ( blit::pressed( blit::button::B ) )
  {
    /* Save this, and take the user into the hi score table. */
    hiscore_save_score( m_score, m_player );
    m_flicker_timer.stop();
    return STATE_HISCORE;
  }
  
  /* Default to the status quo, then. */
  return STATE_DEATH;
}


/* 
 * death_render - draw the player as she enters her name.
 */

void death_render( void )
{
  uint16_t    l_row;
  bee_point_t l_point;
  bee_font_t  l_outline_font, l_minimal_font;
  
  /* Clear the screen to a nice shifting gradient. */
  for( l_row = 0; l_row < blit::fb.bounds.h; l_row++ )
  {
    blit::fb.pen( 
      blit::rgba( 
        (int)( 64.0f + 48.0f * ( sin( M_PI * 2 / blit::fb.bounds.h * l_row  ) ) ), 
        0, 
        (int)( 64.0f + 48.0f * ( cos( M_PI * 2 / blit::fb.bounds.h * l_row ) ) ), 
        255 
      )
    );
    blit::fb.line( point( 0, ( l_row + m_gradient_row ) % blit::fb.bounds.h ), 
             point( blit::fb.bounds.w, ( l_row + m_gradient_row ) % blit::fb.bounds.h ) );
  }
  
  /* Frame everything with bricks; we're a brick game after all! */
  sprite_render( "brick_yellow", 0, 0 );
  sprite_render( "brick_yellow", 16, 0 );
  sprite_render( "brick_yellow", 0, 8 );

  sprite_render( "brick_yellow", 128, 0 );
  sprite_render( "brick_yellow", 144, 0 );
  sprite_render( "brick_yellow", 144, 8 );
  
  sprite_render( "brick_yellow", 0, 112 );
  sprite_render( "brick_yellow", 16, 112 );
  sprite_render( "brick_yellow", 0, 104 );

  sprite_render( "brick_yellow", 128, 112 );
  sprite_render( "brick_yellow", 144, 112 );
  sprite_render( "brick_yellow", 144, 104 );
  
  /* Get hold of the fonts in our new renderer. */
  memcpy( &l_outline_font, bee_text_create_fixed_font( outline_font ), sizeof( bee_font_t ) );
  memcpy( &l_minimal_font, bee_text_create_fixed_font( minimal_font ), sizeof( bee_font_t ) );
  
  /* Put the headings in somewhere sensible. */
  blit::fb.pen( rgba( 255, 255, 255, 255 ) );
  bee_text_set_font( &l_outline_font );
  l_point.x = blit::fb.bounds.w / 2;
  l_point.y = 1;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "NEW HIGH SCORE!" );
  l_point.y = 20;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "%05d", m_score );
  l_point.y = 64;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "LEFT/RIGHT TO SELECT" );
  l_point.y = 80;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "UP/DOWN TO CHANGE" );
  
  /* Now show the initials, in a different font to be distinctive. */
  bee_text_set_font( &l_minimal_font );
  l_point.y = 40;
  l_point.x = ( blit::fb.bounds.w / 2 ) - 10;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "%c", m_player[0] );
  l_point.x = ( blit::fb.bounds.w / 2 );
  bee_text( &l_point, BEE_ALIGN_CENTRE, "%c", m_player[1] );
  l_point.x = ( blit::fb.bounds.w / 2 ) + 10;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "%c", m_player[2] );
  
  /* Draw a cursor around the currently selected letter. */
  blit::fb.pen( m_text_colour );
  blit::fb.line( point( ( blit::fb.bounds.w / 2 ) - 14 + ( 10 * m_cursor ), 38 ), 
                 point( ( blit::fb.bounds.w / 2 ) - 6 + ( 10 * m_cursor ), 38 ) );
  blit::fb.line( point( ( blit::fb.bounds.w / 2 ) - 6 + ( 10 * m_cursor ), 38 ), 
                 point( ( blit::fb.bounds.w / 2 ) - 6 + ( 10 * m_cursor ), 48 ) );
  blit::fb.line( point( ( blit::fb.bounds.w / 2 ) - 6 + ( 10 * m_cursor ), 48 ), 
                 point( ( blit::fb.bounds.w / 2 ) - 14 + ( 10 * m_cursor ), 48 ) );
  blit::fb.line( point( ( blit::fb.bounds.w / 2 ) - 14 + ( 10 * m_cursor ), 48 ), 
                 point( ( blit::fb.bounds.w / 2 ) - 14 + ( 10 * m_cursor ), 38 ) );
  
  /* Lastly, the text inviting the user to press the start button. */
  blit::fb.pen( m_text_colour );
  bee_text_set_font( &l_outline_font );
  l_point.x = blit::fb.bounds.w / 2;
  l_point.y = 100;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "PRESS 'B' TO SAVE" );
}


/* End of death.cpp */
