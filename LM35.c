/*
 ============================================================================
 Name        : LM35.c
 Author      : Yavuz Murat TAÞ
 Description : Measruing Temperature with LM35 Sensor
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

#define SYS_AINO_PATH		"/sys/bus/iio/devices/iio:device0/in_voltage0_raw" // The Path of the Analog Pin 0

#define SOME_BYTES 			100

/*
 *  GPIO read value
 */
int gpio_read_value( void )
{

    int fd;
    char value[5];
    value[4] = 0;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_AINO_PATH );	// SYS_AINO_PATH is written into buf according to the length of buf

    fd = open(buf, O_RDONLY);	// buf is opened in the mode read only and returns a file descriptor (fd)
    if (fd < 0) {	// If the file cannot open, then an error will be thrown
        perror("gpio read value\n");
        return fd;
    }

    read(fd, value, 4);	// The file is open by fd and its content is written into the variable value up to 4 bytes
    close(fd);	// The file is closed after all processes
    return atoi(value);	// The variable value is converted into integer thanks to the function atoi()
}

int main() {

	while (1)
	{
		uint32_t value = gpio_read_value();	//The content of the file of analog input 0 is obtained
		uint32_t temp = ( ( value * 1.8 ) / 4095 ) * 100;	// The temperature value is calculated
		printf("The Temperature Value (in Celcius) is : %d\n", temp );
		usleep(1000 * 1000); // The delay time is decided as 1 second (converting ms to micro)
	}
	return 0;
}
