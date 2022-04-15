## About

This application provides command line access to gcoap, a high-level API for
CoAP messaging. See the [CoAP spec][1] for background, and the
Modules>Networking>CoAP topic in the source documentation for detailed usage
instructions and implementation notes.

## Note

This is examples is a direct copy of the `gcoap` examples in master, as such
it expects IPv6 addressing. But CSP is used here which does not use or support
IP. To workaround this, destination addresses for requests should be formatted
as IPv6 addresses with trailing zeros. Internal the CSP `sock_udp` module ignores
all but the first 2 bytes.

e.g.:

```
> csp
csp
can0       addr: 39:E5 netmask: 4 mtu: 247
           tx: 00000 rx: 00000 txe: 00000 rxe: 00000
           drop: 00000 autherr: 00000 frame: 00000
           txb: 0 (0B) rxb: 0 (0B)
```

The CSP address `39:E5` or `0x39E5` would become `::39e5` Ipv6 address.

### Native networking

Follow the instructions in [README.native.can.md](../../RIOT/tests/candev/README.native.can.md)
to setup two virtual can interfaces.

## Example Use

This examples uses gcoap as a CoAP server or a as a CoAP client.

### Verify setup from RIOT terminal

    > coap info

Expected response:

    CoAP server is listening on port 2
     CLI requests sent: 0
    CoAP open requests: 0

### Send query between RIOT instances

    > coap get ::39e5 2 /.well-known/core
    coap get ::39e5 2 /.well-known/core
    gcoap_cli: sending msg ID 56189, 23 bytes
    > gcoap: response Success, code 2.05, 46 bytes
    </cli/stats>;ct=0;rt="count";obs,</riot/board>
