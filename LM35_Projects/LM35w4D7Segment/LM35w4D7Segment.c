/*
 ============================================================================
 Name        : LM35w4D7Segment.c
 Author      : Yavuz Murat TAS
 Version     : 1.0
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include "LM35w4D7Segment.h"

/* Please Uncomment This Macro Only If You Use COMMAN CATHODE Display */
#define COMMON_CATHODE

#define GPIO_66     			 66
#define GPIO_67     			 67
#define GPIO_69     			 69
#define GPIO_68     			 68
#define GPIO_45     			 45
#define GPIO_44     			 44
#define GPIO_26     			 26
#define GPIO_46     			 46

#define GPIO_48     			 48
#define GPIO_49     			 49
#define GPIO_60     			 60
#define GPIO_115    			 115

#define GPIO_66_P8_7_SEGA        GPIO_66       /*  display pin 8     */
#define GPIO_67_P8_8_SEGB        GPIO_67       /*  display pin 12    */
#define GPIO_69_P8_9_SEGC        GPIO_69       /*  display pin 4     */
#define GPIO_68_P8_10_DP         GPIO_68       /*  display pin 3     */
#define GPIO_45_P8_11_SEGD       GPIO_45       /*  display pin 2     */
#define GPIO_44_P8_12_SEGE       GPIO_44       /*  display pin 1     */
#define GPIO_26_P8_14_SEGF       GPIO_26       /*  display pin 9     */
#define GPIO_46_P8_16_SEGG       GPIO_46       /*  display pin 5     */

#define GPIO_48_P9_15_DIGIT1     GPIO_48       /*  display pin 7     */
#define GPIO_49_P9_23_DIGIT2     GPIO_49       /*  display pin 10    */
#define GPIO_60_P9_12_DIGIT3     GPIO_60       /*  display pin 11    */
#define GPIO_115_P9_27_DIGIT4    GPIO_115      /*  display pin 6     */

#define HIGH_VALUE  			 1
#define LOW_VALUE   		     0

/*	Defining the Directions of the Pins	*/
#define GPIO_DIR_OUT        	 HIGH_VALUE
#define GPIO_DIR_IN         	 LOW_VALUE

#define GPIO_LOW_VALUE      	 LOW_VALUE
#define GPIO_HIGH_VALUE     	 HIGH_VALUE

/*	Defining the Segment as High or Low according to Common Cathode or Anode */
#ifdef COMMON_CATHODE
	#define SEGMENT_ON           HIGH_VALUE
	#define SEGMENT_OFF          LOW_VALUE
#else
	#define SEGMENT_ON           LOW_VALUE
	#define SEGMENT_OFF          HIGH_VALUE
#endif

/* This is the path corresponding to GPIOs in the 'sys' directory */
#define SYS_GPIO_PATH 			 "/sys/class/gpio"

/*	This is the path of the Analog Pin 0	*/
#define SYS_AINO_PATH			 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

#define MAX_BUFF          		 100

/* GPIO Export Pin ( to Activate the Pins to Use ) */
int gpio_export( uint8_t gpio_num )
{
	int fd, len;
	char buf[MAX_BUFF];

	fd = open( SYS_GPIO_PATH "/export", O_WRONLY );
	if ( fd < 0 )
	{
		perror( "ERROR : Opening Export File\n" );
		return fd;
	}

	len = snprintf( buf, sizeof( buf ), "%d", gpio_num );
	write( fd, buf, len );
	close( fd );

	return 0;
}

/*
 *  GPIO Configure Direction
 *  dir_value : 1 means 'out' , 0 means "in"
 */
int gpio_configure_dir( uint8_t gpio_num, uint8_t dir_value )
{
    int fd;
    char buf[MAX_BUFF];

    snprintf( buf, sizeof( buf ), SYS_GPIO_PATH "/gpio%d/direction", gpio_num );

    fd = open( buf, O_WRONLY );

    if ( fd < 0 )
    {
        perror( "ERROR : GPIO Direction Configure\n" );
        return fd;
    }

    if ( dir_value )	// The direction is written to the GPIO's files
    {
        write( fd, "out", 4 ); //3+1  +1 for NULL character
    }

    else
    {
        write( fd, "in", 3 );
    }
    close( fd );

    return 0;
}

