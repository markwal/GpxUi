---
layout: doc
title: Command Line
permalink: commandline.html
---
# gpx [-CFdgilpqrstvw] [-b BAUDRATE] [-b BAUDRATE] [-c CONFIG] [-e EEPROM] [-f DIAMETER] [-m MACHINE] [-N h|t|ht] [-n SCALE] [-x X] [-y Y] [-z Z] IN [OUT]

Options:

	-C	Create temporary file with a copy of the machine configuration
	-F	write X3G on-wire framing data to output file
	-N	Disable writing of the X3G header (start build notice),
	  	tail (end build notice), or both
	-d	simulated ditto printing
	-g	Makerbot/ReplicatorG GCODE flavor
	-i	enable stdin and stdout support for command line pipes
	-l	log to file
	-p	override build percentage
	-q	quiet mode
	-r	Reprap GCODE flavor
	-s	enable USB serial I/O and send x3G output to 3D printer
	-t	truncate filename (DOS 8.3 format)
	-v	verose mode
	-w	rewrite 5d extrusion values

BAUDRATE: the baudrate for serial I/O (default is 115200)

CONFIG: the filename of a custom machine definition (ini file)

EEPROM: the filename of an eeprom settings definition (ini file)

DIAMETER: the actual filament diameter in the printer

MACHINE: the predefined machine type

	some machine definitions have been updated with corrected steps per mm
	the original can be selected by prefixing o to the machine id
	(or1, or1d, or2, or2h, orx, ot7, ot7d)
	c3  = Cupcake Gen3 XYZ, Mk5/6 + Gen4 Extruder
	c4  = Cupcake Gen4 XYZ, Mk5/6 + Gen4 Extruder
	cp4 = Cupcake Pololu XYZ, Mk5/6 + Gen4 Extruder
	cpp = Cupcake Pololu XYZ, Mk5/6 + Pololu Extruder
	cxy = Core-XY with HBP - single extruder
	cxysz = Core-XY with HBP - single extruder, slow Z
	cr1 = Clone R1 Single with HBP
	cr1d = Clone R1 Dual with HBP
	r1  = Replicator 1 - single extruder
	r1d = Replicator 1 - dual extruder
	r2  = Replicator 2 (default)
	r2h = Replicator 2 with HBP
	r2x = Replicator 2X
	t6  = TOM Mk6 - single extruder
	t7  = TOM Mk7 - single extruder
	t7d = TOM Mk7 - dual extruder
	z   = ZYYX - single extruder
	zd  = ZYYX - dual extruder
	fcp = FlashForge Creator Pro

SCALE: the coordinate system scale for the conversion (ABS = 1.0035)

X,Y & Z: the coordinate system offsets for the conversion

	X = the x axis offset
	Y = the y axis offset
	Z = the z axis offset

IN: the name of the sliced gcode input filename

OUT: the name of the X3G output filenameor the serial I/O port

       specify '--' to write to stdout

Examples:
```
	gpx -p -m r2 my-sliced-model.gcode
	gpx -c custom-tom.ini example.gcode /volumes/things/example.x3g
	gpx -x 3 -y -3 offset-model.gcode
	gpx -m c4 -s sio-example.gcode /dev/tty.usbmodem
```
