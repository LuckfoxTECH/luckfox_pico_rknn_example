/*****************************************************************************
* | File      	:   DEV_Config.h
* | Author      :   Luckfox team
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2023-09-05
* | Info        :   Basic version
*
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include "Debug.h"

#define USE_DEV_LIB 1

#ifdef USE_DEV_LIB
    #include "sysfs_gpio.h"
    #include "dev_hardware_SPI.h"
	#include <stdio.h>
	#include <unistd.h>
	#include <stdlib.h>
	#include <pthread.h>
	// #define USE_DEV_LIB_PWM
#endif
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define  MAX_BMP_FILES  25 

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

//GPIO
//Plus
#ifdef  LUCKFOX_PICO_PLUS 
#define LCD_DC_PIN   (34)
#define LCD_CS_PIN   (48)
#define LCD_RST_PIN  (57)
#define LCD_BL_PIN   (4) 
#define TP_CS_PIN    (102)
#define TP_IRQ_PIN   (103)
#define SD_CS_PIN    (97)

#else

#define LCD_DC_PIN   (73)
#define LCD_CS_PIN   (48)
#define LCD_RST_PIN  (57)
#define LCD_BL_PIN   (72) 
#define TP_CS_PIN    (68)
#define TP_IRQ_PIN   (69)
#define SD_CS_PIN    (70)

#endif

//LCD
#define LCD_CS_0		DEV_Digital_Write(LCD_CS_PIN,0)
#define LCD_CS_1		DEV_Digital_Write(LCD_CS_PIN,1)

#define LCD_RST_0		DEV_Digital_Write(LCD_RST_PIN,0)
#define LCD_RST_1		DEV_Digital_Write(LCD_RST_PIN,1)

#define LCD_DC_0		DEV_Digital_Write(LCD_DC_PIN,0)
#define LCD_DC_1		DEV_Digital_Write(LCD_DC_PIN,1)

#define LCD_BL_0		DEV_Digital_Write(LCD_BL_PIN,0)
#define LCD_BL_1		DEV_Digital_Write(LCD_BL_PIN,1)

//TP
#define TP_CS_0		DEV_Digital_Write(TP_CS_PIN,0)
#define TP_CS_1		DEV_Digital_Write(TP_CS_PIN,1)

//SD
#define SD_CS_0		DEV_Digital_Write(SD_CS_PIN,0)
#define SD_CS_1		DEV_Digital_Write(SD_CS_PIN,1)


#define LCD_SetBacklight(Value) DEV_SetBacklight(Value)

/*------------------------------------------------------------------------------------------------------*/
UBYTE DEV_ModuleInit(void);
void DEV_ModuleExit(void);

void DEV_GPIO_Mode(UWORD Pin, UWORD Mode);
void DEV_Digital_Write(UWORD Pin, UBYTE Value);
UBYTE DEV_Digital_Read(UWORD Pin);
void DEV_Delay_ms(UDOUBLE xms);
void DEV_Delay_us(UDOUBLE xus);

void DEV_SPI_WriteByte(UBYTE Value);
void DEV_SPI_Write_nByte(uint8_t *pData, uint32_t Len);
void DEV_SetBacklight(UWORD Value);
#endif
