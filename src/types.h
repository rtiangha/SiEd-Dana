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


#ifndef TYPES_HH
#define TYPES_HH
#include <PalmOS.h>
#include <PalmTypes.h>
#include "text_resource.h"
#include "memhandler.h"
enum { CONTROL, EDIT };
enum {PALM_STYLE,STANDARD_STYLE};
enum { SMALL_CHUNKS, MEDIUM_CHUNKS,LARGE_CHUNKS,OFF_CHUNKS };

#define SMALL_LOAD_CHUNK_SIZE 800
#define MEDIUM_LOAD_CHUNK_SIZE 1800
#define LARGE_LOAD_CHUNK_SIZE 4000
#define MAXIMUM_CHUNK_SIZE 32766
#define MAX_PATH_LENGTH 512

#include "resources.h"

class Position
{
public:
	Position();
	Position(const BlockInt p_x,const Int16 p_line);
	//Position operator =(const Position & rhs);
	Int16 line;
	BlockInt  x;
#ifdef TEST_OBJECT_UNIT

	void perform_tests() __attribute__ ((section("editfns")));
#endif
};
Boolean operator <(const Position & lhs,const Position & rhs);
Boolean operator >(const Position & lhs,const Position & rhs);

class DrawPosition

{
public:
	DrawPosition();
	DrawPosition(const Int16 x,const Int16 line);
	//Position operator =(const Position & rhs);
	Int16 line;
	Int16  x;
#ifdef TEST_OBJECT_UNIT

	void perform_tests() __attribute__ ((section("editfns")));
#endif
};
class block

{
public:
	void init_block();
	void exit_block();
	Boolean ensure_space(const BlockInt amount);
	SiMemChunk * mem_chunk;

	BlockInt n_chars;
#ifdef TEST_OBJECT_UNIT

	void perform_tests() __attribute__ ((section("editfns")));
#endif
};

class block_buffer
{
public:
	void init_block_buffer();
	void exit_block_buffer();
	block * get_block(const Int16 n);
	void release_blocks();
	Boolean add_blocks(const Int16 n,const Int16 pos);
	Int16 n_blocks;
	Int16 n_lines;
	void remove_block(const Int16 n);

#ifdef TEST_OBJECT_UNIT

	Boolean perform_tests();
#endif
#ifdef DEBUG

	Int8 n_locks;
#endif
protected:

	MemHandle blocks;
};




#endif
