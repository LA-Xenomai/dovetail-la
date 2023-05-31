#include <linux/delay.h>
#include <linux/i2c-mux.h>
#include <linux/i2c.h>
#include <linux/gpio/driver.h>
#include <linux/gpio/consumer.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_bridge.h>
#include <drm/drm_drv.h>
#include <drm/drm_edid.h>
#include <drm/drm_print.h>
#include <drm/drm_probe_helper.h>

typedef struct ms1824_cfg_s {
	uint16_t reg;
	uint8_t val;
} ms1824_cfg_t;

#include "ms1824_cfg_480x272.c"

static int ms1824_reset(struct device* dev)
{
	struct gpio_desc* reset_gpio;
	reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	gpiod_direction_output_raw(reset_gpio, 1);
	gpiod_set_value(reset_gpio, 0);
	udelay(1100);
	gpiod_set_value(reset_gpio, 1);
	udelay(1000);
	devm_gpiod_put(dev, reset_gpio);
	return 0;
}

static int ms1824_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	int i = 0;
	uint8_t buf[3];
	uint8_t val;

	ms1824_reset(dev);

	for (i = 0; i < ARRAY_SIZE(MS1824_CFG); i++)
	{
		buf[0] = MS1824_CFG[i].reg & 0xff;
		buf[1] = MS1824_CFG[i].reg >> 8;
		buf[2] = MS1824_CFG[i].val;
		i2c_master_send(client, buf, 3);
//		i2c_master_send(client, buf, 2);
//		i2c_master_recv(client, &val, 1);
//		if(val != MS1824_CFG[i].val)
//		{
//			dev_info(dev, "0x%04x: 0x%02x<-->0x%02x",
//				MS1824_CFG[i].reg, MS1824_CFG[i].val, val);
//		}
	}

	dev_info(dev, "Probe MS1824 FINISH!");

	return 0;
}
static const struct of_device_id ms1824_dt_ids[] = {
	{ .compatible = "macrosilicon,ms1824", },
	{ }
};
MODULE_DEVICE_TABLE(of, ms1824_dt_ids);

static struct i2c_driver ms1824_driver = {
	.probe = ms1824_probe,
	.driver = {
		.name = "ms1824",
		.of_match_table = ms1824_dt_ids,
	},
};

module_i2c_driver(ms1824_driver);

MODULE_AUTHOR("Yize Niu <niuyize@loongson.cn>");
MODULE_DESCRIPTION("VGA -> LCD");
MODULE_LICENSE("GPL");
