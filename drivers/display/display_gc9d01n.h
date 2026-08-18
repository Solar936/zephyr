/*
 * Copyright (c) 2024 NIU Technologies
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9D01N_H_
#define ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9D01N_H_

#include <zephyr/device.h>

/* GC9D01N native color order is RGB (no BGR bit needed) */
#define GC9d01n_MADCTL_BASE 0x00U
#define GC9d01n_TE_BEFORE_DISPLAY_CONFIG 0U

/* Register data lengths */
#define GC9D01N_PWRCTRL2_LEN 1U
#define GC9D01N_PWRCTRL3_LEN 1U
#define GC9D01N_PWRCTRL4_LEN 1U
#define GC9D01N_GAMMA1_LEN   6U
#define GC9D01N_GAMMA2_LEN   6U
#define GC9D01N_GAMMA3_LEN   6U
#define GC9D01N_GAMMA4_LEN   6U

/** GC9D01N DT-configurable registers */
struct gc9d01n_regs {
	uint8_t pwrctrl2[GC9D01N_PWRCTRL2_LEN];
	uint8_t pwrctrl3[GC9D01N_PWRCTRL3_LEN];
	uint8_t pwrctrl4[GC9D01N_PWRCTRL4_LEN];
	uint8_t gamma1[GC9D01N_GAMMA1_LEN];
	uint8_t gamma2[GC9D01N_GAMMA2_LEN];
	uint8_t gamma3[GC9D01N_GAMMA3_LEN];
	uint8_t gamma4[GC9D01N_GAMMA4_LEN];
};

/**
 * GC9d01n_REGS_INIT - read DT properties into a gc9d01n_regs struct named
 * gc9d01n_regs_##n.
 */
#define GC9d01n_REGS_INIT(n)                                                              \
	static const struct gc9d01n_regs gc9d01n_regs_##n = {                             \
		.pwrctrl2 = DT_PROP(DT_INST(n, galaxycore_gc9d01n), pwrctrl2),            \
		.pwrctrl3 = DT_PROP(DT_INST(n, galaxycore_gc9d01n), pwrctrl3),            \
		.pwrctrl4 = DT_PROP(DT_INST(n, galaxycore_gc9d01n), pwrctrl4),            \
		.gamma1   = DT_PROP(DT_INST(n, galaxycore_gc9d01n), gamma1),              \
		.gamma2   = DT_PROP(DT_INST(n, galaxycore_gc9d01n), gamma2),              \
		.gamma3   = DT_PROP(DT_INST(n, galaxycore_gc9d01n), gamma3),              \
		.gamma4   = DT_PROP(DT_INST(n, galaxycore_gc9d01n), gamma4),              \
	}

int gc9d01n_regs_init(const struct device *dev);

#endif /* ZEPHYR_DRIVERS_DISPLAY_DISPLAY_GC9D01N_H_ */
