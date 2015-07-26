SUBDIRS = build/mingw32
MAKEFILES = $(addsuffix /Makefile,$(SUBDIRS))

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