/*
 *  GPIO Write Value
 *  out_value : can be either 0 or 1
 */
int gpio_write_value( uint8_t gpio_num, uint8_t out_value )
{
    int fd;
    char buf[MAX_BUFF];

    snprintf( buf, sizeof( buf ), SYS_GPIO_PATH "/gpio%d/value", gpio_num );

    fd = open( buf, O_WRONLY );
    if ( fd < 0 )
    {
        perror( "ERROR : GPIO Write Value\n" );
        return fd;
    }

    if ( out_value )
    {
        write( fd, "1", 2 );
    }

    else
    {
    	write( fd, "0", 2 );
    }

    close( fd );

    return 0;
}

/*	GPIO Read Value	*/
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

int initialize_all_gpios( void )
{
	/*
	 *	The GPIOs are driven. First function exports the GPIOs,
	 *	Then, function sets the pins whether it is in or output mode.
	 *	Finally, function turn on the LEDs by writing the values 0s and 1s
	 */

	/* First Export the Pins to Use */
	gpio_export( GPIO_66  );
	gpio_export( GPIO_67  );
	gpio_export( GPIO_69  );
	gpio_export( GPIO_68  );
	gpio_export( GPIO_45  );
	gpio_export( GPIO_44  );
	gpio_export( GPIO_26  );
	gpio_export( GPIO_46  );
	gpio_export( GPIO_48  );
	gpio_export( GPIO_49  );
	gpio_export( GPIO_60  );
	gpio_export( GPIO_115 );

    /* Secondly Configure the Direction for Segments */
    gpio_configure_dir( GPIO_66_P8_7_SEGA,  	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_67_P8_8_SEGB,  	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_69_P8_9_SEGC,  	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_68_P8_10_DP,   	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_45_P8_11_SEGD, 	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_44_P8_12_SEGE, 	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_26_P8_14_SEGF, 	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_46_P8_16_SEGG, 	GPIO_DIR_OUT );

    /* Configure the Direction for Digit Control */
    gpio_configure_dir( GPIO_48_P9_15_DIGIT1,   GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_49_P9_23_DIGIT2,  	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_60_P9_12_DIGIT3,  	GPIO_DIR_OUT );
    gpio_configure_dir( GPIO_115_P9_27_DIGIT4, 	GPIO_DIR_OUT );

    /* Make All Segments Off */
    gpio_write_value( GPIO_66_P8_7_SEGA,  		GPIO_LOW_VALUE );
    gpio_write_value( GPIO_67_P8_8_SEGB,  		GPIO_LOW_VALUE );
    gpio_write_value( GPIO_69_P8_9_SEGC,  		GPIO_LOW_VALUE );
    gpio_write_value( GPIO_68_P8_10_DP,   		GPIO_LOW_VALUE );
    gpio_write_value( GPIO_45_P8_11_SEGD, 		GPIO_LOW_VALUE );
    gpio_write_value( GPIO_44_P8_12_SEGE, 		GPIO_LOW_VALUE );
    gpio_write_value( GPIO_26_P8_14_SEGF, 		GPIO_LOW_VALUE );
    gpio_write_value( GPIO_46_P8_16_SEGG, 		GPIO_LOW_VALUE );

    /* Make All Digits Off */
    gpio_write_value( GPIO_48_P9_15_DIGIT1,  	GPIO_HIGH_VALUE );
    gpio_write_value( GPIO_49_P9_23_DIGIT2,  	GPIO_HIGH_VALUE );
    gpio_write_value( GPIO_60_P9_12_DIGIT3,  	GPIO_HIGH_VALUE );
    gpio_write_value( GPIO_115_P9_27_DIGIT4, 	GPIO_HIGH_VALUE );

    return 0;
}


