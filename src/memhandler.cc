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
#include "memhandler.h"
#include "error.h"
#include "text_resource.h"
#ifdef BENCH_OBJECTS
#include "test.h"
#endif
//Memchunk functions


SiMemChunk * SiMemHandler::head_chunk_text=NULL;
SiMemChunk * SiMemHandler::last_chunk_text=NULL;
SiMemChunk * SiMemHandler::head_chunk_non_text=NULL;
SiMemChunk * SiMemHandler::last_chunk_non_text=NULL;

UInt16 SiMemHandler::MEMORY_CHUNK_SIZE = 0;

#ifdef DEBUG
Int16 number_allocated_chunks=0;
#endif

#ifdef DEBUG
UInt32 SiMemHandler::mem_get_free_chunk_count()
{
	SiMemChunk * curr_chunk=head_chunk_text;
	UInt32 free_mem=0;

	for(Int16 i=0;i<2;++i)
	{

		while(curr_chunk!=NULL)
		{
			if(curr_chunk->status&FREE)
				free_mem+=curr_chunk->size;
			curr_chunk=curr_chunk->next_chunk;

		}
		curr_chunk=head_chunk_non_text;
	}

	Char buff[100];
	StrPrintF(buff,"Free memory still allocated:%lu",free_mem);
	//DisplayError(DEBUG_MESSAGE,buff);
	return free_mem;
}

UInt32 SiMemHandler::mem_get_allocated_chunk_count()
{
	SiMemChunk * curr_chunk=head_chunk_text;
	UInt32 free_mem=0;

	for(Int16 i=0;i<2;++i)
	{

		while(curr_chunk!=NULL)
		{
			if(!(curr_chunk->status&FREE))
				free_mem+=curr_chunk->size;
			curr_chunk=curr_chunk->next_chunk;

		}
		curr_chunk=head_chunk_non_text;
	}

	//  Char buff[100];
	//  StrPrintF(buff,"Free memory still allocated:%lu",free_mem);
	//DisplayError(DEBUG_MESSAGE,buff);
	return free_mem;
}

