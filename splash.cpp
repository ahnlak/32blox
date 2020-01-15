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

/* Local headers. */

#include "32blit.hpp"
#include "32blox.hpp"


/* Module variables. */

static rgba      m_text_colour;
static uint16_t  m_gradient_row;


/* Functions. */

using namespace blit;

/*
 * splash_update - cycle the state of the splash for animation purposes. 
 *
 * uint32_t - the gametick so that we animate at a known rate.
 * 
 * Returns gamestate_t, the state to continue in. Should either be SPLASH, 
 * or GAME when the user is ready to play.
 */

gamestate_t splash_update( uint32_t p_time )
{
  /* Update the flickering prompt text. */
  m_text_colour = rgba( p_time % 255, ( p_time % 512 ) / 2, 255 - (p_time % 255), 255 );
  m_gradient_row = ( p_time / 10 ) % 120;
  
  /* Check to see if the player has pressed the start button. */
  if ( blit::pressed( blit::button::A ) )
  {
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
  uint16_t l_row;
  
  /* Clear the screen to a nice shifting gradient. */
  for( l_row = 0; l_row < fb.bounds.h; l_row++ )
  {
    //fb.pen( rgba( 32 + ( l_row / 2 ), 32, 0, 255 ) );
    fb.pen( 
      rgba( 
        (int)( 64.0f + 48.0f * ( sin( M_PI * 2 / fb.bounds.h * l_row  ) ) ), 
        0, 
        (int)( 64.0f + 48.0f * ( cos( M_PI * 2 / fb.bounds.h * l_row ) ) ), 
        255 
      )
    );
    fb.line( point( 0, ( l_row + m_gradient_row ) % fb.bounds.h ), 
             point( fb.bounds.w, ( l_row + m_gradient_row ) % fb.bounds.h ) );
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
  
  /* Lastly, the text inviting the user to press the start button. */
  fb.pen( m_text_colour );
  fb.text( "PRESS 'A' TO START", &outline_font[0][0], point( 34, 100 ), true );
}


/* End of splash.cpp */
