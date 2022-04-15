# name of your application
APPLICATION = gcoap_fs

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
## Use VFS
USEMODULE += vfs
USEMODULE += vfs_default
USEMODULE += vfs_auto_format
## Use long file names
CFLAGS += -DFATFS_FFCONF_OPT_USE_LFN=1
## Include gcoap server fileserver modules
USEMODULE += gcoap
USEMODULE += coapfileserver
## Use CSP as the default network stack
CSP ?= 1
ifeq (1,$(CSP))
  include $(CURDIR)/Makefile.csp.dep
else
  include $(CURDIR)/Makefile.gnrc.dep
endif

include $(RIOTBASE)/Makefile.include

## FatFS image configuration for native
FATFS_IMAGE_FILE_SIZE_MB ?= 128
FATFS_IMAGE_FILE ?= $(CURDIR)/riot_fatfs_disk.img
ifeq ($(BOARD),native)
  CFLAGS += -DMTD_NATIVE_FILENAME=\"$(FATFS_IMAGE_FILE)\"
  CFLAGS += -DMTD_SECTOR_NUM=\($(FATFS_IMAGE_FILE_SIZE_MB)*1024*1024/MTD_SECTOR_SIZE\)
  TERMDEPS += $(FATFS_IMAGE_FILE)
endif

# Increase defualt stack for non-native BOARDs
ifneq ($(BOARD),native)
  CFLAGS += -DGCOAP_STACK_SIZE=THREAD_STACKSIZE_LARGE
endif

.PHONY: fatfs-image

# target to create a FatFS image for native
fatfs-image:
	$(Q)if [ ! -f "$(FATFS_IMAGE_FILE)" ]; then \
  echo "Creating $(FATFS_IMAGE_FILE) ..."; \
  dd if=/dev/zero of=$(FATFS_IMAGE_FILE) bs=1M count=$(FATFS_IMAGE_FILE_SIZE_MB); \
  mkfs.fat $(FATFS_IMAGE_FILE); \
  fi

$(FATFS_IMAGE_FILE): fatfs-image

include $(RIOTMAKE)/default-radio-settings.inc.mk
