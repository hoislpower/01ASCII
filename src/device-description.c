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



#include "device-description.h"


// version of deviceData for checking compatibility with files
const uint8_t deviceDataVersion = 0x01;

// maximum length of address and data words in bytes
const uint8_t maxWordLength = sizeof(uint32_64_t)*8;


// Check whether two BitOrder arrays are equal or not.
// -----------------------------------------------------------------------------
// The function compares until the end of the BitOrder array 
// (MAX_BIT_ORDER_LENGTH) or until the first occurrence of -1 which is the 
// termination symbol of BitOrder arrays.
// IN bitOrder1: Array to compare
// IN bitOrder2: Array to compare
// RETURNS: true if the bit orders are equal, false otherwise.
// -----------------------------------------------------------------------------
int8_t	bitOrdersAreEqual(int8_t *bitOrder1, int8_t *bitOrder2)
{
	uint8_t i;


	for(i=0; i<MAX_BIT_ORDER_LENGTH; i++)
	{
		// compare bit by bit
		if(bitOrder1[i] != bitOrder2[i])
			return false;

		// exit on the first occurrence of -1
		// -> termination of the BitOrder
		if(bitOrder1[i] == BIT_ORDER_END)
			return true;
	}

	return true;
}


// Print a BitOrder array
// -----------------------------------------------------------------------------
// Print the content of the array until -1 (= end of bit order) was found.
// IN bitOrder: Array to print
// -----------------------------------------------------------------------------
void	printBitOrder(int8_t bitOrder[])
{
	uint16_t i;


	// start line with 3 spaces
	printf("   ");

	// print all but one bits
	i=0;
	while((bitOrder[i+1] >= 0) && (i+1 < MAX_BIT_ORDER_LENGTH))
	{
		// print bit by bit separated by a comma and a space
		printf("%i, ", bitOrder[i]);
		i++;
	}
	// print the last bit
	if(bitOrder[i] >= 0)
	{
		printf("%i\r\n", bitOrder[i]);
	}
}


// Display the content of the deviceData structure
// -----------------------------------------------------------------------------
// Print the content of each element in the deviceData structure.
// IN deviceData: Data to be printed.
// -----------------------------------------------------------------------------
void	printDeviceDescription(deviceData device)
{
	printf("device: %s\r\n\r\n", device.name);
	printf("memory size:        %lu bytes\r\n", device.memorySize);
	printf("block size:         %lu bytes\r\n", device.blockSize);
	printf("code start address  %lu bytes\r\n", device.startAddress);
	printf("word length:        %u bits\r\n", device.wordLength);
	printf("address length:     %u bits\r\n", device.addressLength);
	printf("addresses per word: %u\r\n\r\n", device.addressStepPerWord);


	printf("program word bit order: \r\n");
	printBitOrder(device.wordBitOrder[PROGRAM]);
	printf("program word address bit order: \r\n");
	printBitOrder(device.wordAddressBitOrder[PROGRAM]);
	printf("program block address bit order before data: \r\n");
	printBitOrder(device.preDataBlockAddrBitOrder[PROGRAM]);
	printf("program block address bit order after data: \r\n");
	printBitOrder(device.postDataBlockAddrBitOrder[PROGRAM]);
	printf("verify word bit order: \r\n");
	printBitOrder(device.wordBitOrder[VERIFY]);
	printf("verify word address bit order: \r\n");
	printBitOrder(device.wordAddressBitOrder[VERIFY]);
	printf("verify block address bit order before data: \r\n");
	printBitOrder(device.preDataBlockAddrBitOrder[VERIFY]);
	printf("verify block address bit order after data: \r\n");
	printBitOrder(device.postDataBlockAddrBitOrder[VERIFY]);
}


