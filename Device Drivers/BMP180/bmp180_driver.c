#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/init.h>

#include "bmp180.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yavuz Murat TAŞ");
MODULE_DESCRIPTION("A Simple Driver for BMP180 Pressure and Temperature Sensor");
MODULE_VERSION("1.0.0");

extern struct bmp180_chip_info bmp180_info;
extern const struct iio_info bmp180_i2c_info;

/* Declate the probe and remove functions */
static int bmp180_probe(struct i2c_client *client);
static void bmp180_remove(struct i2c_client *client);

static struct of_device_id bmp180_of_i2c_match[] =
{
	{
		.compatible = "ymt, bmp180",
		.data = &bmp180_info
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, bmp180_of_i2c_match);

static struct i2c_device_id bmp180_i2c_id[] =
{
	{ "bmp180", (kernel_ulong_t)&bmp180_info },
	{ }
};
MODULE_DEVICE_TABLE(i2c, bmp180_i2c_id);

/**
 * @brief This function is called on loading the driver 
 */
static int bmp180_probe(struct i2c_client *client)
{
	struct iio_dev *indio_dev;
	struct bmp180_data *data;
	struct device* dev = &client->dev;
	const struct bmp180_chip_info *chip_info;
	/* const struct i2c_device_id *id = i2c_client_get_device_id(client); can also be used but there is some error when i used this code instead of below two of them */
	const struct i2c_driver *drv = to_i2c_driver(client->dev.driver);
	const struct i2c_device_id *id = i2c_match_id(drv->id_table, client);
	
	int ret;
	
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA))
	{
		dev_info(&client->dev, "System does not support SMbus block read/write!\n");
		return 0;
	}
	
	dev_info(&client->dev, "Probe function is called\n");
	
	/* .data section of of_device_id of a device node in device tree are acquired */
	chip_info = of_device_get_match_data(dev);
	
	/* If matching by device tree is failed, then id table matching is used to extract the device data. */
	if (!chip_info)
	{
		chip_info = (const struct bmp180_chip_info *) id->driver_data;
	}
	
	/* Memory allocation for iio device is made. */
	indio_dev = devm_iio_device_alloc(dev, sizeof(*data));
	
	if(!indio_dev)
	{
		dev_info(dev, "There is no enough memory to create the device!\n");
		return -ENOMEM;
	}

	data = iio_priv(indio_dev); /* reference to driver's private information **MUST** be accessed **ONLY** via iio_priv() helper */
	
	mutex_init(&data->lock); /* Mutex is initialized to ensure the synchronization and prevent the threads in the same place concurrently. */

	/* Some initializations for chip_info structure and iio device is made below. */
	data->dev = dev;
	data->chip_info = chip_info;

	indio_dev->name = id->name;
	indio_dev->info = &bmp180_i2c_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = chip_info->channels;
	indio_dev->num_channels = 2;

	/* Necessary configurations are made. */
	ret = data->chip_info->preinit(client);
	
	if(ret < 0)
	{
		dev_info(dev, "Device cannot be configured!\n");
		return ret;
	}

	/* indio_dev is saved into device specific information of dev  */
	dev_set_drvdata(dev, indio_dev);
		
	if (data->chip_info->preinit)
	{
		ret = data->chip_info->preinit(client);
		if (ret)
		{
			dev_info(dev,"Initialization is Failed!\n");
			return -1;
		}
	}
	
	/* Chip calibration paramteres are read and saved. */
	if (data->chip_info->read_calib)
	{
		ret = data->chip_info->read_calib(client, data);
		if (ret < 0)
		{
			/* return dev_err_probe(data->dev, ret,"Reading calibration parameters are failed!\n"); can be used as well instead of below two commands. */
			dev_info(dev,"Reading calibration parameters are failed!\n");
			return -1;
		}
	}

	return devm_iio_device_register(dev, indio_dev);
}

/**
 * @brief This function is called on unloading the driver 
 */
static void bmp180_remove(struct i2c_client *client)
{
	dev_info(&client->dev, "Remove function is called\n");
}

static struct i2c_driver bmp180_driver =
{
	.probe_new = bmp180_probe,
	.remove = bmp180_remove,
	.id_table = bmp180_i2c_id,
	
	.driver =
	{
		.name = "bmp180",
		.of_match_table = bmp180_of_i2c_match,
	}
};

/* This will create the init and exit function automatically */
module_i2c_driver(bmp180_driver);
