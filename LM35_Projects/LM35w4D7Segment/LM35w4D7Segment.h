/*
 ============================================================================
 Name        : LM35w4D7Segment.h
 Author      : Yavuz Murat TAS
 Version     : 1.0
 ============================================================================
*/

#ifndef	_LM35w4D7Segment_H_
#define	_LM35w4D7Segment_H_

int gpio_export( uint8_t gpio_num );
int gpio_configure_dir( uint8_t gpio_num, uint8_t dir_value );
int gpio_write_value( uint8_t gpio_num, uint8_t out_value );
int gpio_read_value( void );
int initialize_all_gpios( void );
void Write_number_to_7segdisplay( uint8_t numberToDisplay );
void display_numbers( uint16_t number );

#endif	/*	_LM35w4D7Segment_H_	*/

