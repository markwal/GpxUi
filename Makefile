# OS Specifics
ifeq ($(OS),Windows_NT)
    # Windows
    QTSUBDIRS = build/mingw32
    QMAKEFLAGS = -r -spec win32-g++ "CONFIG+=debug"
else
    UNAMESYS := $(shell uname -s)
    ifeq ($(UNAMESYS),Darwin)
	# Mac
	QTSUBDIRS = build/darwin
	# what QMAKEFLAGS?
    else
	# Linux
	QTSUBDIRS = build/linux
	QMAKEFLAGS = -r -spec linux-g++-64
    endif
endif

# Variables
SUBDIRS = $(QTSUBDIRS) GPX
QTMAKEFILES := $(addsuffix /Makefile,$(QTSUBDIRS))
QTBIN := $(dir $(abspath $(shell which qmake)))

# Version Variables
# some rigamarole to ensure first character after first dash in version string
# is alphabetic since nuget chokes on a number
GPXUI_VERSION := $(shell git describe --tags --dirty | awk -- '{sub(/-/,"-dev-")}; 1')
GPXUI_RCVERSION := $(shell echo $(GPXUI_VERSION) | awk -- 'BEGIN{FS="[.-]"};{print $$1","$$2","$$3",0"};')
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

first all clean test debug release: build/version.h $(QTMAKEFILES)
	for dir in $(SUBDIRS); do \
		echo "Entering $$dir"; \
		make -C $$dir $@; \
		echo "Exiting $$dir"; \
	done

$(QTMAKEFILES): GpxUi.pro
	mkdir -p $(dir $@)
	cd $(dir $@); qmake ../../$< $(QMAKEFLAGS)

build/version.h: .git/HEAD .git/index
	mkdir -p build
	@echo "#define GPXUI_VERSION \"$(GPXUI_VERSION)\"" > $@
	@echo "#define GPXUI_RCVERSION $(GPXUI_RCVERSION)" >> $@
	@echo "#define GPXUI_ORIGIN \"$(GPXUI_ORIGIN)\"" >> $@

populatewinbin: release
	mkdir -p $(SQUIRRELWINBIN)
	cp GPX/src/gpx/win32_obj/gpx.exe $(SQUIRRELWINBIN)
	cp build/mingw32/release/GpxUi.exe $(SQUIRRELWINBIN)
	cp README.md $(SQUIRRELWINBIN)
	cp RELEASE.md $(SQUIRRELWINBIN)
	cp LICENSE $(SQUIRRELWINBIN)
	cp Squirrel.COPYING $(SQUIRRELWINBIN)

windeployqt: populatewinbin
	windeployqt --release-with-debug-info --no-plugins --no-translations $(SQUIRRELWINBIN)GpxUi.exe

squirrel.windows: $(WINDEPLOYQT)
	nuget pack gpx.nuspec -Version $(GPXUI_VERSION) -BasePath $(SQUIRRELWINBASE) -OutputDirectory $(SQUIRRELWIN)
	Squirrel --releasify build/squirrel.windows/GpxUi.$(GPXUI_VERSION).nupkg --releaseDir=$(SQUIRRELWIN)release