/* This Function Displays the Number on the 7 Segment Display */
void Write_number_to_7segdisplay( uint8_t numberToDisplay )
{
	/*	The LEDs are turned on according to the numbers to be sent to the function	*/

    switch ( numberToDisplay )
	{
    	case 0:
    		gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_ON  );
    		gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_ON  );
    		gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
    		gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_ON  );
    		gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_ON  );
    		gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_ON  );
    		gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_OFF );
    		break;

    	case 1:
    		gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_OFF );
    		gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_ON  );
    		gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
    		gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_OFF );
    		gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_OFF );
    		gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_OFF );
    		gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_OFF );
    		break;

    	case 2:
    		gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_ON  );
    		gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_ON  );
    		gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_OFF );
    		gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_ON  );
    		gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_ON  );
    		gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_OFF );
    		gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_ON  );
    		break;

    	case 3:
    		gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_ON  );
    		gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_ON  );
    		gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
    		gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_ON  );
    		gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_OFF );
    		gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_OFF );
    		gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_ON  );
    		break;

    	case 4:
    		gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_OFF );
    		gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_ON  );
    		gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
    		gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_OFF );
    		gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_OFF );
    		gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_ON  );
    		gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_ON  );
    		break;

		case 5:
			gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_OFF );
			gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_ON  );
			gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_ON  );
		break;

		case 6:
			gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_OFF );
			gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_ON  );
			gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_ON  );
			gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_ON  );
		break;

		case 7:
			gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_OFF );
			gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_OFF );
			gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_OFF );
		break;

		case 8:
			gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_ON  );
			gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_ON  );
			gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_ON  );
		break;

		case 9:
			gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_ON  );
			gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_ON  );
		break;

		case 10:
			gpio_write_value( GPIO_66_P8_7_SEGA,  SEGMENT_OFF );
			gpio_write_value( GPIO_67_P8_8_SEGB,  SEGMENT_OFF );
			gpio_write_value( GPIO_69_P8_9_SEGC,  SEGMENT_OFF );
			gpio_write_value( GPIO_45_P8_11_SEGD, SEGMENT_OFF );
			gpio_write_value( GPIO_44_P8_12_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_26_P8_14_SEGF, SEGMENT_OFF );
			gpio_write_value( GPIO_46_P8_16_SEGG, SEGMENT_OFF );
		break;
    }
}

/*
 * This function implements the logic to write numbers on the 4 digit LED display
 * we turn on and display a number on each digit for a very small amount of time that is 10 microseconds,
 * then we move to next digit.
 * If we could do that very fast enough, than it resembles as if it displays the digits simultaneously.
 */
void display_numbers( uint16_t number )
{
	/* Since we have 4 digits, then the for loop have 4 loop */

    for( uint8_t digit = 4 ; digit > 0 ; digit-- )
    {
    	/*
    	 * The for loop starts with the 4th digit ( right most )
    	 * Turn on each digit for a small amount of time and display the number
    	 */

        switch( digit )
        {
            case 1:
                gpio_write_value( GPIO_48_P9_15_DIGIT1,  GPIO_LOW_VALUE  );
              break;

            case 2:
                gpio_write_value( GPIO_49_P9_23_DIGIT2,  GPIO_LOW_VALUE  );
                break;

            case 3:
                gpio_write_value( GPIO_60_P9_12_DIGIT3,  GPIO_LOW_VALUE  );
               break;

            case 4:
                gpio_write_value( GPIO_115_P9_27_DIGIT4, GPIO_LOW_VALUE  );
                break;
        }

        Write_number_to_7segdisplay( (uint8_t) number % 10 );
        number /= 10;

        /* Display Each Digit Only for 10 Microseconds */
        usleep( 10 );

    	/* Turn off All Segments */
    	Write_number_to_7segdisplay( 10 );

    	/* Turn off All Digits	 */
    	gpio_write_value( GPIO_48_P9_15_DIGIT1,  GPIO_HIGH_VALUE );
    	gpio_write_value( GPIO_49_P9_23_DIGIT2,  GPIO_HIGH_VALUE );
    	gpio_write_value( GPIO_60_P9_12_DIGIT3,  GPIO_HIGH_VALUE );
    	gpio_write_value( GPIO_115_P9_27_DIGIT4, GPIO_HIGH_VALUE );
    }
}

