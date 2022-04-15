.PHONY: all clean build init-submodules static-checks

IGNORE_APPS ?=
APPLICATIONS = $(wildcard $(filter-out $(IGNORE_APPS),$(CURDIR)/apps/*))

all: build
	@true

clean:
	@for app in $(APPLICATIONS); do $(MAKE) -C $$app distclean; done

build:
	@for app in $(APPLICATIONS); do $(MAKE) -C $$app; done

init-submodules:
	@git submodule update --init --recursive
	@git submodule update --remote

info-debug-variable-%:
	@echo $($*)

static-checks:
	@cppcheck --std=c11 --enable=style --quiet ./apps
