# name of your application
APPLICATION = csp_coap_minimal

# If no BOARD is found in the environment, use this default:
BOARD ?= nucleo-f446re

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../RIOT

# Set this to 1 to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 0

# Use CSP Network Stack
USEPKG += libcsp
# Use minimal standard PRNG
USEMODULE += prng_minstd
# Include NanoCoAP
USEMODULE += nanocoap_sock
# libCSP ports must be <64, so arbitrarily pick port 2 as the default
# CoAP port instead of 5683
COAP_PORT ?= 2
CFLAGS += -DCOAP_PORT=$(COAP_PORT)

CFLAGS += -DLOG_LEVEL=LOG_NONE  # disable log output

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