void SiMemHandler::mem_show_mem_usage()
{
	Char buff[200];
	UInt32 free_mem,max_chunk;
	MemHeapFreeBytes(0,&free_mem,&max_chunk);
	UInt32 free_alloc_mem=mem_get_free_chunk_count();
	UInt32 allocated_alloc_mem=mem_get_allocated_chunk_count();

	StrPrintF(buff,"Free mem=%lu, Max Chunk=%lu,free allocated count=%lu, allocated chunk count=%lu",free_mem,max_chunk,free_alloc_mem,allocated_alloc_mem);
	DisplayError(DEBUG_MESSAGE,buff);
#ifdef DEBUG

	check_heap_consistency();
	#ifdef DEBUG_LOG
	output_chunk_list();
	#endif

#endif
	//compact_memory();

}
#endif
#ifdef DEBUG
void SiMemHandler::check_heap_consistency()
{
	//traverse the heap forwards and backwards ensuring the heap
	//is consistent

	SiMemChunk * temp_chunk=head_chunk_text;

	for(Int16 i=0;i<2;++i)
	{
		while(temp_chunk!=NULL)
		{
			ErrFatalDisplayIf(temp_chunk->size==0,"Zero size chunk");

			if(temp_chunk->next_chunk==NULL)
				break;

			if(!(temp_chunk->next_chunk->status&MASTER))
				if(temp_chunk->next_chunk->mem_ptr!=(temp_chunk->mem_ptr+temp_chunk->size))
					ErrDisplay("Non contiguous next blocks - memory leaked");

			temp_chunk=temp_chunk->next_chunk;
		}

		temp_chunk=last_chunk_text;

		while(temp_chunk!=NULL)
		{
			if(temp_chunk->prev_chunk==NULL)
				break;
			if(!(temp_chunk->status&MASTER))
				if(temp_chunk->prev_chunk->mem_ptr+temp_chunk->prev_chunk->size!=(temp_chunk->mem_ptr))
					ErrDisplay("Non contiguous previous blocks - memory leaked");

			temp_chunk=temp_chunk->prev_chunk;

		}
		temp_chunk=head_chunk_non_text;
	}
	//	DisplayError(DEBUG_MESSAGE,"Heap check passed");
}
#endif
#ifdef TEST_OBJECT_UNIT
void SiMemHandler::perform_tests()
{
	//test get chunk
	SiMemChunk * chunk_one;
	SiMemChunk * chunk_two;
	SiMemChunk * chunk_three;
	SiMemChunk * chunk_four;

	//test get_chunk, resize_chunk, free_chunk and allocate_chunk
	for(UInt16 i=2;i<1000;++i)
	{

		chunk_one=get_chunk(NULL,i,NON_TEXT_HEAP);

		chunk_two=get_chunk(NULL,i,TEXT_HEAP);

		chunk_three=get_chunk(chunk_two,i,TEXT_HEAP);


		chunk_four=get_chunk(chunk_one,i,NON_TEXT_HEAP);

		chunk_one=resize_chunk(chunk_one,i*2,TO_END);
		chunk_two=resize_chunk(chunk_two,i/2,TO_START);
		chunk_three=resize_chunk(chunk_three,i/2,TO_END);
		chunk_four=resize_chunk(chunk_four,i*2,TO_START);
		ErrFatalDisplayIf(chunk_one==NULL,"Null Chunk one");
		ErrFatalDisplayIf(chunk_two==NULL,"Null Chunk two");
		ErrFatalDisplayIf(chunk_three==NULL,"Null Chunk three");
		ErrFatalDisplayIf(chunk_four==NULL,"Null Chunk four");

		ErrFatalDisplayIf(chunk_one->size!=i*2,"Chunk_one incorrect size");

		ErrFatalDisplayIf(chunk_two->size!=i/2,"Chunk_two incorrect size");

		ErrFatalDisplayIf(chunk_three->size!=i/2,"Chunk_three incorrect size");

		ErrFatalDisplayIf(chunk_four->size!=i*2,"Chunk_four incorrect size");

		free_chunk(chunk_one);
		free_chunk(chunk_two);
		free_chunk(chunk_three);
		free_chunk(chunk_four);
#ifdef DEBUG

		check_heap_consistency();
#endif

	}

	//test move_data

	for(UInt16 i=4;i<255;i+=4)
	{
		chunk_one=get_chunk(NULL,i,TEXT_HEAP);

		chunk_two=get_chunk(NULL,i,TEXT_HEAP);

		for(UInt16 j=0;j<i;++j)
		{
			chunk_one->mem_ptr[j]=(Char)j/2;
			chunk_two->mem_ptr[j]=(Char)j/2;
		}
		move_data(chunk_one,chunk_two,i/2,FROM_END_TO_START);

		ErrFatalDisplayIf(chunk_one->size!=(i-(i/2)),"Incorrect move_data, chunk_one");
		ErrFatalDisplayIf(chunk_two->size!=(i+(i/2)),"Incorrect move_data, chunk_two");
		for(UInt16 j=0;j<(i-(i/2));++j)
		{
			ErrFatalDisplayIf(chunk_one->mem_ptr[j]!=(Char)j/2,"Incorrect value in data move");
		}
		free_chunk(chunk_one);
		free_chunk(chunk_two);

		chunk_one=get_chunk(NULL,i,TEXT_HEAP);

		chunk_two=get_chunk(chunk_two,i,TEXT_HEAP);

		for(UInt16 j=0;j<i;++j)
		{
			chunk_one->mem_ptr[j]=(Char)j/2;
			chunk_two->mem_ptr[j]=(Char)j/2;
		}

		move_data(chunk_one,chunk_two,i/2,FROM_END_TO_START);

		for(UInt16 j=0;j<(i-(i/2));++j)
		{
			ErrFatalDisplayIf(chunk_one->mem_ptr[j]!=(Char)j/2,"Incorrect value in data move");
			ErrFatalDisplayIf(chunk_two->mem_ptr[j]!=(Char)(j+(i-(i/2)))/2,"Incorrect value in data move");

		}

		ErrFatalDisplayIf(chunk_one->size!=(i-(i/2)),"Incorrect move_data, chunk_one");
		ErrFatalDisplayIf(chunk_two->size!=(i+(i/2)),"Incorrect move_data, chunk_two");

		free_chunk(chunk_one);
		free_chunk(chunk_two);
		chunk_one=get_chunk(NULL,i,TEXT_HEAP);

		chunk_two=get_chunk(chunk_two,i,TEXT_HEAP);

		for(UInt16 j=0;j<i;++j)
		{
			chunk_one->mem_ptr[j]=(Char)j/2;
			chunk_two->mem_ptr[j]=(Char)j/2;
		}
		move_data(chunk_one,chunk_two,i/2,FROM_START_TO_END);
		ErrFatalDisplayIf(chunk_one->size!=(i-(i/2)),"Incorrect move_data, chunk_one");
		ErrFatalDisplayIf(chunk_two->size!=(i+(i/2)),"Incorrect move_data, chunk_two");

		for(UInt16 j=0;j<(i-(i/2));++j)
		{
			ErrFatalDisplayIf(chunk_one->mem_ptr[j]!=(Char)(j+(i-(i/2)))/2,"Incorrect value in data move");
			ErrFatalDisplayIf(chunk_two->mem_ptr[j]!=(Char)(j)/2,"Incorrect value in data move");
		}

		free_chunk(chunk_one);
		free_chunk(chunk_two);
	}
	//test large allocations
	for(UInt16 i=4000;i<32000;i+=200)
	{
		chunk_one=get_chunk(NULL,i,TEXT_HEAP);
		chunk_two=get_chunk(chunk_one,i,TEXT_HEAP);
		chunk_one=resize_chunk(chunk_one,i+100,TO_START);
		chunk_two=resize_chunk(chunk_two,i-100,TO_END);
		ErrFatalDisplayIf(chunk_one->size!=i+100,"Incorrect resize TO_START");
		ErrFatalDisplayIf(chunk_two->size!=i-100,"Incorrect resize TO_END");
		free_chunk(chunk_one);
		free_chunk(chunk_two);
	}
#ifdef DEBUG
	check_heap_consistency();
#endif

	exit_memory();
}

