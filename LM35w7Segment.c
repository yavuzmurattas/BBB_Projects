/*
 ============================================================================
 Name        : LM35w7Segment.c
 Author      : Yavuz Murat TAÞ
 Description : Measuring Temperature with LM35 and Visualizing with 2 7 Segment
 			   Display
 ============================================================================
 */

/*===========================================================================
BBB_expansion_header_P9_pins                	        LM35
=============================================================================
P9.1                                             	   GROUND
P9.39                                                   Vout
P9.32                                                    Vs
============================================================================= */

/*==================================================================================
BBB_expansion_header_P8_pins           GPIO number            1st 7Seg Display segment (right)
===================================================================================
P8.7                                   GPIO_66                     a
P8.8                                   GPIO_67                     b
P8.9                                   GPIO_69                     c
P8.10                                  GPIO_68                     h //decimal point
P8.11                                  GPIO_45                     d
P8.12                                  GPIO_44                     e
P8.14                                  GPIO_26                     f
P8.16                                  GPIO_46                     g
=================================================================================== */

/*==================================================================================
BBB_expansion_header_P8_pins           GPIO number            2nd 7Seg Display segment (left)
===================================================================================
P8.18                                  GPIO_65                     a
P8.26                                  GPIO_61                     b
P9.23                                  GPIO_49                     c
P9.25                                  GPIO_117                    d
P9.27                                  GPIO_115                    e
P8.17                                  GPIO_27                     f
P8.15                                  GPIO_47                     g
=================================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

/*	Defining the 7 Segment Displays Pins	*/
#define GPIO_66     66
#define GPIO_67     67
#define GPIO_69     69
#define GPIO_68     68
#define GPIO_45     45
#define GPIO_44     44
#define GPIO_26     26
#define GPIO_46     46

#define	GPIO_65		65
#define GPIO_61		61
#define GPIO_49		49
#define GPIO_117	117
#define GPIO_115	115
#define GPIO_27		27
#define GPIO_47		47


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


#define HIGH_VALUE  		1
#define LOW_VALUE   		0

/*	Defining the Directions of the Pins	*/
#define GPIO_DIR_OUT        HIGH_VALUE
#define GPIO_DIR_IN         LOW_VALUE

#define GPIO_LOW_VALUE      LOW_VALUE
#define GPIO_HIGH_VALUE     HIGH_VALUE

/*	Defining the Segment to Open or not	*/
#define SEGMENT_ON          HIGH_VALUE
#define SEGMENT_OFF         LOW_VALUE

/* This is the path corresponding to GPIOs in the 'sys' directory */
#define SYS_GPIO_PATH 		"/sys/class/gpio"

#define SYS_AINO_PATH		"/sys/bus/iio/devices/iio:device0/in_voltage0_raw" // The Path of the Analog Pin 0

#define SOME_BYTES 			100

/*
 *  GPIO read value
 */
