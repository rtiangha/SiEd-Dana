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

#include "memchunk.h"

#define STATUS_INDEX 0
#define SIZE_INDEX 2
#define NEXT_CHUNK_INDEX 8
#define PREV_CHUNK_INDEX 4



SiMemChunk()
{}
virtual ~SiMemChunk()
{}
void SiMemChunk::page_in()
{}
void SiMemChunk::page_out()
{}
SiMemChunk * SiMemChunk::next_chunk()
{
	return (SiMemChunk*)mem_ptr[NEXT_CHUNK_INDEX];
}
SiMemChunk * SiMemChunk::prev_chunk()
{
	return (SiMemChunk*)mem_ptr[PREV_CHUNK_INDEX];
}
UInt8 SiMemChunk::status()
{
	return (SiMemChunk*)mem_ptr[STATUS_INDEX];
}
Int16 SiMemChunk::size()
{
	return (SiMemChunk*)mem_ptr[SIZE_INDEX];
}
Char * SiMemChunk::data(Int8 mode)
{}
void SiMemChunk::set_next_chunk(SiMemChunk * chunk)
{}
void SiMemChunk::set_prev_chunk(SiMemChunk * chunk)
{}
void SiMemChunk::set_size(Int16 size)
{}
void SiMemChunk::set_status(Int8 status)
{}
