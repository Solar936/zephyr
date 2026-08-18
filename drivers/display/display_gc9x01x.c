/**
 * Copyright (c) 2023 Mr Beam Lasers GmbH.
 * Copyright (c) 2023 Amrith Venkat Kesavamoorthi <amrith@mr-beam.org>
 * Copyright (c) 2023 Martin Kiepfer <mrmarteng@teleschirm.org>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "display_gc9x01x.h"

#include <zephyr/kernel.h>
#include <zephyr/dt-bindings/display/panel.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/mipi_dbi.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(display_gc9x01x, CONFIG_DISPLAY_LOG_LEVEL);

int gc9x01x_transmit(const struct device *dev, uint8_t cmd, const void *tx_data, size_t tx_len)
{
	const struct gc9x01x_config *config = dev->config;

	return mipi_dbi_command_write(config->mipi_dev, &config->dbi_config, cmd, tx_data, tx_len);
}

static int gc9x01x_exit_sleep(const struct device *dev)
{
	int ret;

	ret = gc9x01x_transmit(dev, GC9X01X_CMD_SLPOUT, NULL, 0);
	if (ret < 0) {
		return ret;
	}
	/* Extra 30ms margin over spec for manufacturing tolerance */
	k_msleep(GC9X01X_SLEEP_IN_OUT_DURATION_MS + 30);

	return 0;
}

#ifdef CONFIG_PM_DEVICE
static int gc9x01x_enter_sleep(const struct device *dev)
{
	int ret;

	ret = gc9x01x_transmit(dev, GC9X01X_CMD_SLPIN, NULL, 0);
	if (ret < 0) {
		return ret;
	}
	k_msleep(GC9X01X_SLEEP_IN_OUT_DURATION_MS + 30);

	return 0;
}
#endif /* CONFIG_PM_DEVICE */

static int gc9x01x_hw_reset(const struct device *dev)
{
	const struct gc9x01x_config *config = dev->config;
	int ret;

	ret = mipi_dbi_reset(config->mipi_dev, 100);
	if (ret < 0) {
		return ret;
	}
	k_msleep(10);

	return 0;
}

static int gc9x01x_display_blanking_off(const struct device *dev)
{
	LOG_DBG("Turning display blanking off");
	return gc9x01x_transmit(dev, GC9X01X_CMD_DISPON, NULL, 0);
}

static int gc9x01x_display_blanking_on(const struct device *dev)
{
	LOG_DBG("Turning display blanking on");
	return gc9x01x_transmit(dev, GC9X01X_CMD_DISPOFF, NULL, 0);
}

static int gc9x01x_set_pixel_format(const struct device *dev,
				    const enum display_pixel_format pixel_format)
{
	struct gc9x01x_data *data = dev->data;
	int ret;
	uint8_t tx_data;
	uint8_t bytes_per_pixel;

	if (pixel_format == PIXEL_FORMAT_RGB_565 || pixel_format == PIXEL_FORMAT_RGB_565X) {
		bytes_per_pixel = 2U;
		tx_data = GC9X01X_PIXFMT_VAL_MCU_16_BIT | GC9X01X_PIXFMT_VAL_RGB_16_BIT;
	} else if (pixel_format == PIXEL_FORMAT_RGB_888) {
		bytes_per_pixel = 3U;
		tx_data = GC9X01X_PIXFMT_VAL_MCU_18_BIT | GC9X01X_PIXFMT_VAL_RGB_18_BIT;
	} else {
		LOG_ERR("Unsupported pixel format");
		return -ENOTSUP;
	}

	ret = gc9x01x_transmit(dev, GC9X01X_CMD_PIXFMT, &tx_data, 1U);
	if (ret < 0) {
		return ret;
	}

	data->pixel_format = pixel_format;
	data->bytes_per_pixel = bytes_per_pixel;

	return 0;
}

static int gc9x01x_set_orientation(const struct device *dev,
				   const enum display_orientation orientation)
{
	const struct gc9x01x_config *config = dev->config;
	struct gc9x01x_data *data = dev->data;
	int ret;
	uint8_t tx_data = config->madctl_flags;

	if (orientation == DISPLAY_ORIENTATION_NORMAL) {
		/* no additional bits */
	} else if (orientation == DISPLAY_ORIENTATION_ROTATED_90) {
		tx_data |= GC9X01X_MADCTL_VAL_MV | GC9X01X_MADCTL_VAL_MY;
	} else if (orientation == DISPLAY_ORIENTATION_ROTATED_180) {
		tx_data |= GC9X01X_MADCTL_VAL_MY | GC9X01X_MADCTL_VAL_MX | GC9X01X_MADCTL_VAL_MH;
	} else if (orientation == DISPLAY_ORIENTATION_ROTATED_270) {
		tx_data |= GC9X01X_MADCTL_VAL_MV | GC9X01X_MADCTL_VAL_MX;
	}

	ret = gc9x01x_transmit(dev, GC9X01X_CMD_MADCTL, &tx_data, 1U);
	if (ret < 0) {
		return ret;
	}

	data->orientation = orientation;

	return 0;
}

