/*
 ============================================================================
 Name        : BMP180.h
 Author      : Yavuz Murat TAS
 Version     : 1.0.0
 ============================================================================
 */

#ifndef	SYSFS_H
#define	SYSFS_H

#include <linux/device.h>
#include <linux/mutex.h>

typedef int8_t								    	  s8;
typedef int16_t									  s16;
typedef int32_t									  s32;
typedef int64_t									  s64;

typedef uint8_t									  u8;
typedef uint16_t								  u16;
typedef uint32_t								  u32;
typedef uint64_t								  u64;

struct sysfs_node_info
{
	char label[20];
	int my_value;
	struct gpio_desc *desc;
};

/**
  @brief: This struct is used for the driver specific informations.
  */
struct sysfs_data
{
	struct sysfs_node_info *node_info;
	struct mutex lock;
	struct class *gpio_class;
	struct device **dev;
	int total_number;	
};

#endif	/* SYSFS_H	*/
