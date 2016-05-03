---
layout: doc
title: EEPROM macros
permalink: eeprommacros.html
order: 4
---

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

