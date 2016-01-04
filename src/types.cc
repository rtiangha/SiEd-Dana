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

#include "types.h"
#include "error.h"
#include "resources.h"
#include "utility.h"

Boolean operator <(const Position & lhs,const Position & rhs)
{
	if(lhs.line<rhs.line)
		return true;
	else if(lhs.line==rhs.line)
		return lhs.x<rhs.x;
	else
		return false;
}

Boolean operator >(const Position & lhs,const Position & rhs)
{
	if(lhs.line>rhs.line)
		return true;
	else if(lhs.line==rhs.line)
		return lhs.x>rhs.x;
	else
		return false;
}

#ifdef TEST_OBJECT_UNIT
void block::perform_tests()
{

	init_block();
	ErrFatalDisplayIf(mem_chunk!=NULL||n_chars!=0,"Incorrect init,block");
	ensure_space(65000);
	ErrFatalDisplayIf(mem_chunk==NULL,"Null chunk, block");
	ErrFatalDisplayIf(mem_chunk->size!=65000,"Incorrect size, block");
	for(UInt16 i=0;i<65000;++i)
		mem_chunk->mem_ptr[i]='a';
	exit_block();
	ErrFatalDisplayIf(mem_chunk!=NULL||n_chars!=0,"Incorrect block exit");
#ifdef DEBUG_LOG

	log_entry("block tests complete");
#endif
}
#endif
void block::init_block()
{
#ifdef TEST_OBJECTS_LOG
	log_entry("block::init_block");
#endif

	n_chars = 0;
	mem_chunk=NULL;
}

void block::exit_block()
{
	if(mem_chunk!=NULL)
	{
		SiMemHandler::free_chunk(mem_chunk);
		mem_chunk=NULL;
	}
	n_chars=0;
}

Boolean block::ensure_space(const BlockInt amount)
{
#ifdef DEBUG
	BlockInt initial_size=0;
	if(mem_chunk!=NULL)
	{
		initial_size=mem_chunk->size;

	}
#endif
	//make sure that amount extra bytes are available in the block's data buffer
	if(mem_chunk==NULL)
	{
		//allocate a new data buffer if one doesn't exist
		SiMemChunk * temp_chunk=SiMemHandler::get_chunk(NULL,amount,TEXT_HEAP);

		if(temp_chunk==NULL)
			return false;
		else
			mem_chunk=temp_chunk;
	}
	else
	{
		BlockInt new_size=mem_chunk->size+amount;
		SiMemChunk * temp_chunk=SiMemHandler::resize_chunk(mem_chunk,new_size,TO_START);
		if(temp_chunk==NULL)
			return false;
		else if(temp_chunk->size!=new_size)
			return false;
		else
			mem_chunk=temp_chunk;
		if(mem_chunk->size==0)
		{
			mem_chunk=NULL;
		}
	}
	return true;

#ifdef DEBUG

	if(mem_chunk!=NULL)
		ErrFatalDisplayIf(mem_chunk->size!=initial_size+amount,"Incorrect block ensure space");
#endif
}
#ifdef TEST_OBJECT_UNIT
void Position::perform_tests()
{
	Position p(65000,31000);
	ErrFatalDisplayIf((p.x!=65000)||(p.line!=31000),"Incorrect init, Position");
#ifdef DEBUG_LOG

	log_entry("Position tests complete");
#endif
}
#endif
Position::Position()
{}
Position::Position(const BlockInt p_x,const Int16 p_line)
{
	x=p_x;
	line=p_line;
}

#ifdef TEST_OBJECT_UNIT
void DrawPosition::perform_tests()
{
	DrawPosition p(31000,31000);
	ErrFatalDisplayIf((p.x!=31000)||(p.line!=31000),"Incorrect init, DrawPosition");
#ifdef DEBUG_LOG

	log_entry("DrawPosition tests complete");
#endif
}
#endif

DrawPosition::DrawPosition()
{}

DrawPosition::DrawPosition(const Int16 p_x,const Int16 p_line)
{
	x=p_x;
	line=p_line;
}

void block_buffer::init_block_buffer()
{

	n_blocks=n_lines=0;
	blocks=NULL;
#ifdef DEBUG

	n_locks=0;
#endif

	add_blocks(INITIAL_LINES_PER_BUFFER,0);
	n_lines=0;

}

