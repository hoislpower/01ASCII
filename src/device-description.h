/*
    01ASCII

    Converter for hexadecimal or binary files into textfiles containing 
    the data in form of ASCII 0s and 1s.

    Authored By Patrick Hoislbauer  <patrick.hoislbauer@gmail.com>

    Copyright (C) 2015 Patrick Hoislbauer

    This file is part of 01ASCII.

    01ASCII is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    01ASCII is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 01ASCII.  If not, see <http://www.gnu.org/licenses/>.

    Diese Datei ist Teil von 01ASCII.

    01ASCII ist Freie Software: Sie können es unter den Bedingungen
    der GNU General Public License, wie von der Free Software Foundation,
    Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
    veröffentlichten Version, weiterverbreiten und/oder modifizieren.

    01ASCII wird in der Hoffnung, dass es nützlich sein wird, aber
    OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
    Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
    Siehe die GNU General Public License für weitere Details.

    Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
    Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
*/



#ifndef _DEVICE_DESCRIPTION_H
#define _DEVICE_DESCRIPTION_H

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


// define maximum devicename length
#define MAX_DEVICE_NAME_LENGTH	1024


// Symbols for bit order arrays
#define UNUSED_BIT	-1
#define LITERAL0_BIT	-2
#define LITERAL1_BIT	-3


// define maximum lengths of words and bit orders
#define MAX_WORD_LENGTH32	(8*sizeof(uint32_t))
#define MAX_WORD_LENGTH64	(8*sizeof(uint64_t))
#define MAX_BIT_ORDER_LENGTH32	MAX_WORD_LENGTH32
#define MAX_BIT_ORDER_LENGTH64	MAX_WORD_LENGTH64


// define default values for device data attributes
#define DEFAULT_START_ADDRESS		0
#define DEFAULT_ADDRESS_STEP_PER_WORD	1
#define DEFAULT_WORD_LENGTH		8
#define DEFAULT_ADDRESS_LENGTH		8


// declare 2 deviceData structures for 32 and 64 bit
#define uint32_64_t uint32_t
#define deviceData deviceData32
#define MAX_BIT_ORDER_LENGTH	MAX_BIT_ORDER_LENGTH32
#include "device-data.h"
#undef uint32_64_t
#undef deviceData
#undef MAX_BIT_ORDER_LENGTH

#define uint32_64_t uint64_t
#define deviceData deviceData64
#define MAX_BIT_ORDER_LENGTH	MAX_BIT_ORDER_LENGTH64
#include "device-data.h"
#undef uint32_64_t
#undef deviceData
#undef MAX_BIT_ORDER_LENGTH

// define datatype for 32 or 64 bit depending on the definition of USING_64BIT
#if(USING_64BIT==1)
	#define uint32_64_t	uint64_t
	#define deviceData	deviceData64
	#define MAX_BIT_ORDER_LENGTH	MAX_BIT_ORDER_LENGTH64
#else
	#define uint32_64_t	uint32_t
	#define deviceData	deviceData32
	#define MAX_BIT_ORDER_LENGTH	MAX_BIT_ORDER_LENGTH32
#endif /* USING_64BIT */


enum {PROGRAM, VERIFY, PROGRAM_VERIFY};


// version of deviceData for checking compatibility with files
extern const uint8_t deviceDataVersion;
extern const uint8_t maxWordLength;


extern	uint8_t	bitOrderLength(int8_t *);
extern	int	bitOrderIsEmpty(int8_t *);
extern	int	bitOrdersAreEqual(int8_t *, int8_t *);
extern	int	programAndVerfiyBitOrdersAreEqual(deviceData *);
extern	void	printDeviceDescription(deviceData *);
extern	void	initializeDeviceData(deviceData *);
extern	int	saveDeviceDescription(deviceData *, char *);
extern	int	loadDeviceDescription(deviceData *, char *);

#endif /* _DEVICE_DESCRIPTION_H */

