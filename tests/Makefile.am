#
#   01ASCII
#
#   Converter for hexadecimal or binary files into textfiles containing 
#   the data in form of ASCII 0s and 1s.
#
#   Authored By Patrick Hoislbauer  <patrick.hoislbauer@gmail.com>
#
#   Copyright (C) 2015 Patrick Hoislbauer
#
#   This file is part of 01ASCII.
#
#   01ASCII is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   01ASCII is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with 01ASCII.  If not, see <http://www.gnu.org/licenses/>.
#
#   Diese Datei ist Teil von 01ASCII.
#
#   01ASCII ist Freie Software: Sie können es unter den Bedingungen
#   der GNU General Public License, wie von der Free Software Foundation,
#   Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
#   veröffentlichten Version, weiterverbreiten und/oder modifizieren.
#
#   01ASCII wird in der Hoffnung, dass es nützlich sein wird, aber
#   OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
#   Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
#   Siehe die GNU General Public License für weitere Details.
#
#   Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
#   Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
#



if HAVE_CHECK
   TESTS = check_devicedescription check_bininput check_hexinput check_bitarray
   TESTS += check_scanner check_converter

   check_PROGRAMS = check_devicedescription check_bininput check_hexinput
   check_PROGRAMS += check_bitarray check_scanner check_converter
else
   TESTS = 

   check_PROGRAMS = 
endif

check_converter_SOURCES = converter_tests.c
check_converter_CFLAGS = @CHECK_CFLAGS@ -I ../src/
check_converter_LDADD = @CHECK_LIBS@ ../src/converter.o
check_converter_LDADD += ../src/device-description.o

check_scanner_SOURCES = scanner_tests.c
check_scanner_CFLAGS = @CHECK_CFLAGS@ -I ../src/
check_scanner_LDADD = @CHECK_LIBS@ ../src/scanner.o

check_bitarray_SOURCES = bitarray_tests.c
check_bitarray_CFLAGS = @CHECK_CFLAGS@ -I ../src/
check_bitarray_LDADD = @CHECK_LIBS@ ../src/bit-array.o

check_hexinput_SOURCES = hexinput_tests.c
check_hexinput_CFLAGS = @CHECK_CFLAGS@ -I ../src/
check_hexinput_LDADD = @CHECK_LIBS@ ../src/hex-input.o
check_hexinput_LDADD += ../src/device-description.o

check_bininput_SOURCES = bininput_tests.c
check_bininput_CFLAGS = @CHECK_CFLAGS@ -I ../src/
check_bininput_LDADD = @CHECK_LIBS@ ../src/bin-input.o
check_bininput_LDADD += ../src/device-description.o

check_devicedescription_SOURCES = devicedescription_tests.c
check_devicedescription_CFLAGS = @CHECK_CFLAGS@ -I ../src/
check_devicedescription_LDADD = @CHECK_LIBS@ ../src/device-description.o


clean-local:
	-rm testdevice

