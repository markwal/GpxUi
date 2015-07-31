# Variables
QTSUBDIRS = build/mingw32
SUBDIRS = $(QTSUBDIRS) gpx
QTMAKEFILES := $(addsuffix /Makefile,$(QTSUBDIRS))
QTBIN := $(dir $(abspath $(shell which qmake)))

# Version Variables
# some rigamarole to ensure first character after first dash in version string
# is alphabetic since nuget chokes on a number
GPXUI_VERSION := $(shell git describe --tags --dirty | awk -- '{sub(/-/,"-dev-")}; 1')
GPXUI_ORIGIN := $(word 2,$(shell git remote -v))

# Squirrel Variables
SQUIRRELWIN = build/squirrel.windows/
SQUIRRELWINBASE = $(SQUIRRELWIN)nuget/
SQUIRRELWINBIN = $(SQUIRRELWINBASE)lib/net45/

# WinDeployQt Variables
# if "static" appears in the abspath of qmake.exe, assume we're statically linking
# because that's how qt works, you build the libraries *and* toolset for static
ifneq (,$(findstring static,$(QTBIN)))
    WINDEPLOYQT = populatewinbin
else
    WINDEPLOYQT = windeployqt
endif


# Rules

.PHONY: first all clean test debug release populatewinbin windeployqt squirrel.windows

first all clean test debug release: $(QTMAKEFILES) build/version.h
	for dir in $(SUBDIRS); do \
		echo "Entering $$dir"; \
		make -C $$dir $@; \
		echo "Exiting $$dir"; \
	done

$(QTMAKEFILES): GpxUi.Pro
	mkdir -p $(dir $@)
	cd $(dir $@); qmake ../../$< -r -spec win32-g++ "CONFIG+=debug"

build/version.h: .git/HEAD .git/index
	@echo "#define GPXUI_VERSION \"$(GPXUI_VERSION)\"" > $@
	@echo "#define GPXUI_ORIGIN \"$(GPXUI_ORIGIN)\"" >> $@

populatewinbin: release
	mkdir -p $(SQUIRRELWINBIN)
	cp GPX/src/gpx/win32_obj/gpx.exe $(SQUIRRELWINBIN)
	cp build/mingw32/release/GpxUi.exe $(SQUIRRELWINBIN)
	cp README.md $(SQUIRRELWINBIN)
	cp LICENSE $(SQUIRRELWINBIN)

windeployqt: populatewinbin
	windeployqt --release-with-debug-info --no-plugins --no-translations $(SQUIRRELWINBIN)GpxUi.exe

squirrel.windows: $(WINDEPLOYQT)
	nuget pack gpx.nuspec -Version $(GPXUI_VERSION) -BasePath $(SQUIRRELWINBASE) -OutputDirectory $(SQUIRRELWIN)
	Squirrel --releasify build/squirrel.windows/GpxUi.$(GPXUI_VERSION).nupkg --releaseDir=$(SQUIRRELWIN)release
