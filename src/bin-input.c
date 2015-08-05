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



#include "input.h"


// Reads a complete bin file and store it into an array
// -----------------------------------------------------------------------------
// The content of the file is copied to programData as a whole.
// programData must be a byte array with at least device->memorySize bytes size.
// IN fileName: Name of the file to be read.
// IN device: Description of the device whose data should be read.
// OUT programData: Byte array that contains the read data.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int	readBinFile(char *fileName, deviceData *device, uint8_t *programData)
{
	uint32_64_t readBytes;
	FILE *file;


	// initialize memory with 0xFF
	memset(programData, 0xFF, device->memorySize);

	// open input file
	file = fopen(fileName, "rb");
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: Could not open file \"%s\"!\r\n", 
			fileName);
		return EXIT_FAILURE;
	}

	// read in the complete device memory
	readBytes = fread(programData, sizeof(*programData), 
						device->memorySize, file);

	fclose(file);

	// readBytes will be less than device->memorySize when the binary
	// file doesn't fill the complete memory of the device (almost always)
	// check if at least 1 byte could be read 
	if(readBytes < 1)
	{
		fprintf(stderr, "ERROR: Could not read from file \"%s\"!\r\n", 
			fileName);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

