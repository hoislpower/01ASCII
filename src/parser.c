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



/* Grammar of the syntax in EBNF                                              */
/*----------------------------------------------------------------------------*/
/* ProgrammingPattern = { Assignment } .                                      */
/*                                                                            */
/* Assignment = ident '=' ( String | Number | BitOrderDescription ) .         */
/*                                                                            */
/* String = '"' ident '"' .                                                   */
/*                                                                            */
/* Number = hexnumber | decnumber .                                           */
/*                                                                            */
/* BitOrderDescription = '{' BitSequence '}' .                                */
/* BitSequence = SequenceLoop { ',' SequenceLoop } .                          */
/* SequenceLoop = SequencePart [ '*' decnumber ] .                            */
/* SequencePart = Range | Literal | Group .                                   */
/* Range = decnumber  [ '-' decnumber ] .                                     */
/* Literal = ''' decnumber ''' .                                              */
/* Group = '(' BitSequence ')' .                                              */
/*----------------------------------------------------------------------------*/

#include "bit-array.h"
#include "scanner.h"
#include "parser.h"


#define MAX_BIT_ARRAY_LENGTH	(3*MAX_BIT_ORDER_LENGTH)

#define DATA_KEYWORD	(keyword == DATA || keyword == PROGRAM_DATA \
						|| keyword == VERIFY_DATA)
#define ADDRESS_KEYWORD	(keyword == ADDRESS || keyword == PROGRAM_ADDRESS \
						|| keyword == VERIFY_ADDRESS)


/* keywords */
static const char *keywordList[] = 
{
	"devicename",
	"memorysize",
	"blocksize",
	"startaddress",
	"addressstep",
	"wordlength",
	"addresslength",
	"programdata",
	"programaddress",
	"verifydata",
	"verifyaddress",
	"data",
	"address",
	""
};

typedef enum
{
	DEVICE_NAME,
	MEMORY_SIZE,
	BLOCK_SIZE,
	START_ADDRESS,
	ADDRESS_STEP,
	WORD_LENGTH,
	ADDRESS_LENGTH,
	PROGRAM_DATA,
	PROGRAM_ADDRESS,
	VERIFY_DATA,
	VERIFY_ADDRESS,
	DATA,
	ADDRESS
}
keywords;

/* keyword types */
enum {STRING_KEYWORD, NUMBER_KEYWORD, BIT_ORDER_KEYWORD};

static const char keywordType[] = 
{
	STRING_KEYWORD,		/* devicename */
	NUMBER_KEYWORD,		/* memorysize */
	NUMBER_KEYWORD,		/* blocksize */
	NUMBER_KEYWORD,		/* startaddress */
	NUMBER_KEYWORD,		/* addressstep */
	NUMBER_KEYWORD,		/* wordlength */
	NUMBER_KEYWORD,		/* addresslength */
	BIT_ORDER_KEYWORD,	/* programdata */
	BIT_ORDER_KEYWORD,	/* programaddress */
	BIT_ORDER_KEYWORD,	/* verifydata */
	BIT_ORDER_KEYWORD,	/* verifyaddress */
	BIT_ORDER_KEYWORD,	/* data */
	BIT_ORDER_KEYWORD	/* address */
};



/* Nonterminal symbols */
int	ProgrammingPattern(deviceData *);
int	Assignment(deviceData *);
int	String(deviceData *, int);
int	Number(deviceData *, int);
int	BitOrderDescription(deviceData *, int);
int	BitSequence(deviceData *, int);
int	SequenceLoop(deviceData *, int);
int	SequencePart(deviceData *, int);
int	Range(deviceData *, int);
int	Literal(deviceData *, int);
int	Group(deviceData *, int);


