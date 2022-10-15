/*
 ============================================================================
 Name        : main.c
 Author      : Yavuz Murat TAS
 Version     : 2.0
 Description : Measuring Temperature with LM35 and Visualizing with 2 Pieces
 			   7 Segment Display
 ============================================================================
*/

/*===========================================================================
BBB_expansion_header_P9_pins                	        LM35
=============================================================================
P9.1                                             	   GROUND
P9.39                                                   Vout
P9.32                                                    Vs
=============================================================================*/

/*==================================================================================
BBB_expansion_header_P8_pins           GPIO Number            1st 7 Seg Display Segment (Right)
===================================================================================
P8.7                                   GPIO_66                     SEG A
P8.8                                   GPIO_67                     SEG B
P8.9                                   GPIO_69                     SEG C
P8.10                                  GPIO_68                     SEG DP (Decimal Point)
P8.11                                  GPIO_45                     SEG D
P8.12                                  GPIO_44                     SEG E
P8.14                                  GPIO_26                     SEG F
P8.16                                  GPIO_46                     SEG G
===================================================================================*/

/*==================================================================================
BBB_expansion_header_P8_pins           GPIO Number            2nd 7 Seg Display Segment (left)
===================================================================================
P8.18                                  GPIO_65                     SEG A
P8.26                                  GPIO_61                     SEG B
P9.23                                  GPIO_49                     SEG C
P9.25                                  GPIO_117                    SEG D
P9.27                                  GPIO_115                    SEG E
P8.17                                  GPIO_27                     SEG F
P8.15                                  GPIO_47                     SEG G
===================================================================================*/

#include <stdio.h>
#include <unistd.h>
#include "LM35w7Segment.h"

int main()
{
	printf( "This Program Measures The Temperature Thanks to The Sensor LM35 "
    		"and Visualizing The Degree in 2 Pieces 7 Segment LEDs Display.\n");

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

	    	printf( "The Temperature Value (in Celcius) is : %d\n", temp );

		    Write_number_to_7segdisplay_1( ( uint8_t ) ( temp % 10 ) );	// The first segment of the temperature is displayed
		    Write_number_to_7segdisplay_2( ( uint8_t ) ( temp / 10 ) );	// The second segment of the temperature is displayed

		    usleep( 1000 * 1000 );	// The delay time is decided as 1 second ( Converting ms to microseconds )
	    }
	}
	return 0;
}

