/*
 ============================================================================
 Name        : LM35w7Segment.h
 Author      : Yavuz Murat TAS
 Version     : 2.0
 ============================================================================
*/

#ifndef	_LM35w7Segment_H_
#define	_LM35w7Segment_H_

#include <stdint.h>

int gpio_export( uint8_t gpio_num );
int gpio_read_value( void );
int gpio_configure_dir( uint8_t gpio_num, uint8_t dir_value );
int gpio_write_value( uint8_t gpio_num, uint8_t out_value );
int initialize_all_gpios( void );
void Write_number_to_7segdisplay_1( uint8_t numberToDisplay );
void Write_number_to_7segdisplay_2( uint8_t numberToDisplay );

#endif	/* _LM35w7Segment_H_ */

