/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     module_ed
 * @{
 *
 * @file
 * @brief       Encounter Data List implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <string.h>
#include <stdalign.h>

#include "xfa.h"
#include "shell.h"
#include "shell_commands.h"

#include "ztimer.h"
#include "mutex.h"
#include "femtocontainer/femtocontainer.h"
#include "shared.h"

#include "blob/application/fletcher32_fc.bin.h"

#include "suit/storage.h"
#include "suit/storage/ram.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL   LOG_DEBUG
#endif
#include "log.h"

static const alignas(uint16_t)  unsigned char wrap_around_data[] __attribute__((aligned(2))) =
    "AD3Awn4kb6FtcsyE0RU25U7f55Yncn3LP3oEx9Gl4qr7iDW7I8L6Pbw9jNnh0sE4DmCKuc"
    "d1J8I34vn31W924y5GMS74vUrZQc08805aj4Tf66HgL1cO94os10V2s2GDQ825yNh9Yuq3"
    "QHcA60xl31rdA7WskVtCXI7ruH1A4qaR6Uk454hm401lLmv2cGWt5KTJmr93d3JsGaRRPs"
    "4HqYi4mFGowo8fWv48IcA3N89Z99nf0A0H2R6P0uI4Tir682Of3Rk78DUB2dIGQRRpdqVT"
    "tLhgfET2gUGU65V3edSwADMqRttI9JPVz8JS37g5QZj4Ax56rU1u0m0K8YUs57UYG5645n"
    "byNy4yqxu7";

static uint8_t _f12r_stack[512];
static mutex_t _f12_lock;

typedef struct {
    __bpf_shared_ptr(const uint16_t *, data);
    uint32_t words;
} fletcher32_ctx_t;

static suit_storage_hook_t pre;
static suit_storage_hook_t post;

static void _lock_region(void *arg)
{
    mutex_t *lock = (mutex_t *)arg;

    mutex_lock(lock);
}

static void _unlock_region(void *arg)
{
    mutex_t *lock = (mutex_t *)arg;

    mutex_unlock(lock);
}

void f12r_storage_init(void)
{
    suit_storage_t *storage = suit_storage_find_by_id(".ram.0");
    suit_storage_ram_t *ram = container_of(storage, suit_storage_ram_t, storage);

    /* install hooks */
    pre.arg = &_f12_lock;
    pre.cb = _lock_region;
    post.arg = &_f12_lock;
    post.cb = _unlock_region;
    suit_storage_add_pre_hook(storage, &pre);
    suit_storage_add_post_hook(storage, &post);

    /* initial payload */
    memcpy(ram->regions[0].mem, fletcher32_fc_bin, sizeof(fletcher32_fc_bin));
    ram->regions[0].occupied = sizeof(fletcher32_fc_bin);
}

void f12r_fletcher_run(void)
{
    suit_storage_t *storage = suit_storage_find_by_id(".ram.0");
    suit_storage_ram_t *ram = container_of(storage, suit_storage_ram_t, storage);

    if (ram->regions[0].occupied == 0) {
        puts("nothing to run");
        return;
    }

    fletcher32_ctx_t ctx = {
        .data = (const uint16_t *)(uintptr_t)wrap_around_data,
        .words = sizeof(wrap_around_data) / 2,
    };
    f12r_t femtoc = {
        .application = ram->regions[0].mem,
        .application_len = ram->regions[0].occupied,
        .stack = _f12r_stack,
        .stack_size = sizeof(_f12r_stack),
    };
    f12r_mem_region_t region;

    f12r_setup(&femtoc);

    f12r_add_region(&femtoc, &region,
                    (void *)wrap_around_data, sizeof(wrap_around_data), FC_MEM_REGION_READ);
    int64_t result = 0;
    uint32_t start = ztimer_now(ZTIMER_USEC);

    for (unsigned i = 0; i < 1000; i++) {
        mutex_lock(&_f12_lock);
        f12r_execute_ctx(&femtoc, &ctx, sizeof(ctx), &result);
        mutex_unlock(&_f12_lock);
    }
    uint32_t stop = ztimer_now(ZTIMER_USEC);

    printf("Result: %" PRIx32 "\n", (uint32_t)result);
    printf("duration: %" PRIu32 " us -> %" PRIu32 " us/exec\n",
           (stop - start), (stop - start) / 1000);
}

static int _fletcher_handler(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    f12r_fletcher_run();
    return 0;
}

SHELL_COMMAND(fletcher, "Run Fletcher 32", _fletcher_handler);
