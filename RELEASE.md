# GpxUi 0.1.6

## Improvements

  * Windows Explorer gets 'Convert to X3g' on the right click menu
  * GPX has new gcode macros

## Bug fixes

  * Linux build works (no setup or releases yet though)
  * Handle .ini location better so that upgrades are hopefully smoother

([Commits](https://github.com/markwal/GpxUi/compare/0.1.5...0.1.6))

# GpxUi 0.1.5

## Improvements

  * Tooltips
  * First autoupdate check waits until the day after a fresh install
  * Adds gpx.exe and gpxui.exe to the path

## Bug fixes

  * Endstop editing in the machine editor now work (rather than all changing
    the x endstop)
  * Releases now include release notes built into the nupkg which prevents
    squirrel from barfing in the log
  * Notify the shell that we've changed the path (hopefully, workaround is to
    logout and log back in)

([Commits](https://github.com/markwal/GpxUi/compare/0.1.2...0.1.5))
