---
layout: doc
title: GPX macros
permalink: macros.html
---
GPX supports macros in the gcode stream to control GPX settings as well as
to access x3g protocol capabilities that don't map directly onto gcode.  The GPX
macro syntax is wrapped in a parenthetical gcode comment so that other gcode processing
software will hopefully benignly ignore them. Each macro must be at the beginning
of a comment and starts with the @ symbol.

Macros can either be of the semicolon ";" or parenthetical "()" form. Some host
and/or slicers strip one form of comment or the other. For example, OctoPrint
strips all ";" comments, so I recommend using the parenthetical "()" style. The
documentation below will show the headings with ";" so we can use the
parenthesis to indicate optional parameters regex style.

### ;@load\_eeprom_map

Asks the bot what flavor and version of firmware is running. Looks for a
built-in (in GPX) eeprom map that matches and selects it. Enables @eread/@ewrite
to use the named eeprom locations from that map.

As of this writing though, there is only one built-in eeprom map: Sailfish 7.7

### ;@eread <<EEPROMID>>

Looks up the EEPROMID in the current eeprom map, ask the bot to read that eeprom
location and displays the result.

Example:

    (@eread T0_EXTRUDER_P_TERM)

### ;@ewrite <<EEPROMID>> <<VALUE>>

Writes the value to the location indicated by the EEPROMID.  The value can be a
hex value indicated by starting with '#' (#1aff20), or a float (with a decimal
point 2.4), a string in quotes, or just a decimal number (only digits)

Examples:

    (@ewrite LED_CUSTOM_COLOR_R #7f)
    (@ewrite LED_CUSTOM_COLOR_G #3c)
    (@ewrite LED_CUSTOM_COLOR_B #4a)
    (@ewrite MACHINE_NAME MrMachine)

### ;@eeprom <<NAME>> <<TYPENAME>> #<<HEX>> (<<LEN>>)

Define your own eeprom mappings for use with eread/ewrite. The eeprom macro defines
one eeprom mapping named <<NAME>> of type <<TYPENAME>> (B=byte, H=unsigned 16-bit integer,
i=signed 32-bit integer, I=unsigned 32-bit integer, f=16-bit fixed point, or s=string)
at address <<HEX>> of length <<LEN>>. <<LEN>> only applies for the string type.

Examples:

    (@eeprom MACHINE_NAME s #22 16)
    (@eeprom MAX_ACCELERATION_AXIS_X H #17c)

### ;@printer (<<TYPE>> | <<PACKING_DENSITY>> | <<DIAMETER>>mm | <<HBP-TEMP>>c | #<<LED-COLOR>>)+

Set the current machine_type as well as other machine configuration options. All
of the parameters are optional and as many as you like can be included. 

<<TYPE>> sets the machine type. It should be one of the following:

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

<<PACKING_DENSITY>> sets the nominal packing density. Extrusions will be scaled by
the packing_density if GPX is configured to recompute extrusions.

<<DIAMETER>> sets the nominal filament diameter. Extrusions (when recompute extrusions
is enabled) will be scaled by the ratio of the actual filament diameter to the nominal
filament diameter.

<<HBP-TEMP>> sets the heated build platform temperature override. It will override
any M-code that sets the build platform temperature to something other than zero.

<<LED-COLOR>> sets the custom LED color

### ;@enable (ditto | progress)

Turns on GPX settings. Currently two settings are supported:

    ditto = turns on GPX ditto printing (repeats all commands on the other nozzle)
    progress = turns on recomputing progress updates (only applies to offline conversion)

### ;@filament <<NAME>> (<<DIAMETER>>mm | <<TEMP>>c | #<<LED_COLOR>>)+

Defines a filament for later reference in other macros such as (@pause), (@left)
and (@right). A <<NAME>> is required, but all of the parameters <<DIAMETER>>,
<<TEMP>> and <<LED_COLOR>> are optional.  <<LED_COLOR>> is a six digit
hexadecimal number indicating RGB.

Examples:

    (@filament redpla 1.77mm 205c #7f0000)
    (@filament bluepla 195c)
    (@filament greenpla #007f00)

### ;@right (<<NAME>>) | (<<PACKING_DENSITY>> | <<DIAMETER>>mm | <<TEMP>>c)+
### ;@left (<<NAME>>) | (<<PACKING_DENSITY>> | <<DIAMETER>>mm | <<TEMP>>c)+

Two major forms of this command. The first just indicates a name that references
an earlier defined filament via the (@filament) macro. In that case, that
filament definition becomes active for the indicated extruder. For single extruder
printers @right is the extruder.

The other case just sets one or mor of the parameters for the right extruder.

<<PACKING_DENSITY>> is a scaling factor for extrusion calculations if you've
turned on "recalculate_5d" in gpx.ini or via the command line "-w".

<<DIAMETER>> sets the actual filament diameter. Extrusions are multiplied by the
ratio of the actual filament diameter to the the nominal diameter for the right
extruder

<<TEMP>> is a nozzle temperature in Celsius. This sets an override which will
override any indicated temperature for the indicated nozzle. So, for example,
after (@right 180c), a later M104 S205 T0 will actually command a set
temperature of 180 degrees. By itself, the macro doesn't set a temperature.

Examples:

    (@right redpla)
    (@left 0.87 1.76mm 200c)
    (@right 1.70mm)

### ;@pause <<ZPOS>> (<<NAME>>)

Sets up a pause when the indicated <<ZPOS>> is reached. This macro should occur
after the gcode has set up the coordinate system (by homing and defining that
position either explicitly or by recall), but before the first command that
moves to or above the indicated <<ZPOS>> coordinate. It is intended to be used at
the end of the start gcode in the slicer.

<<ZPOS>> is a vertical coordinate in mm usually in distance from the print bed.

<<NAME>> is an optional parameter that indicates a filament definition from a
(@filament) that will also set the current filament at the pause position.

### ;@temp <<ZPOS>> <<TEMP>>c
### ;@temperature <<ZPOS>> <<TEMP>>c

Changes the temperature at the indicated <<ZPOS>> position. When the nozzle is at
<<ZPOS>> or greater, the temperature of the current extruder will be set to
<<TEMP>> degrees. Unlike @left & @right, this doesn't set an override for later
temperature commands, it actually emits the command to set the current
temperature. If the <<TEMP>> is greater than 120 then it sets the current
extruder's nozzle temperature to <<TEMP>> degrees celsius. If it less than or
equal to 120 then it sets the heated build platform temperature to <<TEMP>> 
degrees celsius.


