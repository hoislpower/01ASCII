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



#include "converter.h"


// Write the usage of the memory blocks to an array
// -----------------------------------------------------------------------------
// Run through the data and check if there are blocks containing only 0xFF.
// Those blocks are marked as unused in the array.
// IN device: Description of the device whose data to be checked.
// IN programData: Byte array that contains the data to be checked.
// OUT usedBlocks: Array indicating whether a specific memory block is used or 
//                 not. The array must be at least as long as the number of
//                 blocks of the device (device.memorySize / device.blockSize).
// -----------------------------------------------------------------------------
void	findUsedBlocks(deviceData *device, uint8_t *programData,int *usedBlocks)
{
	uint32_64_t block;
	uint32_64_t byte;


	// check block after block
	for(block=0; block<(device->memorySize/device->blockSize); block++)
	{
		// initialize block to be used
		usedBlocks[block] = true;

		// loop until the first byte not equal to 0xFF
		byte = 0;
		while((byte < device->blockSize) && 
			(programData[block*device->blockSize+byte] == 0xFF))
			byte++;

		// set block to be unused if all bytes of the block are 0xFF
		if(byte >= device->blockSize)
			usedBlocks[block] = false;
	}
}


// Converts data or address word into a space separated bit sequence with the 
// given bit order.
// -----------------------------------------------------------------------------
// IN word: Data word to convert into a string.
// IN bitOrder: Bit order describing how to convert the word into a string.
// IN ascii: If ascii is true, the output string will be a space separated
//           bit sequence (0 or 1 ascii symbols) in the right order (defined by
//           bitOrder) with a line break at the end. If ascii is false, the 
//           output string will be in binary format (one byte represents one 
//           bit).
// OUT string: String containing the output bit sequence.
// RETURNS: Length of the string.
// -----------------------------------------------------------------------------
int	wordToOutputString(uint32_64_t word, int8_t *bitOrder, int ascii, 
				char *string)
{
	int bit;


	bit = 0;
	// bit order ends if bitOrder[bit] == UNUSED_BIT
	while(bitOrder[bit] != UNUSED_BIT)
	{
		// check if the output string should be an ascii string
		if(ascii == true)
		{
			// convert literals
			if(bitOrder[bit] == LITERAL0_BIT)
				string[bit*2] = '0';

			if(bitOrder[bit] == LITERAL1_BIT)
				string[bit*2] = '1';

			// convert word bits
			if(word & (1 << bitOrder[bit]))
				string[bit*2] = '1';
			else
				string[bit*2] = '0';

			string[bit*2+1] = ' ';
		}
		else
		{
			// convert literals
			if(bitOrder[bit] == LITERAL0_BIT)
				string[bit] = 0;

			if(bitOrder[bit] == LITERAL1_BIT)
				string[bit] = 1;

			// convert word bits
			if(word & (1 << bitOrder[bit]))
				string[bit] = 1;
			else
				string[bit] = 0;
		}

		bit++;
	}

	// add a line break if the output string is an ascii string
	if(ascii == true && bit > 0)
	{
		string[bit*2] = '\r';
		string[bit*2+1] = '\n';
		bit++;
		string[bit*2] = '\0';

		// return length of the string
		return bit*2;
	}
	else
	{
		// return length of the string
		return bit;
	}

}


