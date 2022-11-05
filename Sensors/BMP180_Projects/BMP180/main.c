/*
 ============================================================================
 Name        : main.c
 Author      : Yavuz Murat TAS
 Version     : 1.0.0
 Description : Measuring Both Temperature and Pressure by BMP180 Sensor with I2C
 Note		 : This program should be compiled with the directive -lm
			   because of pow function in math.h to execute as desired
 ============================================================================
 */

/*================================================
BBB_expansion_header_P9_pins     BMP180 pins
===================================================
P9_19                              SCL
P9_20                              SDA
P9_3                               VCC (3.3V)
P9_1                               GND
==================================================== */

/*
 * Datasheet Referance:
 * 1. BMP180 Digital Pressure Sensor
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <math.h>

#include "BMP180.h"

#define BMP180_SLAVE_ADDR			(0x77)
#define BMP180_SOFT_RESET_REG		(0xE0)

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
    if ( ioctl( i2c_fd, I2C_SLAVE, BMP180_SLAVE_ADDR ) < 0 )
    {
    	perror( "ERROR : Failed to Set I2C Slave Address.\n" );
        close( i2c_fd );
        abort();
    }

    bmp180_init();

    CAL_PARAM_t cal_values;
    PARAMETERS_t parameters;

    bmp180_read_calib( &cal_values );

    double altitude;
    float temperature = 0;
    double pressure = 0;

    while( 1 )
    {
    	bmp180_read_uncompensated_temp( &parameters );
    	bmp180_read_uncompensated_press( &parameters );

    	calc_temp( &parameters, &cal_values );
		calc_press( &parameters, &cal_values );

		temperature = parameters.T / 150;
		pressure = parameters.p / 100;

		double base = pressure / P0;
		double temp;
		temp = pow( base , POWER );
		altitude = (double)44330 * ( 1.0 - temp );

		printf( "Value of the Temperature Value is : %f\n", temperature );
		printf( "Value of the Pressure Value is : %f\n", pressure );
		printf( "Value of the Altitude according to the Sea Level : %f\n", altitude );

		/* The Sensor Will Be Reset */
		bmp180_write( BMP180_SOFT_RESET_REG, 0x00 );
		usleep( 1000 * 1000 );

    }

	return EXIT_SUCCESS;
}
