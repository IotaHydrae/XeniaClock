/**
 * @file aht10.h
 * @author IotaHydrae (writeforever@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-31
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

#ifndef __AHT10_H
#define __AHT10_H

#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

struct aht10_data {
    uint32_t status;
    uint32_t SRH;
    uint32_t ST;

    double real_humidity;
    double real_temperture;
};

enum {
    AHT10_STATUS_OK      = 0x00,
    AHT10_STATUS_BUSY    = 0x01,
    AHT10_STATUS_ERROR   = 0x02,
};

#define AHT10_ADDRESS       0x38
#define AHT10_CMD_RESET     0xe1
#define AHT10_CMD_MEASURE   0xac
#define AHT10_CMD_READ      0x33
#define AHT10_CMD_DUMMY     0x00

int __aht10_read_raw(uint8_t *rbuf, int len);
struct aht10_data aht10_read_humidity_temperture();

#endif /* __AHT10_H */