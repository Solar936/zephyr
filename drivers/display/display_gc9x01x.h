/**
 * Copyright (c) 2023 Mr Beam Lasers GmbH.
 * Copyright (c) 2023 Amrith Venkat Kesavamoorthi <amrith@mr-beam.org>
 * Copyright (c) 2023 Martin Kiepfer <mrmarteng@teleschirm.org>
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_DRIVERS_DISPLAY_GC9X01X_H_
#define ZEPHYR_DRIVERS_DISPLAY_GC9X01X_H_

#include <zephyr/drivers/display.h>
#include <zephyr/drivers/mipi_dbi.h>
#include <zephyr/pm/device.h>
#include <zephyr/sys/util.h>

/* Standard command registers */
#define GC9X01X_CMD_SLPIN    0x10U /* Enter Sleep Mode */
#define GC9X01X_CMD_SLPOUT   0x11U /* Exit Sleep Mode */
#define GC9X01X_CMD_PTLON    0x12U /* Partial Mode ON */
#define GC9X01X_CMD_NORON    0x13U /* Normal Display Mode ON */
#define GC9X01X_CMD_INVOFF   0x20U /* Display Inversion OFF */
#define GC9X01X_CMD_INVON    0x21U /* Display Inversion ON */
#define GC9X01X_CMD_DISPOFF  0x28U /* Display OFF */
#define GC9X01X_CMD_DISPON   0x29U /* Display ON */
#define GC9X01X_CMD_COLSET   0x2AU /* Column Address Set */
#define GC9X01X_CMD_ROWSET   0x2BU /* Row Address Set */
#define GC9X01X_CMD_MEMWR    0x2CU /* Memory Write */
#define GC9X01X_CMD_PTLAR    0x30U /* Partial Area */
#define GC9X01X_CMD_VSCRDEF  0x33U /* Vertical Scrolling Definition */
#define GC9X01X_CMD_TEOFF    0x34U /* Tearing Effect Line OFF */
#define GC9X01X_CMD_TEON     0x35U /* Tearing Effect Line ON */
#define GC9X01X_CMD_MADCTL   0x36U /* Memory Access Control */
#define GC9X01X_CMD_VSCRSADD 0x37U /* Vertical Scrolling Start Address */
#define GC9X01X_CMD_PIXFMT   0x3AU /* Pixel Format Set */
#define GC9X01X_CMD_DFUNCTR  0xB6U /* Display Function Control */
#define GC9X01X_CMD_PWRCTRL1 0xC1U /* Power Control 1 */
#define GC9X01X_CMD_PWRCTRL2 0xC3U /* Power Control 2 */
#define GC9X01X_CMD_PWRCTRL3 0xC4U /* Power Control 3 */
#define GC9X01X_CMD_PWRCTRL4 0xC9U /* Power Control 4 */
#define GC9X01X_CMD_READID1  0xDAU /* Read ID 1 */
#define GC9X01X_CMD_READID2  0xDBU /* Read ID 2 */
#define GC9X01X_CMD_READID3  0xDCU /* Read ID 3 */
#define GC9X01X_CMD_GAMMA1   0xF0U /* Gamma1 (negative polarity) */
#define GC9X01X_CMD_GAMMA2   0xF1U /* Gamma2 */
#define GC9X01X_CMD_GAMMA3   0xF2U /* Gamma3 (positive polarity) */
#define GC9X01X_CMD_GAMMA4   0xF3U /* Gamma4 */
#define GC9X01X_CMD_INREGEN1 0xFEU /* Inter Register Enable 1 */
#define GC9X01X_CMD_INREGEN2 0xEFU /* Inter Register Enable 2 */
#define GC9X01X_CMD_FRAMERATE 0xE8U /* Frame Rate Control */

/* GC9X01X_CMD_MADCTL register fields */
#define GC9X01X_MADCTL_VAL_MY  BIT(7U)
#define GC9X01X_MADCTL_VAL_MX  BIT(6U)
#define GC9X01X_MADCTL_VAL_MV  BIT(5U)
#define GC9X01X_MADCTL_VAL_ML  BIT(4U)
#define GC9X01X_MADCTL_VAL_BGR BIT(3U)
#define GC9X01X_MADCTL_VAL_MH  BIT(2U)

/* GC9X01X_CMD_PIXFMT register fields */
#define GC9X01X_PIXFMT_VAL_RGB_18_BIT 0x60U
#define GC9X01X_PIXFMT_VAL_RGB_16_BIT 0x50U
#define GC9X01X_PIXFMT_VAL_MCU_18_BIT 0x06U
#define GC9X01X_PIXFMT_VAL_MCU_16_BIT 0x05U

/* Sleep in/out duration in ms (see datasheet 6.2.3 and 6.4.2) */
#define GC9X01X_SLEEP_IN_OUT_DURATION_MS 120

/** Display runtime data */
struct gc9x01x_data {
	uint8_t bytes_per_pixel;
	enum display_pixel_format pixel_format;
	enum display_orientation orientation;
};

/** Display static configuration */
struct gc9x01x_config {
	const struct device *mipi_dev;
	struct mipi_dbi_config dbi_config;
	uint8_t pixel_format;
	uint16_t orientation;
	uint16_t x_resolution;
	uint16_t y_resolution;
	bool inversion;
	bool te_before_display_config;
	uint8_t madctl_flags; /* base MADCTL value (model-specific color order bits) */
	const void *regs;
	int (*regs_init_fn)(const struct device *dev);
};

int gc9x01x_transmit(const struct device *dev, uint8_t cmd, const void *tx_data, size_t tx_len);

#endif /* ZEPHYR_DRIVERS_DISPLAY_GC9X01X_H_ */
