/*
 * sprite.cpp - part of 32Blox, a breakout game for the 32blit built to
 * explore the API.
 *
 * This file contains all the sprite data, in a hopefully easy-to-handle way.
 * The raw images are in packed_data structures generated by sprite_builder,
 * but as separate entities rather than a monolithic sprite sheet as there are
 * so few of them, and they're all different sizes.
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

#include <math.h>
#include <stdio.h>
#include <string.h>


/* Local headers. */

#include "32blit.hpp"
#include "32blox.hpp"


/* Module variables. */

/* Raw sprite data. */

#include "assets.h"


/* Module functions. */

/*
 * m_align_x - applies the alignment adjustment in the X axis
 * 
 * int16_t, the initial x position
 * const packed_image *, the sprite data
 * spritealign_t, the alignment factor
 * 
 * Returns the adjusted x value
 */

static int16_t m_align_x( int16_t p_x, const packed_image *p_sprite, spritealign_t p_align )
{
  /* Only have options here where adjustments are needed. */
  switch( p_align )
  {
    case ALIGN_TOPCENTRE:
      return( p_x - ( p_sprite->width / 2 ) );
    case ALIGN_TOPRIGHT:
      return( p_x - p_sprite->width );
    case ALIGN_MIDCENTRE:
      return( p_x - ( p_sprite->width / 2 ) );
    case ALIGN_MIDRIGHT:
      return( p_x - p_sprite->width );
    case ALIGN_BOTCENTRE:
      return( p_x - ( p_sprite->width / 2 ) );
    case ALIGN_BOTRIGHT:
      return( p_x - p_sprite->width );
  }
  
  /* Default to applying no adjustments. */
  return p_x;
}


/*
 * m_align_y - applies the alignment adjustment in the Y axis
 * 
 * int16_t, the initial y position
 * const packed_image *, the sprite data
 * spritealign_t, the alignment factor
 * 
 * Returns the adjusted y value
 */

static int16_t m_align_y( int16_t p_y, const packed_image *p_sprite, spritealign_t p_align )
{
  /* Only have options here where adjustments are needed. */
  switch( p_align )
  {
    case ALIGN_MIDLEFT:
      return( p_y - ( p_sprite->height / 2 ) );
    case ALIGN_MIDCENTRE:
      return( p_y - ( p_sprite->height / 2 ) );
    case ALIGN_MIDRIGHT:
      return( p_y - ( p_sprite->height / 2 ) );
    case ALIGN_BOTLEFT:
      return( p_y - p_sprite->height );
    case ALIGN_BOTCENTRE:
      return( p_y - p_sprite->height );
    case ALIGN_BOTRIGHT:
      return( p_y - p_sprite->height );
  }
  
  /* Default to applying no adjustments. */
  return p_y;
}


/* Functions. */

using namespace blit;

/*
 * sprite_render - write the names sprite to the framebuffer, with the top
 *                 left corner at the co-ordinates given.
 *
 * const char * - the name of the sprite
 * uint16_t     - column to start drawing from (x), or -1 to centre.
 * uint16_t     - row to start drawing from (y), or -1 to centre.
 * spritealign_t- defines the origin point of the render.
 */

void sprite_render( const char *p_sprite, int16_t p_column, int16_t p_row, spritealign_t p_align )
{
  uint8_t             l_index;
  rgba                l_palette[256];
  const packed_image *l_sprite;
  const uint8_t      *l_spritedata;
  uint8_t             l_bitdepth, l_bit, l_pixel;
  uint16_t            l_row, l_column;
  
  /* Step one, find the sprite in the lookup table. */
  for( l_index = 0; m_sprites[l_index].name != NULL; l_index++ )
  {
    if ( strcmp( p_sprite, m_sprites[l_index].name ) == 0 )
    {
      break;
    }
  }

  /* If we didn't find anything, we can't really proceed any further. */
  if ( m_sprites[l_index].data == NULL )
  {
    return;
  }

  /* Step two, extract some basic metrics about the chosen sprite. */
  l_sprite = (const packed_image *)m_sprites[l_index].data;
  l_spritedata = m_sprites[l_index].data + sizeof(packed_image);
  l_bitdepth = ceil( log(l_sprite->palette_entry_count) / log(2) );
  
  /* Step three, if we're centering we finally have the data to do so! */
  if ( p_row == -1 )
  {
    p_row = ( fb.bounds.h - l_sprite->height ) / 2;
  }
  if ( p_column == -1 )
  {
    p_column = ( fb.bounds.w - l_sprite->width ) / 2;
  }
  
  /* Step 3.5, apply any alignment requirements, as best we can. */
  p_column = m_align_x( p_column, l_sprite, p_align );
  p_row = m_align_y( p_row, l_sprite, p_align );

  if ( p_row < -1 ) p_row = 0;
  if ( p_row > fb.bounds.h ) p_row = fb.bounds.h;
  if ( p_column < -1 ) p_column = 0;
  if ( p_column > fb.bounds.w ) p_column = fb.bounds.w;
  
  /* Step four, extract the palette into a more useful form. */
  for( l_index = 0; l_index < l_sprite->palette_entry_count; l_index++ )
  {
    l_palette[l_index] = rgba( l_spritedata[ 0 ], l_spritedata[ 1 ], 
                               l_spritedata[ 2 ], l_spritedata[ 3 ] );
    l_spritedata += 4;
  }
  
  /* And lastly, step five - extract the packed data, and spit it out. */
  l_row = l_column = l_bit = l_pixel = 0;
  for ( ; l_spritedata < (const uint8_t *)l_sprite + l_sprite->byte_count; l_spritedata++ )
  {
    /* Extract each bit from each byte, up to the required bitdepth. */
    for ( l_index = 0; l_index < 8; l_index++ )
    {
      /* Shift the current pixel value another bit up. */
      l_pixel <<= 1;
      
      /* And add in the next bit. */
      l_pixel |= ( ( 0b10000000 >> l_index ) & *l_spritedata ) ? 1 : 0;

      /* And if we've fetched enough bits, spit out that pixel value. */
      if ( ++l_bit == l_bitdepth )
      {
        /* Set the pixel at the current point. */
        fb.pen( l_palette[l_pixel] );
        fb.pixel( point( p_column + l_column, p_row + l_row ) );

        /* And move along to the next column. */
        if ( ++l_column >= l_sprite->width )
        {
          l_column = 0;
          l_row++;
        }
        l_bit = l_pixel = 0;
      }
    }
  }
}


