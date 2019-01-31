# fflibvga
Fofonso's VGA Library for DOS.

A little project of mine (WumpusVGA) required some VGA magifuckery, so I wrote this basing myself on the WONDERFULLY AMAZING Andre LaMothe book The Black Art of 3d Game Programming, check it out!.

This library is very basic, but it has primitive double buffering, and IT actually can function as the basis for some simple games, 
if you need complex stuff like splitscreens or such, this wont work very well for you.

I made the decision to use .PPM files as the stanard file format, and that was a big mistake, in the future I intend to change that to a more manageable file (like a pure binary file, so I can get the filesize down a bit, right now its INSANE).
NOTE: The PPM is the input file to a python script that separates the pallette and a .pgm file containing the indices of the colors, so you must run that to obtain an actual readable file by the library, that loads it into memory by using the load_pgm() function and loads the pallette by using the load_pallette() function.
