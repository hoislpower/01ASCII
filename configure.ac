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



AC_INIT([01ASCII], [1.0], [patrick.hoislbauer@gmail.com])
AM_INIT_AUTOMAKE([])

# create config header file
AC_CONFIG_HEADERS(src/config.h)

# switch to disable 64 support
AC_ARG_ENABLE([64bit], AS_HELP_STRING([--disable-64bit], [Disables 64 bit support.]), 
   AC_DEFINE([USING_64BIT], [0], [Support for 64 bit.]), AC_DEFINE([USING_64BIT], [1], [Support for 64 bit.]))

# check for c compiler and standard c library
AC_PROG_CC
AC_HEADER_STDC

# create makefiles
AC_CONFIG_FILES([
   Makefile
   src/Makefile
])

AC_OUTPUT()

