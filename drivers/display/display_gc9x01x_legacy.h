/*
 * Copyright (c) 2023 Mr Beam Lasers GmbH.
 * Copyright (c) 2023 Amrith Venkat Kesavamoorthi <amrith@mr-beam.org>
 * Copyright (c) 2023 Martin Kiepfer <mrmarteng@teleschirm.org>
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9X01X_LEGACY_H_
#define ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9X01X_LEGACY_H_

#include <zephyr/device.h>

#include "display_gc9x01x.h"

#define GC9x01x_MADCTL_BASE GC9X01X_MADCTL_VAL_BGR
#define GC9x01x_TE_BEFORE_DISPLAY_CONFIG 1U

#define GC9X01X_NUM_DEFAULT_INIT_REGS 12U

struct gc9x01x_regs {
	/* Retained for legacy DT compatibility; the original driver did not send it. */
	uint8_t pwrctrl1[1U];
	uint8_t pwrctrl2[1U];
	uint8_t pwrctrl3[1U];
	uint8_t pwrctrl4[1U];
	uint8_t gamma1[6U];
	uint8_t gamma2[6U];
	uint8_t gamma3[6U];
	uint8_t gamma4[6U];
	uint8_t framerate[1U];
};

/*
 * GC9x01x_REGS_INIT - read DT properties into a gc9x01x_regs struct named
 * gc9x01x_regs_##n.
 */
#define GC9x01x_REGS_INIT(n)                                                         \
	static const struct gc9x01x_regs gc9x01x_regs_##n = {                        \
		.pwrctrl1 = DT_PROP(DT_INST(n, galaxycore_gc9x01x), pwrctrl1),      \
		.pwrctrl2 = DT_PROP(DT_INST(n, galaxycore_gc9x01x), pwrctrl2),      \
		.pwrctrl3 = DT_PROP(DT_INST(n, galaxycore_gc9x01x), pwrctrl3),      \
		.pwrctrl4 = DT_PROP(DT_INST(n, galaxycore_gc9x01x), pwrctrl4),      \
		.gamma1 = DT_PROP(DT_INST(n, galaxycore_gc9x01x), gamma1),          \
		.gamma2 = DT_PROP(DT_INST(n, galaxycore_gc9x01x), gamma2),          \
		.gamma3 = DT_PROP(DT_INST(n, galaxycore_gc9x01x), gamma3),          \
		.gamma4 = DT_PROP(DT_INST(n, galaxycore_gc9x01x), gamma4),          \
		.framerate = DT_PROP(DT_INST(n, galaxycore_gc9x01x), framerate),    \
	}

int gc9x01x_regs_init(const struct device *dev);

#endif /* ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9X01X_LEGACY_H_ */
