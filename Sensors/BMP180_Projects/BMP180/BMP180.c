/*
 ============================================================================
 Name        : BMP180.c
 Author      : Yavuz Murat TAS
 Version     : 1.0.0
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "BMP180.h"

#define BMP180_INIT_VALUE			(0)

/* Registers */
#define BMP180_OUT_XLSB_REG			(0xF8)
#define BMP180_OUT_LSB_REG			(0xF7)
#define BMP180_OUT_MSB_REG			(0xF6)
#define BMP180_CTRL_MEAS_REG		(0xF4)
#define BMP180_SOFT_RESET_REG		(0xE0)
#define BMP180_ID_REG				(0xD0)
#define BMP180_CALIB_REG_STRT		(0xAA)
#define BMP180_CALIB_REG_END		(0xBF)

/*	EEPROM Starting Address */
#define	BMP180_PARAMETER_AC1_MSB	(0xAA)

/*	GPIO Write Function */
u32 bmp180_write( u16 base_addr, u16 data )
{
	s32 ret;
	s8 buf[2];
	buf[0] = base_addr;
	buf[1] = data;
	ret = write( i2c_fd, buf, 2 );

	if ( ret <= 0 )
	{
		perror( "ERROR : Write Failed\n" );
		abort();
	}

	return 0;
}

/*GPIO Read Function */
u32 bmp180_read( u16 base_addr, s8 * veri, u8 len )
{
	u32 ret;
	u8 buf[1];
	buf[0] = base_addr;

	ret = write( i2c_fd, buf, 1 );
	if ( ret <= 0 )
	{
		perror( "ERROR : Write Address Failed\n" );
		abort();
	}

	ret = read( i2c_fd, veri, len );
	if( ret <= 0 )
	{
		perror( "ERROR : Read Failed\n" );
		abort();
	}

	return 0;
}

/*	BMP180 Is Initialized */
void bmp180_init( void )
{
	s8 check_comm;
	bmp180_read( BMP180_ID_REG, &check_comm, 1 );

	if( check_comm != 0x55 )
	{
		perror( "ERROR : Communication is Failed!\n" );
		abort();
	}
}

