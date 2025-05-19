/**
* \file
* \version 1.0
* \author bazhen.paseka
* \date 2205
* \mail bazhen.info(at)gmail.com
*************************************************************************************
* \copyright	Bazhen Paseka
* \copyright	Brovary, Kyiv region
* \copyright	Ukraine
*
*
*************************************************************************************
*
* \brief
*
*/

#ifndef TM1638_LED8KEYS_SM_H_
#define TM1638_LED8KEYS_SM_H_
	
	/*
	 * 		TM1638 LED & KEYS:
	 * 	VCC		-> 3v3	power
	 * 	GND		-> GND	power
	 * 	STB		-> PB9	Output
	 * 	CLK		-> PB8	Output
	 * 	DIO		-> PB7	Output + Input
	 *
	 * 		stm32f103c8t6:
	 * 	SWDIO	-> PA13
	 * 	SWCLK	-> PA14
	 * 	debug 	-> PA2, 9600
	 * 	LED		-> PB2
	 * 	BTN		-> PA0
	 * 	8MHz	-> PD0, PD1
	 */

/*
**************************************************************************
*								INCLUDE FILES
**************************************************************************
*/
	#include <stdio.h>
 	#include <string.h>
	#include <math.h>
	#include "local_config.h"
	#include "main.h"
	#include "usart.h"
	#include "gpio.h"
/*
**************************************************************************
*								    DEFINES
**************************************************************************
*/

/*
**************************************************************************
*								   DATA TYPES
**************************************************************************
*/
	typedef enum {
		BRIGHT_0	=	0,
		BRIGHT_1	=	1,
		BRIGHT_2	=	2,
		BRIGHT_3	=	3,
		BRIGHT_4	=	4,
		BRIGHT_5	=	5,
		BRIGHT_6	=	6,
		BRIGHT_7	=	7,
	} bright_level ;

	typedef enum {
		DEC_TYPE	=	0,
		HEX_TYPE	=	1,
	} cipher_type;

/*
**************************************************************************
*								GLOBAL VARIABLES
**************************************************************************
*/

/*
**************************************************************************
*									 MACRO'S
**************************************************************************
*/
	#if DEBUG_LVL >= 1
		#define DBG1(...)  printf(__VA_ARGS__)
	#else
		#define DBG1(...)
	#endif

	#if DEBUG_LVL >= 2
		#define DBG2(...)  printf(__VA_ARGS__)
	#else
		#define DBG2(...)
	#endif

	#if DEBUG_LVL >= 3
		#define DBG3(...)  printf(__VA_ARGS__)
	#else
		#define DBG3(...)
	#endif


	#define TM1638_STB_HIGH() HAL_GPIO_WritePin(TM1638_STB_GPIO_Port, TM1638_STB_Pin, GPIO_PIN_SET)
	#define TM1638_STB_LOW()  HAL_GPIO_WritePin(TM1638_STB_GPIO_Port, TM1638_STB_Pin, GPIO_PIN_RESET)

	#define TM1638_CLK_HIGH() HAL_GPIO_WritePin(TM1638_CLK_GPIO_Port, TM1638_CLK_Pin, GPIO_PIN_SET)
	#define TM1638_CLK_LOW()  HAL_GPIO_WritePin(TM1638_CLK_GPIO_Port, TM1638_CLK_Pin, GPIO_PIN_RESET)

	#define TM1638_DIO_READ() HAL_GPIO_ReadPin (TM1638_DIO_GPIO_Port, TM1638_DIO_Pin)
	#define TM1638_DIO_HIGH() HAL_GPIO_WritePin(TM1638_DIO_GPIO_Port, TM1638_DIO_Pin, GPIO_PIN_SET)
	#define TM1638_DIO_LOW()  HAL_GPIO_WritePin(TM1638_DIO_GPIO_Port, TM1638_DIO_Pin, GPIO_PIN_RESET)
/*
**************************************************************************
*                              FUNCTION PROTOTYPES
**************************************************************************
*/
	
	void TM1638_Init		(bright_level);
	void TM1638_Clear		(void);
	void TM1638_SetDigit	(uint8_t, uint8_t, uint8_t);
	void TM1638_Set_Cipher	(uint32_t, cipher_type, uint8_t);
	int  TM1638_ReadButtons	(void);

/*
**************************************************************************
*                              		END
**************************************************************************
*/
#endif /* TM1638_LED8KEYS_SM_H_ */
