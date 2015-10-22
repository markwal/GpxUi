---
layout: doc
title: Introduction
permalink: intro.html
---
## Files, Formats and Protocols
I assume the reason you are here is because you have (or at least the use of) a
MakerBot printer or a clone (like a FlashForge Creator) or a clone of a clone
(CTC). These printers run either MakerBot firmware or Sailfish firmware or
either.  Those firmwares get their instructions either from a file on an SD card
or over serial communication with a computer using a USB cable. The instructions
are in a format called x3g.

Often the things we want to print are stored in a format called STL which we
transform into gcode (sometimes called a toolpath) with a software tool called a
slicer and then for our printers, there is one more step to turn gcode into a
binary instruction language called x3g.

{% include image.html url="docs/model_to_plastic.png" caption="From model to plastic (example is <A href='http://www.thingiverse.com/thing:11705'>Squirrel</A> by MBCook)" %}

### STL - STereoLithography

It's capable of storing more, but generally for our purposes, an STL is a list
of triangular facets described by three vertices in x, y, z and a facet normal.
You can create an STL file with design software like Blender or Autodesk Fusion.

An STL file doesn't describe the units, but in most 3d printing software, the
units are assumed to be millimeters.

### G-code

The software that translates the model in STL format to a "toolpath" is called a
slicer. So named because it slices the 3D model into horizontal slices or layers
of a specified thickness. The slicer takes these horizontal slices outputs a
path for the nozzle to take. "Move to this (x, y, z) location while extruding
this much plastic".  The gcode can also have other commands like "turn on the
fan", "display a message on the LCD", etc.

It's called G-code because generally the commands that command motion are start
with a G and have a number that says what kind of command. For example, G0 means
"move to the specified coordinate while extruding the specified amount of
plastic".

### X3G

X3g is basically an all binary form of a list of commands for a 3d printer. It's
similar to G-code and there is often a one-to-one mapping from a particular
G-code command and an X3G command. The main difference is that X3G is all binary
and doesn't require very much parsing in the printer firmware.

X3G is an eXtended version of S3G which was invented early on in the RepRap
project by Zach Hoeken to go with his Sanguino printer controller board. It was
so named (I think) because Sanguino was the 3rd generation electronics platform
for RepRap.

Many RepRap projects have moved on to interpreting the g-code directly in the
firmware. MakerBot stuck with the binary form though because they were already
asking a lot of an 8-bit microcontroller and wasting cycles on text parsing
didn't seem like a valuable way to spend mcu cycles.

X3G is created from G-code using gcode processing software. Some software has
this translation embedded in it like ReplicatorG and MakerBot Desktop. GPX is a
standalone G-code processor so you can easily use it with many different
slicers.

## GPX

GPX was originally created by Dr. Henry Thomas (aka Wingcommander) in April
2013.

It is now maintained by Mark Walker and Dan Newman.
