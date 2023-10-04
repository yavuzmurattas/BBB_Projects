#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/device/class.h>
#include <linux/gpio/consumer.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/kernel.h>

#include "sysfs.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yavuz Murat TAŞ");
MODULE_DESCRIPTION("A Simple Driver for GPIO via Sysfs");
MODULE_VERSION("1.0.0");

struct sysfs_data data;

ssize_t direction_show(struct device *dev, struct device_attribute *attr,char *buf)
{
	int dir;
	char *direction;
	
	struct sysfs_node_info *node_info = dev_get_drvdata(dev);
	
	dir = gpiod_get_direction(node_info->desc);
	
	if(dir < 0)
	{
		dev_info(dev, "Getting direction value in direction attribute has failed!\n");
		return dir;
	}
	
	direction = (dir == 0) ? "out" : "in";
	
	return snprintf(buf, "%s\n", direction);
}

ssize_t direction_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
	int ret;
	
	struct sysfs_node_info *node_info = dev_get_drvdata(dev);

	if(sysfs_streq(buf, "in"))
	{
		ret = gpiod_direction_input(node_info->desc);
	}
	else if (sysfs_streq(buf, "out"))
	{
		ret = gpiod_direction_output(node_info->desc, 0);
	}
	else
	{
		return -EINVAL;
	}
	
	/* Below expression is same as ref ? ref : count */
	return ret ? : count;

}

ssize_t value_show(struct device *dev, struct device_attribute *attr,char *buf)
{
	int value;
	
	struct sysfs_node_info *node_info = dev_get_drvdata(dev);
	
	value = gpiod_get_value(node_info->desc);
	
	return snprintf(buf, "%d\n", value);
}

ssize_t value_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t count)
{
	struct sysfs_node_info *node_info = dev_get_drvdata(dev);
	
	int ret;
	long int value;
	
	ret = kstrtol(buf, 0, &value);
	if(ret)
	{
		return ret;
	}
	
	gpiod_set_value(node_info->desc, value);
	
	return count;
}

ssize_t label_show(struct device *dev, struct device_attribute *attr,char *buf)
{
	struct sysfs_node_info *node_info = dev_get_drvdata(dev);
	return snprintf(buf, "%s\n", node_info->label);
}

static DEVICE_ATTR_RW(direction);
static DEVICE_ATTR_RW(value);
static DEVICE_ATTR_RO(label);

static struct attribute *sysfs_gpio_attrs[] =
{
	&dev_attr_direction.attr,
	&dev_attr_value.attr,
	&dev_attr_label.attr,
	NULL
};

static struct attribute_group sysfs_gpio_attr_grp =
{
	.attrs = sysfs_gpio_attrs
};

static const struct attribute_group *syfs_gpio_attr_grps[] =
{
	&sysfs_gpio_attr_grp,
	NULL		
};

/**
 * @brief This function is called on loading the driver 
 */
static int sysfs_gpio_probe(struct platform_device *client)
{
	struct device *dev = &client->dev;
	int i;
	int ret;
	const char *label;
	int my_value;
	
	struct device_node *parent = client->dev.of_node;
	struct device_node *child = NULL;
	
	struct sysfs_data data;
	
	dev_info(dev, "Probe function is called!\n");
		
	/* Number of childs are obtained with the express of available which means status = "okay" */
	data.total_number = of_get_available_child_count(parent);
	if(data.total_number)
	{
		dev_info(dev, "No device node could be found!\n ");
		return -EINVAL;
	}
	dev_info(dev, "Total number of devices found: %d\n", data.total_number);
	
	data.dev = devm_kzalloc(dev, sizeof(struct device*) * data.total_number, GFP_KERNEL);
	
	for_each_available_child_of_node(parent, child)
	{
		data.node_info = devm_kzalloc(dev, sizeof(struct sysfs_node_info*), GFP_KERNEL);
		if(!data.node_info)
		{
			dev_info(dev, "Memory allocation for node information is failed!\n");
			return -ENOMEM;
		}
		
		if(!of_property_read_string(child, "label", &label))
		{
			dev_info(dev, "Node information 'label' could not be read!\n");
			snprintf(data.node_info->label, sizeof(data.node_info->label), "unkwngpio:%d", i);
		}
		else
		{
			strcpy(data.node_info->label, label);
			dev_info(dev, "The 'label' of %dth node is: %s\n", i, data.node_info->label);
		}
		
		if(!of_property_read_s32(child, "my_value", &my_value))
		{
			dev_info(dev, "Node information 'my_value' could not be read!\n");
			data.node_info->my_value = i;
		}
		else
		{
			data.node_info->my_value = my_value;
			dev_info(dev, "The 'my_value' of %dth node is: %d\n", i, data.node_info->my_value); 
		}
		
		data.node_info->desc = devm_fwnode_get_gpiod_from_child(dev, "bone", &child->fwnode, GPIOD_ASIS, data.node_info->label);
		if(IS_ERR(data.node_info->desc))
		{
			ret = PTR_ERR(data.node_info->desc);
			
			if(ret == -ENOENT)
			{
				dev_info(dev, "There is no GPIO entry in device tree!\n");
			}
			return ret;	
		}
		
		ret = gpiod_direction_output(data.node_info->desc, 0);
		if(ret)
		{
			dev_info(dev, "Setting the direction of GPIO has failed!\n");
			return ret;
		}
		
		data.dev[i] = device_create_with_groups(data.gpio_class, dev, 0, data.node_info, syfs_gpio_attr_grps, data.node_info->label);
		if(IS_ERR(data.dev[i]))
		{
			dev_info(dev, "Device creation has failed!\n");
			return PTR_ERR(data.dev[i]);
		}
		
		i++;	
	}
	
	return 0;
}

/**
 * @brief This function is called on unloading the driver 
 */
static int sysfs_gpio_remove(struct platform_device *client)
{
	int i;
	
	dev_info(&client->dev, "Remove function is called!\n");
	
	for(i = 0; i < data.total_number; i++)
	{
		device_unregister(data.dev[i]);
	}
	
	return 0;
}

static struct of_device_id gpio_sysfs_match[] =
{
	{
		.compatible = "ymt, sysfs"
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, gpio_sysfs_match);


struct platform_driver gpio_sysfs_driver =
{
	.probe = sysfs_gpio_probe,
	.remove = sysfs_gpio_remove,
	.driver ={
		.name = "sysfs_gpio_driver",
		.of_match_table = of_match_ptr(gpio_sysfs_match)
	}
};

int __init gpio_sysfs_init(void)
{
	data.gpio_class = class_create(THIS_MODULE, "sysfs_gpio");
	if(IS_ERR(data.gpio_class))
	{
		pr_info("The class cannot be created!\n");
		return PTR_ERR(data.gpio_class);	
	}
	
	platform_driver_register(&gpio_sysfs_driver);
	pr_info("Class and Driver succesfully created!\n");
	
	return 0;
}

void __exit gpio_sysfs_exit(void)
{
	platform_driver_unregister(&gpio_sysfs_driver);
	class_destroy(data.gpio_class);
	
	pr_info("Class and Driver succesfully Deleted!\n");
}

module_init(gpio_sysfs_init);
module_exit(gpio_sysfs_exit);