#endif

SiMemChunk::SiMemChunk()
{
	prev_chunk=next_chunk=NULL;
	size=0;
	status=FREE;
#ifdef DEBUG

	++number_allocated_chunks;
#endif
}

SiMemChunk::~SiMemChunk()
{
#ifdef DEBUG_LOG
	log_entry("Deleting chunk");
#endif
	//if this is a master chunk and has no dependent chunks, free mem_ptr
	if(status&MASTER)
	{
		if(next_chunk==NULL)
		{
#ifdef DEBUG_LOG
			log_entry("Freeing master pointer");
#endif

			if(mem_ptr!=NULL)
				MemPtrFree(mem_ptr);
		}

		else if(next_chunk->status&MASTER)
		{
#ifdef DEBUG_LOG
			log_entry("Freeing master pointer");
#endif
	  #ifdef DEBUG

			ErrFatalDisplayIf(mem_ptr==NULL,"NULL mem_ptr deleted");
#endif

			if(mem_ptr!=NULL)
				MemPtrFree(mem_ptr);
		}

	}
	//remove self from linked list
	if(next_chunk!=NULL)
		next_chunk->prev_chunk=prev_chunk;
	if(prev_chunk!=NULL)
		prev_chunk->next_chunk=next_chunk;
#ifdef DEBUG

	--number_allocated_chunks;
#endif
}
Boolean SiMemChunk::next_chunk_is(SiMemChunk * other_chunk)
{
	//return true if the chunk passed in is contiguous with this one
	//and not a master chunk
	if(next_chunk==other_chunk)
		return !(other_chunk->status&MASTER);
	else
		return false;

}

Boolean SiMemChunk::expand(const BlockInt change,const UInt8 direction)
{
#ifdef DEBUG_LOG
	log_entry_number("chunk expand ",change);
#endif

	if(TO_START==direction)
	{
		//expand this chunk using free space in the next
		//chunk in the list if possible
		if(next_chunk!=NULL)
		{
			if((next_chunk->status&FREE)&&(!(next_chunk->status&MASTER))&&(next_chunk->size>=change))
			{

				next_chunk->modify_size(change,TO_START,-1);
				size+=change;
				SiMemHandler::check_for_zero_size(next_chunk);
				return true;

			}
		}
	}
	else
	{
		//expand this chunk downwards using free space in the previous chunk
		//if possible
		if((prev_chunk!=NULL)&&(!(status&MASTER)))
		{
			if((prev_chunk->status&FREE)&&(prev_chunk->size>=change))
			{
				prev_chunk->modify_size(change,TO_END,-1);

				size+=change;
				mem_ptr-=change;
				SiMemHandler::check_for_zero_size(prev_chunk);
				return true;
			}
		}
	}
	return false;


}

void SiMemChunk::shrink(const BlockInt change,const UInt8 direction)
{
#ifdef DEBUG
	ErrFatalDisplayIf(change>size,"shrinking chunk to negative size");
	ErrFatalDisplayIf(change==size,"shrinking to zero size");

	Int16 original_size=size;
#endif
#ifdef DEBUG_LOG

	log_entry_number("shrink chunk ",change);

#endif

	Boolean done=false;

	if(TO_START==direction)
	{
		if(next_chunk!=NULL)
		{

			if((next_chunk->status&FREE)&&(!(next_chunk->status&MASTER)))
			{
				next_chunk->modify_size(change,TO_START,1);
				size-=change;
				done=true;

			}
		}
		if(!done)
			SiMemHandler::free_chunk(split(change,TO_START));

	}
	else
	{
		if(prev_chunk!=NULL)
		{

			if((prev_chunk->status&FREE)&&(!(status&MASTER)))
			{
				size-=change;
				mem_ptr+=change;

				prev_chunk->modify_size(change,TO_END,1);
				done=true;

			}
		}
		if(!done)
			SiMemHandler::free_chunk( split(change,TO_END) );
	}
#ifdef DEBUG
	ErrFatalDisplayIf(size!=original_size-change,"Incorrect chunk shrink");
	SiMemHandler::check_heap_consistency();

#endif
}

