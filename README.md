32Blox
======

This is a breakout-style game for the 32blit, which was built primarily as a
learning exercise to comprehend the sparsely documented API :-)


Technical
---------

I have deliberately kept my interaction with the Engine at a minimum; largely
just pixel-pushing rather than trying to use all the baked in sprite stuff,
because I suspect the API will shift a lot under me and the less contact I 
have with it, the less likely I am to get shafted by it!

The downside is that my shonky code isn't necessarily as efficient as Engine
code, but I'll cross that bridge as and when performance becomes an issue. 
For now, there are lots and lots of CPU cycles to play with.

Although originally intended as a hi-res game, I've realised that is totally
over the top for a basic little ball bouncing game so I've switched down to
lores. I think it looks better, ironically.


Language
--------

Please note that this is C++ is name only; apart from calls into the Blit Engine
everything here should be pretty basic, old fashioned C because it's nicer that
way.

Long term, it would be nice to pull together some C bindings into the Engine,
so that we can have pure C projects.
