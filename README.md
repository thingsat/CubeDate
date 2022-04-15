# CubeDate

This repository hold code relating to the implementation of Cubedate, a generic
architecture enabling standards-based, secure software updates for payloads
hosted on a low-power CubeSats.

## Applications

- [csp_coap_minimal](apps/csp_coap_minimal/README.md): a minimal CoAP
over CSP application
- [gnrc_coap_minimal](apps/gnrc_coap_minimal/README.md): a minimal CoAP
over RIOT GNRC network stack application
- [gcoap_fs](apps/gcoap_fs/README.md): a CoAP backed filesystem, by default over CSP
- [gcoap_fs](apps/gcoap_csp_cli/README.md): a CoAP cli/server application to test
CoAP over CSP
- [cubedate](apps/cubedate/README.md): an implementation of Cubedate with the
SUIT over CoAP over CSP over CAN stack.

## Setup

### Requirements

To run on RIOTs native simulator follow the setup instructions in
[README.native.can.md](RIOT/tests/candev/README.native.can.md)

### Initializing the repository

RIOT is included as a submodule of this repository. We provide a `make` helper
target to initialize it. From the root of this repository, issue the following
command:

```
$ make init-submodules
```

### Building the firmwares

From the root directory of this repository, simply issue the following command:

```
$ make
```

### Flashing the firmwares

For each firmware use the RIOT way of flashing them. For example, in
`apps/gnrc_gcoap_minimal`, use:

```
$ make -C tapps/gnrc_gcoap_minimal flash
```
