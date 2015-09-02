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
#include "parser.h"
#include "converter.h"


#define COMMAND_POSITION			1

#define	COMPILE_COMMAND				"compile"
#define COMPILE_MIN_ARGUMENT_NUM		4

#define GENERATE_COMMAND			"generate"
#define GENERATE_ALL_OPTION			"-a"
#define GENERATE_BINARY_OPTION			"-b"
#define GENERATE_HEX_INPUT_OPTION		"-h"
#define	GENERATE_MIN_ARGUMENT_NUM		5

#define USAGE_STRING "Usage: 01ASCII compile INPUTFILE OUTPUTFILE\r\n"\
			"       01ASCII generate [OPTION] DEVICEFILE INPUTFILE"\
			" OUTPUTFILE\r\n\r\n       Options:\r\n       -a   "\
			"Generate programming data for the whole memory space."\
			"\r\n            Default is to skip empty blocks.\r\n"\
			"\r\n       -b   Generate binary output files.\r\n"\
			"            Default is to generate space separated "\
			"ascii files.\r\n\r\n       -h   Input file is an "\
			"intel hex file.\r\n            Default is a binary "\
			"file.\r\n\r\n\r\n"


int main(int argc, char *argv[])
{
	int commandFound;
	int nextArgument;
	int ascii;
	int hexInput;
	int generateAllBlocks;
	char deviceFileName[FILENAME_MAX];
	char inputFileName[FILENAME_MAX];
	char outputFileName[FILENAME_MAX];
	deviceData device;
	uint8_t *programData;


	// check for minimal number of arguments
	if(argc < COMMAND_POSITION+1)
	{
		fprintf(stderr, "ERROR: Wrong number of arguments!\r\n");
		fprintf(stderr, USAGE_STRING);
		return EXIT_FAILURE;
	}

	commandFound = false;
	nextArgument = 1;
	strcpy(deviceFileName, "");
	strcpy(inputFileName, "");
	strcpy(outputFileName, "");

	// compile source file
	if(strcmp(argv[COMMAND_POSITION], COMPILE_COMMAND) == 0)
	{
		commandFound = true;
		nextArgument++;

		// check the number of arguments for this command
		if(argc < COMPILE_MIN_ARGUMENT_NUM)
		{
			fprintf(stderr, "ERROR: Wrong number of arguments!"
				"\r\n");
			fprintf(stderr, USAGE_STRING);
			return EXIT_FAILURE;
		}

		// process command line arguments
		while(nextArgument < argc)
		{
			// input file name
			if(strcmp(inputFileName, "") == 0)
				strcpy(inputFileName, argv[nextArgument]);
			// output file name
			else if(strcmp(outputFileName, "") == 0)
				strcpy(outputFileName, argv[nextArgument]);
			// unknown argument
			else
			{
				fprintf(stderr, "ERROR: Unknown argument \"%s\""
					"!\r\n", argv[nextArgument]);
				fprintf(stderr, USAGE_STRING);
				return EXIT_FAILURE;
			}

			nextArgument++;
		}

		// check if input file name has been set
		if(strcmp(inputFileName, "") == 0)
		{
			fprintf(stderr, "ERROR: Missing INPUTFILE!\r\n");
			fprintf(stderr, USAGE_STRING);
			return EXIT_FAILURE;
		}

		// check if output file name has been set
		if(strcmp(outputFileName, "") == 0)
		{
			fprintf(stderr, "ERROR: Missing OUTPUTFILE!\r\n");
			fprintf(stderr, USAGE_STRING);
			return EXIT_FAILURE;
		}

		// compile source file
		if(compileSourceFile(inputFileName, outputFileName) != 
								EXIT_SUCCESS)
			return EXIT_FAILURE;
	}

	// generate data and address files
	if(strcmp(argv[COMMAND_POSITION], GENERATE_COMMAND) == 0)
	{
		commandFound = true;
		nextArgument++;

		// check the number of arguments for this command
		if(argc < GENERATE_MIN_ARGUMENT_NUM)
		{
			fprintf(stderr, "ERROR: Wrong number of arguments!"
				"\r\n");
			fprintf(stderr, USAGE_STRING);
			return EXIT_FAILURE;
		}

		// option default values
		ascii = true;
		hexInput = false;
		generateAllBlocks = false;

		// process command line arguments
		while(nextArgument < argc)
		{
			// generate all blocks option
			if(strcmp(argv[nextArgument], GENERATE_ALL_OPTION) == 0)
				generateAllBlocks = true;
			// generate binary data option
			else if(strcmp(argv[nextArgument],
						GENERATE_BINARY_OPTION) == 0)
				ascii = false;
			// hex file input option
			else if(strcmp(argv[nextArgument],
						GENERATE_HEX_INPUT_OPTION) == 0)
				hexInput = true;
			// device file name
			else if(strcmp(deviceFileName, "") == 0)
				strcpy(deviceFileName, argv[nextArgument]);
			// input file name
			else if(strcmp(inputFileName, "") == 0)
				strcpy(inputFileName, argv[nextArgument]);
			// output file name
			else if(strcmp(outputFileName, "") == 0)
				strcpy(outputFileName, argv[nextArgument]);
			// unknown argument
			else
			{
				fprintf(stderr, "ERROR: Unknown argument \"%s\""
					"!\r\n", argv[nextArgument]);
				fprintf(stderr, USAGE_STRING);
				return EXIT_FAILURE;
			}

			nextArgument++;
		}

		// check if device file name has been set
		if(strcmp(deviceFileName, "") == 0)
		{
			fprintf(stderr, "ERROR: Missing DEVICEFILE!\r\n");
			fprintf(stderr, USAGE_STRING);
			return EXIT_FAILURE;
		}

		// check if input file name has been set
		if(strcmp(inputFileName, "") == 0)
		{
			fprintf(stderr, "ERROR: Missing INPUTFILE!\r\n");
			fprintf(stderr, USAGE_STRING);
			return EXIT_FAILURE;
		}

		// check if output file name has been set
		if(strcmp(outputFileName, "") == 0)
		{
			fprintf(stderr, "ERROR: Missing OUTPUTFILE!\r\n");
			fprintf(stderr, USAGE_STRING);
			return EXIT_FAILURE;
		}

		// load device data
		if(loadDeviceDescription(&device, deviceFileName)!=EXIT_SUCCESS)
			return EXIT_FAILURE;

		// allocate memory for the input data
		if((programData = malloc(device.memorySize)) == NULL)
		{
			fprintf(stderr, "ERROR: Could not allocate %lu bytes "
				"of data!\r\n", device.memorySize);
			return EXIT_FAILURE;
		}

		// initialize memory with 0xFF
		memset(programData, 0xFF, device.memorySize);

		// read input file
		if(hexInput == true)
		{
			// read input file as intel hex file
			if(readHexFile(inputFileName, &device, programData)
								!= EXIT_SUCCESS)
			{
				free(programData);
				return EXIT_FAILURE;
			}
		}
		else
		{
			// read input file as binary file
			if(readBinFile(inputFileName, &device, programData)
								!= EXIT_SUCCESS)
			{
				free(programData);
				return EXIT_FAILURE;
			}
		}


		// write output files
		if(generateOutputFiles(outputFileName, &device, programData,
				ascii, generateAllBlocks) != EXIT_SUCCESS)
		{
			free(programData);
			return EXIT_FAILURE;
		}

		free(programData);
	}

	// check if one of the commands was executed
	if(commandFound == false)
	{
		fprintf(stderr, "ERROR: Command \"%s\" not found!\r\n",
			argv[COMMAND_POSITION]);
		fprintf(stderr, USAGE_STRING);
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

