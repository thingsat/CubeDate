# Minimal Cubedate Application Example

This application shows a minimal Cubedate application. It implements a
SUIT over CoAP over CSP over CAN stack. By default it is ran on native,
but can be adapted to run on real hardware.

On native dummy data is published as an update to VFS storage located
at `/nvm0/SLOT0.txt`. This mimics the workflow for a firmware update
to a RIOT slot. Use the `make suit/publish-fw` target for this

Updates can also be published to RAM storage, similar the workflow for
mission files, use the `make suit/publish-fletcher` target for this.

## Native Setup

Follow the instructions in [README.native.can.md](../../RIOT/tests/candev/README.native.can.md) to setup two virtual can interfaces.

## Note

The CoAP module in RIOT expects IPv6 addressing. But CSP is used here which does
not use or support IP. To workaround this, destination addresses for requests
should be formatted as IPv6 addresses with trailing zeros. Internally the CSP
`sock_udp` module ignores all but the first 2 bytes.

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


## Automatic test

Provided the required setup an automatic test can be ran as:

```
$ python apps/cubedate/tests-with-config/01-run.p
...
Fetching firmware |█████████████████████████| 100%
Finalizing payload store
Verifying image digest
Starting digest verification against image
Install correct payload
Verifying image digest
Starting digest verification against image
Verified installed payload
suit seq_no
suit seq_no
seq_no: 32
```

It will perform an update while at the same time modifying the sequence numbers.
