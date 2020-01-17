/*
 * hiscore.cpp - part of 32Blox, a breakout game for the 32blit built to
 * explore the API.
 *
 * This file rather naughtily does two things; it renders the high score 
 * screen as well as dealing with long-term storage of the high score table.
 * 
 * Yes, in a proper, object-y world this would be two different objects, or
 * something. This is a litle console game, so cut me some slack.
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

static struct { uint32_t score; char name[3]; } m_scores[MAX_SCORES];
static rgba      m_text_colour;
static uint16_t  m_gradient_row;


/* Functions. */

using namespace blit;


/*
 * hiscore_init - loads any saved high score table from long term storage
 */

void hiscore_init( void )
{
  uint8_t l_index;
  
  /* Start by making sure the local storage is clear. */
  for( l_index = 0; l_index < MAX_SCORES; l_index++ )
  {
    m_scores[l_index].score = 0;
    m_scores[l_index].name[0] = m_scores[l_index].name[1] = m_scores[l_index].name[2] = ' '; 
  }
}


/*
 * hiscore_get_score - returns the high score for a given position.
 * 
 * uint8_t - the position in the high score table to return a value for
 * 
 * Returns the high score in question.
 */

uint32_t hiscore_get_score( uint8_t p_position )
{
  /* Obviously we can't return scores that don't exist. */
  if ( p_position >= MAX_SCORES )
  {
    return 0;
  }
  
  /* Then simply return it from the internal array. */
  return m_scores[p_position].score;
}


/*
 * hiscore_save_score - saves the high score into the appropriate place in 
 *                      the table, and flush to long term storage
 * 
 * uint32_t - the score that was scored
 * const char * - the scorer of said score!
 */

void hiscore_save_score( uint32_t p_score, const char *p_name )
{
  uint8_t l_index, l_index2;
  
  /* Find the right slot in the high score table to save this to. */
  for ( l_index = 0; l_index < MAX_SCORES; l_index++ )
  {
    if ( p_score > m_scores[l_index].score )
    {
      break;
    }
  }
  
  /* If we didn't find a slot, we cannae save it. */
  if ( l_index >= MAX_SCORES )
  {
    return;
  }
  
  /* Shuffle everything below it down a bit. */
  for ( l_index2 = MAX_SCORES - 1; l_index2 > l_index; l_index2-- )
  {
    m_scores[l_index2].score = m_scores[l_index2-1].score;
    m_scores[l_index2].name[0] = m_scores[l_index2-1].name[0];
    m_scores[l_index2].name[1] = m_scores[l_index2-1].name[1];
    m_scores[l_index2].name[2] = m_scores[l_index2-1].name[2];
  }
  
  /* And lastly, save the new one. */
  m_scores[l_index].score = p_score;
  m_scores[l_index].name[0] = p_name[0];
  m_scores[l_index].name[1] = p_name[1];
  m_scores[l_index].name[2] = p_name[2];
}


/*
 * hiscore_update - updates the high score display in the normal gameloop.
 * 
 * uint32_t - the time tick of the update, in milliseconds.
 * 
 * Returns the new gamestate.
 */

gamestate_t hiscore_update( uint32_t p_time )
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
  return STATE_HISCORE;
}


/*
 * hiscore_render - renders the high score display in the normal gameloop.
 */

void hiscore_render( void )
{
  uint8_t       l_index;
  uint16_t      l_row;
  char          l_buffer[32];
  bee_point_t   l_point;
  
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
  
  /* Title the screen, although it's probably pretty obvious... */
  fb.pen( rgba( 255, 255, 255, 255 ) );
  l_point.x = fb.bounds.w / 2;
  l_point.y = 1;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "HIGH SCORES" );
  
  /* Now just render the list, with a nice colour gradient. */
  for ( l_index = 0; l_index < MAX_SCORES; l_index++ )
  {
    /* If we hit the bottom of the table, we can jump out now. */
    if ( m_scores[l_index].score == 0 )
    {
      break;
    }
    
    /* So, set the pen a little darker. */
    fb.pen( rgba( 255 - ( 15 * l_index ), 255 - ( 25 * l_index ), 255 - ( 15 * l_index ), 255 ) );
    l_point.y = 14 + ( 8 * l_index );
    bee_text( &l_point, BEE_ALIGN_CENTRE, "%05d %c %c %c",  m_scores[l_index].score,
             m_scores[l_index].name[0], m_scores[l_index].name[1], m_scores[l_index].name[2] );
  }
  
  /* Lastly, the text inviting the user to press the start button. */
  fb.pen( m_text_colour );
  l_point.y = 100;
  bee_text( &l_point, BEE_ALIGN_CENTRE, "PRESS 'A' TO START" );
}


/* End of hiscore.cpp */
