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



#ifndef _HEX_INPUT_H
#define _HEX_INPUT_H


#define START_CODE				':'

#define DATA_RECORD				0
#define	END_OF_FILE_RECORD			1
#define	EXTENDED_SEGMENT_ADDRESS_RECORD		2
#define	START_SEGMENT_ADDRESS_RECORD		3
#define	EXTENDED_LINEAR_ADDRESS_RECORD		4
#define	START_LINEAR_ADDRESS_RECORD		5
#define MAX_RECORD_TYPE				5

#define	LINE_HEADER_LENGTH			9

#define BYTE_COUNT_POS				0
#define ADDRESS_POS				1
#define RECORD_TYPE_POS				3


#define MAX_LINE_DATA_LENGTH			255
#define MAX_LINE_LENGTH		(MAX_LINE_DATA_LENGTH+LINE_HEADER_LENGTH+1)*2


// datatype for storing information of one hex file line
typedef struct
{
	uint8_t	byteCount;
	uint8_t	recordType;
	uint8_t checksum;
	uint32_t address;
	uint32_t extendedAddress;

	uint8_t	data[MAX_LINE_DATA_LENGTH];
}
hexFileLine;


#endif /* _HEX_INPUT_H */

