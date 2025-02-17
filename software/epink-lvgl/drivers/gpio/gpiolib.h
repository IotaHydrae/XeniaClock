/**
 * @file gpiolib.h
 * @author IotaHydrae (writeforever@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-30
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

/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Internal GPIO functions.
 *
 * Copyright (C) 2013, Intel Corporation
 * Copyright (C) 2023, iotahydrae
 * Author: Mika Westerberg <mika.westerberg@linux.intel.com>
 */

#ifndef GPIOLIB_H
#define GPIOLIB_H

#include <common/types.h>
#include <common/list.h>
#include <linux/device.h>

#define GPIOCHIP_NAME	"gpiochip"

struct gpio_device;
struct gpio_desc;

/**
 * struct gpio_device - internal state container for GPIO devices
 * @dev: the GPIO device struct
 * @chrdev: character device for the GPIO device
 * @id: numerical ID number for the GPIO chip
 * @mockdev: class device used by the deprecated sysfs interface (may be
 * NULL)
 * @owner: helps prevent removal of modules exporting active GPIOs
 * @chip: pointer to the corresponding gpiochip, holding static
 * data for this device
 * @descs: array of ngpio descriptors.
 * @ngpio: the number of GPIO lines on this GPIO device, equal to the size
 * of the @descs array.
 * @base: GPIO base in the DEPRECATED global Linux GPIO numberspace, assigned
 * at device creation time.
 * @label: a descriptive name for the GPIO device, such as the part number
 * or name of the IP component in a System on Chip.
 * @data: per-instance data assigned by the driver
 * @list: links gpio_device:s together for traversal
 * @notifier: used to notify subscribers about lines being requested, released
 *            or reconfigured
 * @sem: protects the structure from a NULL-pointer dereference of @chip by
 *       user-space operations when the device gets unregistered during
 *       a hot-unplug event
 * @pin_ranges: range of pins served by the GPIO driver
 *
 * This state container holds most of the runtime variable data
 * for a GPIO device and can hold references and live on after the
 * GPIO chip has been removed, if it is still being used from
 * userspace.
 */
struct gpio_device {
	struct device		dev;
	// struct cdev		chrdev;
	int			id;
	struct device		*mockdev;
	// struct module		*owner;
	struct gpio_chip	*chip;
	// struct gpio_desc	*descs;
	int			base;
	u16			ngpio;
	const char		*label;
	void			*data;
	struct list_head        list;
	// struct blocking_notifier_head notifier;
	// struct rw_semaphore	sem;

#ifdef CONFIG_PINCTRL
	/*
	 * If CONFIG_PINCTRL is enabled, then gpio controllers can optionally
	 * describe the actual pin range which they serve in an SoC. This
	 * information would be used by pinctrl subsystem to configure
	 * corresponding pins for gpio usage.
	 */
	struct list_head pin_ranges;
#endif
};

/**
 * struct gpio_desc - Opaque descriptor for a GPIO
 *
 * @gdev:		Pointer to the parent GPIO device
 * @flags:		Binary descriptor flags
 * @label:		Name of the consumer
 * @name:		Line name
 * @hog:		Pointer to the device node that hogs this line (if any)
 * @debounce_period_us:	Debounce period in microseconds
 *
 * These are obtained using gpiod_get() and are preferable to the old
 * integer-based handles.
 *
 * Contrary to integers, a pointer to a &struct gpio_desc is guaranteed to be
 * valid until the GPIO is released.
 */
struct gpio_desc {
	struct gpio_device	*gdev;
	unsigned long		flags;
/* flag symbols are bit numbers */
#define FLAG_REQUESTED	0
#define FLAG_IS_OUT	1
#define FLAG_EXPORT	2	/* protected by sysfs_lock */
#define FLAG_SYSFS	3	/* exported via /sys/class/gpio/control */
#define FLAG_ACTIVE_LOW	6	/* value has active low */
#define FLAG_OPEN_DRAIN	7	/* Gpio is open drain type */
#define FLAG_OPEN_SOURCE 8	/* Gpio is open source type */
#define FLAG_USED_AS_IRQ 9	/* GPIO is connected to an IRQ */
#define FLAG_IRQ_IS_ENABLED 10	/* GPIO is connected to an enabled IRQ */
#define FLAG_IS_HOGGED	11	/* GPIO is hogged */
#define FLAG_TRANSITORY 12	/* GPIO may lose value in sleep or reset */
#define FLAG_PULL_UP    13	/* GPIO has pull up enabled */
#define FLAG_PULL_DOWN  14	/* GPIO has pull down enabled */
#define FLAG_BIAS_DISABLE    15	/* GPIO has pull disabled */
#define FLAG_EDGE_RISING     16	/* GPIO CDEV detects rising edge events */
#define FLAG_EDGE_FALLING    17	/* GPIO CDEV detects falling edge events */
#define FLAG_EVENT_CLOCK_REALTIME	18 /* GPIO CDEV reports REALTIME timestamps in events */
#define FLAG_EVENT_CLOCK_HTE		19 /* GPIO CDEV reports hardware timestamps in events */

	/* Connection label */
	const char		*label;
	/* Name of the GPIO */
	const char		*name;
#ifdef CONFIG_OF_DYNAMIC
	struct device_node	*hog;
#endif
#ifdef CONFIG_GPIO_CDEV
	/* debounce period in microseconds */
	unsigned int		debounce_period_us;
#endif
};

int gpiod_request(struct gpio_desc *desc, const char *label);
void gpiod_free(struct gpio_desc *desc);

#endif