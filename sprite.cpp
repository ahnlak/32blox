/*
 * sprite.hpp - part of 32Blox, a breakout game for the 32blit built to
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


/* Functions. */

using namespace blit;

/*
 * sprite_render - write the names sprite to the framebuffer, with the top
 *                 left corner at the co-ordinates given.
 *
 * const char * - the name of the sprite
 * uint16_t     - row to start drawing from (x)
 * uint16_t     - column to start drawing from (y)
 */

void sprite_render( const char *p_sprite, uint16_t p_column, uint16_t p_row )
{
  int           l_index;
  rgba          l_palette[256];
  packed_image *l_sprite;
  uint8_t      *l_spritedata;
  uint8_t       l_bitdepth, l_bit, l_pixel;
  uint16_t      l_row, l_column;
  
  /* Step one, sanity check that the co-ords are within the framebuffer. */
/*__RETURN__*/

  /* Step two, find the sprite in the lookup table. */
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

  /* Step three, extract some basic metrics about the chosen sprite. */
  l_sprite = (packed_image *)m_sprites[l_index].data;
  l_spritedata = m_sprites[l_index].data + sizeof(packed_image);
  l_bitdepth = ceil( log(l_sprite->palette_entry_count) / log(2) );
  
  /* Step four, extract the palette into a more useful form. */
  for( l_index = 0; l_index < l_sprite->palette_entry_count; l_index++ )
  {
    l_palette[l_index] = rgba( l_spritedata[ 0 ], l_spritedata[ 1 ], l_spritedata[ 2 ] );
    l_spritedata += 4;
  }
  
  /* And lastly, step five - extract the packed data, and spit it out. */
  l_row = l_column = l_bit = l_pixel = 0;
  for ( ; l_spritedata < (uint8_t *)l_sprite + l_sprite->byte_count; l_spritedata++ )
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

/* End of sprite.cpp */