void block_buffer::exit_block_buffer()
{
	//delete the blocks contained in the buffer/
	//and tidy up
	if(blocks!=NULL)
	{
		block * b=(block*)MemHandleLock(blocks);
		for(Int16 i=0;i<n_blocks;++i)
		{


			b[i].exit_block();
		}
		MemHandleUnlock(blocks);
		MemHandleFree(blocks);

		//n_blocks=n_lines=0;
	}
}

block * block_buffer::get_block(const Int16 n)
{
	//return a pointer to the nth block in this buffer
	//must call release_blocks on this buffer at some point afterwards
#ifdef DEBUG
	++n_locks;
#endif

	block *blocks_array=(block*)MemHandleLock(blocks);

	return &(blocks_array[n]);
}

void block_buffer::release_blocks()
{
	//unlock the block data if required
	//if(n_locks>0)
#ifdef DEBUG
	ErrFatalDisplayIf(n_locks==0,"Unlocking unlocked block_buffer");
	--n_locks;
#endif

	MemHandleUnlock(blocks);
}

Boolean block_buffer::add_blocks(const Int16 n,Int16 pos)
{
#ifdef DEBUG
	ErrFatalDisplayIf(n_locks>0,"add with locked block");

#endif

	if(n==0)
		return true;
	//insert n new blocks into this buffer, starting at the position pos
	//pass in END_BUFFER as the position to always insert at the end
	if(pos==END_BUFFER)
		pos=n_blocks;

	if(blocks==NULL)
	{
		blocks=MemHandleNew(n*sizeof(block));
#ifdef DEBUG

		n_locks=0;
#endif

	}
	else
	{
		Err err=MemHandleResize(blocks,((n_blocks+n)*sizeof(block)));
		if(err!=errNone||MemHandleSize(blocks)!=((n_blocks+n)*sizeof(block)))
		{
			DisplayError(OUT_OF_MEMORY,"Block allocation");
			return false;
		}

	}
	if(blocks==NULL)
		return false;
	block * b=(block*)MemHandleLock(blocks);

	//if not the final block, move up the existing ones
	if(pos<n_blocks)
	{
		MemMove(b+(pos+n),b+pos,(n_blocks-pos)*sizeof(block));
	}

	n_blocks+=n;
	n_lines+=n;
	for(Int16 i=pos;i<pos+n;++i)
	{
		b[i].init_block();
	}

	MemHandleUnlock(blocks);
	return true;
}

void block_buffer::remove_block(const Int16 n)
{
	//remove the nth block from this buffer
	if(n>=n_blocks)
		return;
	block * b=(block*)MemHandleLock(blocks);
	b[n].exit_block();
	if(n+1<n_blocks)
	{
		MemMove(b+n,b+n+1,(n_blocks-n-1)*sizeof(block));
	}
	MemHandleUnlock(blocks);
	--n_blocks;
	if(n<n_lines)
		--n_lines;
	if(n_blocks==0)
	{
		MemHandleFree(blocks);
		blocks=NULL;
	}
	else
		MemHandleResize(blocks,n_blocks*sizeof(block));
}

#ifdef TEST_OBJECT_UNIT
Char buff[200]="incorrect block count, block_buffer";
Boolean block_buffer::perform_tests()
{
	init_block_buffer();

	ErrFatalDisplayIf(MemHandleSize(blocks)!=(n_blocks*sizeof(block)),"block_buffer test_fail : handle size");
	Int16 total=n_blocks;

	
	for(Int16 i=0;i<100;++i)
	{
		ErrFatalDisplayIf(!add_blocks(i,(i>0?i-1:0)),"Unable to add, block_buffer");
		total+=i;

		if(n_blocks!=total)
		{
			StrPrintF(buff+StrLen(buff)," n_blocks=%i, total=%i",n_blocks,total);
			ErrFatalDisplay(buff);
		}
	}

	for(Int16 i=n_blocks;i>=0;--i)
	{
		remove_block(i);




		ErrFatalDisplayIf(total!=n_blocks,"Incorrect block count on remove");

		//decrement after because the first remove shouldn't do anything
		--total;
	}
	exit_block_buffer();
	ErrFatalDisplayIf(n_blocks!=0||blocks!=NULL,"Incorrect exit on block_buffer");
#ifdef DEBUG_LOG

	log_entry("block_buffer tests complete");
#endif

	return true;
}
#endif