inline void SiMemChunk::modify_size(const UInt16 change,const UInt8 direction,const Int8 grow)
{
#ifdef DEBUG_LOG
	log_entry_number("chunk modify_size ",change);
#endif
  #ifdef DEBUG

	ErrFatalDisplayIf(direction==TO_START&&status&MASTER,"Modifying master pointer");

	ErrFatalDisplayIf(grow<0&&size<change,"modifying size to a negative amount");
#endif

	if(direction==TO_START)
	{
		if(grow<0)
		{
			size-=change;
			mem_ptr+=change;
		}
		else
		{
			mem_ptr-=change;
			size+=change;
		}



	}
	else
	{
		if(grow>0)
			size+=change;
		else
			size-=change;
	}
}

SiMemChunk * SiMemChunk::split(const BlockInt change,const UInt8 direction)
{
	//split TO_END puts new chunk before this one
	//split TO_START puts new chunk after this one
	//ie position refers to where this chunk ends up
#ifdef DEBUG
	UInt16 original_size=size;
	ErrFatalDisplayIf(change>size,"Splitting off end of chunk");


#endif
  #ifdef DEBUG_LOG

	if(direction==TO_START)
		log_entry_number("split chunk TO_START ",change);
	else
		log_entry_number("split chunk TO_END ",change);

#endif
	//work out where to do the split
	//change is positive but indicates how much to shrink by

	SiMemChunk * new_chunk=NULL;

	if(direction==TO_START)
	{
		//want to keep contents at start of chunk

		new_chunk=new SiMemChunk();
		if(NULL==new_chunk)
		{
			SiMemHandler::out_of_memory();
		}
		new_chunk->mem_ptr=mem_ptr+(size-change);

		new_chunk->size=change;

		modify_size(change,TO_END,-1);

		insert_after(new_chunk);

	}
	else
	{
		//want to keep contents at end of chunk

		new_chunk=new SiMemChunk();
		if(NULL==new_chunk)
		{
			SiMemHandler::out_of_memory();

		}
		new_chunk->mem_ptr=mem_ptr;
		new_chunk->size=change;



		if(status&MASTER)
		{
			new_chunk->status|=MASTER;
			status&=~MASTER;
		}
		modify_size(change,TO_START,-1);


		insert_before(new_chunk);

	}
	if(!(status&FREE))
		SiMemHandler::allocate_chunk(new_chunk);

#ifdef DEBUG

	if(new_chunk!=NULL)
		ErrFatalDisplayIf((new_chunk->size+size)!=original_size,"Incorrect chunk split");
	ErrFatalDisplayIf(size!=original_size-change,"Incorrect chunk split: original");
	SiMemHandler::check_heap_consistency();


#endif

	return new_chunk;

}


void SiMemChunk::insert_before(SiMemChunk * new_chunk)
{
#ifdef DEBUG_LOG
	log_entry_number("insert_before ",new_chunk->size);
#endif

	if(this==SiMemHandler::head_chunk_text)
	{
		SiMemHandler::head_chunk_text=new_chunk;
	}
	else if(this==SiMemHandler::head_chunk_non_text)
	{
		SiMemHandler::head_chunk_non_text=new_chunk;
	}
	if(prev_chunk!=NULL)
	{
		prev_chunk->next_chunk=new_chunk;
		new_chunk->prev_chunk=prev_chunk;

	}

	prev_chunk=new_chunk;
	new_chunk->next_chunk=this;

}

void SiMemChunk::insert_after(SiMemChunk * new_chunk)
{
#ifdef DEBUG_LOG
	log_entry_number("insert_after ",new_chunk->size);
#endif

	if(this==SiMemHandler::last_chunk_text)
		SiMemHandler::last_chunk_text=new_chunk;
	else if(this==SiMemHandler::last_chunk_non_text)
		SiMemHandler::last_chunk_non_text=new_chunk;

	if(next_chunk!=NULL)
	{
		next_chunk->prev_chunk=new_chunk;
		new_chunk->next_chunk=next_chunk;
	}
	next_chunk=new_chunk;
	new_chunk->prev_chunk=this;
#ifdef DEBUG

	SiMemHandler::check_heap_consistency();
#endif

}