// Initialize a deviceData structure.
// -----------------------------------------------------------------------------
// This procedure must be called once before using any deviceData structure.
// IN device: deviceData structure to initialize
// -----------------------------------------------------------------------------
void	initializeDeviceData(deviceData *device)
{
	uint8_t i;


	// set all characters of the name array to '\0'
	strncpy(device->name, "", MAX_DEVICE_NAME_LENGTH);

	// set other attributes to default values
	device->memorySize = 0;
	device->blockSize = 0;
	device->startAddress = DEFAULT_START_ADDRESS;
	device->addressStepPerWord = DEFAULT_ADDRESS_STEP_PER_WORD;
	device->wordLength = DEFAULT_WORD_LENGTH;
	device->addressLength = DEFAULT_ADDRESS_LENGTH;

	// set all elements of the bit order arrays to -1
	for(i=0; i<MAX_BIT_ORDER_LENGTH; i++)
	{
		device->wordBitOrder[PROGRAM][i] = BIT_ORDER_END;
		device->wordBitOrder[VERIFY][i] = BIT_ORDER_END;
		device->wordAddressBitOrder[PROGRAM][i] = BIT_ORDER_END;
		device->wordAddressBitOrder[VERIFY][i] = BIT_ORDER_END;
		device->preDataBlockAddrBitOrder[PROGRAM][i] = BIT_ORDER_END;
		device->preDataBlockAddrBitOrder[VERIFY][i] = BIT_ORDER_END;
		device->postDataBlockAddrBitOrder[PROGRAM][i] = BIT_ORDER_END;
		device->postDataBlockAddrBitOrder[VERIFY][i] = BIT_ORDER_END;
	}
}


