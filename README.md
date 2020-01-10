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


Language
--------

Please note that this is C++ is name only; apart from calls into the Blit Engine
everything here should be pretty basic, old fashioned C because it's nicer that
way.

Long term, it would be nice to pull together some C bindings into the Engine,
so that we can have pure C projects.
