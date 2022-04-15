# Default Makefile, for host native GNRC-based networking

# Name of your application
APPLICATION = gcoap_example

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Use CSP network stack
USEPKG += libcsp
# libCSP ports must be <64, so arbitrarily pick port 2 as the default
# CoAP port instead of 5683
COAP_PORT ?= 2
CFLAGS += -DCOAP_PORT=$(COAP_PORT)
CFLAGS += -DCONFIG_GCOAP_PORT=$(COAP_PORT)
# Include gcoap
USEMODULE += gcoap

# Required by gcoap example
USEMODULE += od
USEMODULE += fmt
USEMODULE += netutils
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