static int gc9x01x_set_mem_area(const struct device *dev, const uint16_t x, const uint16_t y,
				const uint16_t w, const uint16_t h)
{
	int ret;
	uint16_t spi_data[2];

	spi_data[0] = sys_cpu_to_be16(x);
	spi_data[1] = sys_cpu_to_be16(x + w - 1U);
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_COLSET, &spi_data[0], 4U);
	if (ret < 0) {
		return ret;
	}

	spi_data[0] = sys_cpu_to_be16(y);
	spi_data[1] = sys_cpu_to_be16(y + h - 1U);
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_ROWSET, &spi_data[0], 4U);
	if (ret < 0) {
		return ret;
	}

	return 0;
}

static int gc9x01x_write(const struct device *dev, const uint16_t x, const uint16_t y,
			 const struct display_buffer_descriptor *desc, const void *buf)
{
	const struct gc9x01x_config *config = dev->config;
	struct gc9x01x_data *data = dev->data;
	int ret;
	const uint8_t *write_data_start = (const uint8_t *)buf;
	struct display_buffer_descriptor mipi_desc;
	uint16_t write_cnt;
	uint16_t nbr_of_writes;
	uint16_t write_h;

	__ASSERT(desc->width <= desc->pitch, "Pitch is smaller than width");
	__ASSERT((desc->pitch * data->bytes_per_pixel * desc->height) <= desc->buf_size,
		 "Input buffer too small");

	LOG_DBG("Writing %dx%d (w,h) @ %dx%d (x,y)", desc->width, desc->height, x, y);
	ret = gc9x01x_set_mem_area(dev, x, y, desc->width, desc->height);
	if (ret < 0) {
		return ret;
	}

	if (desc->pitch > desc->width) {
		write_h = 1U;
		nbr_of_writes = desc->height;
		mipi_desc.height = 1;
		mipi_desc.buf_size = desc->width * data->bytes_per_pixel;
	} else {
		write_h = desc->height;
		mipi_desc.height = desc->height;
		mipi_desc.buf_size = desc->width * data->bytes_per_pixel * write_h;
		nbr_of_writes = 1U;
	}

	mipi_desc.width = desc->width;
	/* Per MIPI API, pitch must always match width */
	mipi_desc.pitch = desc->width;
	mipi_desc.frame_incomplete = desc->frame_incomplete;

	ret = gc9x01x_transmit(dev, GC9X01X_CMD_MEMWR, NULL, 0);
	if (ret < 0) {
		return ret;
	}

	for (write_cnt = 0U; write_cnt < nbr_of_writes; ++write_cnt) {
		ret = mipi_dbi_write_display(config->mipi_dev, &config->dbi_config,
					     write_data_start, &mipi_desc, data->pixel_format);
		if (ret < 0) {
			return ret;
		}
		write_data_start += desc->pitch * data->bytes_per_pixel;
	}

	return 0;
}

static void gc9x01x_get_capabilities(const struct device *dev,
				     struct display_capabilities *capabilities)
{
	struct gc9x01x_data *data = dev->data;
	const struct gc9x01x_config *config = dev->config;

	memset(capabilities, 0, sizeof(struct display_capabilities));

	capabilities->supported_pixel_formats = PIXEL_FORMAT_RGB_565 | PIXEL_FORMAT_RGB_565X | PIXEL_FORMAT_RGB_888;
	capabilities->current_pixel_format = data->pixel_format;

	if (data->orientation == DISPLAY_ORIENTATION_NORMAL ||
	    data->orientation == DISPLAY_ORIENTATION_ROTATED_180) {
		capabilities->x_resolution = config->x_resolution;
		capabilities->y_resolution = config->y_resolution;
	} else {
		capabilities->x_resolution = config->y_resolution;
		capabilities->y_resolution = config->x_resolution;
	}

	capabilities->current_orientation = data->orientation;
}

static int gc9x01x_configure(const struct device *dev)
{
	const struct gc9x01x_config *config = dev->config;
	int ret;

	/* Enable extended register access, common to all GC9X01X family members */
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_INREGEN1, NULL, 0);
	if (ret < 0) {
		return ret;
	}
	ret = gc9x01x_transmit(dev, GC9X01X_CMD_INREGEN2, NULL, 0);
	if (ret < 0) {
		return ret;
	}

	/* Sub-model specific register initialization */
	ret = config->regs_init_fn(dev);
	if (ret < 0) {
		return ret;
	}

	if (config->te_before_display_config) {
		ret = gc9x01x_transmit(dev, GC9X01X_CMD_TEON, NULL, 0);
		if (ret < 0) {
			return ret;
		}
	}

	/* Pixel format */
	ret = gc9x01x_set_pixel_format(dev, config->pixel_format);
	if (ret < 0) {
		return ret;
	}

	/* Orientation */
	ret = gc9x01x_set_orientation(dev, config->orientation);
	if (ret < 0) {
		return ret;
	}

	/* Display inversion */
	if (config->inversion) {
		ret = gc9x01x_transmit(dev, GC9X01X_CMD_INVON, NULL, 0);
		if (ret < 0) {
			return ret;
		}
	}

	/* Enable tearing effect line after display configuration for newer models. */
	if (!config->te_before_display_config) {
		ret = gc9x01x_transmit(dev, GC9X01X_CMD_TEON, NULL, 0);
		if (ret < 0) {
			return ret;
		}
	}

	return 0;
}

