/*
 * Copyright (c) 2024 NIU Technologies
 * SPDX-License-Identifier: Apache-2.0
 */

#include "display_gc9d01n.h"
#include "display_gc9x01x.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(display_gc9x01x, CONFIG_DISPLAY_LOG_LEVEL);

/*
 * GC9D01N (compatible "galaxycore,gc9d01n") register init sequence.
 * Fixed values are from the BOE NT071BB-C12 panel reference code.
 * Panel: 0.71" round 160x160 TFT, VCI=IOVCC=3.3V.
 *
 * Excluded from this function (handled elsewhere):
 *   INREGEN1/2 (0xFE/0xEF) - sent by gc9x01x_configure() before calling here
 *   COLMOD (0x3A)           - sent by gc9x01x_set_pixel_format()
 *   MADCTL (0x36)           - sent by gc9x01x_set_orientation()
 *   SLPOUT (0x11)           - sent by gc9x01x_exit_sleep()
 *   DISPON (0x29)           - sent by application via blanking_off()
 *   RAMWR  (0x2C)           - sent by gc9x01x_write()
 */

/* 32-byte gate-order mapping register (longest single command in the sequence) */
static const uint8_t gc9d01n_r6e[32] = {
	0x03U, 0x03U, 0x01U, 0x01U, 0x00U, 0x00U, 0x0FU, 0x0FU,
	0x0DU, 0x0DU, 0x0BU, 0x0BU, 0x09U, 0x09U, 0x00U, 0x00U,
	0x00U, 0x00U, 0x0AU, 0x0AU, 0x0CU, 0x0CU, 0x0EU, 0x0EU,
	0x10U, 0x10U, 0x00U, 0x00U, 0x02U, 0x02U, 0x04U, 0x04U,
};

int gc9d01n_regs_init(const struct device *dev)
{
	const struct gc9x01x_config *config = dev->config;
	const struct gc9d01n_regs *regs = config->regs;
	int ret;

	/* Enable all inter-register banks (0x80-0x8F = 0xFF) */
	static const uint8_t ff = 0xFFU;

	for (uint8_t cmd = 0x80U; cmd <= 0x8FU; cmd++) {
		ret = gc9x01x_transmit(dev, cmd, &ff, 1U);
		if (ret < 0) {
			return ret;
		}
	}

	/* Fixed display configuration registers */
	static const uint8_t ec[]  = {0x01U};
	static const uint8_t r74[] = {0x02U, 0x0EU, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U};
	static const uint8_t r98[] = {0x3EU};
	static const uint8_t r99[] = {0x3EU};
	static const uint8_t b5[]  = {0x0DU, 0x0DU};
	static const uint8_t r60[] = {0x38U, 0x0FU, 0x79U, 0x67U};
	static const uint8_t r61[] = {0x38U, 0x11U, 0x79U, 0x67U};
	static const uint8_t r64[] = {0x38U, 0x17U, 0x71U, 0x5FU, 0x79U, 0x67U};
	static const uint8_t r65[] = {0x38U, 0x13U, 0x71U, 0x5BU, 0x79U, 0x67U};
	static const uint8_t r6a[] = {0x00U, 0x00U};
	static const uint8_t r6c[] = {0x22U, 0x02U, 0x22U, 0x02U, 0x22U, 0x22U, 0x50U};
	static const uint8_t bf[]  = {0x01U};
	static const uint8_t f9[]  = {0x40U};
	static const uint8_t r9b[] = {0x3BU};
	static const uint8_t r93[] = {0x33U, 0x7FU, 0x00U};
	static const uint8_t r7e[] = {0x30U};
	static const uint8_t r70[] = {0x0DU, 0x02U, 0x08U, 0x0DU, 0x02U, 0x08U};
	static const uint8_t r71[] = {0x0DU, 0x02U, 0x08U};
	static const uint8_t r91[] = {0x0EU, 0x09U};

	ret = gc9x01x_transmit(dev, 0xECU, ec,  sizeof(ec));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x74U, r74, sizeof(r74));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x98U, r98, sizeof(r98));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x99U, r99, sizeof(r99));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0xB5U, b5,  sizeof(b5));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x60U, r60, sizeof(r60));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x61U, r61, sizeof(r61));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x64U, r64, sizeof(r64));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x65U, r65, sizeof(r65));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x6AU, r6a, sizeof(r6a));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x6CU, r6c, sizeof(r6c));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x6EU, gc9d01n_r6e, sizeof(gc9d01n_r6e));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0xBFU, bf,  sizeof(bf));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0xF9U, f9,  sizeof(f9));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x9BU, r9b, sizeof(r9b));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x93U, r93, sizeof(r93));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x7EU, r7e, sizeof(r7e));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x70U, r70, sizeof(r70));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x71U, r71, sizeof(r71));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, 0x91U, r91, sizeof(r91));
	if (ret < 0) {
		return ret;
	}

	/* DT-configurable power control registers */
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

	/*
	 * DT-configurable gamma registers.
	 * Send order F0, F2, F1, F3 matches the panel manufacturer reference sequence.
	 */
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_GAMMA1, regs->gamma1, sizeof(regs->gamma1));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_GAMMA3, regs->gamma3, sizeof(regs->gamma3));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_GAMMA2, regs->gamma2, sizeof(regs->gamma2));
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_GAMMA4, regs->gamma4, sizeof(regs->gamma4));
	if (ret < 0) {
		return ret;
	}

	return 0;
}
