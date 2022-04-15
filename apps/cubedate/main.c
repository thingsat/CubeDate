/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       File system usage example application
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "shell.h"
#include "board.h"

#include "xfa.h"
#include "vfs_default.h"

#include "suit/transport/coap.h"
#include "suit/storage.h"
#include "suit/storage/ram.h"
#include "suit/storage/vfs.h"

#if IS_USED(MODULE_LIBCSP)
#include "csp/csp_iflist.h"
#endif

#if IS_USED(MODULE_GNRC)
#include "net/nanocoap.h"
#include "suit/transport/coap.h"
#include "thread.h"
#include "kernel_defines.h"

#define COAP_INBUF_SIZE (256U)
/* Extend stacksize of nanocoap server thread */
static char _nanocoap_server_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
#define NANOCOAP_SERVER_QUEUE_SIZE     (8)
static msg_t _nanocoap_server_msg_queue[NANOCOAP_SERVER_QUEUE_SIZE];

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    /* this line adds the whole "/suit"-subtree */
    SUIT_COAP_SUBTREE,
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);

static void *_nanocoap_server_thread(void *arg)
{
    (void)arg;

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_nanocoap_server_msg_queue, NANOCOAP_SERVER_QUEUE_SIZE);

    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));

    return NULL;
}
#endif

/* add handled storages */
XFA_USE(char*, suit_storage_files_reg);
XFA(suit_storage_files_reg, 0) char* _manifest = VFS_DEFAULT_DATA "/SLOT0.TXT";
XFA(suit_storage_files_reg, 0) char* _firmware = VFS_DEFAULT_DATA "/SLOT1.TXT";

/* fletcher functions */
extern void f12r_storage_init(void);

int main(void)
{
    suit_storage_init_all();
    f12r_storage_init();

#if IS_USED(MODULE_GNRC)
    ipv6_addr_t addr;
    const char addr_str[] = "2001:db8::3";
    ipv6_addr_from_str(&addr, addr_str);
    gnrc_netif_ipv6_addr_add(gnrc_netif_iter(NULL), &addr, 64, 0);
#endif
#if IS_USED(MODULE_LIBCSP)
    csp_iface_t *iface = csp_iflist_get_by_name("can0");
    iface->addr = 0x0022;
#endif

    /* start suit coap updater thread */
    suit_coap_run();

#if IS_USED(MODULE_GNRC)
    /* start nanocoap server thread */
    thread_create(_nanocoap_server_stack, sizeof(_nanocoap_server_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  _nanocoap_server_thread, NULL, "nanocoap server");
#endif

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