static int gc9x01x_init(const struct device *dev)
{
	int ret;

	if (!device_is_ready(((const struct gc9x01x_config *)dev->config)->mipi_dev)) {
		LOG_ERR("MIPI DBI device is not ready");
		return -ENODEV;
	}

	ret = gc9x01x_hw_reset(dev);
	if (ret < 0) {
		LOG_ERR("Could not reset display (%d)", ret);
		return ret;
	}

	gc9x01x_display_blanking_on(dev);

	ret = gc9x01x_configure(dev);
	if (ret < 0) {
		LOG_ERR("Could not configure display (%d)", ret);
		return ret;
	}

	ret = gc9x01x_exit_sleep(dev);
	if (ret < 0) {
		LOG_ERR("Could not exit sleep mode (%d)", ret);
		return ret;
	}

	return 0;
}

#ifdef CONFIG_PM_DEVICE
static int gc9x01x_pm_action(const struct device *dev, enum pm_device_action action)
{
	int ret;

	switch (action) {
	case PM_DEVICE_ACTION_RESUME:
		ret = gc9x01x_exit_sleep(dev);
		break;
	case PM_DEVICE_ACTION_SUSPEND:
		ret = gc9x01x_enter_sleep(dev);
		break;
	default:
		ret = -ENOTSUP;
		break;
	}

	return ret;
}
#endif /* CONFIG_PM_DEVICE */

static DEVICE_API(display, gc9x01x_api) = {
	.blanking_on = gc9x01x_display_blanking_on,
	.blanking_off = gc9x01x_display_blanking_off,
	.write = gc9x01x_write,
	.get_capabilities = gc9x01x_get_capabilities,
	.set_pixel_format = gc9x01x_set_pixel_format,
	.set_orientation = gc9x01x_set_orientation,
};

#define INST_DT_GC9X01X(n, t) DT_INST(n, galaxycore_gc##t)

#define GC9X01X_INIT(n, t)                                                                  \
	GC##t##_REGS_INIT(n);                                                               \
                                                                                            \
	static const struct gc9x01x_config gc##t##_config_##n = {                           \
		.mipi_dev = DEVICE_DT_GET(DT_PARENT(INST_DT_GC9X01X(n, t))),                \
		.dbi_config = {                                                             \
			.mode = MIPI_DBI_MODE_SPI_4WIRE,                                    \
			.config = MIPI_DBI_SPI_CONFIG_DT(INST_DT_GC9X01X(n, t),             \
							 SPI_OP_MODE_MASTER |           \
							 SPI_WORD_SET(8), 0),           \
		},                                                                          \
		.pixel_format = DT_PROP(INST_DT_GC9X01X(n, t), pixel_format),                \
		.orientation = DT_ENUM_IDX(INST_DT_GC9X01X(n, t), orientation),              \
		.x_resolution = DT_PROP(INST_DT_GC9X01X(n, t), width),                       \
		.y_resolution = DT_PROP(INST_DT_GC9X01X(n, t), height),                      \
		.inversion = DT_PROP(INST_DT_GC9X01X(n, t), display_inversion),              \
		.te_before_display_config = GC##t##_TE_BEFORE_DISPLAY_CONFIG,                 \
		.madctl_flags = GC##t##_MADCTL_BASE,                                        \
		.regs = &gc##t##_regs_##n,                                                  \
		.regs_init_fn = gc##t##_regs_init,                                          \
	};                                                                                  \
	static struct gc9x01x_data gc##t##_data_##n;                                        \
                                                                                            \
	PM_DEVICE_DT_DEFINE(INST_DT_GC9X01X(n, t), gc9x01x_pm_action);                     \
	DEVICE_DT_DEFINE(INST_DT_GC9X01X(n, t), gc9x01x_init,                               \
			 PM_DEVICE_DT_GET(INST_DT_GC9X01X(n, t)),                          \
			 &gc##t##_data_##n, &gc##t##_config_##n,                          \
			 POST_KERNEL, CONFIG_DISPLAY_INIT_PRIORITY, &gc9x01x_api)

#define DT_INST_FOREACH_GC9X01X_STATUS_OKAY(t) \
	LISTIFY(DT_NUM_INST_STATUS_OKAY(galaxycore_gc##t), GC9X01X_INIT, (;), t)

#ifdef CONFIG_GC9101A
#include "display_gc9101a.h"
DT_INST_FOREACH_GC9X01X_STATUS_OKAY(9101a);
#endif

#ifdef CONFIG_GC9D01N
#include "display_gc9d01n.h"
DT_INST_FOREACH_GC9X01X_STATUS_OKAY(9d01n);
#endif

#ifdef CONFIG_GC9X01X
#include "display_gc9x01x_legacy.h"
DT_INST_FOREACH_GC9X01X_STATUS_OKAY(9x01x);
#endif
