QTSUBDIRS = build/mingw32
SUBDIRS = $(QTSUBDIRS) gpx
QTMAKEFILES = $(addsuffix /Makefile,$(QTSUBDIRS))
QTBIN = $(dir $(abspath $(shell which qmake)))
GPXUI_VERSION = $(shell git describe --tag)
SQUIRRELWIN = build/squirrel.windows/
SQUIRRELWINBIN = build/squirrel.windows/lib/net45/

.PHONY: first all clean test debug release

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

justtesting:
	mkdir -p $(SQUIRRELWINBIN)
	cp build/mingw32/release/GpxUi.exe $(SQUIRRELWINBIN)
	windeployqt --release-with-debug-info --no-plugins $(SQUIRRELWINBIN)GpxUi.exe

squirrel.windows: release
	mkdir -p $(SQUIRRELWINBIN)
	cp GPX/src/gpx/win32_obj/gpx.exe $(SQUIRRELWINBIN)
	cp build/mingw32/release/GpxUi.exe $(SQUIRRELWINBIN)
	windeployqt --release-with-debug-info --no-plugins --no-translations $(SQUIRRELWINBIN)GpxUi.exe
	nuget pack gpx.nuspec -Version $(GPXUI_VERSION) -BasePath $(SQUIRRELWIN) -OutputDirectory $(SQUIRRELWIN)
	Squirrel --releasify build/squirrel.windows/GpxUi.$(GPXUI_VERSION).nupkg --releaseDir=$(SQUIRRELWIN)release