/*
 * sprite_size - returns the size of the named sprite.
 * 
 * const char * - the name of the sprite in question
 * 
 * Returns the size of the sprite
 */

size sprite_size( const char *p_sprite )
{
  uint8_t             l_index;
  const packed_image *l_sprite;
  
  /* Step one, find the sprite in the lookup table. */
  for( l_index = 0; m_sprites[l_index].name != NULL; l_index++ )
  {
    if ( strcmp( p_sprite, m_sprites[l_index].name ) == 0 )
    {
      break;
    }
  }

  /* If we didn't find anything, we can't really proceed any further. */
  if ( m_sprites[l_index].data == NULL )
  {
    return size( 0, 0 );
  }

  /* Step two, extract some basic metrics about the chosen sprite. */
  l_sprite = (const packed_image *)m_sprites[l_index].data;  
  
  return size( l_sprite->width, l_sprite->height );
}


/*
 * sprite_collide - calculate if two sprites will collide on a pixel basis.
 * 
 * const char * - the name of the first sprite
 * uint16_t     - column to start drawing from (x), or -1 to centre.
 * uint16_t     - row to start drawing from (y), or -1 to centre.
 * spritealign_t- defines the origin point of the render.
 * 
 * const char * - the name of the second sprite
 * uint16_t     - column to start drawing from (x), or -1 to centre.
 * uint16_t     - row to start drawing from (y), or -1 to centre.
 * spritealign_t- defines the origin point of the render.
 */

bool sprite_collide( const char *pa_sprite, int16_t pa_column, int16_t pa_row, spritealign_t pa_align,
                     const char *pb_sprite, int16_t pb_column, int16_t pb_row, spritealign_t pb_align )
{
  uint8_t             l_index;
  rgba                la_palette[256], lb_palette[256];
  const packed_image *la_sprite = NULL, *lb_sprite = NULL;
  const uint8_t      *la_spritedata, *lb_spritedata;
  uint8_t             la_bitdepth, lb_bitdepth;
  rect                la_bounds, lb_bounds;
  uint8_t             l_bit, l_pixel;
  uint16_t            l_row, l_column;
  
  /* First off, we will need to have hold of both sprites */
  for( l_index = 0; m_sprites[l_index].name != NULL; l_index++ )
  {
    /* Match the first sprite, unless we already found it. */
    if ( ( la_sprite == NULL ) && ( strcmp( pa_sprite, m_sprites[l_index].name ) == 0 ) )
    {
      /* Extract the details, but carry on to match the second. */
      la_sprite = (const packed_image *)m_sprites[l_index].data;
      la_spritedata = m_sprites[l_index].data + sizeof(packed_image);
      la_bitdepth = ceil( log(la_sprite->palette_entry_count) / log(2) );
    }
    
    /* Do the same for the second sprite. */
    if ( ( lb_sprite == NULL ) && ( strcmp( pb_sprite, m_sprites[l_index].name ) == 0 ) )
    {
      /* Extract the details, but carry on to match the second. */
      lb_sprite = (const packed_image *)m_sprites[l_index].data;
      lb_spritedata = m_sprites[l_index].data + sizeof(packed_image);
      lb_bitdepth = ceil( log(lb_sprite->palette_entry_count) / log(2) );
    }
    
    /* Now if we've found both sprites we can jump out early. */
    if ( ( la_sprite != NULL ) && ( lb_sprite != NULL ) )
    {
      break;
    }
  }

  /* If we didn't find anything, we can't really proceed any further. */
  if ( ( la_sprite == NULL ) || ( lb_sprite == NULL ) )
  {
    return false;
  }

  /* Next up, work out the bounds of both sprites, taking into account alignment. */
  la_bounds.w = la_sprite->width;
  la_bounds.h = la_sprite->height;
  la_bounds.x = m_align_x( pa_column, la_sprite, pa_align );
  la_bounds.y = m_align_y( pa_row, la_sprite, pa_align );
  
  lb_bounds.w = lb_sprite->width;
  lb_bounds.h = lb_sprite->height;
  lb_bounds.x = m_align_x( pb_column, lb_sprite, pb_align );
  lb_bounds.y = m_align_y( pb_row, lb_sprite, pb_align );
  
  /* Now, if these rectangles don't intersect there can't be a collision. */
  if ( !la_bounds.intersects( lb_bounds ) )
  {
    return false;
  }
  
  /* For now then, let's assume a collision :) */
  return true;
  
  
  
  /* If we've got to the end, we found no further collisions. */
  return false;
}



/* End of sprite.cpp */
