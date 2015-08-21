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
