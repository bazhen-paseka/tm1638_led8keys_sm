/**
* \file
* \version 1.0
* \author bazhen.paseka
** \date 2025
*
*************************************************************************************
* \copyright	Bazhen Paseka
* \copyright	Brovary, Kyiv region
* \copyright	Ukraine
*
*************************************************************************************
*
* \brief
*
*/

/*
**************************************************************************
*							INCLUDE FILES
**************************************************************************
*/
	#include "main.h"
	#include "usart.h"
	#include "gpio.h"
	#include "local_config.h"
	#include "tm1638_led8keys_sm.h"

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
*							LOCAL DEFINES
**************************************************************************
*/


/*
**************************************************************************
*							LOCAL CONSTANTS
**************************************************************************
*/


/*
**************************************************************************
*						    LOCAL DATA TYPES
**************************************************************************
*/


/*
**************************************************************************
*							  LOCAL TABLES
**************************************************************************
*/

/*
**************************************************************************
*								 MACRO'S
**************************************************************************
*/


/*
**************************************************************************
*						    GLOBAL VARIABLES
**************************************************************************
*/
	char debug_buffer[0xff]; // глобальний приймальний буфер

    const uint8_t SEG_TAB[] = {
    		0b00111111,	// 0		0
    		0b00000110,	// 1		1
    		0b01011011, // 2		2
			0b01001111, // 3		3
			0b01100110, // 4		4
			0b01101101, // 5		5
			0b01111101, // 6		6
			0b00000111, // 7		7
			0b01111111, // 8		8
			0b01101111, // 9		9
			0b01110111, // a		10
			0b01111100, // b		11
			0b00111001, // c		12
			0b01011110, // d		13
			0b01111001, // e		14
			0b01110001, // f		15
			0b00000001, // ^		16
			0b01000000, // -		17
			0b00001000, // _		18
			0b10000000, // _point_	19
			0b00000000, // _blank_	20
    };
    const uint8_t LED_TAB[] = {
			0x00, // Off
			0xFF, // On
    };

/*
**************************************************************************
*                        LOCAL FUNCTION PROTOTYPES
**************************************************************************
*/
	void TM1638_Dio_Output	(void);
	void TM1638_Dio_Input	(void);
	void TM1638_SendByte	(uint8_t);

/*
**************************************************************************
*                           GLOBAL FUNCTIONS
**************************************************************************
*/

/*
**************************************************************************
*                           LOCAL FUNCTIONS
**************************************************************************
*/

void TM1638_Dio_Output(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = TM1638_DIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TM1638_DIO_GPIO_Port, &GPIO_InitStruct);
} //**************************************************************************

void TM1638_Dio_Input(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = TM1638_DIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(TM1638_DIO_GPIO_Port, &GPIO_InitStruct);
} //**************************************************************************

void TM1638_SendByte(uint8_t data) {
    TM1638_Dio_Output();
    for (int i = 0; i < 8; i++) {
        TM1638_CLK_LOW();
        if (data & 0x01)
            TM1638_DIO_HIGH();
        else
            TM1638_DIO_LOW();
        data >>= 1;
        TM1638_CLK_HIGH();
    }
} //**************************************************************************

void TM1638_Init(bright_level _bright_lvl) {
    TM1638_STB_LOW();
    TM1638_SendByte(0x88 | _bright_lvl); // Включити дисплей + яскравість
    TM1638_STB_HIGH();
} //**************************************************************************

void TM1638_SetDigit(uint8_t pos, uint8_t value, uint8_t led) {
	TM1638_Init(BRIGHT_2); // захист від зависання індикації

    TM1638_STB_LOW();
    TM1638_SendByte(0x44);  // Адресна передача
    TM1638_STB_HIGH();

    TM1638_STB_LOW();
    TM1638_SendByte(0xC0 + pos*2); // Адреса позиції
    TM1638_SendByte(SEG_TAB[value]);
    TM1638_STB_HIGH();

    //HAL_Delay(1);

    TM1638_STB_LOW();
    TM1638_SendByte(0xC0 + pos*2 + 1); // Адреса позиції
    TM1638_SendByte(LED_TAB[led]); // Світлодіод: ON або OFF
    TM1638_STB_HIGH();

} //**************************************************************************

void TM1638_Set_Cipher	(uint32_t _cipher_u32, cipher_type _hex_flag, uint8_t _led_u8) {
	uint32_t division;
	if ( _hex_flag == 0) {
		division = 10;
	} else {
		division = 16;
	}

	uint8_t val_arr[8] = {0};
	uint8_t led_arr[8] = {0};

	for (uint8_t pos = 0; pos <8; pos++) {
		val_arr[7-pos] = _cipher_u32 % division;
		led_arr[7-pos] = (_led_u8 >> (pos)) & 1;
		_cipher_u32 = _cipher_u32 / division;
	}

	for (uint8_t pos = 0; pos <8; pos++) {
		TM1638_SetDigit(pos, val_arr[pos], led_arr[pos]);
	}
}  //**************************************************************************

void TM1638_Clear(void) {
	for (uint8_t pos = 0; pos <8; pos++) {
		TM1638_SetDigit(pos, 21, 0);
	}
} //**************************************************************************

int TM1638_ReadButtons(void) {
    uint8_t buttons = 0;

    TM1638_STB_LOW();
    TM1638_SendByte(0x42); // Команда зчитування
    TM1638_Dio_Input();\

    for (uint8_t i = 0; i < 4; i++) {
        uint8_t data = 0;
        for (uint8_t b = 0; b < 8; b++) {
            TM1638_CLK_LOW();
            if (TM1638_DIO_READ())
                data |= (1 << b);
            TM1638_CLK_HIGH();
        }

        // Розбір кожного біта
        if (data & 0x01) { buttons |= (1 << i);     } // S1..S4
        if (data & 0x10) { buttons |= buttons | (1 << (i+4)); } // S5..S8
    }

    TM1638_Dio_Output();
    TM1638_STB_HIGH();

    if (buttons > 0) {
		int btn = 0;
		for (uint8_t i = 0; i < 8; i++) {
			if (buttons & (1 << i)) {
				btn |= btn | i; // Кнопки нумеруються з 1 до 8
			}
		}
		return btn+1;
    }
    return -1;
} //**************************************************************************