//memory handler functions
void SiMemHandler::initialise_memory()
{
	select_chunk_size();
}

void SiMemHandler::out_of_memory()
{
	compact_memory();
	DisplayError(OUT_OF_MEMORY,OUT_OF_MEMORY_WARNING);
}

void SiMemHandler::exit_memory()
{
	SiMemChunk * curr_chunk=last_chunk_text;
	SiMemChunk * temp_chunk;

	for(Int16 i=0;i<2;++i)
	{
		while(curr_chunk!=NULL)
		{
			temp_chunk=curr_chunk->prev_chunk;

			delete_chunk(curr_chunk);
			curr_chunk=temp_chunk;
		}
		curr_chunk=last_chunk_non_text;
	}
	head_chunk_text=last_chunk_text=head_chunk_non_text=last_chunk_non_text=NULL;
#ifdef DEBUG

	ErrFatalDisplayIf(number_allocated_chunks!=0,"Unfreed chunks remain on exit");
	#ifdef DEBUG_LOG
	output_chunk_list();
	#endif

#endif

}

void SiMemHandler::select_chunk_size()
{
	UInt32 free_mem,max_chunk;
	MemHeapFreeBytes(0,&free_mem,&max_chunk);
	if(free_mem<SMALL_MEM_AMOUNT)
		MEMORY_CHUNK_SIZE=SMALL_MEM_CHUNK;
	else
		MEMORY_CHUNK_SIZE=LARGE_MEM_CHUNK;
}

SiMemChunk * SiMemHandler::get_chunk(SiMemChunk* parent,const BlockInt size,const Int16 allocation_type)
{

	if(size==0)
		return NULL;
#ifdef DEBUG_LOG

	log_entry_number("get chunk ",size);
#endif

	SiMemChunk * ret_chunk=find_free_chunk(parent,size,allocation_type);
	if(NULL==ret_chunk)
	{
		ret_chunk=create_chunk(size,allocation_type);
		if(ret_chunk==NULL)
		{
			compact_memory();
			ret_chunk=find_free_chunk(parent,size,allocation_type);
			if(ret_chunk==NULL)
				ret_chunk=create_chunk(size,allocation_type);

		}
	}
#ifdef DEBUG
	if(ret_chunk!=NULL)
		ErrFatalDisplayIf(ret_chunk->size!=size,"Incorrect chunk size on get_chunk");
#endif
	// if(ret_chunk!=NULL)
	//   allocate_chunk(ret_chunk);
	if(ret_chunk==NULL&&size>0)
		out_of_memory();
	return ret_chunk;

}

SiMemChunk * 	SiMemHandler::get_chunk_quick(const BlockInt size)
{
	SiMemChunk * ret_chunk=create_chunk(size,TEXT_HEAP);
	if(NULL==ret_chunk)
		return get_chunk(NULL,size,TEXT_HEAP);
	else
		return ret_chunk;

}

SiMemChunk * SiMemHandler::resize_chunk(SiMemChunk * chunk,const BlockInt new_size,const UInt8 direction)
{
#ifdef DEBUG_LOG
	log_entry_number("resize chunk ",new_size);
#endif
  #ifdef DEBUG

	ErrFatalDisplayIf((chunk->status&FREE),"Resizing free chunk");
#endif

	if(0==new_size)
	{
		free_chunk(chunk);
		chunk=NULL;
	}
	else if(new_size>chunk->size)
	{
		//try to resize into free space
		if(!chunk->expand(new_size-chunk->size,direction))
		{
			chunk=reallocate_chunk(chunk,new_size,direction);
		}
	}
	else if(new_size<chunk->size)
	{
		chunk->shrink(chunk->size-new_size,direction);
	}
#ifdef DEBUG
	if(chunk!=NULL)
		ErrFatalDisplayIf(chunk->size!=new_size,"Incorrect chunk resize");
#endif

	if(chunk==NULL&&new_size>0)
		out_of_memory();
	return chunk;
}

