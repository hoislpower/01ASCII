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



// -----------------------------------------------------------------------------
// Terminal symbols: '!'   ...exclamationMarkSym
//                   '"'   ...doubleQuoteSym
//                   '''   ...singleQuoteSym
//                   '-'   ...hyphenSym
//                   '*'   ...asteriskSym
//                   ','   ...commaSym
//                   '('   ...leftParenthesisSym
//                   ')'   ...rightParenthesisSym
//                   '{'   ...leftCurlyBracketSym
//                   '}'   ...rightCurlyBracketSym
//                   '='   ...equalSym
// -----------------------------------------------------------------------------


#ifndef _SCANNER_H
#define _SCANNER_H


#include "device-description.h"


#define	MAX_IDENT_LENGTH	255


// terminal symbols
static const char terminalSymList[] = 
{
	'\"',	// doubleQuoteSym
	'\'',	// singleQuoteSym
	'-',	// hyphenSym
	'*',	// asteriskSym
	',',	// commaSym
	'(',	// leftParenthesisSym
	')',	// rightParenthesisSym
	'{',	// leftCurlyBracketSym
	'}',	// rightCurlyBracketSym
	'=',	// equalSym
	'\0'	// no terminal symbol (just termination of the list)
};


// symbols
typedef enum
{
	DOUBLE_QUOTE_SYM,
	SINGLE_QUOTE_SYM,
	HYPHEN_SYM,
	ASTERISK_SYM,
	COMMA_SYM,
	LEFT_PARENTHESIS_SYM,
	RIGHT_PARENTHESIS_SYM,
	LEFT_CURLY_BRACKET_SYM,
	RIGHT_CURLY_BRACKET_SYM,
	EQUAL_SYM,

	IDENT_SYM,
	HEX_NUMBER_SYM,
	DEC_NUMBER_SYM,

	NEWLINE_SYM,
	UNKNOWN_SYM,
	EOF_SYM
}
symbol;


extern	int	initializeScanner(char *);
extern	void	disposeScanner();
extern	int	scanNextSymbol();
extern	int	scanNextSymbolSkipNewline();
extern	int	getCurrentSymbol();
extern	char *	getCurrentIdentName();
extern	uint32_64_t	getCurrentNumberValue();
extern	int	getCurrentLine();
extern	int	getCurrentColumn();

#endif /* _SCANNER_H */

