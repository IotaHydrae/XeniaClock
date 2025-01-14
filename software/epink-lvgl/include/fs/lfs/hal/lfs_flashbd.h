/*
 * Block device in spi nor flash
 *
 * Copyright (c) 2023, IotaHydrae <writeforever@foxmail.com>.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef LFS_FLASHBD_H
#define LFS_FLASHBD_H

#include "fs/lfs/lfs.h"
#include "fs/lfs/lfs_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct lfs_flashbd_config {
    // 8-bit erase value to simulate erasing with. -1 indicates no erase
    // occurs, which is still a valid block device
    int32_t erase_value;

    // Optional statically allocated buffer for the block device.
    void *buffer;
};

// flashbd state
typedef struct lfs_rambd {
    uint8_t *buffer;
    const struct lfs_flashbd_config *cfg;
} lfs_flashbd_t;

// Create a RAM block device using the geometry in lfs_config
int lfs_flashbd_create(const struct lfs_config *cfg);
int lfs_flashbd_createcfg(const struct lfs_config *cfg,
        const struct lfs_flashbd_config *bdcfg);

// Clean up memory associated with block device
int lfs_flashbd_destroy(const struct lfs_config *cfg);

// Read a block
int lfs_flashbd_read(const struct lfs_config *cfg, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size);

// Program a block
//
// The block must have previously been erased.
int lfs_flashbd_prog(const struct lfs_config *cfg, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size);

// Erase a block
//
// A block must be erased before being programmed. The
// state of an erased block is undefined.
int lfs_flashbd_erase(const struct lfs_config *cfg, lfs_block_t block);

// Sync the block device
int lfs_flashbd_sync(const struct lfs_config *cfg);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif