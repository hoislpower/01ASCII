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



// Datatype for device data
// -----------------------------------------------------------------------------
// The datatype contains all neccessary information about a device to
// generate program data.
// -----------------------------------------------------------------------------
typedef struct
{
	char name[MAX_DEVICE_FILENAME_LENGTH];

	// size of memory and blocks in bytes
	uint32_64_t memorySize;
	uint32_64_t blockSize;

	// code start address
	uint32_64_t startAddress;

	// the address is incremented by this value when accessing the next word (default is 1)
	uint8_t addressStepPerWord;

	// lengths of words and addresses in bits
	uint8_t wordLength;
	uint8_t addressLength;

	// data and address bit orders ([2] -> program and verify)
	int8_t wordBitOrder[2][MAX_BIT_ORDER_LENGTH];
	int8_t wordAddressBitOrder[2][MAX_BIT_ORDER_LENGTH];
	int8_t blockAddressBeforeDataBitOrder[2][MAX_BIT_ORDER_LENGTH];
	int8_t blockAddressAfterDataBitOrder[2][MAX_BIT_ORDER_LENGTH];
}
deviceData;

