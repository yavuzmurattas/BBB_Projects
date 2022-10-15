/*
 ============================================================================
 Name        : main.c
 Author      : Yavuz Murat TAS
 Version     : 1.0
 Description : Measuring the Temperature with LM35 and Visualizing with
 	 	 	   the 4 Digit 7 Segment Display
 ============================================================================
*/

/*===========================================================================
BBB_expansion_header_P9_pins                	        LM35
=============================================================================
P9.1                                             	   GROUND
P9.39                                                   Vout
P9.32                                                    Vs
============================================================================= */

/*==========================================================================================
BBB_expansion_header_P8_pins          GPIO Number            4-digit 7 Seg Display Pin Number
=============================================================================================
P8.7                                   GPIO_66                     8   ( SEG A )
P8.8                                   GPIO_67                     12  ( SEG B )
P8.9                                   GPIO_69                     4   ( SEG C )
P8.10                                  GPIO_68                     3   ( DP    )
P8.11                                  GPIO_45                     2   ( SEG D )
P8.12                                  GPIO_44                     1   ( SEG E )
P8.14                                  GPIO_26                     9   ( SEG F )
P8.16                                  GPIO_46                     5   ( SEG G )
============================================================================================ */

/*==========================================================================================
BBB_expansion_header_P9_pins          GPIO Number            4-digit 7 Seg Display Pin Number
=============================================================================================
P9.15                                  GPIO_48                     7   ( DIGIT 1 )
P9.23                                  GPIO_49                     10  ( DIGIT 2 )
P9.12                                  GPIO_60                     11  ( DIGIT 3 )
P9.27                                  GPIO_115                    6   ( DIGIT 4 )
============================================================================================= */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "LM35w4D7Segment.h"

int main( void )
{
    printf( "This Program Measures The Temperature Thanks to The Sensor LM35 "
    		"and Visualizing The Degree in 4 Digit 7 Segment LEDs Display.\n");

	if ( initialize_all_gpios() < 0 )	// If the GPIOs cannot be set, then an error is put
	{
		printf( "Error: GPIO Init Failed !\n" );
	    return 1;
	}

	else
	{
	    while ( 1 )
	    {
	    	uint16_t value = ( uint16_t ) gpio_read_value();	//The content of the file of analog input 0 is obtained
	    	uint16_t temp  = ( ( ( value * 1.8 ) / 4095 ) * 100 );	// The temperature value is calculated
	    	printf( "The Temperature Value ( in Celcius ) is : %d\n", temp );
		    display_numbers( temp );	// The temperature is displayed
		    usleep( 1000 ); // The delay time is decided as 1ms second ( Converting ms to microseconds )
	    }
	}

	return 0;
}


