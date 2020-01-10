/*
 * 32blox.hpp - part of 32Blox, a breakout game for the 32blit built to 
 * explore the API.
 *
 * This is the main header, which defines structures, constants and enums as
 * well as our function prototypes. Note that program data (sprites, audio et
 * al) can be found in their own header files.
 *
 * Please note that this is a first attempt at understanding a somewhat fluid
 * API on a shiny new bit of kit, so it probably is not full of 'best practice'.
 * It will hopefully serve as some sort of starting point, however.
 *
 * Coyright (C) 2020 Pete Favelle <pete@fsquared.co.uk>
 *
 * This software is provided under the MIT License. See LICENSE.txt for details.
 */

#ifndef   _32BLOX_HPP_
#define   _32BLOX_HPP_

/* Constants. */

/* Enums. */

typedef enum {
  STATE_SPLASH,
  STATE_GAME,
  STATE_DEATH,
  STATE_HISCORE
} gamestate_t;

/* Structures. */

/* Function prototypes. */

void init( void );
void update( uint32_t );
void render( uint32_t );

void splash_render( void );
void splash_update( uint32_t );

void sprite_render( const char *, uint16_t, uint16_t );


#endif /* _32BLOCK_HPP_ */

/* End of 32blox.hpp */
