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



#include "bit-array.h"


/* global Variables */
static	int8_t *array = NULL;
static	int arrayIndex;
static	int arrayLength;


/* Initializes the bit array                                                  */
/* This procedure must be called once before using the other procedures.      */
/*----------------------------------------------------------------------------*/
/* IN length: Length of the new bit array.                                    */
/* RETURNS: EXIT_FAILURE if a failure occurred, EXIT_SUCCESS otherwise.       */
/*----------------------------------------------------------------------------*/
int	initializeBitArray(int length)
{
	int i;


	arrayLength = length;
	arrayIndex = 0;

	/* allocate memory for the array and an additional buffer for */
	/* temporarily storing parts of the bit array */
	array = malloc(arrayLength*sizeof(int8_t));
	if(array == NULL)
	{
		fprintf(stderr, "ERROR: Could not allocate memory for bit "
			"array.\r\n");
		return EXIT_FAILURE;
	}

	/* initialize the array with unused bits */
	for(i=0; i<arrayLength; i++)
		array[i] = UNUSED_BIT;

	return EXIT_SUCCESS;
}


/* Disposes the bit array                                                     */
/* This procedure must be called once when the bit array is no longer used.   */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
void	disposeBitArray()
{
	if(array != NULL)
		free(array);
}


/* Get the bit array                                                          */
/* The returned bit array must not be written.                                */
/*----------------------------------------------------------------------------*/
/* RETURNS: The bit array.                                                    */
/*----------------------------------------------------------------------------*/
int8_t*	getBitArray()
{
	return array;
}


/* Get the current index.                                                     */
/*----------------------------------------------------------------------------*/
/* RETURNS: Current index within the bit array.                               */
/*----------------------------------------------------------------------------*/
int	bitArrayGetCurrentIndex()
{
	return arrayIndex;
}


/* Appends a bit range to the bit array                                       */
/* e.g.: When startBit=3 and endBit=5 then 3,4,5 is added to the array.       */
/*----------------------------------------------------------------------------*/
/* IN startBit: Start bit of the range.                                       */
/* IN endBit: End bit of the range.                                           */
/* RETURNS: EXIT_FAILURE if the result exceeded the length of the array,      */
/*          EXIT_SUCCESS otherwise.                                           */
/*          If EXIT_FAILURE is returned the bit array will be left unchanged. */
/*----------------------------------------------------------------------------*/
int	bitArrayAdd(int8_t startBit, int8_t endBit)
{
	int i;


	if(endBit >= startBit)
	{
		/* check if the length of the array will be exceeded */
		if(arrayIndex + endBit-startBit+1 > arrayLength)
			return EXIT_FAILURE;

		/* add the bits to the array */
		for(i=startBit; i<=endBit; i++)
		{
			array[arrayIndex] = i;
			arrayIndex++;
		}
	}
	else
	{
		/* check if the length of the array will be exceeded */
		if(arrayIndex + startBit-endBit+1 > arrayLength)
			return EXIT_FAILURE;

		/* add the bits to the array */
		for(i=startBit; i>=endBit; i--)
		{
			array[arrayIndex] = i;
			arrayIndex++;
		}
	}

	return EXIT_SUCCESS;
}


/* Repeat parts of the array.                                                 */
/*----------------------------------------------------------------------------*/
/* IN startIndex: Start index of the array part to repeat.                    */
/*                (The repeat includes the element at this index.)            */
/* IN endIndex: End index of the array part to repeat.                        */
/*              (The repeat includes the element at this index.)              */
/* IN repeatNum: Number of repeats.                                           */
/* RETURNS: EXIT_FAILURE if the result exceeded the length of the array,      */
/*          EXIT_SUCCESS otherwise.                                           */
/*          If EXIT_FAILURE is returned the bit array will be left unchanged. */
/*----------------------------------------------------------------------------*/
int	bitArrayRepeat(int startIndex, int endIndex, int repeatNum)
{
	int i;
	int sequenceLength;


	/* calculate the length of the sequence */
	sequenceLength = endIndex - startIndex + 1;

	/* exit if there is nothing to repeat (return no failure) */
	if(sequenceLength < 1)
		return EXIT_SUCCESS;

	/* check if the length of the array will be exceeded */
	if(arrayIndex + sequenceLength*repeatNum > arrayLength)
		return EXIT_FAILURE;

	/* shift the part which follows the repeat section */
	/* to the end of the array */
	memcpy(array+endIndex+1+sequenceLength*repeatNum, array+endIndex+1, 
		arrayIndex-endIndex-1);

	/* repeat the given bit sequence */
	for(i=0; i<repeatNum; i++)
	{
		memcpy(array+endIndex+1+i*sequenceLength, array+startIndex,
			sequenceLength);
		arrayIndex += sequenceLength;
	}

	return EXIT_SUCCESS;
}