// Write the address file
// -----------------------------------------------------------------------------
// Write a file with program and verify addresses depending on the value of 
// mode.
// IN fileNameBase: First part of the output file name. If mode is PROGRAM,
//                  "_program_address" is appended to fileNameBase. If mode is
//                  VERIFY, "_verify_address" is appended to fileNameBase. If
//                  mode is PROGRAM_VERIFY, "_address" is appended to
//                  fileNameBase.
// IN device: Device structure that contains all needed information about the 
//            device.
// IN programData: Byte array that contains the data.
// IN usedBlocks: Array indicating whether a specific memory block is used or 
//                not.
// IN mode: If mode is PROGRAM, the program bit order is used for generating
//          the address data. If mode is VERIFY, the verify bit order is used.
//          If mode is PROGRAM_VERIFY, it will be assumed that the program and
//          verify bit orders are equal.
// IN ascii: If ascii is true, the output format will be a space separated
//           bit sequence (0 or 1 ascii symbols). If ascii is false, the 
//           output format will be binary (one byte represents one bit).
// IN generateAllBlocks: If generateAllBlocks is false, only used data blocks
//                       are written to the output files. If generateAllBlocks
//                       is true, all data blocks will be written.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int	writeAddressFile(char *fileNameBase, deviceData *device, 
				uint8_t *programData, int *usedBlocks, int mode,
				int ascii, int generateAllBlocks)
{
	int writtenBytes;
	int bufferLength;
	char buffer[MAX_BIT_ORDER_LENGTH*2 + 3];
	char fileName[FILENAME_MAX];
	FILE *file;

	uint32_64_t byte;
	uint32_64_t addressWord;
	uint32_64_t blockAddressWord;


	// set output file name
	strcpy(fileName, fileNameBase);
	switch(mode)
	{
		case PROGRAM:
		strcpy(fileName + strlen(fileName), "_program_address");
		break;

		case VERIFY:
		strcpy(fileName + strlen(fileName), "_verify_address");
		break;

		case PROGRAM_VERIFY:
		strcpy(fileName + strlen(fileName), "_address");
		mode = PROGRAM; // overwrite mode
		break;

		default:
		break;
	}

	// create file
	file = fopen(fileName, "w");
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: Could not create file \"%s\"!\r\n",
			fileName);
		return EXIT_FAILURE;
	}

	// write addesses to file
	byte = 0;
	while(byte < device->memorySize)
	{
		// go to the next block if the current byte is the start 
		// of a new unused block
		if(((byte % device->blockSize) == 0) && 
			(usedBlocks[byte / device->blockSize] == false) &&
			(generateAllBlocks != true))
		{
			byte = byte + device->blockSize;
		}
		else
		{
			// calculate address word
			addressWord = device->startAddress + (byte / 
					(device->wordLength/8) *
					device->addressStepPerWord);

			// start of a new memory block...
			if((byte % device->blockSize) == 0)
			{
				// save start address of block
				blockAddressWord = addressWord;

				// write block address at the beginning
				// of a new block to the file
				bufferLength = wordToOutputString(
					blockAddressWord, 
					device->preDataBlockAddrBitOrder[mode],
					ascii, buffer);

				writtenBytes = fwrite(buffer, sizeof(*buffer),
							bufferLength, file);

				if(writtenBytes != bufferLength)
				{
					fclose(file);
					fprintf(stderr, "ERROR: Could not write"
						"to file \"%s\"! (wrote %i "
						"bytes)\r\n", fileName, 
						writtenBytes);
					return EXIT_FAILURE;
				}
			}


			// write the address word to the file
			bufferLength = wordToOutputString(addressWord,
					device->wordAddressBitOrder[mode],
								ascii, buffer);

			writtenBytes = fwrite(buffer, sizeof(*buffer),
						bufferLength, file);
			if(writtenBytes != bufferLength)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Could not write to file"
					" \"%s\"! (wrote %i bytes)\r\n",
					fileName, writtenBytes);
				return EXIT_FAILURE;
			}

			// increment byte
			byte = byte + device->wordLength / 8;


			// end of a memory block...
			if((byte % device->blockSize) == 0)
			{
				// write block address at the end
				// of the old block to the file
				bufferLength = wordToOutputString(
					blockAddressWord,
					device->postDataBlockAddrBitOrder[mode],
					ascii, buffer);

				writtenBytes = fwrite(buffer, sizeof(*buffer),
							bufferLength, file);

				if(writtenBytes != bufferLength)
				{
					fclose(file);
					fprintf(stderr, "ERROR: Could not write"
						"to file \"%s\"! (wrote %i "
						"bytes)\r\n", fileName, 
						writtenBytes);
					return EXIT_FAILURE;
				}
			}
		}
	}

	// close file
	fclose(file);

	return EXIT_SUCCESS;
}


