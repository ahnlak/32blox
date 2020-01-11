#!/bin/bash
#
# Asset builder; runs all the game assets through an appropriate builder
# and adds them all into a single, includeable chunk of code.

SPRITEBUILDER=~/dev/32blit-beta/tools/sprite-builder

# Back up any existing asset file.
if [ -f assets.h ]
then
  mv -f assets.h assets.h.bak
fi

# Generate a sensible header into the new file
cat > assets.h << ENDOFHEADER
/*
 * assets.h - this is an auto-generated asset file. Please do not edit!
 */
ENDOFHEADER

# Sprites first, which will be any png lurking in the asset folder
asset_list=()
for sprite in `ls assets/*.png`
do
  # Work out the appropriate packed data name
  asset_name=`basename -s .png $sprite | sed "s/-/_/g"`
  
  # Generate the packed data
  $SPRITEBUILDER packed --arrayname m_sprite_$asset_name $sprite >> assets.h
  
  # And push this asset into the generated set
  asset_list+=($asset_name)
done

# And lastly, put out the sprite lookup table
echo "static const struct { const char *name; uint8_t *data; } m_sprites[] = {" >> assets.h
for asset in ${asset_list[@]}
do
  echo "{ \"$asset\", m_sprite_$asset }," >> assets.h
done
echo "{ NULL, NULL }};" >> assets.h
