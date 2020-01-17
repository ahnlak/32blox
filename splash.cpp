/*
 * splash.cpp - part of 32Blox, a breakout game for the 32blit built to
 * explore the API.
 *
 * This file handles the rendering of the splash screen, shown on startup and
 * between games. Essentially this is just an animated page showing the high
 * score and asking for the start button to be pressed.
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

static rgba         m_text_colour;
static uint16_t     m_gradient_row;
static blit::timer  m_flicker_timer;


/* Module functions. */

/*
 * _splash_flicker_timer_update - callback for the font flicker and background
 */

void _splash_flicker_timer_update( blit::timer &p_timer )
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
 * splash_update - cycle the state of the splash for animation purposes. 
 *
 * Returns gamestate_t, the state to continue in. Should either be SPLASH, 
 * or GAME when the user is ready to play.
 */

gamestate_t splash_update( void )
{
  /* If it's not running, we need to set up the flicker timer. */
  if ( !m_flicker_timer.started )
  {
    m_flicker_timer.init( _splash_flicker_timer_update, 20, -1 );
    m_flicker_timer.start();
  }
 
  /* Check to see if the player has pressed the start button. */
  if ( blit::pressed( blit::button::A ) )
  {
    m_flicker_timer.stop();
    return STATE_GAME;
  }
  
  /* Default to the status quo, then. */
  return STATE_SPLASH;
}


/* 
 * splash_render - draw the current splash screen state onto the screen.
 */

void splash_render( void )
{
  uint16_t    l_row;
  bee_point_t l_point;
  bee_font_t  l_outline_font;
  
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
  
  /* Drop in the main logo nice and central(ish). */
  sprite_render( "logo", -1, 15 );
  
  /* Get hold of the outline font in our new renderer. */
  memcpy( &l_outline_font, bee_text_create_fixed_font( outline_font ), sizeof( bee_font_t ) );
  bee_text_set_font( &l_outline_font );
  
  /* Lastly, the text inviting the user to press the start button. */
  blit::fb.pen( m_text_colour );
  l_point.x = blit::fb.bounds.w / 2;
  l_point.y = 100;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "PRESS 'A' TO START" );
}


/* End of splash.cpp */
