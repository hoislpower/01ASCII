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
#include "hex-input.h"


// Calculate the checksum of the line
// -----------------------------------------------------------------------------
// First the sum of all bytes (checksum is skipped) of the line is calculated. 
// Then the two's complement of the result is returned.
// IN hexFileLine: Data structure containing the information of the line to
//                 calculate the checksum for.
// RETURNS: The checksum of the given line.
// -----------------------------------------------------------------------------
uint8_t	calculateChecksum(hexFileLine line)
{
	int byte;
	uint8_t checksum;
	int returnValue;


	checksum = line.byteCount + line.recordType;
	checksum = checksum + ((line.address & 0xFF00) >> 8);
	checksum = checksum + (line.address & 0xFF);

	for(byte=0; byte<line.byteCount; byte++)
		checksum = checksum + line.data[byte];

	// The function always returns an int value even if the return value of
	// the function is decalred as an uint8_t.
	// This might cause problems when parsing the result of the function to
	// an integer value because the upper bits might be set.
	// So the value is first passed to an integer before returned.
	returnValue = (checksum ^ 0xFF) + 1;
	return returnValue & 0xFF;
}


// Save the hex line to the program data
// -----------------------------------------------------------------------------
// Save the data of the hex file line to programData at the appropriate
// address.
// IN line: Line to be saved.
// IN device: Description of the device whose data to be stored.
// OUT programData: Program data where to store the line.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int	saveHexLineToProgramData(hexFileLine line, deviceData *device,
							uint8_t *programData)
{
	int	byte;


	// check if the addresses are within the address boundaries
	if(device->startAddress > line.extendedAddress + line.address)
		return EXIT_FAILURE;

	if((line.extendedAddress + line.address + line.byteCount)
				>= (device->startAddress + device->memorySize))
		return EXIT_FAILURE;

	// copy bytes to programData
	for(byte=0; byte < line.byteCount; byte++)
		programData[line.extendedAddress + line.address + byte - 
					device->startAddress] = line.data[byte];

	return EXIT_SUCCESS;
}


// Convert hex data into decimal data
// -----------------------------------------------------------------------------
// Converts the hexadecimal data in hexBuffer into decimal data by combining 
// always two hex digits to one decimal digit. The result is stored in 
// decBuffer.
// The function returns a failure if one of the digits is no hexadecimal digit.
// IN hexBuffer: Array of hexadecimal data to convert.
// OUT decBuffer: Array where the decimal data is stored. The array must be at
//                least half as long as the hexBuffer array.
// IN hexBufferLength: Length of hexBuffer in hex digits.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int	convertHexToDecBuffer(uint8_t *hexBuffer, uint8_t *decBuffer, 
							uint8_t hexBufferLength)
{
	uint8_t digit;
	uint8_t offset;


	for(digit=0; digit<hexBufferLength; digit++)
	{
		// check for invalid characters
		if(isxdigit(hexBuffer[digit]) == 0)
			return EXIT_FAILURE;

		// distinguish between digits and letters
		hexBuffer[digit] = toupper(hexBuffer[digit]);
		if(isdigit(hexBuffer[digit]) != 0)
			offset = '0';
		else
			offset = 'A' - 10;

		// convert digit
		if(digit % 2)
			decBuffer[digit/2] += (hexBuffer[digit] - offset);
		else
			decBuffer[digit/2] = (hexBuffer[digit] - offset) << 4;
	}

	return EXIT_SUCCESS;
}


