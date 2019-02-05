# Fofonso's VGA Library for DOS - fflibvga
------------------------------------------------------------------

A while back I started a project of a clone of the Hunt the Wumpus game for DOS and VGA graphics (WumpusVGA, check it out!), this library appeared as that game was being made (I probably had more fun developing this library than the game itself, :P), in the beginning it was very specific code made for that game, but later, when I started making another game with VGA graphics (AtomsVGA) I noticed that it needed some changes to be more generic, this version of the library fulfills that and I am pretty happy with it.

I had a lot of fun developing this, and I intend to continue adding some stuff as I go.

This entire thing was written based on the WONDERFUL Andre LaMothe book The Black Art of 3d Game Programming, and basically everything Michael Abrash ever wrote, seriously, that guy is a genius.

However, this is a pretty basic library, and you won't be able to do much of serious game development with it (And why do that with VGA, you are crazier than me if you want to do serious dev stuff with this).

It uses a new file format I and a friend of mine (Francisco Viana) came up with called .FIS (Fofonso's Image Storage), based on the ZSoft's PCX format, included is a python file to convert .PPM files to .FIS (I really like PPM, but it's way to big for this), it has primitive RLE compression (And it is not the same one that PCX uses, bear that in mind), the file format is defined in the /FIS/FISFileDefinition file.

You might ask yourself, "Why!!? Why create your own file format if there are thousands of other ones ready for you to use, with better compression and all that!!? WHY?"

And that's a really valid question with a really simple answer, which is: "Because it was fun!", and it works for what I need it to work, isn't that the mark of a good file format?

The functions defined in this library allows you to:

Draw individual pixels
Draw Vertical and Horizontal lines
Change the color palette of the VGA
Load images and display them on screen at any position
Print characters, strings and integers on screen.
