/*
 ============================================================================
 Name        : BMP180.c
 Author      : Yavuz Murat TAS
 Version     : 1.0.0
 ============================================================================
 */
 
#include <linux/module.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/iio/iio.h>
#include <linux/iio/sysfs.h>
#include <linux/iio/types.h>
#include <linux/of.h>
#include <linux/i2c.h>
#include <asm/current.h>
#include <asm/unaligned.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/of_device.h>

#include "bmp180.h"

#define BMP180_INIT_VALUE		0

/* Registers */
#define BMP180_OUT_XLSB_REG		0xF8
#define BMP180_OUT_LSB_REG		0xF7
#define BMP180_OUT_MSB_REG		0xF6
#define BMP180_CTRL_MEAS_REG		0xF4
#define BMP180_SOFT_RESET_REG		0xE0
#define BMP180_ID_REG			0xD0
#define BMP180_CALIB_REG_STRT		0xAA
#define BMP180_CALIB_REG_END		0xBF

/* EEPROM Starting Address */
#define	BMP180_PARAMETER_AC1_MSB	0xAA

static int bmp180_read_raw(struct iio_dev * indio_dev, struct iio_chan_spec const * chan, int *val, int *val2, long mask)
{
	/* Private datas are sent to the this variable. */
	struct bmp180_data *bmp180 = iio_priv(indio_dev);
	struct i2c_client *client = to_i2c_client(bmp180->dev);
	int ret;
	
	mutex_lock(&bmp180->lock);

	if(mask == IIO_CHAN_INFO_PROCESSED)
	{
		switch(chan->type)
		{
			case IIO_TEMP:
				ret = i2c_smbus_read_byte_data(client, BMP180_OUT_XLSB_REG);
				dev_info(bmp180->dev, "The value of the XLSB reg: %d\n", &ret);
				ret = bmp180->chip_info->read_temp(client, bmp180);
				*val = ret;
				
				break;
				
			case IIO_PRESSURE:
				ret = bmp180->chip_info->read_press(client, bmp180);
				*val = ret;
				break;
				
			default:
				dev_info(bmp180->dev, "Wrong data channel!\n");
				return -EINVAL;					
		}
	/* case IIO_CHAN_INFO_OVERSAMPLING_RATIO: KISMI DA YAPILABİLİR. İNCELENMELİ. */
	}		
	
	else
	{
		dev_info(bmp180->dev, "Wrong mask!\n");
		return -EINVAL;
	}

	mutex_unlock(&bmp180->lock);
	
	return IIO_VAL_INT;
	
}

static int bmp180_write_raw(struct iio_dev * indio_dev, struct iio_chan_spec const * chan, int val, int val2, long mask)
{
	/* Private datas are sent to the this variable. */
/*
	struct bmp180_driver *bmp180 = iio_priv(indio_dev);
	int ret;
	
	SONRA YAPILACAK.
	if(mask == IIO_CHAN_INFO_PROCESSED)
	{
		ret = i2c_smbus_write_byte_data(bmp180->client, BMP180_ID_REG, 1);
		
		if(ret < 0)
		{
			pr_info( "Raw value cannot be read.\n" );
			return ret;
		}
		
		*val = ret;
	}
	
	else 
		return -EINVAL;
		
	return IIO_VAL_INT;
*/
	return 0;
}

