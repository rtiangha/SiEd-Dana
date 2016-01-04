/*
 * SiEd - A text editor for PalmOS 3.5+ Copyright (C) 2003 Benjamin Roe
 * Contact Email:ben@benroe.com
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
#include "sections.h"
#include "resources.h"

#ifndef SIMEMHANDLE_HH
#define SIMEMHANDLE_HH

#define FREE (BlockInt)1
#define MASTER (BlockInt)4
#define LARGE_MEM_CHUNK 16000
#define SMALL_MEM_CHUNK 4096
#define SMALL_MEM_AMOUNT 256000
enum {TO_START,TO_END};
enum {FROM_END_TO_START,FROM_START_TO_END};


enum {NON_TEXT_HEAP,TEXT_HEAP};

class SiMemChunk
{
public:
	SiMemChunk();
	~SiMemChunk();
	Boolean expand(const BlockInt change,const UInt8 direction);
	void shrink(const BlockInt change,const UInt8 direction);
	SiMemChunk * split(const BlockInt change,const UInt8 direction);
	void insert_after(SiMemChunk * new_chunk);
	void insert_before(SiMemChunk * new_chunk);
	inline void modify_size(const BlockInt size,const UInt8 direction,const Int8 grow);

	Boolean next_chunk_is(SiMemChunk * other_chunk);

	Char * mem_ptr;
	SiMemChunk * next_chunk;
	SiMemChunk * prev_chunk;
	UInt8 status;
	BlockInt size;
};

//memory handler functions
class SiMemHandler
{
public:
	friend class SiMemChunk;

	static void 		initialise_memory() __attribute__ ((section ("utilfns")));
	static void 		exit_memory() __attribute__ ((section ("utilfns")));
	static SiMemChunk * 	get_chunk(SiMemChunk* parent,const BlockInt size,const Int16 allocation_type) __attribute__ ((section ("utilfns"))) ;
	static SiMemChunk * 	get_chunk_quick(const BlockInt size)  __attribute__ ((section ("utilfns")));
	static SiMemChunk *	resize_chunk(SiMemChunk * chunk,const BlockInt new_size,const UInt8 direction) __attribute__ ((section ("utilfns")));
	static void 		free_chunk(SiMemChunk * chunk)  __attribute__ ((section ("utilfns")));
	static SiMemChunk *	move_data(SiMemChunk * src_chunk,SiMemChunk * dst_chunk,const BlockInt src_offset,const UInt8 direction) ;
	static void 			compact_memory() __attribute__ ((section ("utilfns")));

#ifdef DEBUG

	static void mem_show_mem_usage() __attribute__ ((section ("utilfns")));

#endif

	static void out_of_memory();

#ifdef TEST_OBJECT_UNIT

	static void perform_tests() __attribute__ ((section ("utilfns")));
#endif
#ifdef DEBUG

	static UInt32 mem_get_free_chunk_count() __attribute__ ((section ("utilfns")));
	static UInt32 mem_get_allocated_chunk_count() __attribute__ ((section ("utilfns")));
	static void output_chunk_list() __attribute__ ((section ("utilfns")));

#endif

	static UInt16 MEMORY_CHUNK_SIZE;
private:
#ifdef DEBUG

	static void check_heap_consistency() __attribute__ ((section ("utilfns")));
#endif

	static void defragment_chunks(SiMemChunk * start_chunk) __attribute__ ((section ("utilfns")));
	static SiMemChunk * split_chunk(SiMemChunk * chunk,const BlockInt split_pos,const UInt8 direction) __attribute__ ((section ("utilfns")));
	static SiMemChunk* reallocate_chunk(SiMemChunk * chunk,const BlockInt new_size,const UInt8 direction) __attribute__ ((section ("utilfns")));

	static SiMemChunk * create_chunk(const BlockInt size,const Int16 allocation_type);
	static SiMemChunk * find_free_chunk(SiMemChunk * start_chunk,const BlockInt min_size,const Int16 allocation_type);

	static inline void allocate_chunk(SiMemChunk* chunk) ;
	static void merge_chunks(SiMemChunk * first_chunk,SiMemChunk * second_chunk) ;
	static void delete_chunk(SiMemChunk * chunk) ;
	static void insert_after_chunk(SiMemChunk* new_chunk,SiMemChunk* parent) ;
	static SiMemChunk * check_for_zero_size(SiMemChunk * chunk) __attribute__ ((section ("utilfns")));


	static void select_chunk_size();
	static SiMemChunk * head_chunk_text;
	static SiMemChunk * last_chunk_text;
	static SiMemChunk * head_chunk_non_text;
	static SiMemChunk * last_chunk_non_text;


};


#endif