/* Parse a device description source file into a device description object    */
/* file.                                                                      */
/*----------------------------------------------------------------------------*/
/* IN inputFileName: File to be parsed.                                       */
/* OUT outputFileName: File to be generated by the parser.                    */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	compileSourceFile(char *inputFileName, char *outputFileName)
{
	deviceData device;


	/* initialize the device data structure */
	initializeDeviceData(&device);

	/* delegate the scanner to open the input file */
	if(initializeScanner(inputFileName) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	/* parse the input file into the deviceData structure */
	if(ProgrammingPattern(&device) != EXIT_SUCCESS)
	{
		disposeScanner();
		return EXIT_FAILURE;
	}
	disposeScanner();

	/* generate the device description file */
	saveDeviceDescription(&device, outputFileName);
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}


/* ProgrammingPattern = { Assignment } .                                      */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	ProgrammingPattern(deviceData *device)
{
	/* expecting any number of Assignment statements */
	while(getCurrentSymbol() == IDENT_SYM)
	{
		if(Assignment(device) != EXIT_SUCCESS) return EXIT_FAILURE;
	}

	/* check if there are any unexpected symbols at the end of the file */
	if(getCurrentSymbol() != EOF_SYM)
	{
		fprintf(stdout, "FAILURE: Unexpected symbol at line %i column "
			"%i.\r\n", getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}

	/* check if all necessary information was found */
	/* check if the device name was found */
	if(strcmp(device->name, "") == 0)
	{
		fprintf(stdout, "FAILURE: The device name has not been set."
			"\r\n");
		return EXIT_FAILURE;
	}

	/* memorySize must not be less than blockSize */
	if(device->memorySize < device->blockSize)
	{
		fprintf(stdout, "FAILURE: The memory size must be greater or "
			"equal to the block size.\r\n         Memory size: \t"
			"%lu\r\n         Block size: \t%lu\r\n", 
			device->memorySize, device->blockSize);
		return EXIT_FAILURE;
	}

	/* memorySize must be greater than 0 */
	if(device->memorySize < 1)
	{
		fprintf(stdout, "FAILURE: The memory size must be set to a "
			"value greater than 0.\r\n");
		return EXIT_FAILURE;
	}

	/* program data word bit order must be set */
	if(bitOrderIsEmpty(device->wordBitOrder[PROGRAM]) == true)
	{
		fprintf(stdout, "FAILURE: The program data word bit order has "
			"not been set.\r\n");
		return EXIT_FAILURE;
	}

	/* verify data word bit order must be set */
	if(bitOrderIsEmpty(device->wordBitOrder[VERIFY]) == true)
	{
		fprintf(stdout, "FAILURE: The verify data word bit order has "
			"not been set.\r\n");
		return EXIT_FAILURE;
	}

	/* program address word bit order must be set */
	if(bitOrderIsEmpty(device->wordAddressBitOrder[PROGRAM]) == true)
	{
		fprintf(stdout, "FAILURE: The program address word bit order "
			"has not been set.\r\n");
		return EXIT_FAILURE;
	}

	/* verify address word bit order must be set */
	if(bitOrderIsEmpty(device->wordAddressBitOrder[VERIFY]) == true)
	{
		fprintf(stdout, "FAILURE: The verify address word bit order "
			"has not been set.\r\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/* Assignment = ident '=' ( String | Number | BitOrderDescription ) .         */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	Assignment(deviceData *device)
{
	int expectedSymbol;
	int keyword;
	char keywordString[MAX_IDENT_LENGTH];


	/* expecting an ident */
	if(getCurrentSymbol() != IDENT_SYM)
	{
		fprintf(stdout, "FAILURE: Ident expected at line %i column %i."
			"\r\n", getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}
	/* copy the identifier to keyword */
	strcpy(keywordString, getCurrentIdentName());

	keyword = 0;
	while((strcmp(keywordList[keyword], keywordString) != 0) && 
					(strcmp(keywordList[keyword], "") != 0))
		keyword++;

	/* check if the keyword is valid */
	if(strcmp(keywordList[keyword], "") == 0)
	{
		fprintf(stdout, "FAILURE: Unknown keyword %s at line %i column "
			"%i.\r\n", keywordString, getCurrentLine(),
			getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbol();

	/* get expected following symbol */
	switch(keywordType[keyword])
	{
		case STRING_KEYWORD:
		expectedSymbol = DOUBLE_QUOTE_SYM;
		break;

		case NUMBER_KEYWORD:
		expectedSymbol = HEX_NUMBER_SYM;
		break;

		case BIT_ORDER_KEYWORD:
		expectedSymbol = LEFT_CURLY_BRACKET_SYM;
		break;
	}

	/* expecting an equal symbol */
	if(getCurrentSymbol() != EQUAL_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' expected at line %i column %i."
			"\r\n", terminalSymList[EQUAL_SYM], getCurrentLine(),
			getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbol();


	switch(getCurrentSymbol())
	{
		case DOUBLE_QUOTE_SYM:
		/* check keyword semantic */
		if(expectedSymbol != DOUBLE_QUOTE_SYM)
		{
			if(expectedSymbol == HEX_NUMBER_SYM)
				fprintf(stdout, "FAILURE: Number expected after"
					" \"%s\" keyword at line %i column %i."
					"\r\n", keywordList[keyword], 
					getCurrentLine(), getCurrentColumn());
			else
				fprintf(stdout, "FAILURE: '%c' expected after "
					"\"%s\" keyword at line %i column %i."
					"\r\n", terminalSymList[expectedSymbol], 
					keywordList[keyword], getCurrentLine(), 
					getCurrentColumn());

			return EXIT_FAILURE;
		}

		if(String(device, keyword) != EXIT_SUCCESS) return EXIT_FAILURE;
		break;


		case DEC_NUMBER_SYM:
		case HEX_NUMBER_SYM:
		/* check keyword semantic */
		if(expectedSymbol != HEX_NUMBER_SYM)
		{
			fprintf(stdout, "FAILURE: '%c' expected after \"%s\" "
				"keyword at line %i column %i.\r\n", 
				terminalSymList[expectedSymbol], 
				keywordList[keyword], getCurrentLine(), 
				getCurrentColumn());
			return EXIT_FAILURE;
		}

		if(Number(device, keyword) != EXIT_SUCCESS) return EXIT_FAILURE;
		break;


		case NEWLINE_SYM:
		/* new line after equal symbol is only allowed */
		/* when a BitSequence is following */
		scanNextSymbolSkipNewline();
		case LEFT_CURLY_BRACKET_SYM:
		/* check keyword semantic */
		if(expectedSymbol != LEFT_CURLY_BRACKET_SYM)
		{
			if(expectedSymbol == HEX_NUMBER_SYM)
				fprintf(stdout, "FAILURE: Number expected after"
					" \"%s\" keyword at line %i column %i."
					"\r\n", keywordList[keyword], 
					getCurrentLine(), getCurrentColumn());
			else
				fprintf(stdout, "FAILURE: '%c' expected after "
					"\"%s\" keyword at line %i column %i."
					"\r\n", terminalSymList[expectedSymbol], 
					keywordList[keyword], getCurrentLine(), 
					getCurrentColumn());

			return EXIT_FAILURE;
		}

		if(initializeBitArray(MAX_BIT_ARRAY_LENGTH) != EXIT_SUCCESS)
			return EXIT_FAILURE;

		if(BitOrderDescription(device, keyword) != EXIT_SUCCESS) 
		{
			disposeBitArray();
			return EXIT_FAILURE;
		}
		disposeBitArray();
		break;


		default:
		fprintf(stdout, "FAILURE: String, number or bit sequence "
			"expected at line %i column %i.\r\n", 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/* String = '"' ident '"' .                                                   */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	String(deviceData *device, int keyword)
{
	/* expecting a double quote symbol */
	if(getCurrentSymbol() != DOUBLE_QUOTE_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' expected at line %i column %i."
			"\r\n", terminalSymList[DOUBLE_QUOTE_SYM], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbol();

	/* expecting an ident */
	if(getCurrentSymbol() != IDENT_SYM)
	{
		fprintf(stdout, "FAILURE: Ident expected at line %i column %i."
			"\r\n", getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}

	/* add the string to the device data structure */
	switch(keyword)
	{
		case DEVICE_NAME:
		if(strcmp(device->name, "") == 0)
		{
			/* copy the name of the device */
			strcpy(device->name, getCurrentIdentName());
		}
		else
		{
			fprintf(stdout, "FAILURE: Device name has already been "
				"set at line %i column %i.\r\n",
				getCurrentLine(), getCurrentColumn());
			return EXIT_FAILURE;
		}
		break;

		default:
		fprintf(stdout, "FAILURE: Tried to assign a string to %s at "
			"line %i column %i.\r\n", keywordList[keyword], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}

	scanNextSymbol();

	/* expecting a double quote symbol */
	if(getCurrentSymbol() != DOUBLE_QUOTE_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' expected at line %i column %i."
			"\r\n", terminalSymList[DOUBLE_QUOTE_SYM], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbolSkipNewline();

	return EXIT_SUCCESS;
}


/* Number = hexnumber | decnumber .                                           */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	Number(deviceData *device, int keyword)
{
	/* expecting a hexnumber or a decnumber symbol */
	if((getCurrentSymbol() != DEC_NUMBER_SYM) && 
					(getCurrentSymbol() != HEX_NUMBER_SYM))
	{
		fprintf(stdout, "FAILURE: Number expected at line %i column %i."
			"\r\n", getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}

	/* add number to the device data structure */
	switch(keyword)
	{
		case MEMORY_SIZE:
		if(device->memorySize == 0)
		{
			device->memorySize = getCurrentNumberValue();
		}
		else
		{
			fprintf(stdout, "FAILURE: Memory size has already been "
				"set at line %i column %i.\r\n",
				getCurrentLine(), getCurrentColumn());
			return EXIT_FAILURE;
		}
		break;

		case BLOCK_SIZE:
		if(device->blockSize == 0)
		{
			device->blockSize = getCurrentNumberValue();
		}
		else
		{
			fprintf(stdout, "FAILURE: Block size has already been "
				"set at line %i column %i.\r\n",
				getCurrentLine(), getCurrentColumn());
			return EXIT_FAILURE;
		}
		break;

		case START_ADDRESS:
		device->startAddress = getCurrentNumberValue();
		break;

		case ADDRESS_STEP:
		device->addressStepPerWord = getCurrentNumberValue();
		break;

		case WORD_LENGTH:
		/* check if word length is divisible by 8 */
		if(getCurrentNumberValue() % 8)
		{
			fprintf(stdout, "FAILURE: Word length must be divisible"
				" by 8 at line %i column %i.\r\n", 
				getCurrentLine(), getCurrentColumn());
			return EXIT_FAILURE;
		}
		device->wordLength = getCurrentNumberValue();
		break;

		case ADDRESS_LENGTH:
		device->addressLength = getCurrentNumberValue();
		break;

		default:
		fprintf(stdout, "FAILURE: Tried to assign a number to %s at "
			"line %i column %i.\r\n", keywordList[keyword], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}

	scanNextSymbolSkipNewline();

	return EXIT_SUCCESS;
}


/* BitOrderDescription = '{' BitSequence '}' .                                */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	BitOrderDescription(deviceData *device, int keyword)
{
	int Line;
	int Column;
	int bitOrderLength;
	int8_t *bitOrder;


	/* expecting a left curly bracket symbol */
	if(getCurrentSymbol() != LEFT_CURLY_BRACKET_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' expected at line %i column %i."
			"\r\n", terminalSymList[LEFT_CURLY_BRACKET_SYM], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbolSkipNewline();

	/* process the whole bit order description */
	while((getCurrentSymbol() == DEC_NUMBER_SYM) || 
		(getCurrentSymbol() == SINGLE_QUOTE_SYM) ||
		(getCurrentSymbol() == LEFT_PARENTHESIS_SYM))
	{
		/* expecting a BitSequence statement */
		if(BitSequence(device, keyword) != EXIT_SUCCESS)
			return EXIT_FAILURE;
	}

	/* if blockSize has not been set -> set it to memory size */
	if(device->blockSize == 0)
		device->blockSize = device->memorySize;

	/* store the bit order in the right array */
	bitOrder = getBitArray();
	if(bitOrder == NULL)
	{
		fprintf(stderr, "ERROR: Invalid bit array.\r\n");
		return EXIT_FAILURE;
	}
	bitOrderLength = bitArrayGetCurrentIndex();

	/* set program data bit order */
	if(keyword == PROGRAM_DATA || keyword == DATA)
	{
		if(bitOrderLength > MAX_BIT_ORDER_LENGTH)
		{
			fprintf(stderr, "ERROR: Length of program data bit "
				"order exceeds the maximum number of %i.\r\n",
				MAX_BIT_ORDER_LENGTH);
			return EXIT_FAILURE;
		}

		memcpy(device->wordBitOrder[PROGRAM], bitOrder, bitOrderLength);
	}

	/* set verify data bit order */
	if(keyword == VERIFY_DATA || keyword == DATA)
	{
		if(bitOrderLength > MAX_BIT_ORDER_LENGTH)
		{
			fprintf(stderr, "ERROR: Length of verify data bit "
				"order exceeds the maximum number of %i.\r\n",
				MAX_BIT_ORDER_LENGTH);
			return EXIT_FAILURE;
		}

		memcpy(device->wordBitOrder[VERIFY], bitOrder, bitOrderLength);
	}

	/* set program address bit order */
	if(keyword == PROGRAM_ADDRESS || keyword == ADDRESS)
	{
		if(bitOrderLength > MAX_BIT_ORDER_LENGTH)
		{
			fprintf(stderr, "ERROR: Length of program address bit "
				"order exceeds the maximum number of %i.\r\n",
				MAX_BIT_ORDER_LENGTH);
			return EXIT_FAILURE;
		}

		if(bitOrderIsEmpty(device->wordAddressBitOrder[PROGRAM])== true)
			memcpy(device->wordAddressBitOrder[PROGRAM], bitOrder, 
				bitOrderLength);
		else
			memcpy(device->postDataBlockAddrBitOrder[PROGRAM],
				bitOrder, bitOrderLength);
	}

	/* set verify address bit order */
	if(keyword == VERIFY_ADDRESS || keyword == ADDRESS)
	{
		if(bitOrderLength > MAX_BIT_ORDER_LENGTH)
		{
			fprintf(stderr, "ERROR: Length of verify address bit "
				"order exceeds the maximum number of %i.\r\n",
				MAX_BIT_ORDER_LENGTH);
			return EXIT_FAILURE;
		}

		if(bitOrderIsEmpty(device->wordAddressBitOrder[VERIFY]) == true)
			memcpy(device->wordAddressBitOrder[VERIFY], bitOrder, 
				bitOrderLength);
		else
			memcpy(device->postDataBlockAddrBitOrder[VERIFY],
				bitOrder, bitOrderLength);
	}

	/* skip potential new lines */
	Line = getCurrentLine();
	Column = getCurrentColumn();
	if(getCurrentSymbol() == NEWLINE_SYM)
		scanNextSymbolSkipNewline();

	/* expecting a right curly bracket symbol */
	if(getCurrentSymbol() != RIGHT_CURLY_BRACKET_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' or '%c' expected at line %i "
			"column %i.\r\n", terminalSymList[COMMA_SYM], 
			terminalSymList[RIGHT_CURLY_BRACKET_SYM], Line, Column);
		return EXIT_FAILURE;
	}
	scanNextSymbolSkipNewline();

	return EXIT_SUCCESS;
}


/* BitSequence = SequenceLoop { ',' SequenceLoop } .                          */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	BitSequence(deviceData *device, int keyword)
{
	/* expecting a SequenceLoop statement */
	if(SequenceLoop(device, keyword) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	/* expecting any number of SequenceLoop statements */
	while(getCurrentSymbol() == COMMA_SYM)
	{
		/* a line break is allowed after a comma */
		scanNextSymbolSkipNewline();

		/* expecting a SequenceLoop statement */
		if(SequenceLoop(device, keyword) != EXIT_SUCCESS)
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/* SequenceLoop = SequencePart [ '*' decnumber ] .                            */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	SequenceLoop(deviceData *device, int keyword)
{
	int result;
	int blockSizeFound;

	int startIndex;
	int endIndex;
	uint32_64_t repeatNum;

	int8_t *bitOrder;


	/* remember start index of the following bit sequence part */
	startIndex = bitArrayGetCurrentIndex();

	/* expecting a SequencePart statement */
	if(SequencePart(device, keyword) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	/* optional repeat part */
	if(getCurrentSymbol() == ASTERISK_SYM)
	{
		scanNextSymbol();

		/* expecting a decnumber symbol */
		if(getCurrentSymbol() != DEC_NUMBER_SYM)
		{
			fprintf(stdout, "FAILURE: Number expected at line %i "
				"column %i.\r\n", getCurrentLine(), 
				getCurrentColumn());
			return EXIT_FAILURE;
		}
		repeatNum = getCurrentNumberValue()-1;
		endIndex = bitArrayGetCurrentIndex()-1;

		/* check if the number is higher than 0 */
		if(repeatNum < 1)
		{
			fprintf(stdout, "FAILURE: Number at line %i column %i "
				"must be greater than 1.\r\n", getCurrentLine(),
				getCurrentColumn());
			return EXIT_FAILURE;
		}


		/* if the current bit order is an address bit order and */
		/* the block size and the repeat number are equal */
		/* -> the current repeat number means a repetition */
		/*    over the whole block size */
		blockSizeFound = false;
		if(repeatNum == device->blockSize && ADDRESS_KEYWORD)
		{
			blockSizeFound = true;
		}
		else
		{
			/* repeat the sequence */
			result =bitArrayRepeat(startIndex, endIndex, repeatNum);
			if(result != EXIT_SUCCESS)
			{
				if(device->blockSize == 0 && ADDRESS_KEYWORD)
				{
					device->blockSize = repeatNum;
					blockSizeFound = true;
				}
				else
				{

					fprintf(stdout, "FAILURE: Ambiguous "
						"block size at line %i column "
						"%i.\r\n         The length of"
						"the bit array has been "
						"exceeded.", getCurrentLine(),
						getCurrentColumn());
					return EXIT_FAILURE;
				}
			}
		}

		/* get the preDataBlockAddrBitOrder and the */
		/* wordAddressBitOrderout of the bit array datastructure */
		if(blockSizeFound == true)
		{
			bitOrder = getBitArray();

			/* check the length of the bit orders */
			if((endIndex-startIndex > MAX_BIT_ORDER_LENGTH) ||
					(startIndex-1 > MAX_BIT_ORDER_LENGTH))
			{
				fprintf(stdout, "FAILURE: Length of the bit "
					"order has been exceeded at line %i "
					"column %i.\r\n", getCurrentLine(),
					getCurrentColumn());
				return EXIT_FAILURE;
			}

			/* get program address bit orders */
			if(keyword == ADDRESS || keyword == PROGRAM_ADDRESS)
			{
				memcpy(device->preDataBlockAddrBitOrder[PROGRAM]
					, bitOrder, startIndex-1);
				memcpy(device->wordAddressBitOrder[PROGRAM],
					bitOrder+startIndex,
					endIndex-startIndex);
			}

			/* get verify address bit orders */
			if(keyword == ADDRESS || keyword == VERIFY_ADDRESS)
			{
				memcpy(device->preDataBlockAddrBitOrder[VERIFY]
					, bitOrder, startIndex-1);
				memcpy(device->wordAddressBitOrder[VERIFY],
					bitOrder+startIndex,
					endIndex-startIndex);
			}

			/* clear the bit array */
			disposeBitArray();
			initializeBitArray(MAX_BIT_ARRAY_LENGTH);
		}

		scanNextSymbol();
	}
	
	return EXIT_SUCCESS;
}


/* SequencePart = Range | Literal | Group .                                   */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	SequencePart(deviceData *device, int keyword)
{
	switch(getCurrentSymbol())
	{
		case DEC_NUMBER_SYM:
		/* expecting a Range statement */
		if(Range(device, keyword) != EXIT_SUCCESS)
			return EXIT_FAILURE;
		break;

		case SINGLE_QUOTE_SYM:
		/* expecting a Literal statement */
		if(Literal(device, keyword) != EXIT_SUCCESS)
			return EXIT_FAILURE;
		break;

		case LEFT_PARENTHESIS_SYM:
		/* expecting a Group statement */
		if(Group(device, keyword) != EXIT_SUCCESS)
			return EXIT_FAILURE;
		break;

		default:
		fprintf(stdout, "FAILURE: Number, '%c' or '%c' expected at line"
			" %i column %i.\r\n", terminalSymList[SINGLE_QUOTE_SYM],
			terminalSymList[LEFT_PARENTHESIS_SYM], getCurrentLine(),
			getCurrentColumn());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/* Range = decnumber  [ '-' decnumber ] .                                     */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	Range(deviceData *device, int keyword)
{
	int8_t startRange;
	int8_t endRange;
	uint8_t wordLength;


	/* get length of the address or data word */
	if(DATA_KEYWORD)
		wordLength = device->wordLength;
	else
		wordLength = device->addressLength;

	/* expecting a decnumber symbol */
	if(getCurrentSymbol() != DEC_NUMBER_SYM)
	{
		fprintf(stdout, "FAILURE: Number expected at line %i column %i."
			"\r\n", getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}

	/* check if the number is less than the word length */
	if(getCurrentNumberValue() >= wordLength)
	{
		fprintf(stdout, "FAILURE: Number at line %i column %i must be "
			"less than %i.\r\n", getCurrentLine(),
			getCurrentColumn(), wordLength);
		return EXIT_FAILURE;
	}

	/* set start and end of the range */
	startRange = getCurrentNumberValue();
	endRange = getCurrentNumberValue();
	scanNextSymbol();


	/* optional part  */
	if(getCurrentSymbol() == HYPHEN_SYM)
	{
		scanNextSymbol();

		/* expecting a decnumber symbol */
		if(getCurrentSymbol() != DEC_NUMBER_SYM)
		{
			fprintf(stdout, "FAILURE: Number expected at line %i "
				"column %i.\r\n", getCurrentLine(),
				getCurrentColumn());
			return EXIT_FAILURE;
		}

		/* set end of the range */
		endRange = getCurrentNumberValue();

		/* check if the number is less than the word length */
		if(endRange >= wordLength)
		{
			fprintf(stdout, "FAILURE: Number at line %i column %i "
				"must be less than %i.\r\n", getCurrentLine(),
				getCurrentColumn(), wordLength);
			return EXIT_FAILURE;
		}

		scanNextSymbol();
	}

	/* add the range to the bit array */
	if(bitArrayAdd(startRange, endRange) != EXIT_SUCCESS)
	{
		fprintf(stdout, "FAILURE: Length of the bit order has been "
			"exceeded at line %i column %i.\r\n", getCurrentLine(),
			getCurrentColumn());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


/* Literal = ''' decnumber ''' .                                              */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	Literal(deviceData *device, int keyword)
{
	int result;


	/* expecting a single quote symbol */
	if(getCurrentSymbol() != SINGLE_QUOTE_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' expected at line %i column %i."
			"\r\n", terminalSymList[SINGLE_QUOTE_SYM], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbol();

	/* expecting a decnumber symbol */
	if(getCurrentSymbol() != DEC_NUMBER_SYM)
	{
		fprintf(stdout, "FAILURE: Number expected at line %i column %i."
			"\r\n", getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}

	/* check if the number is either 0 or 1 */
	if(getCurrentNumberValue() > 1)
	{
		fprintf(stdout, "FAILURE: Number at line %i column %i must be "
			"either 0 or 1.\r\n", getCurrentLine(),
			getCurrentColumn());
		return EXIT_FAILURE;
	}
	/* add the literal to the bit array */
	if(getCurrentNumberValue() == 0)
		result = bitArrayAdd(LITERAL0_BIT, LITERAL0_BIT);
	if(getCurrentNumberValue() == 1)
		result = bitArrayAdd(LITERAL1_BIT, LITERAL1_BIT);

	if(result != EXIT_SUCCESS)
	{
		fprintf(stdout, "FAILURE: Length of the bit order has been "
			"exceeded at line %i column %i.\r\n", getCurrentLine(),
			getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbol();

	/* expecting a single quote symbol */
	if(getCurrentSymbol() != SINGLE_QUOTE_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' expected at line %i column %i."
			"\r\n", terminalSymList[SINGLE_QUOTE_SYM], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbol();

	return EXIT_SUCCESS;
}


/* Group = '(' BitSequence ')' .                                              */
/*----------------------------------------------------------------------------*/
/* OUT device: Device structure for storing the parsed data.                  */
/* IN keyword: Current keyword.                                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	Group(deviceData *device, int keyword)
{
	/* expecting a left parenthesis symbol */
	if(getCurrentSymbol() != LEFT_PARENTHESIS_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' expected at line %i column %i."
			"\r\n", terminalSymList[LEFT_PARENTHESIS_SYM], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbol();

	/* expecting a BitSequence statement */
	if(BitSequence(device, keyword) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	/* expecting a right parenthesis symbol */
	if(getCurrentSymbol() != RIGHT_PARENTHESIS_SYM)
	{
		fprintf(stdout, "FAILURE: '%c' expected at line %i column %i."
			"\r\n", terminalSymList[RIGHT_PARENTHESIS_SYM], 
			getCurrentLine(), getCurrentColumn());
		return EXIT_FAILURE;
	}
	scanNextSymbol();

	return EXIT_SUCCESS;
}

