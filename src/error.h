/*
 * SiEd-Dana - A text editor optimized for the AlphaSmart Dana.
 * Copyright (C) 2016 Reg Tiangha
 * Contact:  reg@reginaldtiangha.com
 *
 * Portions of SiEd-Dana are based off the original SiEd project,
 * Copyright (C) 2003-2005 Benjamin Roe
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <PalmOS.h>
#include <PalmTypes.h>
#include "resources.h"
#include "sections.h"
#ifndef ERROR_HPP
#define ERROR_HPP
enum { FILE_NOT_FOUND, UNABLE_TO_SAVE, FILE_ACCESS_ERROR, OUT_OF_MEMORY, UNKNOWN_FORM, NOT_SUPPORTED, DATABASE_ERROR, SCREEN_ERROR,NOT_IMPLEMENTED, CLIPBOARD_TRUNCATION,INVALID_FILE_NAME,UNABLE_TO_LOAD_LIB,NOT_ENCRYPTED,UNABLE_TO_CREATE_DIR,NO_COMPRESSED_SUPPORT,FILE_TRUNCATION_ERROR,DEBUG_MESSAGE};
void DisplayError(const UInt16 type, const char *data) __attribute__ ((section ("utilfns")));
#ifdef __DEBUG_LOG__
void DebugMessage(const char* c) __attribute__ ((section ("utilfns")));

void DebugNumber(const char * c,const Int32 n) __attribute__ ((section ("utilfns")));
#endif

#ifdef DEBUG_LOG
void log_entry(const char * buff) __attribute__ ((section ("utilfns")));
void log_entry_number(const char * buff,const Int32 num) __attribute__ ((section ("utilfns")));
#endif

#endif
