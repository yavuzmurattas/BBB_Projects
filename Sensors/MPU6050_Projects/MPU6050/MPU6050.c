/*============================================================================
 Name        : MPU6050.c
 Author      : Yavuz Murat TAS
 Version     : 1.0.0
 ============================================================================*/

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "MPU6050.h"

#define MPU6050_POW_REG					0x6B

#define MPUS6050_I2C_SLV2_ADDR			0x2B
#define MPU6050_WHO_AM_I_REG			0x70

/*	MPU6050 Acceleration Registers	*/
#define MPU6050_ACCEL_XOUT_H			0x3B
#define MPU6050_ACCEL_XOUT_L			0x3C
#define MPU6050_ACCEL_YOUT_H			0x3D
#define MPU6050_ACCEL_YOUT_L			0x3E
#define MPU6050_ACCEL_ZOUT_H			0x3F
#define MPU6050_ACCEL_ZOUT_L			0x40

/*	MPU6050 Gyroscope Registers	*/
#define MPU6050_GYRO_XOUT_H				0x43
#define MPU6050_GYRO_XOUT_L				0x44
#define MPU6050_GYRO_YOUT_H				0x45
#define MPU6050_GYRO_YOUT_L				0x46
#define MPU6050_GYRO_ZOUT_H				0x47
#define MPU6050_GYRO_ZOUT_L				0x48

#define MPU6050_GYRO_CONFIG				0x1B
#define MPU6050_ACCEL_CONFIG 			0x1C

#define MPU6050_INIT_VALUE				0


#define I2C_DEVICE_FILE					"/dev/i2c-2"

int i2c_fd;

/*	GPIO Write Value */
u32 mpu6050_write( u16 base_addr, u16 data )
{
	s32 ret;
	s8 buf[2];
	buf[0] = base_addr;
	buf[1] = data;
	ret = write( i2c_fd, buf, 2 );

	if ( ret <= 0 )
	{
		perror( "ERROR : Write Failed\n" );
		abort();
	}

	return 0;
}

/*	GPIO Read Value */
u32 mpu6050_read( s16 base_addr, s8 * veri, u8 len )
{
	u32 ret;
	u8 buf[1];
	buf[0] = base_addr;

	ret = write( i2c_fd, buf, 1 );
	if ( ret <= 0 )
	{
		perror( "ERROR : Write Address Failed\n" );
		abort();
	}

	ret = read( i2c_fd, veri, len );
	if( ret <= 0 )
	{
		perror( "ERROR : Read Failed\n" );
		abort();
	}

	return 0;
}

/*	MPU6050 Initialization and Checking whether or not It Is Correct */
void mpu6050_init( void )
{
	s8 i2c_slv_addr;
	/* The Sleep Mode of the Sensor Is Disabled	*/
	mpu6050_write( MPU6050_POW_REG, 0x00 );
	usleep( 500 );
	/*	Slave Address of the Sensor Is Obtained */
	mpu6050_read( MPUS6050_I2C_SLV2_ADDR, &i2c_slv_addr, 1 );
	i2c_slv_addr &= ~( 1 << 7 );

	/* Checking Whether the Correct Sensor or not */
	if( MPU6050_WHO_AM_I_REG != 0x55 && MPU6050_SLAVE_ADDR == i2c_slv_addr )
	{
		perror( "ERROR : Communication is Failed!\n" );
		abort();
	}

	/*	Initialization of both Gyroscope and Acceleration Parts of the Sensor */
	mpu6050_write( MPU6050_GYRO_CONFIG,  0x00 );
	usleep( 500 );
	mpu6050_write( MPU6050_ACCEL_CONFIG, 0x00 );
	usleep( 500 );
}

/*	Gaining the Values of the Acceleration	*/
void mpu6050_read_acc( s16 * acc_values )
{
    //each axis value is of 2byte, so we need a buffer of 6bytes.
	s8 acc_buffer[6] = { MPU6050_INIT_VALUE, MPU6050_INIT_VALUE, MPU6050_INIT_VALUE,
						 MPU6050_INIT_VALUE, MPU6050_INIT_VALUE, MPU6050_INIT_VALUE };

    //start reading from the base address of accelerometer values i.e MPU6050_REG_ACC_X_HIGH
    mpu6050_read( MPU6050_ACCEL_XOUT_H, acc_buffer, 6 );

    /* pBuffer[0]= acc x axis value , pBuffer[1]= acc y axis value , pBuffer[2]= acc z axis value  */
    acc_values[0] =  (s32) ( ( acc_buffer[0]  << 8 ) |  acc_buffer[1]  );
    acc_values[1] =  (s32) ( ( acc_buffer[2]  << 8 ) |  acc_buffer[3]  );
    acc_values[2] =  (s32) ( ( acc_buffer[4]  << 8 ) |  acc_buffer[5]  );

}

/*	Gaining the Values of the Gyroscope	*/
void mpu6050_read_gyro( s16 * gyro_values )
{
    //each axis value is of 2byte, so we need a buffer of 6bytes.
	s8 gyro_buffer[6] = { MPU6050_INIT_VALUE, MPU6050_INIT_VALUE, MPU6050_INIT_VALUE,
						  MPU6050_INIT_VALUE, MPU6050_INIT_VALUE, MPU6050_INIT_VALUE };

    //start reading from the base address of accelerometer values i.e MPU6050_REG_ACC_X_HIGH
    mpu6050_read( MPU6050_GYRO_XOUT_H, gyro_buffer, 6 );

    /* pBuffer[0]= acc x axis value , pBuffer[1]= acc y axis value , pBuffer[2]= acc z axis value  */
    gyro_values[0] =  (s32) ( ( gyro_buffer[0]  << 8 ) |  gyro_buffer[1]  );
    gyro_values[1] =  (s32) ( ( gyro_buffer[2]  << 8 ) |  gyro_buffer[3]  );
    gyro_values[2] =  (s32) ( ( gyro_buffer[4]  << 8 ) |  gyro_buffer[5]  );

}