int gpio_read_value( void )
{

    int fd;
    char value[5];
    value[5] = 0;
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

/*
 *  GPIO configure direction
 *  dir_value : 1 means 'out' , 0 means "in"
 */
int gpio_configure_dir(uint32_t gpio_num, uint8_t dir_value)
{
    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/direction", gpio_num);

    fd = open(buf, O_WRONLY);

    if (fd < 0) {
        perror("gpio direction configure\n");
        return fd;
    }

    if (dir_value)	// The direction is written to the GPIO's files
        write(fd, "out", 4); //3+1  +1 for NULL character
    else
        write(fd, "in", 3);

    close(fd);
    return 0;
}

/*
 *  GPIO write value
 *  out_value : can be either 0 or 1
 */
int gpio_write_value(uint32_t gpio_num, uint8_t out_value)
{

    int fd;
    char buf[SOME_BYTES];

    snprintf(buf, sizeof(buf), SYS_GPIO_PATH "/gpio%d/value", gpio_num);

    fd = open(buf, O_WRONLY);
    if (fd < 0) {
        perror("gpio write value\n");
        return fd;
    }

    if (out_value)
        write(fd, "1", 2);
    else
        write(fd, "0", 2);

    close(fd);
    return 0;
}

/*
 *	This function initializes all the gpios for this application
 */
int initialize_all_gpios(void)
{
	/*
	 *	The GPIOs are driven. First function sets whether it is in or output mode.
	 *	Second function turn on the LEDs by writing the values 0s and 1s
	 */

    gpio_configure_dir(GPIO_66,GPIO_DIR_OUT);
    gpio_write_value(GPIO_66, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_67,GPIO_DIR_OUT);
    gpio_write_value(GPIO_67, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_69,GPIO_DIR_OUT);
    gpio_write_value(GPIO_69, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_68,GPIO_DIR_OUT);
    gpio_write_value(GPIO_68, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_45,GPIO_DIR_OUT);
    gpio_write_value(GPIO_45, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_44,GPIO_DIR_OUT);
    gpio_write_value(GPIO_44, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_26,GPIO_DIR_OUT);
    gpio_write_value(GPIO_26, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_46,GPIO_DIR_OUT);
    gpio_write_value(GPIO_46, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_65,GPIO_DIR_OUT);
    gpio_write_value(GPIO_65, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_61,GPIO_DIR_OUT);
    gpio_write_value(GPIO_61, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_49,GPIO_DIR_OUT);
    gpio_write_value(GPIO_49, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_117,GPIO_DIR_OUT);
    gpio_write_value(GPIO_117, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_115,GPIO_DIR_OUT);
    gpio_write_value(GPIO_115, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_27,GPIO_DIR_OUT);
    gpio_write_value(GPIO_27, GPIO_LOW_VALUE );

    gpio_configure_dir(GPIO_47,GPIO_DIR_OUT);
    gpio_write_value(GPIO_47, GPIO_LOW_VALUE );

    return 0;

}

/* This function displays number on the 1st 7segment display */
void Write_number_to_7segdisplay_1(uint8_t numberToDisplay)
{
	/*
	 *	The LEDs are turned on according to the numbers to be sent to the function
	 */

    switch (numberToDisplay){

    case 0:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_OFF);
    break;

    case 1:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_OFF);
    break;

    case 2:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_OFF);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 3:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 4:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 5:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 6:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 7:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_OFF);
    break;

    case 8:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 9:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_ON);
    break;

    case 10:
        gpio_write_value(GPIO_66_P8_7_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_67_P8_8_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_69_P8_9_SEGC, SEGMENT_OFF);
        gpio_write_value(GPIO_45_P8_11_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_26_P8_14_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_46_P8_16_SEGG, SEGMENT_OFF);
    break;
    }
}

/* This function displays number on the 1st 7segment display */
void Write_number_to_7segdisplay_2(uint8_t numberToDisplay)
{

    switch (numberToDisplay){

    case 0:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_OFF);
    break;

    case 1:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_OFF);
    break;

    case 2:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_OFF);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_ON);
    break;

    case 3:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_ON);
    break;

    case 4:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_ON);
    break;

    case 5:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_ON);
    break;

    case 6:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_ON);
    break;

    case 7:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_OFF);
    break;

    case 8:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_115_P9_27_SEGE, SEGMENT_ON);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_ON);
    break;

    case 9:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_ON);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_ON);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_ON);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_ON);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_ON);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_ON);
    break;

    case 10:
        gpio_write_value(GPIO_65_P8_18_SEGA, SEGMENT_OFF);
        gpio_write_value(GPIO_61_P8_26_SEGB, SEGMENT_OFF);
        gpio_write_value(GPIO_49_P9_23_SEGC, SEGMENT_OFF);
        gpio_write_value(GPIO_117_P9_25_SEGD, SEGMENT_OFF);
        gpio_write_value(GPIO_44_P8_12_SEGE, SEGMENT_OFF);
        gpio_write_value(GPIO_27_P8_17_SEGF, SEGMENT_OFF);
        gpio_write_value(GPIO_47_P8_15_SEGG, SEGMENT_OFF);
    break;
    }
}
int main() {

	if ( initialize_all_gpios() < 0)	// If the GPIOs cannot be set, then an error is put
	{
	        printf("Error: GPIO init failed !\n");
	}
	else
	{
	    while (1)
	    {
	    	uint32_t value = gpio_read_value();	//The content of the file of analog input 0 is obtained
	    	uint32_t temp = ( ( value * 1.8 ) / 4095 ) * 100;	// The temperature value is calculated
	    	printf("The Temperature Value (in Celcius) is : %d\n", temp );
		    Write_number_to_7segdisplay_1((temp%10));	// The first segment of the temperature is displayed
		    Write_number_to_7segdisplay_2((temp/10));	// The second segment of the temperature is displayed
		    /*	suspend execution for microsecond intervals	*/
		    usleep(1000 * 1000); //converting ms to micro
	    }
	}
	return 0;
}

