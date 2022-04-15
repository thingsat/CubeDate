# name of your application
APPLICATION = cubedate
# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# Modules to include:
## Add common shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
## Use CSP as the default network stack
CSP ?= 1
ifeq (1,$(CSP))
  include $(CURDIR)/Makefile.csp.dep
else
  include $(CURDIR)/Makefile.gnrc.dep
endif
## Include SUIT with default CoAP Transport
USEMODULE += suit
USEMODULE += suit_transport_coap
### Configure one RAM regions with 2K
USEMODULE += suit_storage_ram
CFLAGS += -DCONFIG_SUIT_STORAGE_RAM_REGIONS=1
CFLAGS += -DCONFIG_SUIT_STORAGE_RAM_SIZE=2048
## Add an application for sofwate updates
F12R_DIRS  += $(CURDIR)/application/
F12R_BLOBS += application/fletcher32_fc.bin
USEPKG += femto-container
CFLAGS += -I$(CURDIR)
## Display a progress bar during firmware download
USEMODULE += progress_bar

# Ensure both slot bin files are always generated and linked to avoid compiling
# during the test. This ensures that "BUILD_IN_DOCKER=1 make test"
# can rely on them being present without having to trigger re-compilation.
BUILD_FILES += $(SLOT_RIOT_ELFS:%.elf=%.bin)
# The test needs the linked slot binaries without header in order to be able to
# create final binaries with specific APP_VER values. The CI RasPi test workers
# don't compile themselves and re-create signed images, thus add the required
# files here so they will be submitted along with the test jobs.
TEST_EXTRA_FILES += $(SLOT_RIOT_ELFS) $(SUIT_SEC) $(SUIT_PUB)
# For now `examples/%/tests" still rely on the test applicaton being reset after
# a terminal is opened to synchronize.
TESTRUNNER_RESET_AFTER_TERM ?= 1

# Add custom SUIT targets
include $(CURDIR)/Makefile.suit.custom

include $(RIOTBASE)/Makefile.include

# Add custom targets for publishing directly to a FS storage device
suit/publish-fs: $(SUIT_MANIFESTS) $(SUIT_MANIFEST_PAYLOADS)
	$(Q)if [ "$${BOARD}" = "native" ]; then \
	sudo mkdir -p $(SUIT_COAP_FSROOT); \
  sudo mount -o loop,umask=000 $(SUIT_FATFS_IMAGE) $(SUIT_COAP_FSROOT); \
	fi
	mkdir -p $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	cp $^ $(SUIT_COAP_FSROOT)/$(SUIT_COAP_BASEPATH)
	$(Q)for file in $(notdir $^); do \
		echo "published \"$$file\""; \
		echo "       as \"$(SUIT_COAP_ROOT)/$$file\""; \
	done
	$(Q)if [ "$${BOARD}" = "native" ]; then \
	sudo umount $(SUIT_COAP_FSROOT); \
	fi

suit/publish-fletcher: suit/publish-fs
	@true

suit/publish-fw: suit/publish-fs
	@true

include $(RIOTMAKE)/default-radio-settings.inc.mk
