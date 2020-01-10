/*
 * splash.hpp - part of 32Blox, a breakout game for the 32blit built to
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

rgba m_text_colour;

/* Functions. */

using namespace blit;

/*
 * splash_update - cycle the state of the splash for animation purposes. 
 *
 * uint32_t - the gametick so that we animate at a known rate.
 */

void splash_update( uint32_t p_time )
{
  m_text_colour = rgba( p_time % 255, ( p_time % 512 ) / 2, 255 - (p_time % 255), 255 );
}


/* 
 * splash_render - draw the current splash screen state onto the screen.
 */

void splash_render( void )
{
  /* Clear the screen back to something sensible. */
  fb.pen( rgba( 0, 0, 0, 255 ) );
  fb.clear();
  
  /* Frame everything with bricks; we're a brick game after all! */
  sprite_render( "brick_yellow", 0, 0 );
  sprite_render( "brick_yellow", 16, 0 );
  sprite_render( "brick_yellow", 0, 8 );

  sprite_render( "brick_yellow", 288, 0 );
  sprite_render( "brick_yellow", 304, 0 );
  sprite_render( "brick_yellow", 304, 8 );
  
  sprite_render( "brick_yellow", 0, 232 );
  sprite_render( "brick_yellow", 16, 232 );
  sprite_render( "brick_yellow", 0, 224 );

  sprite_render( "brick_yellow", 288, 232 );
  sprite_render( "brick_yellow", 304, 232 );
  sprite_render( "brick_yellow", 304, 224 );
  
  /* Drop in the main logo nice and central(ish). */
  sprite_render( "logo", 35, 25 );
  
  /* Lastly, the text inviting the user to press the start button. */
  fb.pen( m_text_colour );
  fb.text( "Press 'A' to start", &outline_font[0][0], point( 50, 170 ), true );
}


/* End of splash.cpp */