void SiMemHandler::free_chunk(SiMemChunk * chunk)
{
	if(NULL==chunk)
		return;
#ifdef DEBUG_LOG

	log_entry_number("free chunk ",chunk->size);
#endif


	chunk->status|=FREE;

	SiMemChunk * temp_chunk=chunk->next_chunk;

	while(temp_chunk!=NULL)
	{

		if(chunk->next_chunk_is(temp_chunk)&&(temp_chunk->status&FREE))
		{
			merge_chunks(chunk,temp_chunk);
			temp_chunk=chunk->next_chunk;

		}
		else
			break;

	}

	if(!(chunk->status&MASTER))
	{

		while(chunk->prev_chunk!=NULL)
		{
			if(chunk->prev_chunk->status&FREE)
			{
				temp_chunk=chunk->prev_chunk;
				merge_chunks(temp_chunk,chunk);
				chunk=temp_chunk;
				if(chunk->status&MASTER)
					break;

			}
			else
				break;
		}


	}

	if(chunk->status&MASTER)
	{
		if(chunk->next_chunk==NULL)
			delete_chunk(chunk);
		else if(chunk->next_chunk->status&MASTER)
			delete_chunk(chunk);
	}
#ifdef DEBUG
	check_heap_consistency();
#endif




}
SiMemChunk * SiMemHandler::check_for_zero_size(SiMemChunk * chunk)
{


	if(0==chunk->size)
	{

		Boolean done=false;

		if(chunk->status&MASTER)
		{

			if(chunk->next_chunk!=NULL)
			{
				if(!(chunk->next_chunk->status&MASTER)&&(chunk->next_chunk->mem_ptr==chunk->mem_ptr))
				{
					chunk->next_chunk->status|=MASTER;
					chunk->status&=~MASTER;


					done=true;

				}
			}
			if(!done)
			{
				if(chunk->prev_chunk!=NULL)
				{

					if(chunk->prev_chunk->mem_ptr==chunk->mem_ptr)
					{
						chunk->prev_chunk->status|=MASTER;
						chunk->status&=~MASTER;
						done=true;
					}
				}

			}

#ifdef DEBUG
			ErrFatalDisplayIf(!done,"Unable to pass on MASTER chunk status");
#endif

		}
		delete_chunk(chunk);
		return NULL;

	}
	else
		return chunk;

}

SiMemChunk * SiMemHandler::move_data(SiMemChunk * src_chunk,SiMemChunk * dst_chunk,const BlockInt src_offset,const UInt8 direction)
{

#ifdef DEBUG_LOG
	if(direction==FROM_START_TO_END)
		log_entry_number("move data FROM_START_TO_END ",src_offset);
	else
		log_entry_number("move data FROM_END_TO_START ",src_offset);
#endif
#ifdef DEBUG

	BlockInt original_size=src_chunk->size;
	BlockInt orig_dst_size=0;
	BlockInt orig_src_size=src_chunk->size;

	if(dst_chunk!=NULL)
	{
		orig_dst_size=dst_chunk->size;

		original_size+=dst_chunk->size;
	}

#endif
	if(dst_chunk==NULL)
	{
#ifdef DEBUG_LOG
		log_entry("move data NULL dst");

#endif

		if(direction==FROM_START_TO_END)
			dst_chunk=src_chunk->split(src_offset,TO_END);
		else
			dst_chunk=src_chunk->split(src_chunk->size-src_offset,TO_START);
	}

	else if(FROM_START_TO_END==direction)
	{
		if(dst_chunk->next_chunk_is(src_chunk))
		{
			dst_chunk->modify_size(src_offset,TO_END,1);
			src_chunk->modify_size(src_offset,TO_START,-1);
			src_chunk=check_for_zero_size(src_chunk);

		}
		else
		{
			Int16 orig_size=dst_chunk->size;

			dst_chunk=resize_chunk(dst_chunk,dst_chunk->size+src_offset,TO_START);
			if(dst_chunk!=NULL)
			{

				MemMove(dst_chunk->mem_ptr+orig_size,src_chunk->mem_ptr,src_offset);
			}

			src_chunk=resize_chunk(src_chunk,src_chunk->size-src_offset,TO_END);

		}

	}
	else
	{
		if(src_chunk->next_chunk_is(dst_chunk))
		{
			dst_chunk->modify_size((src_chunk->size-src_offset),TO_START,1);
			src_chunk->modify_size((src_chunk->size-src_offset),TO_END,-1);
			src_chunk=check_for_zero_size(src_chunk);
		}
		else
		{
			dst_chunk=resize_chunk(dst_chunk,dst_chunk->size+(src_chunk->size-src_offset),TO_END);
			if(dst_chunk!=NULL)
			{
				MemMove(dst_chunk->mem_ptr,src_chunk->mem_ptr+src_offset,src_chunk->size-src_offset);
			}

			src_chunk=resize_chunk(src_chunk,src_offset,TO_START);
		}

	}
#ifdef DEBUG
	if(src_chunk!=NULL&&dst_chunk!=NULL)
	{
		ErrFatalDisplayIf(dst_chunk->size+src_chunk->size!=original_size,"Incorrect sizes on move data");
		ErrFatalDisplayIf(dst_chunk->status&FREE,"Destination chunk freed on move_data");
		ErrFatalDisplayIf(src_chunk->status&FREE,"Source chunk freed on move_data");
	}

	if(direction==FROM_END_TO_START)
	{
		if(src_chunk!=NULL)
			ErrFatalDisplayIf((src_chunk->size!=src_offset),"Incorrect src chunk size on move data");
		if(dst_chunk!=NULL)
			ErrFatalDisplayIf(dst_chunk->size!=(orig_dst_size+(orig_src_size-src_offset)),"Incorrect dst chunk size on move data");
	}
	else
	{
		if(dst_chunk!=NULL)
			ErrFatalDisplayIf(dst_chunk->size!=(orig_dst_size+(src_offset)),"Incorrect dst chunk size on move data");
		if(src_chunk!=NULL)
			ErrFatalDisplayIf((src_chunk->size!=orig_src_size-src_offset),"Incorrect dst chunk size on move data");
	}

#endif

#ifdef DEBUG_LOG
	log_entry("move_data done");
#endif

	return dst_chunk;

}

