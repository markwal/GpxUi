QTSUBDIRS = build/mingw32
SUBDIRS = $(QTSUBDIRS) gpx
QTMAKEFILES = $(addsuffix /Makefile,$(QTSUBDIRS))

.PHONY: first all clean test debug release

first all clean test debug release: $(MAKEFILES)
	for dir in $(SUBDIRS); do \
		echo "Entering $$dir"; \
		make -C $$dir $@; \
		echo "Exiting $$dir"; \
	done

$(MAKEFILES): GpxUi.Pro
	mkdir -p $(dir $@)
	cd $(dir $@); qmake ../../$< "CONFIG+=debug"
