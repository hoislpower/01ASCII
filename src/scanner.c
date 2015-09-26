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



/*----------------------------------------------------------------------------*/
/* Terminal classes: -ident     ...Consists of one or more letters.           */
/*                   -hexnumber ...Consists of a leading zero, followed by 'x'*/
/*                                 or 'X', followed by one or more hexadecimal*/
/*                                 digits (0-9, A-F).                         */
/*                   -decnumber ...Consists of one or more decimal digits.    */
/*----------------------------------------------------------------------------*/

#include "scanner.h"


/* redefine isblank function for c90 compatibility */
#undef  isblank
#define isblank(character)		(character == ' ' || character == '\t')


/* global variables for saving information about the current state of the */
/* scanner. */
static char currentCharacter;
static char previousCharacter;
static int currentSymbol;

static char commentBuffer[BUFSIZ];
static char currentIdentName[MAX_IDENT_LENGTH];
static uint32_64_t currentNumberValue;

static int currentSymbolLine;
static int currentSymbolColumn;

static int currentLine;
static int currentColumn;
static FILE *file;


/* Get the next character.                                                    */
/*----------------------------------------------------------------------------*/
/* Read the next character from inputFile and store it in currentCharacter.   */
/*----------------------------------------------------------------------------*/
void	getNextCharacter()
{
	do
	{
		/* read next character */
		previousCharacter = currentCharacter;
		currentCharacter = fgetc(file);

		/* skip second line break character if there is one */
		if((previousCharacter == '\r') && (currentCharacter == '\n'))
		{
			currentCharacter = fgetc(file);
		}

		/* check if a new line was found */
		if((previousCharacter == '\r') || (previousCharacter == '\n'))
		{
			currentLine++;
			currentColumn = 0;
		}
		currentColumn++;
	}
	while(isblank(currentCharacter));

	/* read the rest of the line if a comment was found */
	if(currentCharacter == '!')
	{
		fgets(commentBuffer, BUFSIZ, file);
		currentCharacter = '\n';
	}
}


/* Initialize the scanner.                                                    */
/*----------------------------------------------------------------------------*/
/* This function must be called once before using the scanner.                */
/* IN fileName: File to be read by the scanner.                               */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	initializeScanner(char *fileName)
{
	/* open the input file */
	file = fopen(fileName, "r");
	if(file == NULL)
	{
		fprintf(stderr, "ERROR: Could not open %s for reading!\r\n", 
			fileName);
		return EXIT_FAILURE;
	}

	/* initialize line and column number */
	currentLine = 1;
	currentColumn = 0;
	currentCharacter = '\0';
	getNextCharacter();
	scanNextSymbol();

	return EXIT_SUCCESS;
}


/* Dispose the scanner.                                                       */
/*----------------------------------------------------------------------------*/
/* This function must be called once if the scanner is no longer used.        */
/*----------------------------------------------------------------------------*/
void	disposeScanner()
{
	/* close the input file */
	fclose(file);
}


