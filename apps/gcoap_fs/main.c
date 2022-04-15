/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     apps
 * @{
 *
 * @file
 * @brief       CoAP server VFS
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "shell.h"
#include "board.h"

#include "vfs_default.h"

#include "net/gcoap.h"
#include "net/coapfileserver.h"

#if IS_USED(MODULE_LIBCSP)
#include "csp/csp.h"
#endif

#define MAIN_QUEUE_SIZE (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* Export all the file system */
static const struct coapfileserver_entry vfs = { "", 1 };

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    { "/vfs", COAP_GET | COAP_MATCH_SUBTREE, coapfileserver_handler, (void *)&vfs },
};

static gcoap_listener_t _listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    GCOAP_SOCKET_TYPE_UNDEF,
    NULL,
    NULL,
    NULL
};

int main(void)
{

#if IS_USED(MODULE_GNRC)
    ipv6_addr_t addr;
    const char addr_str[] = "2001:db8::2";
    ipv6_addr_from_str(&addr, addr_str);
    gnrc_netif_ipv6_addr_add(gnrc_netif_iter(NULL), &addr, 64, 0);
#endif
#if IS_USED(MODULE_LIBCSP)
    csp_iface_t *iface = csp_iflist_get_by_name("can0");
    iface->addr = 0x0033;
#endif

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    gcoap_register_listener(&_listener);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