/* Reading the Calibration Parameters from EEPROM of BMP180	*/
void bmp180_read_calib( CAL_PARAM_t * cal_values )
{
    //each axis value is of 2byte, so we need a buffer of 6bytes.
	s8 calib_buffer[22] = { BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
							BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
							BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
							BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
							BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE, BMP180_INIT_VALUE,
							BMP180_INIT_VALUE, BMP180_INIT_VALUE };

    //start reading from the base address of accelerometer values i.e MPU6050_REG_ACC_X_HIGH
    bmp180_read( BMP180_PARAMETER_AC1_MSB, calib_buffer, 22 );

    /* pBuffer[0]= acc x axis value , pBuffer[1]= acc y axis value , pBuffer[2]= acc z axis value  */
    cal_values->AC1 =  (s32) ( ( calib_buffer[0]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[1]  );
    cal_values->AC2 =  (s32) ( ( calib_buffer[2]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[3]  );
    cal_values->AC3 =  (s32) ( ( calib_buffer[4]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[5]  );
    cal_values->AC4 =  (u32) ( ( calib_buffer[6]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[7]  );
    cal_values->AC5 =  (u32) ( ( calib_buffer[8]  << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[9]  );
    cal_values->AC6 =  (u32) ( ( calib_buffer[10] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[11] );
    cal_values->B1  =  (s32) ( ( calib_buffer[12] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[13] );
    cal_values->B2  =  (s32) ( ( calib_buffer[14] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[15] );
    cal_values->MB  =  (s32) ( ( calib_buffer[16] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[17] );
    cal_values->MC  =  (s32) ( ( calib_buffer[18] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[19] );
    cal_values->MD  =  (s32) ( ( calib_buffer[20] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) |  calib_buffer[21] );
}

/* Reading and Calculating the Uncompensated Value of Temperature */
void bmp180_read_uncompensated_temp( PARAMETERS_t * PARAMETERS )
{
	bmp180_write( BMP180_CTRL_MEAS_REG, 0x2E );
	usleep( 4.5 * 1000 );

	s8 f6_f7_reg_val[2];
	bmp180_read( BMP180_OUT_MSB_REG,  f6_f7_reg_val, 2 );

	PARAMETERS->UT = (s32)( ( f6_f7_reg_val[0] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) | f6_f7_reg_val[1] );

}

/* Reading and Calculating the Uncompensated Value of Pressure */
void bmp180_read_uncompensated_press( PARAMETERS_t * PARAMETERS )
{
	s8 f6_f7_f8_reg_val[3];

	s16 val = ( s16 ) ( 0x34 | ( OSS_VALUE << BMP180_SHIFT_BIT_POSITION_BY_06_BITS ) );
	bmp180_write( BMP180_CTRL_MEAS_REG, val );

	#if 	OSS_VALUE == 0
		usleep( 4.5 * 1000 );

	#elif	OSS_VALUE == 1
		usleep( 7.5 * 1000 );

	#elif 	OSS_VALUE == 2
		usleep( 13.5 * 1000 );

	#elif 	OSS_VALUE == 3
		usleep( 25.5 * 1000 );

	#warning OSS Value is wrong

	#endif

	bmp180_read( BMP180_OUT_MSB_REG, f6_f7_f8_reg_val, 3 );

	PARAMETERS->UP = (s32)( (s32)(( f6_f7_f8_reg_val[0] << BMP180_SHIFT_BIT_POSITION_BY_16_BITS ) | ( f6_f7_f8_reg_val[1] << BMP180_SHIFT_BIT_POSITION_BY_08_BITS ) | f6_f7_f8_reg_val[2] ) >> ( 8 - OSS_VALUE ) );
}

/* Calculating the Value of Temperature */
void calc_temp( PARAMETERS_t * PARAMETERS, CAL_PARAM_t * cal_values )
{
	PARAMETERS->X1 = ( ( PARAMETERS->UT - (s32)cal_values->AC6 ) * (s32)cal_values->AC5 ) >> BMP180_SHIFT_BIT_POSITION_BY_15_BITS;
	PARAMETERS->X2 = ( (s32)cal_values->MC << BMP180_SHIFT_BIT_POSITION_BY_11_BITS ) / ( PARAMETERS->X1 + cal_values->MD );
	PARAMETERS->B5 = PARAMETERS->X1 + PARAMETERS->X2;

	PARAMETERS->T = ( PARAMETERS->B5 + 8 ) >> BMP180_SHIFT_BIT_POSITION_BY_04_BITS;
}

/* Calculating the Value of Pressure */
void calc_press( PARAMETERS_t * PARAMETERS, CAL_PARAM_t * cal_values )
{
	PARAMETERS->B6 = PARAMETERS->B5 - 4000;

	PARAMETERS->X1 = ( (s32)cal_values->B2 * ( ( PARAMETERS->B6 * PARAMETERS->B6 ) >> BMP180_SHIFT_BIT_POSITION_BY_12_BITS ) ) >> BMP180_SHIFT_BIT_POSITION_BY_11_BITS;
	PARAMETERS->X2 = ( (s32)cal_values->AC2 * PARAMETERS->B6 ) >> BMP180_SHIFT_BIT_POSITION_BY_11_BITS;
	PARAMETERS->X3 = PARAMETERS->X1 + PARAMETERS->X2;
	PARAMETERS->B3 = ( ( ( ( ( (s32)cal_values->AC1 ) * 4 + PARAMETERS->X3 ) << OSS_VALUE ) + 2 ) >> BMP180_SHIFT_BIT_POSITION_BY_02_BITS );

	PARAMETERS->X1 = ( cal_values->AC3 * PARAMETERS->B6 ) >> BMP180_SHIFT_BIT_POSITION_BY_13_BITS;
	PARAMETERS->X2 = ( cal_values->B1 * ( ( PARAMETERS->B6 * PARAMETERS->B6 ) >> BMP180_SHIFT_BIT_POSITION_BY_12_BITS ) ) >> BMP180_SHIFT_BIT_POSITION_BY_16_BITS;
	PARAMETERS->X3 = ( ( PARAMETERS->X1 + PARAMETERS->X2 ) + 2 ) >> BMP180_SHIFT_BIT_POSITION_BY_02_BITS;
	PARAMETERS->B4 = ( (u32)cal_values->AC4 * (u32)( PARAMETERS->X3 + 32768 ) ) >> BMP180_SHIFT_BIT_POSITION_BY_15_BITS;

	PARAMETERS->B7 = ( (u32)(PARAMETERS->UP - PARAMETERS->B3 ) * ( 50000 >> OSS_VALUE ) );

	if( PARAMETERS->B7 < 0x80000000 )
	{
		PARAMETERS->p = ( PARAMETERS->B7 * 2 ) / PARAMETERS->B4;
	}

	else
	{
		PARAMETERS->p = ( PARAMETERS->B7 / PARAMETERS->B4 ) << BMP180_SHIFT_BIT_POSITION_BY_01_BIT;
	}

	PARAMETERS->X1 = ( PARAMETERS->p >> BMP180_SHIFT_BIT_POSITION_BY_08_BITS );
	PARAMETERS->X1 *= PARAMETERS->X1;
	PARAMETERS->X1 = ( PARAMETERS->X1 * 3038 ) >> BMP180_SHIFT_BIT_POSITION_BY_16_BITS;
	PARAMETERS->X2 = ( PARAMETERS->p * (-7357) ) >> BMP180_SHIFT_BIT_POSITION_BY_16_BITS;

	PARAMETERS->p  = PARAMETERS->p + ( ( PARAMETERS->X1 + PARAMETERS->X2 + 3791 ) >> BMP180_SHIFT_BIT_POSITION_BY_04_BITS );

	#if	OSS_VALUE == 1
		PARAMETERS->p /= 2;

	#elif 	OSS_VALUE == 2
		PARAMETERS->p /= 4;

	#elif 	OSS_VALUE == 3
		PARAMETERS->p /= 8;

	#warning OSS Value is wrong

	#endif
}