SiMemChunk* SiMemHandler::reallocate_chunk(SiMemChunk * chunk,const BlockInt new_size,const UInt8 direction)
{
#ifdef DEBUG_LOG
	log_entry_number("reallocate chunk ",new_size);
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(chunk->status&FREE,"Reallocating free chunk");
#endif
#ifdef BENCH_OBJECTS

	add_reallocation(chunk->size);
#endif

	SiMemChunk * new_chunk=get_chunk(chunk,new_size,TEXT_HEAP);
	if(new_chunk!=NULL)
	{
		//allocate_chunk(new_chunk);

		if(direction==TO_START)
		{
			MemMove(new_chunk->mem_ptr,chunk->mem_ptr,chunk->size);
		}
		else
		{
			MemMove(new_chunk->mem_ptr+(new_size-chunk->size),chunk->mem_ptr,chunk->size);
		}
		free_chunk(chunk);
	}
#ifdef DEBUG
	if(new_chunk!=NULL)
		ErrFatalDisplayIf(new_chunk->size!=new_size,"Incorrect reallocate of chunk");
	check_heap_consistency();

#endif

	return new_chunk;
}

SiMemChunk * 	SiMemHandler::create_chunk(const BlockInt size,const Int16 allocation_type)
{
#ifdef DEBUG_LOG
	log_entry_number("create chunk ",size);
#endif

	UInt32 free_mem,max_chunk;
	MemHeapFreeBytes(0,&free_mem,&max_chunk);
	if(max_chunk<(UInt32)(MEMORY_CHUNK_SIZE*2)||max_chunk<(UInt32)(size*2)||free_mem<(UInt32)(size*2)||free_mem<(UInt32)(MEMORY_CHUNK_SIZE*2))
		return NULL;

	SiMemChunk * new_chunk=new SiMemChunk();
	if(new_chunk!=NULL)
	{
		BlockInt rounded_size=MEMORY_CHUNK_SIZE;

		new_chunk->status=MASTER;

		if(size>rounded_size)
		{
			rounded_size=size;
		}

		new_chunk->mem_ptr=(Char*)MemPtrNew(rounded_size);
		if(new_chunk->mem_ptr==NULL)
		{
			out_of_memory();

			delete(new_chunk);
			return NULL;
		}

		new_chunk->size=rounded_size;


		if(allocation_type==TEXT_HEAP)
		{

			if(head_chunk_text==NULL)
				head_chunk_text=new_chunk;
			if(last_chunk_text==NULL)
				last_chunk_text=new_chunk;
			else
				last_chunk_text->insert_after(new_chunk);

		}
		else
		{
			if(head_chunk_non_text==NULL)
				head_chunk_non_text=new_chunk;
			if(last_chunk_non_text==NULL)
				last_chunk_non_text=new_chunk;
			else
				last_chunk_non_text->insert_after(new_chunk);
		}
		if(size<rounded_size)
			resize_chunk(new_chunk,size,TO_START);
#ifdef DEBUG

		ErrFatalDisplayIf(!(new_chunk->status&MASTER),"Chunk un-mastered by resizing");
#endif

	}
#ifdef DEBUG
	if(new_chunk!=NULL)
		ErrFatalDisplayIf(new_chunk->size!=size,"Incorrect size chunk on create");
#endif

	return new_chunk;

}

