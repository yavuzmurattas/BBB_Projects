/*============================================================================
 Name        : MPU6050.c
 Author      : Yavuz Murat TAS
 Version     : 1.0.0
 Description : Measuring both Acceleration and Position Using MPU6050 Sensor
 ============================================================================*/

/*================================================
BBB_expansion_header_P9_pins     MPU6050 pins
===================================================
P9_19                              SCL
P9_20                              SDA
P9_3                               VCC (3.3V)
P9_1                               GND
P9_2							   AD0	(For Slave Addr 0x68)
P9_4							   AD0	(For Slave Addr 0x69)
==================================================== */

/*
 * Datasheet Refeeances
 * 1. MPU-6000 and MPU-6050 Product Specification Revision 3.4
 * 2. MPU-6000 and MPU-6050 Register Map and Descriptions Revision 2.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "MPU6050.h"

#define I2C_DEVICE_FILE		"/dev/i2c-2"

extern int i2c_fd;

int main( void )
{
	/* First Open the I2C Device File */
    if ( ( i2c_fd = open( I2C_DEVICE_FILE, O_RDWR ) ) < 0 )
    {
    	perror( "ERROR : Failed to Open I2C Device File.\n" );
    	abort();
    }

    /* Set the I2C Slave Address Using ioctl I2C_SLAVE Command */
    if ( ioctl( i2c_fd, I2C_SLAVE, MPU6050_SLAVE_ADDR ) < 0 )
    {
    	perror( "ERROR : Failed to Set I2C Slave Address.\n" );
        close( i2c_fd );
        abort();
    }

    /*	The Sensor Is Initialized */
    mpu6050_init();

	s16 acc_values[3], gyro_values[3];
	double accx, accy, accz, gyrox, gyroy, gyroz;

    while( 1 )
    {
    	mpu6050_read_acc( acc_values );		// The Acceleration Values are Obtained
    	mpu6050_read_gyro( gyro_values );	// The Acceleration Values are Obtained

    	/*	Both the Acceleration Values and the Gyroscope Values are Calculated */
    	accx = (double)acc_values[0] / ACCEL_FS_SENSITIVITY;
    	accy = (double)acc_values[1] / ACCEL_FS_SENSITIVITY;
    	accz = (double)acc_values[2] / ACCEL_FS_SENSITIVITY;

    	gyrox = (double)gyro_values[0] / GYRO_FS_SENSITIVITY;
    	gyroy = (double)gyro_values[1] / GYRO_FS_SENSITIVITY;
    	gyroz = (double)gyro_values[2] / GYRO_FS_SENSITIVITY;

    	printf( "ACC Values in g: X = %.2f, Y = %.2f, Z = %.2f\n", accx, accy, accz );
    	printf(	"GYRO Values in dps: X = %.2f, Y = %.2f, Z = %.2f\n", gyrox, gyroy, gyroz );

    	usleep( 1000 * 1000 );
    }

	return EXIT_SUCCESS;
}