/* Scan and return the next symbol.                                           */
/*----------------------------------------------------------------------------*/
/* RETURNS: Symbol that has been scanned.                                     */
/*----------------------------------------------------------------------------*/
int	scanNextSymbol()
{
	int i;


	/* remember current position */
	currentSymbolLine = currentLine;
	currentSymbolColumn = currentColumn;

	/* check if the character is a newline symbol */
	if((currentCharacter == '\r') || (currentCharacter == '\n'))
	{
		getNextCharacter();
		currentSymbol = NEWLINE_SYM;
		return currentSymbol;
	}

	/* check if the end of file is reached */
	if(currentCharacter == EOF)
	{
		currentSymbol = EOF_SYM;
		return currentSymbol;
	}


	/* check if the character is a terminal symbol */
	currentSymbol=0;
	while((terminalSymList[currentSymbol] != currentCharacter) && 
				(terminalSymList[currentSymbol] != '\0'))
		currentSymbol++;

	/* return the terminal symbol if one was found */
	if(terminalSymList[currentSymbol] != '\0')
	{
		getNextCharacter();
		return currentSymbol;
	}

	/* check if the character is a hex or dec number */
	if(isdigit(currentCharacter))
	{
		currentNumberValue = 0;

		/* if the first digit is a 0 it might be a hex number */
		if(currentCharacter == '0')
		{
			/* if the next character is a 'x', */
			/* a hex number was found */
			getNextCharacter();
			if(tolower(currentCharacter) == 'x')
			{
				/* check if the 'x' is followed by a hex digit*/
				/* and otherwise return an unknown symbol */
				getNextCharacter();
				if(!isxdigit(currentCharacter))
				{
					currentSymbol = UNKNOWN_SYM;
					return currentSymbol;
				}

				/* put together the hexadecimal number */ 
				/* by using the horner scheme */
				while(isxdigit(currentCharacter))
				{
					if(isdigit(currentCharacter))
					{
						/* add one of the digits 0-9 */
						currentNumberValue *= 16;
						currentNumberValue += 
							currentCharacter-'0';
					}
					else
					{
						/* add one of the digits A-F */
						currentNumberValue *= 16; 
						currentNumberValue += 
							currentCharacter-'A'+10;
					}
					getNextCharacter();
				}

				currentSymbol = HEX_NUMBER_SYM;
				return currentSymbol;
			}
		}

		/* if no hex number was found and currentCharacter is a digit,*/
		/* a decimal number was found */
		if(isdigit(currentCharacter))
		{
			/* put together the decimal number */
			/* by using the horner scheme */
			while(isdigit(currentCharacter))
			{
				currentNumberValue *= 10;
				currentNumberValue += currentCharacter-'0';
				getNextCharacter();
			}
		}

		currentSymbol = DEC_NUMBER_SYM;
		return currentSymbol;
	}

	/* check if the character is part of an ident */
	if(isalpha(currentCharacter))
	{
		i=0;

		currentIdentName[i] = tolower(currentCharacter);
		getNextCharacter();
		i++;

		/* put together ident */
		while((i < MAX_IDENT_LENGTH - 1) && (isalnum(currentCharacter)))
		{
			currentIdentName[i] = tolower(currentCharacter);
			getNextCharacter();
			i++;
		}
		currentIdentName[i] = '\0';

		/* return IDENT_SYM if the maximum length hasn't been exceeded*/
		if(!isalnum(currentCharacter))
		{
			currentSymbol = IDENT_SYM;
			return currentSymbol;
		}
	}

	/* return UNKNOWN_SYM if an unexpected character or */
	/* a too long ident was found */
	getNextCharacter();
	currentSymbol = UNKNOWN_SYM;
	return currentSymbol;
}


/* Scan and return the next symbol and skip newline symbols.                  */
/*----------------------------------------------------------------------------*/
/* RETURNS: Symbol that has been scanned.                                     */
/*----------------------------------------------------------------------------*/
int	scanNextSymbolSkipNewline()
{
	do
	{
		scanNextSymbol();
	}
	while(currentSymbol == NEWLINE_SYM);

	return currentSymbol;
}


/* Get the current symbol.                                                    */
/*----------------------------------------------------------------------------*/
/* RETURNS: Current symbol.                                                   */
/*----------------------------------------------------------------------------*/
int	getCurrentSymbol()
{
	return currentSymbol;
}


/* Get the name of the current ident.                                         */
/*----------------------------------------------------------------------------*/
/* RETURNS: Name of the current ident.                                        */
/* If the current symbol is not an ident, the returned string is              */
/* unpredictable.                                                             */
/*----------------------------------------------------------------------------*/
char *	getCurrentIdentName()
{
	return currentIdentName;
}


/* Get the value of the current number.                                       */
/*----------------------------------------------------------------------------*/
/* RETURNS: Value of the current number.                                      */
/* If the current symbol is not a number, the returned value is               */
/* unpredictable.                                                             */
/*----------------------------------------------------------------------------*/
uint32_64_t	getCurrentNumberValue()
{
	return currentNumberValue;
}


/* Get the number of the current line.                                        */
/*----------------------------------------------------------------------------*/
/* RETURNS: Number of the current line.                                       */
/*----------------------------------------------------------------------------*/
int	getCurrentLine()
{
	return currentSymbolLine;
}


/* Get the number of the current column.                                      */
/*----------------------------------------------------------------------------*/
/* RETURNS: Number of the current column.                                     */
/*----------------------------------------------------------------------------*/
int	getCurrentColumn()
{
	return currentSymbolColumn;
}


