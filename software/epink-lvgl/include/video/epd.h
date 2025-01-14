/**
 * @file epd.h
 * @author IotaHydrae (writeforever@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-17
 *
 * MIT License
 *
 * Copyright 2022 IotaHydrae(writeforever@foxmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *
 */

#pragma once

#ifndef __EPD_H
#define __EPD_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "common/tools.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "display_manager.h"
#include "spi/native_spi.h"

#define TEST_DOC "This document describes how to write an ALSA \
(Advanced Linux Sound Architecture) driver. The document focuses \
mainly on PCI soundcards. In the case of other device types, the \
API might be different, too. However, at least the ALSA kernel \
API is consistent, and therefore it would be still a bit help \
for writing them."

#define TEST_DOC2 "This variable provides a means of enabling or \
disabling features of a recipe on a per-recipe basis. PACKAGECONFIG \
blocks are defined in recipes when you specify features and then \
arguments that define feature behaviors."

/* Pins Define of rpi-pico
 *
 * The SPI interface using default spi0
 *
 * RES  <->  GP14
 * DC   <->  GP15
 * BUSY <->  GP20
 */
#define EPINK_RES_PIN   15
#define EPINK_DC_PIN    14
#define EPINK_CS_PIN    13
#define EPINK_BUSY_PIN  12

/* ========== epink panel info ========== */
#define EPINK_WIDTH         200
#define EPINK_HEIGHT        200
#define EPINK_PAGE_SIZE     8
#define EPINK_LINE_WIDTH_IN_PAGE (EPINK_WIDTH/EPINK_PAGE_SIZE)
#define EPINK_BPP 1
#define EPINK_COLOR_WHITE (0xff)
#define EPINK_COLOR_BLACK (0x00)
#define EPINK_DISP_BUFFER_SIZE (EPINK_WIDTH*EPINK_HEIGHT/8)
#define EPINK_DISP_BUFFER_OFFSET(p,x)(p*EPINK_WIDTH + x)

/* ========== epink panel settings ========== */
#define EPINK_UPDATE_MODE_FULL 1
#define EPINK_UPDATE_MODE_PART 2

#ifndef DEFAULT_EPINK_UPDATE_MODE
    #define DEFAULT_EPINK_UPDATE_MODE EPINK_UPDATE_MODE_PART
#endif

// #define EPINK_DEBUG_MODE
#define EPINK_COORD_CHECK
#ifdef EPINK_DEBUG_MODE
    #define EPINK_DEBUG(...) printf(__VA_ARGS__)
#else
    #define EPINK_DEBUG
#endif

#define EPINK_ERROR(...) printf("[ERROR]" __VA_ARGS__)

/* Base  */
void epink_dc_set();
void epink_dc_clr();
void epink_res_set();
void epink_res_clr();
void epink_reset();

void epink_write_byte(uint8_t val);
void epink_write_command(uint8_t command);
void epink_write_data(uint8_t data);
void epink_wait_busy();
int epink_wait_busy_timeout(uint32_t timeout_ms);

void epink_init(uint8_t mode);
// void epink_clear(uint8_t color);
void epink_blank();
void epink_flush(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, void *colorp);
void epink_draw_pixel(uint8_t x, uint8_t y, uint16_t color);

#define EPINK_USE_INIT              1
#define EPINK_USE_FLUSH             1
#define EPINK_USE_CLEAR             1
#define EPINK_USE_BLANK             1
#define EPINK_USE_SET_UPDATE_MODE   1
#define EPINK_USE_PUT_PIXEL         1

extern uint8_t epink_disp_buffer[EPINK_DISP_BUFFER_SIZE];
extern uint8_t epink_disp_buffer_old[EPINK_DISP_BUFFER_SIZE];
extern unsigned char fontdata_mini_4x6[1536];
extern unsigned char fontdata_8x16[4096];

#endif // !__EPD_H