/* Reading the Calibration Parameters from EEPROM of BMP180	*/
int bmp180_read_calib(struct i2c_client *client, struct bmp180_data *data)
{
	int ret;
	//each axis value is of 2byte, so we need a buffer of 6bytes.
	u8 calib_buffer[22] = { BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
				BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
				BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
				BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
				BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
				BMP180_INIT_VALUE, BMP180_INIT_VALUE };

	//start reading from the base address of accelerometer values i.e MPU6050_REG_ACC_X_HIGH
	ret = i2c_master_recv(client, calib_buffer, 16);
	if(ret < 0)
	{
		dev_info(&client->dev, "Registers for calibration cannot be read in first i2c_master_recv!\n");
		return ret;
	}
	ret = i2c_master_recv(client, &calib_buffer[16], 6);
/*	
	ret = i2c_smbus_read_i2c_block_data(client, BMP180_PARAMETER_AC1_MSB, 23, calib_buffer);
*/	
	if(ret < 0)
	{
		dev_info(&client->dev, "Registers for calibration cannot be read in second i2c_master_recv!\n");
		return ret;
	}

	/* calib_buffer[0]= acc x axis value , calib_buffer[1]= acc y axis value , calib_buffer[2]= acc z axis value  */
	data->bmp180.AC1 =  (s32) ( ( calib_buffer[0]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[1]  );
	data->bmp180.AC2 =  (s32) ( ( calib_buffer[2]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[3]  );
	data->bmp180.AC3 =  (s32) ( ( calib_buffer[4]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[5]  );
	data->bmp180.AC4 =  (u32) ( ( calib_buffer[6]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[7]  );
	data->bmp180.AC5 =  (u32) ( ( calib_buffer[8]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[9]  );
	data->bmp180.AC6 =  (u32) ( ( calib_buffer[10] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[11] );
	data->bmp180.B1  =  (s32) ( ( calib_buffer[12] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[13] );
	data->bmp180.B2  =  (s32) ( ( calib_buffer[14] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[15] );
	data->bmp180.MB  =  (s32) ( ( calib_buffer[16] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[17] );
	data->bmp180.MC  =  (s32) ( ( calib_buffer[18] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[19] );
	data->bmp180.MD  =  (s32) ( ( calib_buffer[20] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[21] );

	return 0;
}

/* Reading and Calculating the Uncompensated Value of Temperature */
s32 bmp180_read_uncompensated_temp(struct i2c_client *client, struct bmp180_data *data)
{
	struct bmp180_parameters bmp180_params = data->bmp180_params;
	int ret;
	s16 f6_f7;
		
	ret = i2c_smbus_write_byte_data(client, BMP180_CTRL_MEAS_REG, 0x2E );
	if(ret < 0)
	{
		dev_info(data->dev, "Writing to the sensor is failed!\n");
		return ret;
	}
	
	usleep_range(4500, 5000);

	s8 f6_f7_reg_val[2];
	f6_f7 = i2c_smbus_read_word_data(client, BMP180_OUT_MSB_REG);
	f6_f7_reg_val[0] = (s8)(f6_f7 >> 1);
	f6_f7_reg_val[1] = (s8)(f6_f7 << 1);

	bmp180_params.UT = (s32)( ( f6_f7_reg_val[0] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) | f6_f7_reg_val[1] );

	return 0;

}

/* Reading and Calculating the Uncompensated Value of Pressure */
s32 bmp180_read_uncompensated_press(struct i2c_client *client, struct bmp180_data *data)
{
	struct bmp180_parameters bmp180_params = data->bmp180_params;
	s8 f6_f7_f8_reg_val[3];
	int ret;
	int i;

	s16 val = ( s16 ) ( 0x34 | ( OSS_VALUE << BMP180_SHIFT_BIT_POSITION_BY_06_BITS ) );

	ret = i2c_smbus_write_byte_data(client, BMP180_CTRL_MEAS_REG, val );
	
	if(ret < 0)
	{
		dev_info(data->dev, "Writing to the sensor is failed!\n");
		return ret;
	}

	#if 	OSS_VALUE == 0
		usleep_range(4500, 5000);

	#elif	OSS_VALUE == 1
		usleep_range(7500, 10000);

	#elif 	OSS_VALUE == 2
		usleep_range(13500, 17000);

	#elif 	OSS_VALUE == 3
		usleep_range(25500, 30000);

	#warning OSS Value is wrong

	#endif
	for(i = 0; i < 3; i++)
	{
	f6_f7_f8_reg_val[i] = i2c_smbus_read_byte_data(client, BMP180_OUT_MSB_REG+i);
	}
	
	if(ret < 0)
    	{
    		dev_info(data->dev, "Registers for calibration cannot be read!\n");
    		return ret;
    	}

	bmp180_params.UP = (s32)( (s32)(( f6_f7_f8_reg_val[0] << BMP180_SHIFT_BIT_POSITION_BY_16_BITS ) | ( f6_f7_f8_reg_val[1] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) | f6_f7_f8_reg_val[2] ) >> ( 8 - OSS_VALUE ) );
	
	return 0;
}

/* Calculating the Value of Temperature */
s32 bmp180_read_temp(struct i2c_client *client, struct bmp180_data *data)
{
	struct bmp180_calib bmp180 = data->bmp180;
	struct bmp180_parameters bmp180_params = data->bmp180_params;
	int ret;
	
	ret = bmp180_read_uncompensated_temp(client, data);
	if(ret < 0)
	{
		dev_info(data->dev, "Temperature cannot be read!\n");
		return ret;
	}
	
	bmp180_params.X1 = ( ( bmp180_params.UT - (s32)bmp180.AC6 ) * (s32)bmp180.AC5 ) >> BMP180_SHIFT_BIT_POSITION_BY_15_BITS;
	bmp180_params.X2 = ( (s32)bmp180.MC << BMP180_SHIFT_BIT_POSITION_BY_11_BITS ) / ( bmp180_params.X1 + bmp180.MD );
	bmp180_params.B5 = bmp180_params.X1 + bmp180_params.X2;

	bmp180_params.T = ( bmp180_params.B5 + 8 ) >> BMP180_SHIFT_BIT_POSITION_BY_04_BITS;
	
	return 0;
}

/* Calculating the Value of Pressure */
s32 bmp180_read_press(struct i2c_client *client, struct bmp180_data *data)
{
	struct bmp180_calib bmp180 = data->bmp180;
	struct bmp180_parameters bmp180_params = data->bmp180_params;
	int ret;
	
	ret = bmp180_read_uncompensated_press(client, data);
	if(ret < 0)
	{
		dev_info(data->dev, "Temperature cannot be read!\n");
		return ret;
	}
	
	bmp180_params.B6 = bmp180_params.B5 - 4000;

	bmp180_params.X1 = ( (s32)bmp180.B2 * ( ( bmp180_params.B6 * bmp180_params.B6 ) >> BMP180_SHIFT_BIT_POSITION_BY_12_BITS ) ) >> BMP180_SHIFT_BIT_POSITION_BY_11_BITS;
	bmp180_params.X2 = ( (s32)bmp180.AC2 * bmp180_params.B6 ) >> BMP180_SHIFT_BIT_POSITION_BY_11_BITS;
	bmp180_params.X3 = bmp180_params.X1 + bmp180_params.X2;
	bmp180_params.B3 = ( ( ( ( ( (s32)bmp180.AC1 ) * 4 + bmp180_params.X3 ) << OSS_VALUE ) + 2 ) >> BMP180_SHIFT_BIT_POSITION_BY_02_BITS );

	bmp180_params.X1 = ( bmp180.AC3 * bmp180_params.B6 ) >> BMP180_SHIFT_BIT_POSITION_BY_13_BITS;
	bmp180_params.X2 = ( bmp180.B1 * ( ( bmp180_params.B6 * bmp180_params.B6 ) >> BMP180_SHIFT_BIT_POSITION_BY_12_BITS ) ) >> BMP180_SHIFT_BIT_POSITION_BY_16_BITS;
	bmp180_params.X3 = ( ( bmp180_params.X1 + bmp180_params.X2 ) + 2 ) >> BMP180_SHIFT_BIT_POSITION_BY_02_BITS;
	bmp180_params.B4 = ( (u32)bmp180.AC4 * (u32)( bmp180_params.X3 + 32768 ) ) >> BMP180_SHIFT_BIT_POSITION_BY_15_BITS;

	bmp180_params.B7 = ( (u32)(bmp180_params.UP - bmp180_params.B3 ) * ( 50000 >> OSS_VALUE ) );

	if( bmp180_params.B7 < 0x80000000 )
	{
		bmp180_params.p = ( bmp180_params.B7 * 2 ) / bmp180_params.B4;
	}

	else
	{
		bmp180_params.p = ( bmp180_params.B7 / bmp180_params.B4 ) << BMP180_SHIFT_BIT_POSITION_BY_01_BIT;
	}

	bmp180_params.X1 = ( bmp180_params.p >> BMP180_SHIFT_BIT_POSITION_BY_08_BITS );
	bmp180_params.X1 *= bmp180_params.X1;
	bmp180_params.X1 = ( bmp180_params.X1 * 3038 ) >> BMP180_SHIFT_BIT_POSITION_BY_16_BITS;
	bmp180_params.X2 = ( bmp180_params.p * (-7357) ) >> BMP180_SHIFT_BIT_POSITION_BY_16_BITS;

	bmp180_params.p  = bmp180_params.p + ( ( bmp180_params.X1 + bmp180_params.X2 + 3791 ) >> BMP180_SHIFT_BIT_POSITION_BY_04_BITS );

	#if	OSS_VALUE == 1
		bmp180_params.p /= 2;

	#elif 	OSS_VALUE == 2
		bmp180_params.p /= 4;

	#elif 	OSS_VALUE == 3
		bmp180_params.p /= 8;

	#warning OSS Value is wrong

	#endif
	
	return 0;
}

/* BMP180 Is Initialized */
int bmp180_pre_init(struct i2c_client *client)
{

	s32 id;
	id = i2c_smbus_read_byte_data(client, BMP180_ID_REG);

	if( id != 0x55 )
	{
		dev_info(&client->dev, "ERROR : Communication is Failed, ID is Wrong!\n" );
		return id;
	}

	return 0;
}

static const struct iio_chan_spec bmp180_channels[] =
{
	/* This channel representing the temperature channel of the sensor because of the .type = IIO_TEMP. */
	{
		.type = IIO_TEMP,
		.info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED) /* This equals to the ( 1UL << IIO_CHAN_INFO_RAW ), which IIO_CHAN_INFO_RAW is defined in iio/types.h */
	},
	
	/* This channel representing the temperature channel of the sensor because of the .type = IIO_PRESSURE. */
	{
		.type = IIO_PRESSURE,
		.info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED)
	},
};

const struct iio_info bmp180_i2c_info =
{
	.read_raw  = &bmp180_read_raw,
	.write_raw = &bmp180_write_raw,
};

/**
  @brief: Information regarding the sensor is initiated.
  */
const struct bmp180_chip_info bmp180_info =
{
	.channels = bmp180_channels,
	.num_channels = 2, /* ARRAY_SIZE() macro can be used as well. */
	
	.oversampling_setting = OSS_VALUE,
	
	.read_temp  = bmp180_read_temp,
	.read_press = bmp180_read_press,
	.read_calib = bmp180_read_calib,
	.preinit    = bmp180_pre_init,
};
