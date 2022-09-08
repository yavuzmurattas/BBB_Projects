/*
 ============================================================================
 Name        : LM35w7Segment.c
 Author      : Yavuz Murat TAS
 Version     : 2.0
 ============================================================================
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "LM35w7Segment.h"

/* Please Uncomment This Macro Only If You Use COMMAN CATHODE Display */
#define COMMON_CATHODE

/*	Defining the 7 Segment Displays Pins	*/
#define GPIO_66     			66
#define GPIO_67     			67
#define GPIO_69     			69
#define GPIO_68     			68
#define GPIO_45     			45
#define GPIO_44     			44
#define GPIO_26     			26
#define GPIO_46     			46

#define	GPIO_65					65
#define GPIO_61					61
#define GPIO_49					49
#define GPIO_117				117
#define GPIO_115				115
#define GPIO_27					27
#define GPIO_47					47

#define GPIO_66_P8_7_SEGA       GPIO_66
#define GPIO_67_P8_8_SEGB       GPIO_67
#define GPIO_69_P8_9_SEGC       GPIO_69
#define GPIO_68_P8_10_DP        GPIO_68
#define GPIO_45_P8_11_SEGD      GPIO_45
#define GPIO_44_P8_12_SEGE      GPIO_44
#define GPIO_26_P8_14_SEGF      GPIO_26
#define GPIO_46_P8_16_SEGG      GPIO_46

#define GPIO_65_P8_18_SEGA      GPIO_65
#define GPIO_61_P8_26_SEGB      GPIO_61
#define GPIO_49_P9_23_SEGC      GPIO_49
#define GPIO_117_P9_25_SEGD     GPIO_117
#define GPIO_115_P9_27_SEGE     GPIO_115
#define GPIO_27_P8_17_SEGF      GPIO_27
#define GPIO_47_P8_15_SEGG      GPIO_47

#define HIGH_VALUE  			1
#define LOW_VALUE   			0

/*	Defining the Directions of the Pins	*/
#define GPIO_DIR_OUT        	HIGH_VALUE
#define GPIO_DIR_IN         	LOW_VALUE

#define GPIO_LOW_VALUE      	LOW_VALUE
#define GPIO_HIGH_VALUE    		HIGH_VALUE

/*	Defining the Segment to Open or not	*/
#ifdef COMMON_CATHODE
	#define SEGMENT_ON          HIGH_VALUE
	#define SEGMENT_OFF         LOW_VALUE
#else
	#define SEGMENT_ON          LOW_VALUE
	#define SEGMENT_OFF         HIGH_VALUE
#endif

/* This is the path corresponding to GPIOs in the 'sys' directory */
#define SYS_GPIO_PATH 			"/sys/class/gpio"

/*	The Path of the Analog Pin 0 */
#define SYS_AINO_PATH			"/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

#define MAX_BUFF 				100

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

/*
 *  GPIO configure direction
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
 *  GPIO write value
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

/*
 *	This function initializes all the gpios for this application
 */
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
	gpio_export( GPIO_65  );
	gpio_export( GPIO_61  );
	gpio_export( GPIO_49  );
	gpio_export( GPIO_117 );
	gpio_export( GPIO_115 );
	gpio_export( GPIO_27  );
	gpio_export( GPIO_47  );

	gpio_configure_dir( GPIO_66, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_66, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_67, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_67, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_69, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_69, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_68, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_68, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_45, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_45, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_44, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_44, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_26, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_26, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_46, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_46, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_65, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_65, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_61, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_61, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_49, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_49, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_117, GPIO_DIR_OUT );
    gpio_write_value( GPIO_117, GPIO_LOW_VALUE );

    gpio_configure_dir( GPIO_115, GPIO_DIR_OUT );
    gpio_write_value( GPIO_115, GPIO_LOW_VALUE );

    gpio_configure_dir( GPIO_27, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_27, GPIO_LOW_VALUE  );

    gpio_configure_dir( GPIO_47, GPIO_DIR_OUT  );
    gpio_write_value( GPIO_47, GPIO_LOW_VALUE  );

    return 0;
}

/* This Function Displays Number on the 1st 7 Segment Display */
void Write_number_to_7segdisplay_1( uint8_t numberToDisplay )
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

		default:
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

/* This Function Displays Number on the 2nd 7 Segment Display */
void Write_number_to_7segdisplay_2( uint8_t numberToDisplay )
{
	switch ( numberToDisplay )
    {
		case 0:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_ON  );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_ON  );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_OFF );
		break;

		case 1:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_OFF );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_OFF );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_OFF );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_OFF );
		break;

		case 2:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_OFF );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_ON  );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_OFF );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_ON  );
		break;

		case 3:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_OFF );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_ON  );
		break;

		case 4:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_OFF );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_OFF );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_ON  );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_ON  );
		break;

		case 5:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_OFF );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_ON  );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_ON  );
		break;

		case 6:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_OFF );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_ON  );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_ON  );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_ON  );
		break;

		case 7:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_OFF );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_OFF );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_OFF );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_OFF );
		break;

		case 8:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_115_P9_27_SEGE, SEGMENT_ON  );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_ON  );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_ON  );
		break;

		case 9:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_ON  );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_ON  );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_ON  );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_ON  );
			gpio_write_value( GPIO_44_P8_12_SEGE,  SEGMENT_OFF );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_ON  );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_ON  );
		break;

		default:
			gpio_write_value( GPIO_65_P8_18_SEGA,  SEGMENT_OFF );
			gpio_write_value( GPIO_61_P8_26_SEGB,  SEGMENT_OFF );
			gpio_write_value( GPIO_49_P9_23_SEGC,  SEGMENT_OFF );
			gpio_write_value( GPIO_117_P9_25_SEGD, SEGMENT_OFF );
			gpio_write_value( GPIO_44_P8_12_SEGE,  SEGMENT_OFF );
			gpio_write_value( GPIO_27_P8_17_SEGF,  SEGMENT_OFF );
			gpio_write_value( GPIO_47_P8_15_SEGG,  SEGMENT_OFF );
		break;
    }
}

