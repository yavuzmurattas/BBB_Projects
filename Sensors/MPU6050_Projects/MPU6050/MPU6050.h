/*============================================================================
 Name        : MPU6050.c
 Author      : Yavuz Murat TAS
 Version     : 1.0.0
 ============================================================================*/

#ifndef	_MPU6050_H_
#define	_MPU6050_H_

/*	Set the AD0 Pin Position ( If It is Grounded, Then Write 0, or If It is Sourced, Then Write 1 )	*/
#define	AD0_PIN_POSITION				0

#if		AD0_PIN_POSITION == 0
	#define MPU6050_SLAVE_ADDR			0x68

#elif	AD0_PIN_POSITION == 1
	#define MPU6050_SLAVE_ADDR			0x69

#else
	#error You Made A Wrong Setting
#endif

/*	MPU6050 Gyroscope Full Scale (FS) Sensitivity Settings	*/
#define GYRO_FS_SENSITIVITY_0			131
#define GYRO_FS_SENSITIVITY_1			65.5
#define GYRO_FS_SENSITIVITY_2			32.8
#define GYRO_FS_SENSITIVITY_3			16.4

/*	MPU6050 Acceleration Full Scale (FS) Sensitivity Settings	*/
#define ACCEL_FS_SENSITIVITY_0			16384
#define ACCEL_FS_SENSITIVITY_1			8192
#define ACCEL_FS_SENSITIVITY_2			4096
#define ACCEL_FS_SENSITIVITY_3			2048


#define	GYRO_SENSITIVITY_SETTING		0
#define	ACCEL_SENSITIVITY_SETTING 		0

/*	Gyroscope Sensitivity and Gyroscope Full Scale (FS) Sensitivity Is Determined	*/
enum GYRO_SENSITIVITY
{
	FS_SEL_0,
	FS_SEL_1,
	FS_SEL_2,
	FS_SEL_3
};

#if		GYRO_SENSITIVITY_SETTING == 0
	#define	GYRO_SENSITIVITY_SETTING 	FS_SEL_0
	#define GYRO_FS_SENSITIVITY			GYRO_FS_SENSITIVITY_0

#elif	GYRO_SENSITIVITY_SETTING == 1
	#define	GYRO_SENSITIVITY_SETTING	FS_SEL_1
	#define GYRO_FS_SENSITIVITY			GYRO_FS_SENSITIVITY_1

#elif	GYRO_SENSITIVITY_SETTING == 2
	#define	GYRO_SENSITIVITY_SETTING	FS_SEL_2
	#define GYRO_FS_SENSITIVITY			GYRO_FS_SENSITIVITY_2

#elif	GYRO_SENSITIVITY_SETTING == 3
	#define	GYRO_SENSITIVITY_SETTING	FS_SEL_3
	#define GYRO_FS_SENSITIVITY			GYRO_FS_SENSITIVITY_3

#else
	#error You Made A Wrong Setting

#endif

/*	Acceleration Sensitivity and Acceleration Full Scale (FS) Sensitivity Is Determined	*/
enum ACCEL_SENSITIVITY
{
	AFS_SEL_0,
	AFS_SEL_1,
	AFS_SEL_2,
	AFS_SEL_3
};

#if		ACCEL_SENSITIVITY_SETTING == 0
	#define	ACCEL_SENSITIVITY_SETTING 	AFS_SEL_0
	#define ACCEL_FS_SENSITIVITY		ACCEL_FS_SENSITIVITY_0

#elif	ACCEL_SENSITIVITY_SETTING == 1
	#define	ACCEL_SENSITIVITY_SETTING	AFS_SEL_1
	#define ACCEL_FS_SENSITIVITY		ACCEL_FS_SENSITIVITY_1

#elif	ACCEL_SENSITIVITY_SETTING == 2
	#define	ACCEL_SENSITIVITY_SETTING	AFS_SEL_2
	#define ACCEL_FS_SENSITIVITY		ACCEL_FS_SENSITIVITY_2

#elif	ACCEL_SENSITIVITY_SETTING == 3
	#define	ACCEL_SENSITIVITY_SETTING	AFS_SEL_3
	#define ACCEL_FS_SENSITIVITY		ACCEL_FS_SENSITIVITY_3

#else
	#error You Made A Wrong Setting

#endif

typedef uint8_t							u8;
typedef uint16_t						u16;
typedef uint32_t						u32;

typedef int8_t							s8;
typedef int16_t							s16;
typedef int32_t							s32;

u32 mpu6050_write( u16 base_addr, u16 data );
u32 mpu6050_read( s16 base_addr, s8 * veri, u8 len );
void mpu6050_init( void );
void mpu6050_read_acc( s16 * acc_values );
void mpu6050_read_gyro( s16 * gyro_values );

#endif	/* _MPU6050_H_	*/