SiMemChunk * 	SiMemHandler::find_free_chunk(SiMemChunk * start_chunk,const BlockInt min_size,const Int16 allocation_type)
{
#ifdef DEBUG_LOG
	log_entry_number("find_free chunk ",min_size);
#endif
	//traverse the chunk list and find a free chunk that
	//is at least min_size big
	//start using the parent chunk passed in to avoid having to traverse whole list most of the time
	SiMemChunk * curr_chunk=NULL;
	if(start_chunk!=NULL)
	{

		if(start_chunk->next_chunk!=NULL)
			if((start_chunk->next_chunk->status&FREE)&&start_chunk->next_chunk->size>=min_size)
				curr_chunk=start_chunk->next_chunk;

		if(curr_chunk==NULL)
			if(start_chunk->prev_chunk!=NULL)
				if((start_chunk->prev_chunk->status&FREE)&&start_chunk->prev_chunk->size>=min_size)
					curr_chunk=start_chunk->prev_chunk;
	}

	if(curr_chunk==NULL)
	{
		if(allocation_type==TEXT_HEAP)
			curr_chunk=head_chunk_text;
		else
			curr_chunk=head_chunk_non_text;
		while(curr_chunk!=NULL)
		{
			if(curr_chunk->size>=min_size&&(curr_chunk->status&FREE))
				break;
			else
				curr_chunk=curr_chunk->next_chunk;

		}
	}

	if(curr_chunk!=NULL)
	{
#ifdef DEBUG
		ErrFatalDisplayIf(!(curr_chunk->status&FREE),"Non free chunk returned by find_free_chunk");
#endif

		allocate_chunk(curr_chunk);
		curr_chunk=resize_chunk(curr_chunk,min_size,TO_START);
	}
#ifdef DEBUG
	if(curr_chunk!=NULL)
	{

		ErrFatalDisplayIf(curr_chunk->size!=min_size,"Incorrect chunk size returned by find_free_chunk");
	}
#endif
	return curr_chunk;

}

inline void SiMemHandler::allocate_chunk(SiMemChunk* chunk)
{
#ifdef DEBUG_LOG
	log_entry_number("allocate chunk ",chunk->size);
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(!(chunk->status&FREE),"Double allocating chunk");
#endif

	chunk->status&=~FREE;

}

void SiMemHandler::merge_chunks(SiMemChunk * first_chunk,SiMemChunk * second_chunk)
{
#ifdef DEBUG
	ErrFatalDisplayIf(first_chunk==NULL||second_chunk==NULL,"Merging NULL chunks");

	ErrFatalDisplayIf(!first_chunk->next_chunk_is(second_chunk),"Merging non-contiguous chunks");
	ErrFatalDisplayIf((!(first_chunk->status&FREE))||(!(second_chunk->status&FREE)),"Merging non-free chunks");
	ErrFatalDisplayIf(second_chunk->status&MASTER,"Merging second master chunk");
	ErrFatalDisplayIf(first_chunk->size==0,"merging zero size chunk");
	BlockInt orig_size=first_chunk->size+second_chunk->size;

#endif
#ifdef DEBUG_LOG

	log_entry_number("merge chunks ",first_chunk->size);
#endif

	first_chunk->size+=second_chunk->size;

	delete_chunk(second_chunk);
#ifdef DEBUG

	ErrFatalDisplayIf(first_chunk->size!=orig_size,"Incorrect chunk merge");

#endif
}

void SiMemHandler::delete_chunk(SiMemChunk * chunk)
{
#ifdef DEBUG_LOG
	log_entry_number("delete chunk ",chunk->size);
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(chunk==NULL,"Deleting NULL chunk");
#endif

	if(chunk==head_chunk_text)
		head_chunk_text=head_chunk_text->next_chunk;
	else if(chunk==head_chunk_non_text)
		head_chunk_non_text=head_chunk_non_text->next_chunk;
	if(chunk==last_chunk_text)
		last_chunk_text=last_chunk_text->prev_chunk;
	else if(chunk==last_chunk_non_text)
		last_chunk_non_text=last_chunk_non_text->prev_chunk;

	delete chunk;

}

void SiMemHandler::compact_memory()
{
#ifdef DEBUG_LOG
	log_entry_number("compact memory ",number_allocated_chunks);
#endif
}

#ifdef DEBUG_LOG
void SiMemHandler::output_chunk_list()
{
	SiMemChunk * curr_chunk=head_chunk_text;
	#ifdef DEBUG_LOG
	log_entry("TEXT HEAP");
	#endif

	for(Int16 i=0;i<2;++i)
	{

		while(curr_chunk!=NULL)
		{

			if(curr_chunk->status&MASTER)
				log_entry("Master Chunk");
			if(curr_chunk->status&FREE)
				log_entry_number("Free space ",curr_chunk->size);
			else
				log_entry_number("Allocated space ",curr_chunk->size);

			log_entry_number("Address ",(UInt32)curr_chunk->mem_ptr);

			curr_chunk=curr_chunk->next_chunk;
		}
		if(i==0)
			log_entry("NON TEXT HEAP");

		curr_chunk=head_chunk_non_text;
	}

}
#endif