// Write a data file
// -----------------------------------------------------------------------------
// Writes a file with program or verify data depending on the value of 
// mode.
// IN fileNameBase: First part of the output file name. If mode is PROGRAM,
//                  "_program_data" is appended to fileNameBase. If mode is
//                  VERIFY, "_verify_data" is appended to fileNameBase. If
//                  mode is PROGRAM_VERIFY, "_data" is appended to
//                  fileNameBase.
// IN device: Device structure that contains all needed information about the 
//            device.
// IN programData: Byte array that contains the data.
// IN usedBlocks: Array indicating whether a specific memory block is used or 
//                not.
// IN mode: If mode is PROGRAM, the program bit order is used for generating
//          the data. If mode is VERIFY, the verify bit order is used.
//          If mode is PROGRAM_VERIFY, it will be assumed that the program and
//          verify bit orders are equal.
// IN ascii: If ascii is true, the output format will be a space separated
//           bit sequence (0 or 1 ascii symbols). If ascii is false, the 
//           output format will be binary (one byte represents one bit).
// IN generateAllBlocks: If generateAllBlocks is false, only used data blocks
//                       are written to the output files. If generateAllBlocks
//                       is true, all data blocks will be written.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int	writeDataFile(char *fileNameBase, deviceData *device,
				uint8_t *programData, int *usedBlocks, int mode,
				int ascii, int generateAllBlocks)
{
	int i;
	int writtenBytes;
	int bufferLength;
	char buffer[MAX_BIT_ORDER_LENGTH*2 + 3];
	char fileName[FILENAME_MAX];
	FILE *file;

	uint32_64_t byte;
	uint32_64_t dataWord;


	// set output file name
	strcpy(fileName, fileNameBase);
	switch(mode)
	{
		case PROGRAM:
		strcpy(fileName + strlen(fileName), "_program_data");
		break;

		case VERIFY:
		strcpy(fileName + strlen(fileName), "_verify_data");
		break;

		case PROGRAM_VERIFY:
		strcpy(fileName + strlen(fileName), "_data");
		mode = PROGRAM; // overwrite mode
		break;

		default:
		break;
	}

	// create file
	file = fopen(fileName, "w");
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: Could not create file \"%s\"!\r\n",
			fileName);
		return EXIT_FAILURE;
	}


	// write data to file
	byte = 0;
	while(byte < device->memorySize)
	{
		// go to the next block if the current byte is the start
		// of a new unused block
		if(((byte % device->blockSize) == 0) &&
			(usedBlocks[byte / device->blockSize] == false) &&
			(generateAllBlocks != true))
		{
			byte = byte + device->blockSize;
		}
		else
		{
			// put together data word
			dataWord = 0;
			for(i=0; i < device->wordLength/8; i++)
			{
				dataWord += (programData[byte+i] << 8*i);
			}

			// write the data word to the file
			bufferLength = wordToOutputString(dataWord,
						device->wordBitOrder[mode],
						ascii, buffer);
			writtenBytes = fwrite(buffer, sizeof(*buffer),
						bufferLength, file);
			if(writtenBytes != bufferLength)
			{
				fclose(file);
				fprintf(stderr, "ERROR: Could not write to file"
					" \"%s\"! (wrote %i bytes)\r\n", 
					fileName, writtenBytes);
				return EXIT_FAILURE;
			}

			byte = byte + device->wordLength/8;
		}
	}

	// close file
	fclose(file);

	return EXIT_SUCCESS;
}


// Write all output files
// -----------------------------------------------------------------------------
// Four files will be generated by default.
// (program data, program address, verify data, verify address)
// If the PROGRAM and VERIFY bit orders are equal the function generates just 
// two files.
// IN fileNameBase: First part of the output file names.
// IN device: Device structure that contains all needed information about the
//            device.
// IN programData: Byte array that contains the data.
// IN ascii: If ascii is true, the output format will be a space separated
//           bit sequence (0 or 1 ascii symbols). If ascii is false, the 
//           output format will be binary (one byte represents one bit).
// IN generateAllBlocks: If generateAllBlocks is false, only used data blocks
//                       are written to the output files. If generateAllBlocks
//                       is true, all data blocks will be written.
// RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.
// -----------------------------------------------------------------------------
int	generateOutputFiles(char *fileNameBase, deviceData *device, 
					uint8_t *programData, int ascii,
					int generateAllBlocks)
{
	int bitOrdersEqual;
	int usedBlocks[device->memorySize/device->blockSize];


	// find used blocks
	if(generateAllBlocks != true)
		findUsedBlocks(device, programData, usedBlocks);

	// check if program and verify bit orders are equal
	bitOrdersEqual = programAndVerfiyBitOrdersAreEqual(device);

	// write data files
	if(bitOrdersEqual == true)
	{
		if(writeDataFile(fileNameBase, device, programData,
					usedBlocks, PROGRAM_VERIFY, ascii,
					generateAllBlocks) == EXIT_FAILURE)
			return EXIT_FAILURE;
	}
	else
	{
		if(writeDataFile(fileNameBase, device, programData, 
					usedBlocks, PROGRAM, ascii,
					generateAllBlocks) == EXIT_FAILURE)
			return EXIT_FAILURE;

		if(writeDataFile(fileNameBase, device, programData,
					usedBlocks, VERIFY, ascii,
					generateAllBlocks) == EXIT_FAILURE)
			return EXIT_FAILURE;
	}

	// write address files
	if(bitOrdersEqual == true)
	{
		if(writeAddressFile(fileNameBase, device, programData,
					usedBlocks, PROGRAM_VERIFY, ascii,
					generateAllBlocks) == EXIT_FAILURE)
			return EXIT_FAILURE;
	}
	else
	{
		if(writeAddressFile(fileNameBase, device, programData,
					usedBlocks, PROGRAM, ascii,
					generateAllBlocks) == EXIT_FAILURE)
			return EXIT_FAILURE;

		if(writeAddressFile(fileNameBase, device, programData,
					usedBlocks, VERIFY, ascii,
					generateAllBlocks) == EXIT_FAILURE)
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