// Read a complete hex file and stores it into an array
// -----------------------------------------------------------------------------
// The function converts the data of the hex file into binary data and stores 
// it in programData. programData must be a byte array with at least of
// device->memorySize bytes size.
// A failure is returned if the hex is inconsistent or can not be read.
// IN fileName: Name of the hex file to be read.
// IN device: Description of the device whose data to be stored.
// OUT programData: Byte array that contains the read data.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int	readHexFile(char *fileName, deviceData *device, uint8_t *programData)
{
	int endOfFile;
	uint8_t	decBuffer[MAX_LINE_DATA_LENGTH];
	uint8_t	hexBuffer[MAX_LINE_LENGTH];
	uint32_64_t lineNumber;
	FILE *file;
	hexFileLine line;


	// open input file
	file = fopen(fileName, "rb");
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: Could not open file \"%s\"!\r\n", 
			fileName);
		return EXIT_FAILURE;
	}

	line.extendedAddress = 0;

	lineNumber = 0;
	endOfFile = false;
	while(endOfFile == false)
	{
		lineNumber++;

		// read line
		hexBuffer[0] = '\r';
		while(hexBuffer[0] == '\r' || hexBuffer[0] == '\n')
		{
			if(fgets((char *)hexBuffer, MAX_LINE_LENGTH, file) == 0)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Could not read from "
					"file \"%s\"!\r\n       Maybe an End Of"
					" File record is missing.\r\n", 
					fileName);
				return EXIT_FAILURE;
			}
		}

		// check for start code
		if(hexBuffer[0] != START_CODE)
		{
			fclose(file);
			fprintf(stderr, "ERROR: Failure in hex file \"%s\"!\r\n"
				"       No start code found at line %lu!\r\n", 
				fileName, lineNumber);
			return EXIT_FAILURE;
		}

		// convert data to decimal numbers
		if(convertHexToDecBuffer(hexBuffer+1, decBuffer, 
					LINE_HEADER_LENGTH-1) != EXIT_SUCCESS)
		{
			fclose(file);
			fprintf(stderr, "ERROR: Failure in hex file \"%s\"!\r\n"
				"       Invalid character at line %lu!\r\n", 
				fileName, lineNumber);
			return EXIT_FAILURE;
		}

		// save header data
		line.byteCount = decBuffer[BYTE_COUNT_POS];
		line.recordType = decBuffer[RECORD_TYPE_POS];
		line.address = decBuffer[ADDRESS_POS+1] + 
						(decBuffer[ADDRESS_POS] << 8);

		// convert data to decimal numbers
		if(convertHexToDecBuffer(hexBuffer + LINE_HEADER_LENGTH,
			decBuffer, (line.byteCount+1)*2) != EXIT_SUCCESS)
		{
			fclose(file);
			fprintf(stderr, "ERROR: Failure in hex file \"%s\"!\r\n"
				"       Invalid character at line %lu!\r\n", 
				fileName, lineNumber);
			return EXIT_FAILURE;
		}

		// save data and checksum
		memcpy(line.data, decBuffer, line.byteCount);
		line.checksum = decBuffer[line.byteCount];

		// check for correct checksum
		if(line.checksum != calculateChecksum(line))
		{
			fclose(file);
			fprintf(stderr, "ERROR: Failure in hex file \"%s\"!\r\n"
				"       Invalid checksum at line %lu!\r\n", 
				fileName, lineNumber);
			return EXIT_FAILURE;
		}

		// check record type and process data
		switch(line.recordType)
		{
			case DATA_RECORD:
			// save data to device structure
			if(saveHexLineToProgramData(line, device, programData) 
								!= EXIT_SUCCESS)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Failure in hex file "
					"\"%s\"!\r\n       The address at line "
					"%lu is not within the address "
					"boundaries of the device!\r\n", 
					fileName, lineNumber);
				return EXIT_FAILURE;
			}
			break;

			case END_OF_FILE_RECORD:
			// byte count must be 0x00 and address must be 0x0000
			if(line.byteCount != 0 || line.address != 0)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Failure in hex file "
					"\"%s\"!\r\n       Invalid end of file "
					"record at line %lu!\r\n", fileName, 
					lineNumber);
				return EXIT_FAILURE;
			}

			// last record -> exit loop
			endOfFile = true;
			break;

			case EXTENDED_SEGMENT_ADDRESS_RECORD:
			// byte count must be 0x02 and address must be 0x0000
			if(line.byteCount != 2 || line.address != 0)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Failure in hex file \" "
					"%s\"!\r\n       Invalid extended "
					"segment address record at line %lu!\r"
					"\n", fileName, 
					lineNumber);
				return EXIT_FAILURE;
			}

			// set the bits 4-19 of the address
			line.extendedAddress = (line.data[0] << 12) + 
							(line.data[1] << 4);
			break;

			case START_SEGMENT_ADDRESS_RECORD:
			// byte count must be 0x04 and address must be 0x0000
			if(line.byteCount != 4 || line.address != 0)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Failure in hex file "
					"\"%s\"!\r\n       Invalid start "
					"segment address record at line %lu!\r"
					"\n", fileName, 
					lineNumber);
				return EXIT_FAILURE;
			}

			// ignore start segment address record
			// (not needed for programming flash)
			fprintf(stdout, "Found \"start segment address\" "
				"record in hex file!\r\n       The record is "
				"not needed for flash programming and "
				"therefore will be ignored!\r\n");
			break;

			case EXTENDED_LINEAR_ADDRESS_RECORD:
			// byte count must be 0x02 and address must be 0x0000
			if(line.byteCount != 2 || line.address != 0)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Failure in hex file \" "
					"%s\"!\r\n       Invalid extended "
					"linear address record at line %lu!\r\n"
					, fileName, lineNumber);
				return EXIT_FAILURE;
			}

			// set the two MSBytes of the address
			line.extendedAddress = (line.data[0] << 24) + 
							(line.data[1] << 16);
			break;

			case START_LINEAR_ADDRESS_RECORD:
			// byte count must be 0x04 and address must be 0x0000
			if(line.byteCount != 4 || line.address != 0)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Failure in hex file "
					"\"%s\"!\r\n       Invalid start "
					"linear address record at line %lu!\r\n"
					, fileName, lineNumber);
				return EXIT_FAILURE;
			}

			// ignore start linear address record 
			// (not needed for programming flash)
			fprintf(stdout, "Found \"start linear address\" "
				"record in hex file!\r\n       The record is "
				"not needed for flash programming and "
				"therefore will be ignored!\r\n");
			break;

			default:
			fclose(file);
			fprintf(stderr, "ERROR: Failure in hex file \"%s\"!\r\n"
				"       Invalid record type at line %lu!\r\n",
				fileName, lineNumber);
			return EXIT_FAILURE;
		}
	}

	fclose(file);

	return EXIT_SUCCESS;
}

