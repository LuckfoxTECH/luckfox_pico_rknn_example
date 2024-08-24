/*****************************************************************************
* | File      	:	Debug.h
* | Author      :   Luckfox team
* | Function    :	debug with printf
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2023-09-05
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>

#define USE_DEBUG 1
#if USE_DEBUG
	#define DEBUG(__info,...) printf("Debug : " __info,##__VA_ARGS__)
#else
	#define DEBUG(__info,...)  
#endif

#endif//__DEBUG_H

