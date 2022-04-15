# csp_coap_minimal example

This is a minimalistic example for a CoAP server using CSP (CubeSatProtocol)
network stack.

The application will auto initialize the device's default CAN interface.

You can ping the printed address from another device to check if the node is
alive, you can also perform a CoAP GET request to `/.well-known/core`.

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

The CSP address `39:E5` or `0x39E5` would become `::39e5` Ipv6 address which
is the address printed in this examples. But if pinging through CSP, e.g.
through the [`tests/pkg_libcsp`](../../RIOT/tests/pkg_libcsp/) CLI then use
the only the CSP address fraction.
