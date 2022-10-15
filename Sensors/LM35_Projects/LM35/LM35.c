/*
 ============================================================================
 Name        : LM35.c
 Author      : Yavuz Murat TAS
 Version     : 2.0
 Description : Measuring Temperature with LM35 Sensor
 ============================================================================
*/

/*===========================================================================
BBB_expansion_header_P9_pins                	        LM35
=============================================================================
P9.1                                             	   GROUND
P9.39                                                   Vout
P9.32                                                    Vs
============================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

/* This is the Path of the Analog Pin 0 */
#define SYS_AINO_PATH		"/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

#define MAX_BUFF 			100

/*	GPIO Read Value */
int gpio_read_value( void );

int main()
{
    printf( "This Program Measures The Temperature Thanks to The Sensor LM35\n" );

	while ( 1 )
	{
		uint16_t value = ( uint16_t ) gpio_read_value();	//The content of the file of analog input 0 is obtained
		uint16_t temp  = ( ( ( value * 1.8 ) / 4095 ) * 100 );	// The temperature value is calculated
		printf( "The Temperature Value ( in Celcius ) is : %d\n", temp );
		usleep( 1000 * 1000 ); // The delay time is decided as 1 second ( Converting ms to microseconds )
	}

	return 0;
}

int gpio_read_value( void )
{

    int fd;
    char value[5];
    value[5] = 0;
    char buf[MAX_BUFF];

    snprintf( buf, sizeof( buf ), SYS_AINO_PATH );	// SYS_AINO_PATH is written into buf according to the length of buf

    fd = open( buf, O_RDONLY );	// buf is opened in the mode read only and returns a file descriptor (fd)
    if ( fd < 0 )	// If the file cannot open, then an error will be thrown
    {
        perror( "ERROR : GPIO Read Value\n" );
        return fd;
    }

    read( fd, value, 4 );	// The file is open by fd and its content is written into the variable value up to 4 bytes
    close( fd );	// The file is closed after all processes

    return atoi( value );	// The variable value is converted into integer thanks to the function atoi()
}

