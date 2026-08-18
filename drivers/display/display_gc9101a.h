/*
 * Copyright (c) 2023 Mr Beam Lasers GmbH.
 * Copyright (c) 2023 Amrith Venkat Kesavamoorthi <amrith@mr-beam.org>
 * Copyright (c) 2023 Martin Kiepfer <mrmarteng@teleschirm.org>
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9101A_H_
#define ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9101A_H_

#include <zephyr/device.h>
#include "display_gc9x01x.h"

/* GC9101A uses BGR color order natively */
#define GC9101a_MADCTL_BASE GC9X01X_MADCTL_VAL_BGR
#define GC9101a_TE_BEFORE_DISPLAY_CONFIG 0U

/* Register data lengths */
#define GC9101A_PWRCTRL2_LEN  1U
#define GC9101A_PWRCTRL3_LEN  1U
#define GC9101A_PWRCTRL4_LEN  1U
#define GC9101A_GAMMA1_LEN    6U
#define GC9101A_GAMMA2_LEN    6U
#define GC9101A_GAMMA3_LEN    6U
#define GC9101A_GAMMA4_LEN    6U
#define GC9101A_FRAMERATE_LEN 1U

/** GC9101A DT-configurable registers */
struct gc9101a_regs {
	uint8_t pwrctrl2[GC9101A_PWRCTRL2_LEN];
	uint8_t pwrctrl3[GC9101A_PWRCTRL3_LEN];
	uint8_t pwrctrl4[GC9101A_PWRCTRL4_LEN];
	uint8_t gamma1[GC9101A_GAMMA1_LEN];
	uint8_t gamma2[GC9101A_GAMMA2_LEN];
	uint8_t gamma3[GC9101A_GAMMA3_LEN];
	uint8_t gamma4[GC9101A_GAMMA4_LEN];
	uint8_t framerate[GC9101A_FRAMERATE_LEN];
};

/**
 * GC9101a_REGS_INIT - read DT properties into a gc9101a_regs struct named
 * gc9101a_regs_##n.
 */
#define GC9101a_REGS_INIT(n)                                                              \
	static const struct gc9101a_regs gc9101a_regs_##n = {                             \
		.pwrctrl2 = DT_PROP(DT_INST(n, galaxycore_gc9101a), pwrctrl2),            \
		.pwrctrl3 = DT_PROP(DT_INST(n, galaxycore_gc9101a), pwrctrl3),            \
		.pwrctrl4 = DT_PROP(DT_INST(n, galaxycore_gc9101a), pwrctrl4),            \
		.gamma1   = DT_PROP(DT_INST(n, galaxycore_gc9101a), gamma1),              \
		.gamma2   = DT_PROP(DT_INST(n, galaxycore_gc9101a), gamma2),              \
		.gamma3   = DT_PROP(DT_INST(n, galaxycore_gc9101a), gamma3),              \
		.gamma4   = DT_PROP(DT_INST(n, galaxycore_gc9101a), gamma4),              \
		.framerate = DT_PROP(DT_INST(n, galaxycore_gc9101a), framerate),          \
	}

int gc9101a_regs_init(const struct device *dev);

#endif /* ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9101A_H_ */
