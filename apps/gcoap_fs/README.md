# GCoAP FileSystem

This examples set-up a CoAP server VFS. This application mimics an OBC
which would hold the updates file destined to the ThingSat payload.

## Note

The `gcoap` module in master expects IPv6 addressing. But CSP is used here which does
not use or support IP. To workaround this, destination addresses for requests should be formatted
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

## Description

This basic example shows how to use a file system with RIOT in your embedded
application.

In particular, this example shows:

- how to mount/format/unmount a file system, either with spiffs, littlefs, fatfs
  or constfs
- how to open/read/write/close a file with and without newlib

In RIOT, most file systems use a `mtd` as flash interface. So to use this
example one must define `MTD_0`. `MTD_0` is a pointer to a `mtd_dev_t`
instance.

This example uses `littlefs` as default file system on the whole `mtd`.
A `constfs` file system is also demonstrated with two files.

All the RIOT file systems are used through the `vfs` interface, and on most
platforms files can be accessed transparently with `open/close/read/write/...`
functions.
With newlib, `fopen/fclose/fread/fwrite/...` can also be used transparently.

## Shell commands

The following commands are available:

- `format`: should be called the first time only, it will format the `mtd`
  with the configured file system
- `mount`: mount the file system on the configured mount point (default is
  `/sda` but it can be configured with `FLASH_MOUNT_POINT` define). The
  `constfs` file system is mounted automatically on `/const` when the
   application starts
- `umount`: unmount `/sda`
- `cat <file>`: similarly to unix `cat` unix command, it prints the given
  `<file>` on stdout
- `tee <file> <str>`: similarly to `tee` unix command, it writes `<str>` in
  `<file>`

Besides of these commands, the default `vfs` commands can be used, for
instance:

- `vfs df`: shows all mountpoints and used/available file system space
- `vfs ls <path>`: list files

## Example on `native` with `constfs`

- Build and run the `gcoap_fs` example application on the `native` target:

```
make -C examples/gcoap_fs all term
[...]
main(): This is RIOT! (Version: 2018.04-devel-/examples/gcoap_fs)
constfs mounted successfully
```

- List the available files in the `constfs` predefined partition:

```
> ls /const
ls /const
/hello-world
/hello-riot
total 2 files
```

- Print the content of the files:

```
> cat /const/hello-world
cat /const/hello-world
Hello World!
> cat /const/hello-riot
cat /const/hello-riot
Hello RIOT!
```

You can access the file system from another board that runs the `gcoap_csp_cli`
example

```
> coap get ::33 2 /vfs/nvm0/
gcoap_cli: sending msg ID 32856, 16 bytes
> gcoap: response Success, code 2.05, 17 bytes
<fw/>,<cubedate/>
