QTSUBDIRS = build/mingw32
SUBDIRS = $(QTSUBDIRS) gpx
QTMAKEFILES = $(addsuffix /Makefile,$(QTSUBDIRS))
GPXUI_VERSION = $(shell git describe --tag)

.PHONY: first all clean test debug release

first all clean test debug release: $(QTMAKEFILES) build/version.h
	for dir in $(SUBDIRS); do \
		echo "Entering $$dir"; \
		make -C $$dir $@; \
		echo "Exiting $$dir"; \
	done

$(QTMAKEFILES): GpxUi.Pro
	mkdir -p $(dir $@)
	cd $(dir $@); qmake ../../$< "CONFIG+=debug"

build/version.h: .git/HEAD .git/index
	@echo "#define GPXUI_VERSION $(GPXUI_VERSION)" > $@
