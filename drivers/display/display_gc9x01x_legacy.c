/*
 * Copyright (c) 2023 Mr Beam Lasers GmbH.
 * Copyright (c) 2023 Amrith Venkat Kesavamoorthi <amrith@mr-beam.org>
 * Copyright (c) 2023 Martin Kiepfer <mrmarteng@teleschirm.org>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "display_gc9x01x_legacy.h"
#include "display_gc9x01x.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(display_gc9x01x, CONFIG_DISPLAY_LOG_LEVEL);

/* Legacy GC9X01X family default initialization sequence. */
#define GC9X01X_DEFAULT_INIT_MAX_DATA 12U

struct gc9x01x_default_init_regs {
	uint8_t cmd;
	uint8_t len;
	uint8_t data[GC9X01X_DEFAULT_INIT_MAX_DATA];
};

static const struct gc9x01x_default_init_regs gc9x01x_default_init_regs[] = {
	{0xEBU, 1U, {0x14U}},
	{0x84U, 1U, {0x40U}},
	{0x85U, 1U, {0xFFU}},
	{0x86U, 1U, {0xFFU}},
	{0x87U, 1U, {0xFFU}},
	{0x88U, 1U, {0x0AU}},
	{0x89U, 1U, {0x21U}},
	{0x8AU, 1U, {0x00U}},
	{0x8BU, 1U, {0x80U}},
	{0x8CU, 1U, {0x01U}},
	{0x8DU, 1U, {0x01U}},
	{0x8EU, 1U, {0xFFU}},
	{0x8FU, 1U, {0xFFU}},
	{0xB6U, 2U, {0x00U, 0x20U}},
	{0x90U, 4U, {0x08U, 0x08U, 0x08U, 0x08U}},
	{0xBDU, 1U, {0x06U}},
	{0xBCU, 1U, {0x00U}},
	{0xFFU, 3U, {0x60U, 0x01U, 0x04U}},
	{0xBEU, 1U, {0x11U}},
	{0xE1U, 2U, {0x10U, 0x0EU}},
	{0xDFU, 3U, {0x21U, 0x0CU, 0x02U}},
	{0xEDU, 2U, {0x1BU, 0x0BU}},
	{0xAEU, 1U, {0x77U}},
	{0xCDU, 1U, {0x63U}},
	{0x70U, 9U, {0x07U, 0x07U, 0x04U, 0x0EU, 0x0FU, 0x09U, 0x07U, 0x08U, 0x03U}},
	{0x62U, 12U, {0x18U, 0x0DU, 0x71U, 0xEDU, 0x70U, 0x70U,
		      0x18U, 0x0FU, 0x71U, 0xEFU, 0x70U, 0x70U}},
	{0x63U, 12U, {0x18U, 0x11U, 0x71U, 0xF1U, 0x70U, 0x70U,
		      0x18U, 0x13U, 0x71U, 0xF3U, 0x70U, 0x70U}},
	{0x64U, 7U, {0x28U, 0x29U, 0xF1U, 0x01U, 0xF1U, 0x00U, 0x07U}},
	{0x66U, 10U, {0x3CU, 0x00U, 0xCDU, 0x67U, 0x45U, 0x45U, 0x10U, 0x00U, 0x00U, 0x00U}},
	{0x67U, 10U, {0x00U, 0x3CU, 0x00U, 0x00U, 0x00U, 0x01U, 0x54U, 0x10U, 0x32U, 0x98U}},
	{0x74U, 7U, {0x10U, 0x85U, 0x80U, 0x00U, 0x00U, 0x4EU, 0x00U}},
	{0x98U, 2U, {0x3EU, 0x07U}},
};

int gc9x01x_regs_init(const struct device *dev)
{
	const struct gc9x01x_config *config = dev->config;
	const struct gc9x01x_regs *regs = config->regs;
	int ret;

	for (int i = 0; i < ARRAY_SIZE(gc9x01x_default_init_regs); i++) {
		ret = gc9x01x_transmit(dev, gc9x01x_default_init_regs[i].cmd,
				       gc9x01x_default_init_regs[i].data,
				       gc9x01x_default_init_regs[i].len);
		if (ret < 0) {
			return ret;
		}
	}

	ret = gc9x01x_transmit(dev, GC9X01X_CMD_PWRCTRL2, regs->pwrctrl2, sizeof(regs->pwrctrl2));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_PWRCTRL3, regs->pwrctrl3, sizeof(regs->pwrctrl3));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_PWRCTRL4, regs->pwrctrl4, sizeof(regs->pwrctrl4));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_GAMMA1, regs->gamma1, sizeof(regs->gamma1));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_GAMMA2, regs->gamma2, sizeof(regs->gamma2));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_GAMMA3, regs->gamma3, sizeof(regs->gamma3));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_GAMMA4, regs->gamma4, sizeof(regs->gamma4));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_FRAMERATE, regs->framerate,
			       sizeof(regs->framerate));
	if (ret < 0) {
		return ret;
	}

	return 0;
}