// Save the deviceData to a file.
// -----------------------------------------------------------------------------
// Save the device description to a file.
// The first two bytes of the file are the deviceDataVersion and the 
// maxWordLength. The rest of the structure is written as a whole.
// IN device: Device description to be written to a file.
// IN fileName: Name of the file to be written.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int8_t	saveDeviceDescription(deviceData *device, char *fileName)
{
	uint32_t writtenBytes;
	FILE *file;


	// create the file
	file = fopen(fileName, "wb+");
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: Could not create device file \
			\"%s\"!\r\n", fileName);
		return EXIT_FAILURE;
	}

	// write the version of the deviceData structure
	if(fwrite(&deviceDataVersion, sizeof(uint8_t), 1, file) != 1)
	{
		fclose(file);
		fprintf(stderr, "ERROR: Could not write the version of the \
			device file \"%s\"!\r\n", fileName);
		return EXIT_FAILURE;
	}

	// write the maximum word length (32 or 64 bit)
	if(fwrite(&maxWordLength, sizeof(uint8_t), 1, file) != 1)
	{
		fclose(file);
		fprintf(stderr, "ERROR: Could not write the maximum word \
			length to the device file \"%s\"!\r\n", fileName);
		return EXIT_FAILURE;
	}

	// write the complete structure to the file
	writtenBytes = fwrite(device, sizeof(uint8_t), sizeof(deviceData),file);
	fclose(file);

	if(writtenBytes != sizeof(deviceData))
	{
		fprintf(stderr, "ERROR: Could not write to device file \"%s\"! \
			 (wrote %u bytes)\r\n", fileName, writtenBytes);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


// Load the deviceData from a file.
// -----------------------------------------------------------------------------
// Load the deviceData structure with the content of a device description file.
// The function tries to load the file no matter if it is a 32 or 64 bit file.
// The function exits with a failure if the current structure is 32 bit, 
// the file is 64 bit and the data of the file does not fit into a 32 bit 
// structure.
// OUT device: Device description read out from the file.
// IN fileName: Name of the file to be read.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int8_t	loadDeviceDescription(deviceData *device, char *fileName)
{
	uint8_t	fileMaxBitOrderLength;
	uint8_t fileVersion;
	uint8_t fileWordLength;
	uint8_t	readBuffer[sizeof(deviceData64)];
	uint8_t *readPointer;
	uint32_t readBytes;
	uint64_t upper4Bytes;
	FILE *file;


	// initialize the device data structure
	initializeDeviceData(device);

	// open the file
	file = fopen(fileName, "rb");
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: Could not open device file \
			\"%s\"!\r\n", fileName);
		return EXIT_FAILURE;
	}

	// read the version of the file
	if(fread(&fileVersion, sizeof(uint8_t), 1, file) != 1)
	{
		fclose(file);
		fprintf(stderr, "ERROR: Could not read the version of the \
			device file \"%s\"!\r\n", fileName);
		return EXIT_FAILURE;
	}

	// compare the version of the file with the current deviceData version
	if(fileVersion != deviceDataVersion)
	{
		fclose(file);
		printf("The device file \"%s\" has a wrong version and could \
			not be loaded!\r\n", fileName);
		return EXIT_FAILURE;
	}

	// read the maximum word length (32 or 64 bit)
	if(fread(&fileWordLength, sizeof(uint8_t), 1, file) != 1)
	{
		fclose(file);
		fprintf(stderr, "ERROR: Could not read the maximum word length \
			from the device file \"%s\"!\r\n", fileName);
		return EXIT_FAILURE;
	}

	// check for a valid maximum word length
	if(fileWordLength != MAX_WORD_LENGTH32 && 
	   fileWordLength != MAX_WORD_LENGTH64)
	{
		fclose(file);
		fprintf(stderr, "ERROR: The device file \"%s\" has an invalid \
			maximum word length of %u and could not be read!\r\n", 
			fileName, fileWordLength);
		return EXIT_FAILURE;
	}


	// read the complete structure from the file
	if(fileWordLength == MAX_WORD_LENGTH32)
	{
		// read a 32 bit file
		readBytes = fread(readBuffer, sizeof(uint8_t), 
						sizeof(deviceData32), file);
		if(readBytes != sizeof(deviceData32))
		{
			fclose(file);
			fprintf(stderr, "ERROR: Could not read from device \
				file \"%s\"! (read %u bytes of %lu)\r\n", 
				fileName, readBytes, sizeof(deviceData32));
			return EXIT_FAILURE;
		}

		fileMaxBitOrderLength = MAX_BIT_ORDER_LENGTH32;
	}
	else
	{
		// read a 64 bit file
		readBytes = fread(readBuffer, sizeof(uint8_t), 
						sizeof(deviceData64), file);
		if(readBytes != sizeof(deviceData64))
		{
			fclose(file);
			fprintf(stderr, "ERROR: Could not read from device \
				file \"%s\"! (read %u bytes of %lu)\r\n", 
				fileName, readBytes, sizeof(deviceData64));
			return EXIT_FAILURE;
		}

		fileMaxBitOrderLength = MAX_BIT_ORDER_LENGTH64;
	}

	// Filename
	strncpy(device->name, (char *) readBuffer, MAX_DEVICE_NAME_LENGTH);
	readPointer = readBuffer + MAX_DEVICE_NAME_LENGTH;

	// memorySize
	device->memorySize = *((uint32_t *) readPointer);
	readPointer = readPointer + sizeof(uint32_t);
	if(fileWordLength == MAX_WORD_LENGTH64)
	{
		// read the upper 32 bit
		upper4Bytes = *((uint32_t *) readPointer);

		// Check if the upper 4 bytes are 0
		if(maxWordLength == MAX_WORD_LENGTH32 && upper4Bytes != 0)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			device->memorySize = device->memorySize + 
					(upper4Bytes << (sizeof(uint32_t)*8));
			readPointer = readPointer + sizeof(uint32_t);
		}
	}

	// blockSize
	device->blockSize = *((uint32_t *) readPointer);
	readPointer = readPointer + sizeof(uint32_t);
	if(fileWordLength == MAX_WORD_LENGTH64)
	{
		// read the upper 32 bit
		upper4Bytes = *((uint32_t *) readPointer);

		// Check if the upper 4 bytes are 0
		if(maxWordLength == MAX_WORD_LENGTH32 && upper4Bytes != 0)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			device->blockSize = device->blockSize + 
					(upper4Bytes << (sizeof(uint32_t)*8));
			readPointer = readPointer + sizeof(uint32_t);
		}
	}

	// startAddress
	device->startAddress = *((uint32_t *) readPointer);
	readPointer = readPointer + sizeof(uint32_t);
	if(fileWordLength == MAX_WORD_LENGTH64)
	{
		// read the upper 32 bit
		upper4Bytes = *((uint32_t *) readPointer);

		// Check if the upper 4 bytes are 0
		if(maxWordLength == MAX_WORD_LENGTH32 && upper4Bytes != 0)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			device->startAddress = device->startAddress + 
					(upper4Bytes << (sizeof(uint32_t)*8));
			readPointer = readPointer + sizeof(uint32_t);
		}
	}

	// addressStepPerWord
	device->addressStepPerWord = (uint8_t) *readPointer;
	readPointer = readPointer + sizeof(uint8_t);

	// wordLength
	device->wordLength = (uint8_t) *readPointer;
	readPointer = readPointer + sizeof(uint8_t);

	// addressLength
	device->addressLength = (uint8_t) *readPointer;
	readPointer = readPointer + sizeof(uint8_t);

	// wordBitOrder PROGRAM
	if(fileWordLength > maxWordLength)
	{
		if((int8_t) readPointer[maxWordLength] != BIT_ORDER_END)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			memcpy(device->wordBitOrder[PROGRAM], readPointer, 
							MAX_BIT_ORDER_LENGTH);
		}
	}
	else
	{
		memcpy(device->wordBitOrder[PROGRAM], readPointer, 
							fileMaxBitOrderLength);
	}
	readPointer = readPointer + fileMaxBitOrderLength;

	// wordBitOrder VERIFY
	if(fileWordLength > maxWordLength)
	{
		if((int8_t) readPointer[maxWordLength] != BIT_ORDER_END)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			memcpy(device->wordBitOrder[VERIFY], readPointer, 
							MAX_BIT_ORDER_LENGTH);
		}
	}
	else
	{
		memcpy(device->wordBitOrder[VERIFY], readPointer, 
							fileMaxBitOrderLength);
	}
	readPointer = readPointer + fileMaxBitOrderLength;

	// wordAddressBitOrder PROGRAM
	if(fileWordLength > maxWordLength)
	{
		if((int8_t) readPointer[maxWordLength] != BIT_ORDER_END)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			memcpy(device->wordAddressBitOrder[PROGRAM], 
					readPointer, MAX_BIT_ORDER_LENGTH);
		}
	}
	else
	{
		memcpy(device->wordAddressBitOrder[PROGRAM], 
					readPointer, fileMaxBitOrderLength);
	}
	readPointer = readPointer + fileMaxBitOrderLength;

	// wordAddressBitOrder VERIFY
	if(fileWordLength > maxWordLength)
	{
		if((int8_t) readPointer[maxWordLength] != BIT_ORDER_END)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			memcpy(device->wordAddressBitOrder[VERIFY], 
					readPointer, MAX_BIT_ORDER_LENGTH);
		}
	}
	else
	{
		memcpy(device->wordAddressBitOrder[VERIFY], 
					readPointer, fileMaxBitOrderLength);
	}
	readPointer = readPointer + fileMaxBitOrderLength;

	// blockAddressBeforeDataBitOrder PROGRAM
	if(fileWordLength > maxWordLength)
	{
		if((int8_t) readPointer[maxWordLength] != BIT_ORDER_END)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			memcpy(device->preDataBlockAddrBitOrder[PROGRAM], 
					readPointer, MAX_BIT_ORDER_LENGTH);
		}
	}
	else
	{
		memcpy(device->preDataBlockAddrBitOrder[PROGRAM], 
					readPointer, fileMaxBitOrderLength);
	}
	readPointer = readPointer + fileMaxBitOrderLength;

	// blockAddressBeforeDataBitOrder VERIFY
	if(fileWordLength > maxWordLength)
	{
		if((int8_t) readPointer[maxWordLength] != BIT_ORDER_END)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			memcpy(device->preDataBlockAddrBitOrder[VERIFY], 
					readPointer, MAX_BIT_ORDER_LENGTH);
		}
	}
	else
	{
		memcpy(device->preDataBlockAddrBitOrder[VERIFY], 
					readPointer, fileMaxBitOrderLength);
	}
	readPointer = readPointer + fileMaxBitOrderLength;

	// blockAddressAfterDataBitOrder PROGRAM
	if(fileWordLength > maxWordLength)
	{
		if((int8_t) readPointer[maxWordLength] != BIT_ORDER_END)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			memcpy(device->postDataBlockAddrBitOrder[PROGRAM], 
					readPointer, MAX_BIT_ORDER_LENGTH);
		}
	}
	else
	{
		memcpy(device->postDataBlockAddrBitOrder[PROGRAM], 
					readPointer, fileMaxBitOrderLength);
	}
	readPointer = readPointer + fileMaxBitOrderLength;

	// blockAddressBeforeDataBitOrder VERIFY
	if(fileWordLength > maxWordLength)
	{
		if((int8_t) readPointer[maxWordLength] != BIT_ORDER_END)
		{
			fclose(file);
			fprintf(stdout, "Can not use 64 bit device file \"%s\" \
				with 32 bit version of this program!\r\n", 
				fileName);
			return EXIT_FAILURE;
		}
		else
		{
			memcpy(device->postDataBlockAddrBitOrder[VERIFY], 
					readPointer, MAX_BIT_ORDER_LENGTH);
		}
	}
	else
	{
		memcpy(device->postDataBlockAddrBitOrder[VERIFY], 
					readPointer, fileMaxBitOrderLength);
	}
	readPointer = readPointer + fileMaxBitOrderLength;

	fclose(file);

	return EXIT_SUCCESS;
}

