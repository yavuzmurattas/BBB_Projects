/*
 ============================================================================
 Name        : BMP180.h
 Author      : Yavuz Murat TAS
 Version     : 1.0.0
 ============================================================================
 */

#ifndef	_BMP180_H_
#define	_BMP180_H_


/*	Please Tune the Setting of the Oversampling : 0, 1, 2, 3 */
#define OVERSAMPLING_SETTING					  (0)
#define P0										  (1013.25F)
#define POWER									  (0.190295F)

#define BMP180_SHIFT_BIT_POSITION_BY_01_BIT       (1)
#define BMP180_SHIFT_BIT_POSITION_BY_02_BITS      (2)
#define BMP180_SHIFT_BIT_POSITION_BY_04_BITS      (4)
#define BMP180_SHIFT_BIT_POSITION_BY_06_BITS      (6)
#define BMP180_SHIFT_BIT_POSITION_BY_08_BITS      (8)
#define BMP180_SHIFT_BIT_POSITION_BY_11_BITS      (11)
#define BMP180_SHIFT_BIT_POSITION_BY_12_BITS      (12)
#define BMP180_SHIFT_BIT_POSITION_BY_13_BITS      (13)
#define BMP180_SHIFT_BIT_POSITION_BY_15_BITS      (15)
#define BMP180_SHIFT_BIT_POSITION_BY_16_BITS      (16)

typedef int8_t								      s8;
typedef int16_t									  s16;
typedef int32_t									  s32;
typedef int64_t									  s64;

typedef uint8_t									  u8;
typedef uint16_t								  u16;
typedef uint32_t								  u32;
typedef uint64_t								  u64;

int i2c_fd;

typedef enum OSS_VALUES
{
	OSS_VALUE_0,
	OSS_VALUE_1,
	OSS_VALUE_2,
	OSS_VALUE_3,
}OSS_VALUES_t;

#if		OVERSAMPLING_SETTING == 0
	#define	OSS_VALUE 							OSS_VALUE_0

#elif	OVERSAMPLING_SETTING == 1
	#define	OSS_VALUE							OSS_VALUE_1

#elif	OVERSAMPLING_SETTING == 2
	#define	OSS_VALUE							OSS_VALUE_2

#elif	OVERSAMPLING_SETTING == 3
	#define	OSS_VALUE							OSS_VALUE_3

#else
	#error You Made A Wrong Setting

#endif

typedef struct CAL_PARAM
{
	s16 AC1;
	s16 AC2;
	s16 AC3;
	u16 AC4;
	u16 AC5;
	u16 AC6;
	s16 B1;
	s16 B2;
	s16 MB;
	s16 MC;
	s16 MD;
}CAL_PARAM_t;

typedef struct PARAMETERS
{
	s32 X1;
	s32 X2;
	s32 X3;

	s32 B3;
	u32 B4;
	s32 B5;
	s32 B6;
	u32 B7;

	s32 UT;
	s32 UP;

	s32 T;
	s32 p;

}PARAMETERS_t;

u32 bmp180_write( u16 base_addr, u16 data );
u32 bmp180_read( u16 base_addr, s8 * veri, u8 len );
void bmp180_init( void );

void bmp180_read_calib( CAL_PARAM_t * cal_values );
/*
 *  This function used for read the calibration
 *  parameter from the register EEPROM
 *
 *  Parameter  |  MSB    |  LSB    |  bit
 * ------------|---------|---------|-----------
 *      AC1    |  0xAA   | 0xAB    | 0 to 7
 *      AC2    |  0xAC   | 0xAD    | 0 to 7
 *      AC3    |  0xAE   | 0xAF    | 0 to 7
 *      AC4    |  0xB0   | 0xB1    | 0 to 7
 *      AC5    |  0xB2   | 0xB3    | 0 to 7
 *      AC6    |  0xB4   | 0xB5    | 0 to 7
 *      B1     |  0xB6   | 0xB7    | 0 to 7
 *      B2     |  0xB8   | 0xB9    | 0 to 7
 *      MB     |  0xBA   | 0xBB    | 0 to 7
 *      MC     |  0xBC   | 0xBD    | 0 to 7
 *      MD     | 0xBE    | 0xBF    | 0 to 7
 *
 *
 */

void bmp180_read_uncompensated_temp( PARAMETERS_t * PARAMETERS );
void bmp180_read_uncompensated_press( PARAMETERS_t * PARAMETERS );
void calc_temp( PARAMETERS_t * PARAMETERS, CAL_PARAM_t * cal_values );


#endif	/* _BMP180_H_	*/
