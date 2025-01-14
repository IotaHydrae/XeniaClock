/**
 * @file tools.h
 * @author IotaHydrae (writeforever@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-20
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

#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include <linux/types.h>
#include "bitops.h"

#define DEBUG 1

#if DEBUG
    #define pr_debug(fmt, ...) printf("[ DEBUG ] +%d %s : "fmt, __LINE__, __func__, ##__VA_ARGS__);
#else
    #define pr_debug
#endif

// #define pr_info(fmt, ...) printf("[ INFO ] +%d %s : "fmt, __LINE__, __func__, ##__VA_ARGS__);
// #define pr_warn(fmt, ...) printf("[ WARN ] +%d %s : "fmt, __LINE__, __func__, ##__VA_ARGS__);
// #define pr_err(fmt, ...) printf("[ ERROR ] +%d %s : "fmt, __LINE__, __func__, ##__VA_ARGS__);

#ifndef ARRAY_SIZE
    #define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

// #ifndef BIT
//     #define BIT(x) (1 << x)
// #endif

#ifndef BITMASK
    #define BITMASK(x)  (~(1 << x))
#endif

#ifndef container_of
/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({          \
        const typeof(((type *)0)->member) * __mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); })
#endif

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#endif