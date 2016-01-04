
#include "sidocument.h"
#include "error.h"
#include "textview.h"
#include "utility.h"
#include "preferences.h"
#define FILE_OPEN_MAX_LINES 1000
#define SLOSH_LINES_CHUNK 30
#define TIDY_LINE_BUFFER 100
#define LINE_ESTIMATE_ACCURACY 100
#define SLOSH_LINES_WAIT_DISPLAY_LIMIT 300
#define LINE_WIDTH_ESTIMATE_LIMIT 800
#define REDRAW_CHROME_INTERVAL 100
#define LARGE_CHANGE_THRESHOLD 20
#define MAX_NUMBER_BLOCKS 32000

SiDocument::SiDocument()
{
	done_insert=false;
	m_opening_file=false;
	text_change_handle=NULL;
	number_changes=0;
	text_change_list=NULL;
	document_length=max_char = 0;
	number_blocks=0;
	block_buffer_array_handle=NULL;
	number_block_buffers=0;
#ifdef USE_CHAR_CACHE

	cached_get_char_block_index=0;
	cached_bound_offset=0;
	cached_bound_byte_offset=0;
#endif

	allow_change_add=true;
	dirty_flag=false;
	m_background_tidy=false;

	apparent_number_blocks=0;
	n_lines_estimated=1;
	line_width_estimate=25;
	last_line_tidied=0;
	m_memory_error_occurred=false;
#ifdef USE_LINE_CACHE

	cached_get_line_index=0;
	cached_line_block_buffer_index=0;
	cached_line_start_index=0;
#endif
}
void SiDocument::finished_initial_opening()
{
#ifdef TEST_OBJECT_LOG
	log_entry_number("line_width_estimate=",line_width_estimate);
#endif


	Int32 w=(LINE_ESTIMATE_ACCURACY/2+line_width_estimate)/LINE_ESTIMATE_ACCURACY;
	if(w==0)
		w=1;
	Int16 n_lines_estimate=get_doc_length()/w;
	if(last_line_tidied>=get_number_blocks()-1)
		set_apparent_number_blocks(get_number_blocks());
	else
		set_apparent_number_blocks(n_lines_estimate);
}
void SiDocument::ready_for_change()
{
	done_insert=false;
}
Boolean SiDocument::has_done_insert() const
{
	return done_insert;
}
SiDocument::~SiDocument()
{
	free_text();
}

Char * SiDocument::get_char_bound(const Int16 index,Char* data,const BlockInt  char_offset,BlockInt  *offset)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDoc::get_char_bound");
#endif

	if(char_offset==0)
	{
		(*offset)=0;
		return data;
	}
	BlockInt  i;
#ifdef USE_CHAR_CACHE

	if(index==cached_get_char_block_index)
	{
		//block is cached
		if(char_offset>=cached_bound_offset)
		{
			//use cached offset data
			(*offset)=cached_bound_byte_offset;
			i=cached_bound_offset;
#ifdef COLLECT_STATISTICS

			SiUtility::add_char_cache_hit(cached_bound_offset);
#endif

		}
		else
		{
			//cache is too late in block, don't use it
			(*offset)=0;
			i=0;
		}
	}
	else
#endif

	{
		(*offset)=0;
		i=0;
	}

#ifdef USE_CHAR_CACHE
    #ifdef COLLECT_STATISTICS
	if(i==0)
		SiUtility::add_char_cache_miss();
#endif

	cached_get_char_block_index=index;
#endif
	//Initialisation already done above
	//Scan through array and find correct char

	for(;i<char_offset;++i)
	{
		(*offset)+=TxtNextCharSize(data,(*offset));
	}

	//set cache for next time
	Char* ret=data+(*offset);
#ifdef USE_CHAR_CACHE

	cached_bound_offset=char_offset;
	cached_bound_byte_offset=(*offset);
#endif

	return ret;
}

void SiDocument::free_text()
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDoc::free_text");
#endif

	if(block_buffer_array_handle!=NULL)
	{
		for(Int16 i=0;i<number_block_buffers;++i)
		{
			block_buffer_array[i].exit_block_buffer();
		}
		MemHandleUnlock(block_buffer_array_handle);
		MemHandleFree(block_buffer_array_handle);
		block_buffer_array_handle=NULL;
		block_buffer_array=NULL;
	}


	SiMemHandler::compact_memory();

	invalidate_cache();
	m_memory_error_occurred=false;
	//make sure all text data is released
	document_length=max_char = 0;
	apparent_number_blocks=number_blocks=0;
	last_line_tidied=0;
	number_block_buffers=0;
	dirty_flag=false;
}

Boolean SiDocument::file_dirty() const
{
	return dirty_flag;
}

void SiDocument::move_to_prev_block(const Int16 line, block * src_block,block * dst_block,const BlockInt byte_offset,const BlockInt char_offset)
{
	SiMemChunk * temp_chunk=SiMemHandler::move_data(src_block->mem_chunk,dst_block->mem_chunk,byte_offset,FROM_START_TO_END);
	if(temp_chunk!=NULL)
		dst_block->mem_chunk=temp_chunk;
	else
	{
		m_memory_error_occurred=true;
		return;
	}
	dst_block->n_chars+=char_offset;
	src_block->n_chars-=char_offset;
	//#ifdef USE_CHAR_CACHE
	//check_cache(line,line-1);
	//#endif
	if(src_block->n_chars==0)
	{
		//      delete_chunk(src_block->mem_chunk);

		src_block->mem_chunk=NULL;
	}
	else
	{
		m_text_view->notify_text_deleted(line,0);
	}
#ifdef DEBUG
	if(src_block!=NULL)
	{
		if(src_block->mem_chunk!=NULL)
			ErrFatalDisplayIf(src_block->mem_chunk->size<src_block->n_chars,"Incorrect char count src_block, move_to_prev_block");
		else
			ErrFatalDisplayIf(src_block->n_chars!=0,"Non-zero src null chunk, move_to_prev_chunk");
	}
	if(dst_block!=NULL)
	{
		if(dst_block->mem_chunk!=NULL)
			ErrFatalDisplayIf(dst_block->mem_chunk->size<dst_block->n_chars,"Incorrect char count dst_block, move_to_prev_block");
		else
			ErrFatalDisplayIf(dst_block->n_chars!=0,"Non-zero null dst chunk, move_to_prev_chunk");
	}
#endif


}

void SiDocument::check_eff_insert_line(Int16 start_line,BlockInt char_offset)
{
	//if the change being made is a large one or involves the addition of a line feed
	//insert a new block in to take the new text
	Int16 b_b_index;
	if(start_line>=get_number_blocks())
		return;
	block * curr_block=get_line(start_line,b_b_index);
	if(curr_block->n_chars>LARGE_CHANGE_THRESHOLD+char_offset)
	{
		done_insert=true;
		release_line(b_b_index);
		insert_block(start_line);
	}
	else if(curr_block->mem_chunk!=NULL&&curr_block->n_chars>0)
	{
		WChar the_char;
		TxtGetPreviousChar(curr_block->mem_chunk->mem_ptr,curr_block->mem_chunk->size,&the_char);
		if(the_char==chrLineFeed)
		{
			done_insert=true;
			release_line(b_b_index);
			insert_block(start_line);
		}
		else
			release_line(b_b_index);
	}
	else
		release_line(b_b_index);

}

Boolean SiDocument::move_to_next_block(const BlockInt char_offset,const BlockInt byte_offset,const Int16 start_line)
{
	//move the text starting at byte_offset from the block start_line
	//to start_line+1. Creates start_line+1 if it doesn't already exist
#ifdef DEBUG
	ErrFatalDisplayIf(byte_offset==0,"Moving entire block to next");
#endif

	Boolean ret;
	SiMemChunk * temp_chunk;
	Int16 b_b_index_start,b_b_index_next;

	ensure_allocated(start_line+1);
	if(!m_opening_file)
		check_eff_insert_line(start_line,char_offset);
	else
	{
		//always insert an extra block if this is during a file open event
		//as it improves file access speed about 5%
		insert_block(start_line);
	}
	block * b_start=get_line(start_line,b_b_index_start);

	if(b_start->mem_chunk==NULL)
	{
		release_line(b_b_index_start);
		return false;
	}
	block * b_next=get_line(start_line+1,b_b_index_next);
	UInt16 correct_size=b_start->mem_chunk->size-byte_offset;
	if(b_next->mem_chunk!=NULL)
		correct_size+=b_next->mem_chunk->size;
	if(byte_offset<b_start->mem_chunk->size)
	{
		temp_chunk=SiMemHandler::move_data(b_start->mem_chunk,b_next->mem_chunk,byte_offset,FROM_END_TO_START);
		if(temp_chunk!=NULL)
		{
			if(temp_chunk->size!=correct_size)
			{
				m_memory_error_occurred=true;
				release_line(b_b_index_next);
				release_line(b_b_index_start);
				return false;
			}
			else
				b_next->mem_chunk=temp_chunk;
		}
		else
		{
			m_memory_error_occurred=true;
			release_line(b_b_index_next);
			release_line(b_b_index_start);
			return false;
		}

		b_next->n_chars+=b_start->n_chars-char_offset;
		b_start->n_chars=char_offset;
		if(b_start->n_chars==0)
		{
			b_start->mem_chunk=NULL;
		}
		ret=true;
	}
	else
		ret=false;
#ifdef DEBUG

	if(b_start->mem_chunk!=NULL)
		ErrFatalDisplayIf(b_start->mem_chunk->size<b_start->n_chars,"Incorrect char count src_block, move_to_next_block");
	if(b_next->mem_chunk!=NULL)
		ErrFatalDisplayIf(b_next->mem_chunk->size<b_next->n_chars,"Incorrect char count dst_block, move_to_next_block");
#endif

	BlockInt n_chars=b_start->n_chars;
	release_line(b_b_index_next);
	release_line(b_b_index_start);
	// #ifdef USE_CHAR_CACHE
	//check_cache(start_line,start_line+1);
	//#endif

	if(n_chars==0)
	{
		remove_block(start_line);
	}
	else if(!m_opening_file)
	{
		Position start(0,start_line+1);
		Position end(n_chars,start_line+1);
		m_text_view->notify_text_deleted(start_line,char_offset);
		m_text_view->notify_text_changed(start,end);
	}
#ifdef DEBUG_LOG
	log_entry("move to next block done");

#endif

	return ret;
}

void SiDocument::move_up_data(block * b,const Position pos,const BlockInt bytes)
{
	if(pos.x>=b->n_chars)
		return;

	BlockInt offset;
	Char * src_data=get_char_bound(pos.line,b->mem_chunk->mem_ptr,pos.x,&offset);

	if(b->mem_chunk->size>(bytes+offset))
	{
		Char * dst_data=src_data+bytes;
#ifdef DEBUG

		ErrFatalDisplayIf(b->mem_chunk->size<(bytes+offset),"Moving up negative amount");
#endif

		MemMove(dst_data,src_data,b->mem_chunk->size-(bytes+offset));
	}

}

void SiDocument::move_down_data(block * b,const Position pos,const BlockInt bytes)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDocument::move down data");
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(pos.x>b->n_chars,"moving data down off end of block");
#endif

	BlockInt offset;
	Char * src_data=get_char_bound(pos.line,b->mem_chunk->mem_ptr,pos.x,&offset);

	if(b->mem_chunk->size>offset)
	{
		Char * dst_data=src_data-bytes;


		MemMove(dst_data,src_data,b->mem_chunk->size-offset);
	}
}
//!
//! Document saved so no longer dirty
//!
void SiDocument::clear_dirty_flag()
{
	dirty_flag = false;
}
void SiDocument::set_dirty_flag()
{
	dirty_flag = true;
}
UInt32 SiDocument::get_doc_length()
{
	return document_length;
}
UInt32 SiDocument::get_number_chars()
{
	return max_char;
}
block * SiDocument::get_line(const Int16 index,Int16 & b_b_index)
{
#ifdef TEST_OBJECT_LOG
	log_entry_number("SiDoc::get_line ",index);
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(index<0,"SiDocument: calling get_line with negative index");
#endif

	Int16 start_index;

	if(index>=number_blocks)
		return NULL;
	//check if the cache is valid and use it if so
#ifdef USE_LINE_CACHE

	if(index==cached_get_line_index)
	{
		b_b_index=cached_line_block_buffer_index;
		return block_buffer_array[cached_line_block_buffer_index].get_block(index-cached_line_start_index);
	}
#endif
	block_buffer * b_b=get_block_buffer(index,&start_index,b_b_index);

	if(b_b==NULL)
	{
		return NULL;
	}
	else
	{
#ifdef USE_LINE_CACHE
		cached_get_line_index=index;
		cached_line_block_buffer_index=b_b_index;
		cached_line_start_index=start_index;
#endif

		return b_b->get_block(index-start_index);
	}

}

//!
//! Unlock the block buffer b_b_index
//! called with the returned b_b_index from a previous get_line
void SiDocument::release_line(Int16 b_b_index)
{
#ifdef TEST_OBJECT_LOG
	log_entry_number("SiDoc::release_line ",b_b_index);
#endif

	if(b_b_index<number_block_buffers)
		block_buffer_array[b_b_index].release_blocks();
}
//!
//! Sum all the blocks in the block_buffers before the one passed in
//!
Int16 SiDocument::get_start_index(block_buffer * b_buffer)
{

	Int16 i=0;
	Int16 start_index=0;
	while(i<number_block_buffers&&b_buffer!=&(block_buffer_array[i]))
	{
		start_index+=block_buffer_array[i].n_blocks;
		++i;
	}
	return start_index;
}
//!
//! Get a pointer to the block buffer containing the required line
//!
block_buffer* SiDocument::get_block_buffer(const Int16 line,Int16 * start_index,Int16 & b_b_index)
{
	if(block_buffer_array_handle==NULL)
	{
		//If no block_buffers have been allocated, work out how many need to be
		//allocated and return NULL to signal the need to create them
		if(start_index!=NULL)
		{
			(*start_index)=1+(line)/INITIAL_BUFFER_SIZE;
		}
		return NULL;
	}

	Int16 curr_line=0;
	Int16 i=0;
	do
	{
		curr_line+=block_buffer_array[i].n_blocks;
		++i;
	}
	while((i<number_block_buffers)&&(curr_line<=line));


	if(curr_line>line)
	{
		//have found a block buffer that contains this block
		--i;
		if(start_index!=NULL)
		{
			(*start_index)=curr_line-block_buffer_array[i].n_blocks;
		}

		b_b_index=i;
	}
	else
	{
		//the line number is off the end of the current block_buffer list
		if(start_index!=NULL)
		{
			(*start_index)=1+(line-curr_line)/INITIAL_BUFFER_SIZE;
		}


		return NULL;
	}

	return &(block_buffer_array[i]);
}
#ifdef USE_CHAR_CACHE
/*void SiDocument::check_cache(const Int16 start_line,const Int16 end_line)
{
  if	((start_line<=cached_get_char_block_index
	  &&end_line>=cached_get_char_block_index)
	 ||cached_get_char_block_index>=number_blocks)
    invalidate_cache();
#ifdef USE_LINE_CACHE
  else if ((start_line<=cached_get_line_index
	    ||end_line<=cached_get_line_index)
	   ||cached_get_line_index>=number_blocks)
    invalidate_cache();
#endif
}*/
#endif
void SiDocument::made_change(const Position start,const Position end)
{
#ifdef DEBUG_LOG
	log_entry_number("made change start.line=",start.line);
	log_entry_number("made change end.line=",end.line);
	log_entry_number("made change end.x=",end.x);
#endif
	// #ifdef USE_CHAR_CHACHE
	//check_cache(start.line,end.line);
	//#endif
	dirty_flag=true;

	m_text_view->notify_text_changed(start,end);
	ready_for_change();
}

void SiDocument::invalidate_cache()
{
#ifdef USE_CHAR_CACHE
	cached_get_char_block_index=SIGNED_NONE;
	cached_bound_offset=0;
	cached_bound_byte_offset=0;
#endif
#ifdef USE_LINE_CACHE

	cached_line_block_buffer_index=cached_line_start_index=cached_get_line_index=0;
#endif
}

Boolean SiDocument::ensure_allocated(const Int16 line)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDoc::ensure_allocated");
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(line>32000,"Line number too great");
#endif

	Int16 b_b_index;

	if(line>=number_blocks)
	{
		Int16 start_index=0;
		block_buffer* b_buffer;

		if((b_buffer=get_block_buffer(line,&start_index,b_b_index))==NULL)
		{
			allocate_new_block_buffers(start_index);
			b_buffer=get_block_buffer(line,&start_index,b_b_index);
		}
		if(b_buffer==NULL)
			return false;
		if((1+line)>start_index+b_buffer->n_blocks)
		{
			if(!b_buffer->add_blocks(1+line-start_index-b_buffer->n_blocks,END_BUFFER))
				return false;
		}

		block * b;

		for(Int16 n=0;n<=line-start_index-b_buffer->n_lines;++n)
		{
			++number_blocks;
			//lock line get_number_blocks()
			b=get_line(number_blocks-1,b_b_index);

			b->init_block();

			//unlock line get_number_blocks()
			release_line(b_b_index);
		}

		b_buffer->n_lines+=(1+line-start_index-b_buffer->n_lines);
	}
	return true;
}

Boolean SiDocument::inserted_text(const BlockInt n_chars,const BlockInt n_bytes,const Int8 grow)
{
	if(grow>0)
	{
		max_char+=n_chars;
		document_length+=n_bytes;
	}
	else
	{
		max_char-=n_chars;
		document_length-=n_bytes;
	}
	dirty_flag=true;
	return true;
}
//!
//! Create some new block_buffers and add them to the linked list
//!
void SiDocument::allocate_new_block_buffers(const Int16 num)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDoc::allocate_new_block_buffers");
#endif

	Int16 start;
	if(num<=0)
		return;
	if(block_buffer_array_handle==NULL)
	{
		block_buffer_array_handle=MemHandleNew(num*sizeof(block_buffer));
		start=0;
	}
	else
	{
		MemHandleUnlock(block_buffer_array_handle);

		MemHandleResize(block_buffer_array_handle,MemHandleSize(block_buffer_array_handle)+num*sizeof(block_buffer));
		start=number_block_buffers;

	}
	number_block_buffers+=num;

	block_buffer_array=(block_buffer*)MemHandleLock(block_buffer_array_handle);
	for(Int16 i=start;i<number_block_buffers;++i)
	{
		block_buffer_array[i].init_block_buffer();
	}

}
void SiDocument::remove_block_buffer(block_buffer* b_buffer)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDoc::remove_block_buffer");
#endif

	Int16 del=0;

	b_buffer->exit_block_buffer();
	if(prev_locked_block_buffer==b_buffer)
	{
		prev_locked_start_index=SIGNED_NONE;
		prev_locked_block_buffer=NULL;
	}
	for(Int16 b=0;b<number_block_buffers;++b)
	{
		if(&(block_buffer_array[b])==b_buffer)
		{
			del=b;
			break;
		}
	}
	if((number_block_buffers-del)>0)
	{
#ifdef DEBUG
		ErrFatalDisplayIf(((number_block_buffers-del)*sizeof(block_buffer))>MemHandleSize(block_buffer_array_handle),"removing block buffer writing off end of buffer");
#endif

		MemMove(block_buffer_array+del,block_buffer_array+del+1,(number_block_buffers-del-1)*sizeof(block_buffer));
	}
	--number_block_buffers;
	if(number_block_buffers>0)
	{
		MemHandleUnlock(block_buffer_array_handle);
		MemHandleResize(block_buffer_array_handle,number_block_buffers*sizeof(block_buffer));
		block_buffer_array=(block_buffer*)MemHandleLock(block_buffer_array_handle);
	}
	else
	{
		block_buffer_array=NULL;
		block_buffer_array_handle=NULL;
	}

}

WChar SiDocument::get_char(Int16 line,block *b,const BlockInt  offset)
{
	BlockInt  byte_offset;
#ifdef DEBUG

	ErrFatalDisplayIf(offset>b->mem_chunk->size,"getting char off end");
#endif

	get_char_bound(line,b->mem_chunk->mem_ptr,offset,&byte_offset);
	WChar the_char=TxtGetChar(b->mem_chunk->mem_ptr,byte_offset);
	return the_char;
}

Position SiDocument::find_end_line(const Int16 line)
{
	//lock line line (const)
	if(number_blocks==0)
		return Position (0,0);
	Int16 b_b_index;
	const BlockInt  x=get_line(line,b_b_index)->n_chars;
	//unlock line line
	release_line(b_b_index);
	return Position(x,line);
}

Position SiDocument::find_end_document()
{
	Position ret(0,0);
	if(number_blocks>0)
	{
		ret.line=number_blocks-1;
		//lock line get_number_blocks()-1
		Int16 b_b_index;
		ret.x=get_line(number_blocks-1,b_b_index)->n_chars;

		//unlock line get_number_blocks()-1
		release_line(b_b_index);
	}
	return ret;
}

void SiDocument::check_newline_position(Position &p,const Int16 dir)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDoc::check newline position");
#endif

	if(p.line>=number_blocks)
		return;

	Int16 b_b_index;
	block * b=get_line(p.line,b_b_index);

	if(p.x>=b->n_chars-1&&b->n_chars>0)
	{
		if(get_char(p.line,b,b->n_chars-1)==chrLineFeed)
		{
			if(dir==0)
			{
				p.x=b->n_chars-1;
				if(b->n_chars>=2)
				{
					if(get_char(p.line,b,b->n_chars-2)==chrCarriageReturn)
						p.x=b->n_chars-2;
				}
			}
			else if(dir>=1)
			{
				BlockInt max_c=b->n_chars-1;
				if(b->n_chars>=2)
				{
					if(get_char(p.line,b,b->n_chars-2)==chrCarriageReturn)
					{
						max_c=b->n_chars-2;
					}
				}

				if(p.x>max_c)
				{
					++(p.line);
					p.x=0;
				}
			}
			else
			{
				p.x=b->n_chars-1;
				if(b->n_chars>=2)
				{
					if(get_char(p.line,b,b->n_chars-2)==chrCarriageReturn)
					{
						p.x=b->n_chars-2;
					}
				}
			}

		}
	}
	release_line(b_b_index);
}

Int16 SiDocument::get_number_blocks() const
{
	return number_blocks;
}
Boolean SiDocument::get_done_insert() const
{
	return done_insert;
}

void SiDocument::insert_block(const Int16 prev_line)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDoc::insert_block");
#endif

	Int16 start_index=0;
	Int16 b_b_index;

	block_buffer* b_b=get_block_buffer(prev_line,&start_index,b_b_index);

	//	made_change(prev_line);
	if(b_b==NULL)
	{
		allocate_new_block_buffers(1);
		b_b=get_block_buffer(prev_line,&start_index,b_b_index);
	}
	b_b->add_blocks(1,prev_line+1-start_index);

	++number_blocks;
	if(!m_background_tidy)
		++apparent_number_blocks;

	block * b=get_line(prev_line+1,b_b_index);

	b->n_chars=0;
	// #ifdef USE_CHAR_CACHE
	//check_cache(prev_line,prev_line+1);
	//#endif
	if(!m_opening_file)
		m_text_view->notify_block_inserted(prev_line+1);

	//unlock line
	release_line(b_b_index);

}
void SiDocument::remove_text_range(const Int16 line,const BlockInt start_x,BlockInt  end_x)
{
#ifdef TEST_OBJECT_LOG
	log_entry_number("SiDoc::remove text range line ",line);
#endif

	Int16 b_b_index;
	BlockInt start_offset,end_offset;
	block * b=get_line(line,b_b_index);
#ifdef DEBUG
	//	ErrFatalDisplayIf(start_x==0&&end_x==b->n_chars,"SiDoc:: uneccessary remove_text_range");
	//	ErrFatalDisplayIf(start_x>b->mem_chunk->size,"removing data off end of block");
	//	ErrFatalDisplayIf(start_x>b->mem_chunk->size,"removing data off end of block");
#endif

	Char * start_data=get_char_bound(line,b->mem_chunk->mem_ptr,start_x,&start_offset);
	Char * end_data=get_char_bound(line,b->mem_chunk->mem_ptr,end_x,&end_offset);
	//#ifdef USE_CHAR_CACHE
	//check_cache(line,line);
	//#endif

	if(end_x<b->n_chars&&start_x<b->n_chars&&b->mem_chunk->size>end_offset)
	{


		MemMove(start_data,end_data,b->mem_chunk->size-end_offset);
	}
	b->n_chars-=(end_x-start_x);

	//shrinking so chunk won't move
	b->mem_chunk=SiMemHandler::resize_chunk(b->mem_chunk,b->mem_chunk->size-(end_offset-start_offset),TO_START);
	if(b->n_chars==0)
		b->mem_chunk=NULL;

	if(start_x>end_x)
		inserted_text((start_x-end_x),(start_offset-end_offset),1);
	else
		inserted_text((end_x-start_x),(end_offset-start_offset),-1);

	m_text_view->notify_text_deleted(line,start_x);

	//unlock line line
	release_line(b_b_index);
}

void SiDocument::remove_block(const Int16 line)
{
	//remove the line from the document
	//and move next lines down

	Int16 b_b_index;
	Int16 start_index;
	//	made_change(line);
	block * b=get_line(line,b_b_index);

	if(b->mem_chunk!=NULL)
		inserted_text(b->n_chars,b->mem_chunk->size,-1);
	release_line(b_b_index);
	block_buffer * b_buffer=get_block_buffer(line,&start_index,b_b_index);
	//actually remove the block from the buffer
	b_buffer->remove_block(line-start_index);
	if(b_buffer->n_blocks<=0)
		remove_block_buffer(b_buffer);
	//#ifdef USE_CHAR_CACHE
	//check_cache(line,line);
	//#endif
	m_text_view->notify_block_deleted(line);

	--number_blocks;
	if(!m_background_tidy)
		--apparent_number_blocks;


	done_insert=true;

}

void SiDocument::set_view(SiTextView * view)
{
	m_text_view=view;
}

const Char * SiDocument::get_stripped_text(const Position start,const Position end, const BlockInt max_bytes)
{
	//copy the text between start and end to a buffer and return the buffer
	//stop if we hit a newline or the max length limit
	//used for the find/replace dialog selection and save as dialog filename guess
	if(max_bytes==0)
		return NULL;

	Int16 b_b_index;
	Int16 curr_line=start.line;
	BlockInt curr_char=start.x;
	BlockInt last_char;
	BlockInt char_count=0;

	//the buffer to use
	Char * ret=(Char*)MemPtrNew(max_bytes+1);
	Boolean stop=false;
	BlockInt byte_offset;

	block * b=get_line(curr_line,b_b_index);
	get_char_bound(start.line,b->mem_chunk->mem_ptr,start.x,&byte_offset);
	release_line(b_b_index);

	BlockInt  init_byte_offset=byte_offset;
	Int16 c_width;
	WChar the_char;
	BlockInt n_bytes=0;
	BlockInt byte_count=0;

	while(curr_line<=end.line&&!stop)
	{

		b=get_line(curr_line,b_b_index);
		if(curr_line==end.line)
			last_char=end.x;
		else
			last_char=b->n_chars;

		while(byte_count<max_bytes&&curr_char<last_char)
		{
			c_width=TxtGetNextChar(b->mem_chunk->mem_ptr,byte_offset,&the_char);
			if(the_char==chrLineFeed)
			{
				stop=true;
				break;
			}
			++char_count;
			++curr_char;
			if(byte_count+c_width<max_bytes)
			{
				byte_offset+=c_width;
				byte_count+=c_width;
			}
			else
				break;

		}

		if(byte_offset>init_byte_offset)
		{
#ifdef DEBUG
			ErrFatalDisplayIf(n_bytes+(byte_offset-init_byte_offset)>max_bytes+1,"Writing off end buffer");
#endif

			MemMove(ret+n_bytes,b->mem_chunk->mem_ptr+init_byte_offset,byte_offset-init_byte_offset);
			n_bytes+=(byte_offset-init_byte_offset);
		}
		release_line(b_b_index);
		++curr_line;
		if(!stop&&curr_line<=end.line&&n_bytes<max_bytes)
			curr_char=init_byte_offset=byte_offset=curr_char=0;
	}


	if(n_bytes>0)
	{
		ret[n_bytes]='\0';
		MemPtrResize(ret,n_bytes+1);
	}
	else
	{
		MemPtrFree(ret);
		ret=NULL;
	}
#ifdef DEBUG
	ErrFatalDisplayIf(n_bytes!=byte_count,"Incorrect byte count in get text");
	//  DisplayError(DEBUG_MESSAGE,ret);
#endif

	return ret;
}

const Char * SiDocument::get_initial_filename()
{
	//return a copy of the first line of text in the document, minus any linefeeds and
	//truncated to max file name length
	if(number_blocks==0)
		return NULL;
	else
	{
		Int16 b_b_index;
		block * b=get_line(0,b_b_index);
		Position start(0,0);

		Position end(b->n_chars,0);
		release_line(b_b_index);
		const Char * ret= get_stripped_text(start,end,MAX_FILENAME_LENGTH);

		return ret;
	}
}

void SiDocument::set_opening_file_mode()
{
	m_opening_file=true;
}

void SiDocument::clear_opening_file_mode()
{
	m_opening_file=false;

}

Int16 SiDocument::get_apparent_number_blocks() const
{
	if(number_blocks<apparent_number_blocks)
		return apparent_number_blocks;
	else
		return number_blocks;
}

void SiDocument::set_apparent_number_blocks(const Int16 num)
{
#ifdef TEST_OBJECT_LOG
	log_entry_number("Apparent number blocks=",num);
#endif

	if(num!=apparent_number_blocks)
	{
		if(num<number_blocks)
			apparent_number_blocks=number_blocks;
		else
			apparent_number_blocks=num;
		m_text_view->draw_status_line_number();
	}


}

//!
//! Move the position passed in the required number of characters
//! +ve = right, -ve = left
void SiDocument::move_position(Position & pos,Int16 amount,Boolean check_newline,Boolean is_cursor)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDocument::move position");
#endif

	Int16 moved=0;
	block * b;
	Int16 b_b_index;
	if(get_number_blocks()==0)
	{
		pos.x=0;
		pos.line=0;
		return;
	}

	if(pos.line>=get_number_blocks()&&amount==-1)
	{
		pos=find_end_document();
		/*      pos.line=get_number_blocks()-1;
		b=get_line(pos.line,b_b_index);
		pos.x=b->n_chars;
		release_line(b_b_index);*/
		return;
	}
	if(pos.line<get_number_blocks())
	{
		b=get_line(pos.line,b_b_index);
		if(pos.x>b->n_chars)
		{
		  if(pos.line<get_number_blocks()-1)
		    {
		      pos.x-=b->n_chars;
		      ++pos.line;
		    }
		  else
		    {
		      pos.x=0;
		    }
		}
		release_line(b_b_index);
	}
	if(amount>0)
	{
		while(moved<amount)
		{
#ifdef DEBUG
			ErrFatalDisplayIf(pos.line<0,"Negative pos.line in move_position +ve");
			ErrFatalDisplayIf(pos.line>=get_number_blocks(),"Too large pos.line in move_position +ve");
#endif

			b=get_line(pos.line,b_b_index);
#ifdef DEBUG

			ErrFatalDisplayIf(b==NULL,"NULL block in move_position +ve 1");
#endif

			pos.x+=(amount-moved);
			moved=amount;
			if(pos.x>b->n_chars)
			{
				++(pos.line);
				if(pos.line>=get_number_blocks())
				{
					pos.line=get_number_blocks()-1;
					release_line(b_b_index);
					b=get_line(pos.line,b_b_index);
#ifdef DEBUG

					ErrFatalDisplayIf(b==NULL,"NULL block in move_position +ve 2");
#endif

					pos.x=b->n_chars;
					moved=amount;

				}
				else
				{
					moved-=(pos.x-b->n_chars);
					if(is_cursor)
						++moved;
					pos.x=0;

				}
			}
			if(abs(moved)>abs(amount))
			{
				pos.x-=(moved-amount);
				moved=amount;
			}

			release_line(b_b_index);
		}
		if(check_newline)
			check_newline_position(pos,amount);
	}
	else
	{
		amount=-amount;
		while(moved<amount)
		{
#ifdef DEBUG
			ErrFatalDisplayIf(pos.line<0||pos.line>=get_number_blocks(),"Invalid pos, move position");
#endif

			b=get_line(pos.line,b_b_index);
#ifdef DEBUG

			ErrFatalDisplayIf(b==NULL,"NULL block in move_position -ve 1");
#endif

			if((UInt16)(amount-moved)>pos.x)
			{
				if(pos.line==0)
				{

					pos.x=0;
					moved=amount;
				}
				else
				{
					--(pos.line);
					moved+=pos.x;
					if(is_cursor)
						++moved;
					release_line(b_b_index);
					b=get_line(pos.line,b_b_index);
#ifdef DEBUG

					ErrFatalDisplayIf(b==NULL,"NULL block in move_position -ve 2");
#endif

					pos.x=b->n_chars;

				}
			}
			else
			{
				pos.x-=(amount-moved);
				moved=amount;
			}

			release_line(b_b_index);
		}
		if(check_newline)
			check_newline_position(pos,-amount);
	}


	//Check cursor isn't after a newline on a line
	//or similar situation
#ifdef DEBUG
	ErrFatalDisplayIf(pos.line<0,"Negative line on move pos");
#endif

}

Boolean SiDocument::ends_in_line_feed(const Int16 line)
{
	if(line<0||line>=get_number_blocks())
		return false;
	else
	{
		Int16 b_b_index;
		block * b=get_line(line,b_b_index);
		Boolean ends=false;
		if(b->n_chars>0)
		{
			if(get_char(line,b,b->n_chars-1)==chrLineFeed)
			{
				ends=true;
			}

		}
		release_line(b_b_index);
		return ends;
	}
}

void SiDocument::check_final_block_allocated()
{
#ifdef TEST_OBJECT_LOG
	log_entry("sitext:check_final_block_allocated");
#endif
	//the last block in the document should be left in even if it's
	//blank if the penultimate line ends with a line feed

	if(get_number_blocks()>0)
	{
		if(ends_in_line_feed(get_number_blocks()-1))
		{

			ensure_allocated(get_number_blocks());

		}

	}

}
void SiDocument::correct_position(Position & p)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiDoc::correct_position");
#endif

	if(p.line>=get_number_blocks())
		return;
	Int16 b_b_index;

	block * b=get_line(p.line,b_b_index);

	Int16 amount=p.x-b->n_chars;

	if(amount>0)
	{
		p.x=b->n_chars;
		move_position(p,amount,true,false);
	}
	release_line(b_b_index);
}
Boolean SiDocument::wrap_back(const Position  start_pos,Position & first_change_pos,Position & cursor_pos,Boolean & increase_redraw)
{
	//Check if the changes made to the text mean that a word should be wrapped
	//back to the previous line

	Boolean wrapped=false;

	if(start_pos.line>0)
	{
		Int16 b_b_index;
		Boolean release=true;

		block *b=get_line(start_pos.line,b_b_index);

		if(b->n_chars>0)
		{
			if(get_char(start_pos.line,b,start_pos.x)!=chrLineFeed)
			{
				release_line(b_b_index);
				release=false;
				BlockInt prev_length=get_line(start_pos.line-1,b_b_index)->n_chars;
				release_line(b_b_index);
				Int16 curr_width=NOT_CALC;
				Position p=tidy_line_too_short(start_pos.line-1,curr_width,wrapped,increase_redraw);
				if(wrapped)
				{
					//redraw from the correct position
					first_change_pos=p;
					//don't take this out muppet.
					first_change_pos.x=prev_length;
					cursor_pos.line=p.line;
					cursor_pos.x+=prev_length;
				}
			}
		}
		if(release)
			release_line(b_b_index);
	}

	return wrapped;
}

void SiDocument::remove_empty_lines()
{
	block * curr_block;
	Int16 b_b_index;
	Int16 last_line=get_number_blocks()-1;

	while(last_line>=0&&get_number_blocks()>0)
	{
		curr_block=get_line(last_line,b_b_index);

		if(0==curr_block->n_chars)
		{
			//line is empty - remove it unless the previous line ends
			//with a linefeed
			release_line(b_b_index);
			if(last_line>0)
			{
				if(!line_ends_with_line_feed(last_line-1))
				{
					remove_block(last_line);
				}

			}
			else
			{
				remove_block(last_line);
			}

		}
		else
		{
			release_line(b_b_index);
			break;
		}

		--last_line;
	}
}

Boolean SiDocument::line_ends_with_line_feed(const Int16 line)
{
	Int16 b_b_index;
	Boolean ret=false;

	block * curr_block=get_line(line,b_b_index);

	if(curr_block->n_chars>0)
	{
		ret=(get_char(line,curr_block,curr_block->n_chars-1)==chrLineFeed);
	}
	release_line(b_b_index);
	return ret;
}

Position SiDocument::tidy_lines(const Position start_pos,const BlockInt byte_offset,Position & first_change_pos,Position & cursor_pos)
{
  //	cursor_pos.line=SIGNED_NONE;
	Boolean increase_redraw=false;
	Boolean contains_line_feed=false;
	first_change_pos=start_pos;
	wrap_back(start_pos,first_change_pos,cursor_pos,increase_redraw);

	Position p=start_pos;
	Int16 b_b_index;
	BlockInt byte_start=byte_offset;
	Int16 curr_width;
	Boolean changed=true;

	Boolean first=true;

	while(p.line<get_number_blocks()&&changed&&(!m_memory_error_occurred))
	{
		changed=false;
		if(0==p.x&&first)
		  {		    
		    check_new_broken_word(p,cursor_pos,changed,first_change_pos);		  
		  }
		contains_line_feed=tidy_line_too_long(p,byte_start,curr_width,changed,&increase_redraw);
		if(changed)
		  if(p<first_change_pos)
		    first_change_pos=p;

		if( ( (!changed)||(has_done_insert())) && (!contains_line_feed) )
		{
			tidy_line_too_short(p.line,curr_width,changed,increase_redraw);
			++(p.line);
			if(p.line<get_number_blocks())
			  {
			    //			    Position temp_cursor_pos=cursor_pos;
			    //	    Position temp_first_change_pos;
			    check_new_broken_word(p,cursor_pos,changed,first_change_pos);
			    // if(cursor_pos.line==SIGNED_NONE&&temp_cursor_pos.line!=SIGNED_NONE)
			    //   cursor_pos=temp_cursor_pos;
	  
			  }
		}
		else
		  ++(p.line);
	

		first=false;

		byte_start=p.x=0;
	}


	remove_empty_lines();
	--p.line;
	if(get_number_blocks()>0)
	{
		if(increase_redraw)
			p.line=get_number_blocks()-1;
		else if(p.line>=get_number_blocks())
			p.line=get_number_blocks()-1;

		p.x=get_line(p.line,b_b_index)->n_chars;
		release_line(b_b_index);
	}
	else
	{
		p.x=p.line=0;
	}
#ifdef DEBUG_LOG
	log_entry_number("first_change_pos.x=",first_change_pos.x);
	log_entry_number("first_change_pos.line=",first_change_pos.line);
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(cursor_pos.x>60000,"Dodgy-looking cursor_pos on tidy");
#endif

	return p;
}

//! Make sure that the text inserted before this is called has not
//! caused there to be a new word broken
//! over a line that should be changed. Mainly has an affect when user undoes a
//! change or pastes a block in
void SiDocument::check_new_broken_word(Position start_pos,Position & cursor_pos,Boolean & changed,Position & first_change_pos)
{

#ifdef DEBUG_LOG
	log_entry("sidocument check_new_broken_word");
#endif
	//must be a previous line to check!
	if(start_pos.line>0)
	{
		Int16 b_b_index_start,b_b_index_prev;
		block *start_block=get_line(start_pos.line,b_b_index_start);
		//		--start_pos.line;
		Boolean done=false;
		//this line must be non-empty
		if(start_block->n_chars>0)
		{
		  
			WChar the_char=TxtGetChar(start_block->mem_chunk->mem_ptr,0);
			//if the first char on the line isn't a delimiter
			//something may need to change
			if(!(SiUtility::is_break_char(the_char)))
			{
				//is the previous line non-empty too?
				block * prev_block=get_line(start_pos.line-1,b_b_index_prev);
				if(prev_block->n_chars>0)
				{
					BlockInt char_offset=prev_block->n_chars;
					BlockInt byte_offset=prev_block->mem_chunk->size;

					//find where the last word on the previous line starts
#ifdef DEBUG

					ErrFatalDisplayIf(byte_offset<char_offset,"Incorrect byte_offset, check_new_broken_word");
#endif
					if(find_prev_delimiter(prev_block->mem_chunk->mem_ptr,byte_offset,char_offset,false))
					  {

					    if(char_offset>0&&char_offset<prev_block->n_chars)
					      {
						//BlockInt dist_to_move=prev_block->n_chars-char_offset;
						release_line(b_b_index_prev);
						release_line(b_b_index_start);
						
						move_to_next_block(char_offset,byte_offset,start_pos.line-1);
						
						if(cursor_pos.x<char_offset)
						  {
						    //						    cursor_pos.x+=dist_to_move;
						  }
						else
						  {
						    cursor_pos.x-=char_offset;
						    cursor_pos.line=start_pos.line;
						  }
						start_pos.x=char_offset;
						if(start_pos<first_change_pos)
						  first_change_pos=start_pos;
						done=changed=true;
					      }
					  }

				}

				//Fixed:17/01/05
				//make sure the prev_block is released
				if(!done)
					release_line(b_b_index_prev);
			}
		}
		if(!done)
			release_line(b_b_index_start);

	}

}

Boolean SiDocument::insert_block_at_end(SiMemChunk * chunk,BlockInt n_chars)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiText::insert_block_at_end");

#endif

	if(n_chars==0)
		return true;

	if(number_blocks>=MAX_NUMBER_BLOCKS)
		return false;

	Int16 b_b_index;
	Boolean changed,increase_redraw;
	if(!ensure_allocated(number_blocks))
		return false;

	//lock line number_blocks

	block * b=get_line(number_blocks-1,b_b_index);

	b->mem_chunk=chunk;

	b->n_chars=n_chars;
	inserted_text(b->n_chars,b->mem_chunk->size,1);

	//unlock line n
	release_line(b_b_index);
	//reuse argument n_chars as curr_width here
	//evil but we're trying to speed things up round here


	Int16 start_lines=0;
	if(SiPreferencesDatabase::get_load_chunk_size()==0||number_blocks<SiPreferencesDatabase::get_load_chunk_size())
	{
		Int16 curr_width=NOT_CALC;
		if(number_blocks>(Int16)1)
			tidy_line_too_short(number_blocks-2,curr_width,changed,increase_redraw);
		Position p(0,number_blocks-1);
		while(p.line<number_blocks&&number_blocks<=SiPreferencesDatabase::get_load_chunk_size()&&!has_had_memory_error())
		{
			tidy_line_too_long(p,0,curr_width,changed,NULL);
			++(p.line);
			++start_lines;
			if(start_lines>REDRAW_CHROME_INTERVAL)
			{
				start_lines=0;
				m_text_view->draw_status_line_number();
			}
			if(number_blocks>=MAX_NUMBER_BLOCKS)
				return false;

		}
		check_final_block_allocated();
	}
	return true;
}



Boolean SiDocument::tidy_line_too_long(Position start_pos,BlockInt byte_offset,Int16 & curr_width,Boolean & changed,Boolean * increase_redraw)
{
#ifdef DEBUG_LOG
	log_entry("SiText::tidy line too long");
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(byte_offset<start_pos.x,"Incorrect char count 1 tidy_line_too_long");
#endif

	Int16 b_b_index;
	//lock line line
	block * curr_block=get_line(start_pos.line,b_b_index);

	//don't bother if this is an empty block
	if(curr_block->n_chars==0)
	{
		release_line(b_b_index);
		if(start_pos.line==get_number_blocks()-1&&ends_in_line_feed(get_number_blocks()-2))
		{
			set_last_tidied_line(start_pos.line);
		}
		else
		{
			curr_width=0;

			remove_block(start_pos.line);
			changed=true;
			if(increase_redraw!=NULL)
				(*increase_redraw)=true;
		}
		return false;
	}
	
	//Boolean contains_line_feed=false;
	WChar the_char;
	changed=false;

	//start with the width of the characters up to the offset of the change
	if(byte_offset>0)
		curr_width=SiUtility::CorrectCharsWidth_byte(curr_block->mem_chunk->mem_ptr,byte_offset,NULL);
	else
		curr_width=0;

	while(byte_offset<curr_block->mem_chunk->size)
	{
		byte_offset+=TxtGetNextChar(curr_block->mem_chunk->mem_ptr,byte_offset,&the_char);
		++(start_pos.x);

		if(the_char==chrLineFeed)
		{
			//is there any text after the linefeed?
			//if so, move it to the next line
			if(start_pos.x<(curr_block->n_chars))
			{
				release_line(b_b_index);

				changed=move_to_next_block(start_pos.x,byte_offset,start_pos.line);
				if(increase_redraw!=NULL)
					(*increase_redraw)=true;

				//improve the line width estimate for incremental loading if necessary
				if(n_lines_estimated<LINE_WIDTH_ESTIMATE_LIMIT)
				{
					++n_lines_estimated;
					line_width_estimate=(line_width_estimate*(n_lines_estimated-1)+LINE_ESTIMATE_ACCURACY*start_pos.x)/n_lines_estimated;
				}
			}
			else
				release_line(b_b_index);
			set_last_tidied_line(start_pos.line);
			return true;
		}
		else
		{
			curr_width+=CorrectCharWidth(the_char,curr_width);

			if(curr_width>(ACTIVEWIDTH))
			{
				//the text in the current line sticks out over the edge of
				// the screen, so move it to the next line
				//first find start of word to move
#ifdef DEBUG
				ErrFatalDisplayIf(byte_offset<start_pos.x,"Incorrect byte_offset, tidy_line_too_long");
#endif

				if(!find_prev_delimiter(curr_block->mem_chunk->mem_ptr,byte_offset,start_pos.x,false))
				  {
				    --start_pos.x;
				    byte_offset-=TxtCharSize(the_char);
				  }

				//update line width estimate
				if(n_lines_estimated<LINE_WIDTH_ESTIMATE_LIMIT)
				{
					++n_lines_estimated;
					line_width_estimate=(line_width_estimate*(n_lines_estimated-1)+LINE_ESTIMATE_ACCURACY*start_pos.x)/n_lines_estimated;
				}
				//then do the moving
				//make sure line is released in case block needs to be moved
				release_line(b_b_index);

				changed=move_to_next_block(start_pos.x,byte_offset,start_pos.line);
				if(get_done_insert()&&NULL!=increase_redraw)
					(*increase_redraw)=true;
				set_last_tidied_line(start_pos.line-1);
				return false;
			}
		}

	}
	//unlock line line
	set_last_tidied_line(start_pos.line);
	release_line(b_b_index);

	return false;
}

void SiDocument::set_last_tidied_line(const Int16 line)
{
	if(line>last_line_tidied)
		last_line_tidied=line;
	if(last_line_tidied>=get_number_blocks())
		last_line_tidied=get_number_blocks()-1;
}
Position SiDocument::tidy_line_too_short(const Int16 line,Int16 &curr_width,Boolean & changed,Boolean & increase_redraw)
{
#ifdef DEBUG_LOG
	log_entry("SiText::tidy_line_too_short");
#endif
	//position returned is where the insert_pos position ends up

	Position insert_pos(0,line);
	if(line+1>=get_number_blocks())
		return insert_pos;

	Int16 b_b_index1,b_b_index2;

	//lock line line const
	block * curr_block;

	//if curr_width not calculated already by a preceding tidy_too_long,do it now
	if(curr_width==NOT_CALC)
	{
		curr_block=get_line(line,b_b_index1);
		if(curr_block->n_chars==0)
			curr_width=0;
		else
		{
			Boolean c_line_feed=false;
			curr_width=SiUtility::CorrectCharsWidth_byte(curr_block->mem_chunk->mem_ptr,curr_block->mem_chunk->size,&c_line_feed);
			if(c_line_feed)
			{
				//if this line has a line feed in it, stop now
				release_line(b_b_index1);
				changed=false;

				return insert_pos;
			}
		}
		release_line(b_b_index1);
	}


	BlockInt number_to_move;
	Int16 byte_offset,rem_width,width_to_move;
	Boolean stop=false;
	Position ret_pos=insert_pos;
	Boolean first=true;
	WChar the_char;
	block * next_block;

	while((curr_width<ACTIVEWIDTH)&&((line+1)<get_number_blocks())&&!(stop))
	{
		curr_block=get_line(line,b_b_index1);
		insert_pos.x=curr_block->n_chars;

		if(first)
		{
			ret_pos.x=curr_block->n_chars;
			first=false;
		}
		//width of gap between edge of screen and this line
		rem_width=(ACTIVEWIDTH)-curr_width;

		byte_offset=width_to_move=0;
		next_block=get_line(line+1,b_b_index2);

		if(next_block->n_chars==0)
		{
			release_line(b_b_index2);
			release_line(b_b_index1);
			number_to_move=0;
			break;
		}
		BlockInt prev_delim_byte=0;
		BlockInt prev_delim_char=0;
		Boolean delim_exists=false;
		//scan forwards through the line until we've found enough characters
		//to fill the gap on the previous line
		for(number_to_move=0;number_to_move<next_block->n_chars;++number_to_move)
		{
			byte_offset+=TxtGetNextChar(next_block->mem_chunk->mem_ptr,byte_offset,&the_char);
			width_to_move+=CorrectCharWidth(the_char,width_to_move+curr_width);
			if(SiUtility::is_break_char(the_char))
			{
				if(width_to_move<rem_width)
				{
					prev_delim_byte=byte_offset;
					prev_delim_char=number_to_move+1;
				}
				delim_exists=true;
			}

			//if this character would bring the next line over the edge
			//ignore it and set counter to move chars before it
			if(width_to_move>rem_width)
			{
				width_to_move-=CorrectCharWidth(the_char,width_to_move+curr_width);
				byte_offset-=TxtCharSize(the_char);

				break;
			}
			else if(the_char==chrLineFeed)
			{
				++number_to_move;
				increase_redraw=true;
				stop=true;
				break;
			}

		}

		//avoid violating word wrap constraints
		if(number_to_move<next_block->n_chars&&!stop)
		{
			if (prev_delim_char>0)
			{
				byte_offset=prev_delim_byte;
				number_to_move=prev_delim_char;
			}
			else
			{
			
			  if(find_delimiter(curr_block->mem_chunk,0))
			    number_to_move=byte_offset=0;
			}
		}

		//if there are still chars to move, do the move
		if(number_to_move>0)
		{
			//if some more characters from the next line will fit on the end of this one, move them in
			ret_pos.x=curr_block->n_chars;
			ret_pos.line=line;

			//do the move
			move_to_prev_block(line+1,next_block,curr_block,byte_offset,number_to_move);

			//tidy up

			if(next_block->n_chars==0)
			{
				release_line(b_b_index2);
				release_line(b_b_index1);

				next_block=NULL;
				remove_block(line+1);
				curr_block=get_line(line,b_b_index1);
				increase_redraw=true;
			}
			else
				release_line(b_b_index2);

			//have made a change so set flag and update the current width of this line
			changed=true;
			curr_width+=width_to_move;
		}
		else
		{
			//no changes made so stop
			release_line(b_b_index2);
			stop=true;
		}

		release_line(b_b_index1);
	}

	return ret_pos;
}

//Utility functions
Boolean SiDocument::find_delimiter(const SiMemChunk * chunk,BlockInt start_offset)
{
	WChar the_char;
	while(start_offset<chunk->size)
	{
		start_offset+=TxtGetNextChar(chunk->mem_ptr,start_offset,&the_char);
		if(the_char==chrLineFeed)
			return true;
		else
			if(TxtCharIsSpace(the_char))
				return true;
	}
	/*if(SiUtility::is_break_char(the_char))
	  return true;*/

	return false;
}

Boolean SiDocument::find_prev_delimiter(const Char * data, BlockInt& byte_offset,BlockInt& char_offset,const Boolean before)
{
	//find the offset of the first delimeter character
	//working backwards from the specified postion

#ifdef DEBUG
  ErrFatalDisplayIf(NULL==data,"NULL data in find_prev_delimiter");
	ErrFatalDisplayIf(byte_offset<char_offset,"Incorrect char count, find_prev_delimiter");
#endif

	BlockInt b_offset=byte_offset;
	BlockInt c_offset=char_offset;

	WChar the_char;

	while(c_offset>1)
	{
#ifdef DEBUG
		ErrFatalDisplayIf(b_offset==0,"SiText::find_prev_delimiter i==0");
#endif

		b_offset-=TxtGetPreviousChar(data,b_offset,&the_char);
		--c_offset;

		if(TxtCharIsSpace(the_char))
		{
		  if(before)
		    {
		      byte_offset=b_offset;
		      char_offset=c_offset;		     
		    }
		  else
		    {
		      byte_offset=b_offset+TxtCharSize(the_char);
		      char_offset=c_offset+1;

		    }
		  return true;
		}
	
		
	}
	return false;

}

//do a bit of sloshing at the end of the document to make sure
//that the text up to the line displayed is ok
void SiDocument::tidy_up_lines(Int16 n_lines)
{
	if(last_line_tidied>=get_number_blocks())
		last_line_tidied=get_number_blocks()-1;

	Position p(0,last_line_tidied);
	Int16 width=0;
	Boolean changed=false;
	m_background_tidy=true;


	Int16 n_lines_done=0;

	Boolean increase_redraw;
	m_text_view->disable_screen_updates();
	while(n_lines>=0&&p.line<get_number_blocks()&&!has_had_memory_error())
	{
		//insert_block(p.line);

		tidy_line_too_long(p,0,width,changed,NULL);
		if(!changed)
		{
			width=NOT_CALC;
			tidy_line_too_short(p.line,width,changed,increase_redraw);
		}
		++(p.line);
		--n_lines;

		++n_lines_done;


	}
	m_text_view->enable_screen_updates();
	m_background_tidy=false;

	if(last_line_tidied>=get_number_blocks()-1)
		set_apparent_number_blocks(get_number_blocks());
	check_final_block_allocated();

}
Boolean SiDocument::ensure_tidied(const Int16 line)
{


	if(line+TIDY_LINE_BUFFER>last_line_tidied&&last_line_tidied<=get_number_blocks()-1)
	{
#ifdef BENCHMARK_TIDY_UP
		Int16 start_time=TimGetTicks();
#endif

		if(last_line_tidied>=get_number_blocks())
			last_line_tidied=get_number_blocks()-1;

		Int16 n_lines=line-last_line_tidied;

		if(n_lines>SLOSH_LINES_WAIT_DISPLAY_LIMIT)
			m_text_view->start_waiting();

		if(n_lines<SLOSH_LINES_CHUNK)
			n_lines=SLOSH_LINES_CHUNK;
		if(n_lines>0)
			tidy_up_lines(n_lines);

		if(n_lines>SLOSH_LINES_WAIT_DISPLAY_LIMIT)
			m_text_view->stop_waiting();


#ifdef BENCHMARK_TIDY_UP

		start_time=TimGetTicks()-start_time;
		Char buff[100];
		StrPrintF(buff,"Ticks to slosh=%i",start_time);
		DisplayError(DEBUG_MESSAGE,buff);
#endif

		return true;
	}
	else
		return false;
}
void SiDocument::make_valid_pos(Position & p)
{
	Int16 b_b_index;
	if(get_number_blocks()==0)
	{
		p.line=0;
		p.x=0;
		return;
	}
	if(p.line>=get_number_blocks())
	{
		p.x=get_line(get_number_blocks()-1,b_b_index)->n_chars;
		p.line=get_number_blocks()-1;
		release_line(b_b_index);
	}
	else if(p.line<0)
		p.line=0;

	block * b=get_line(p.line,b_b_index);
	if(p.x>b->n_chars)
		p.x=b->n_chars;
	release_line(b_b_index);
}
Boolean SiDocument::has_had_memory_error() const
{
	return m_memory_error_occurred;
}
void SiDocument::set_memory_error(Boolean m)
{
	m_memory_error_occurred=m;
}

#ifdef TEST_OBJECT_UNIT
Boolean SiDocument::perform_tests()
{
	//perform all the unit tests for this object. Return false if failed.
	//Should leave state of object identical to when instantiated

	//allocate 10 blocks
	ensure_allocated(9);

#ifdef TEST_OBJECT_LOG

	log_entry("SiDocument::tests() 1");
#endif

	ErrFatalDisplayIf(get_number_blocks()!=10,"SiDocument test failed: ensure_allocated");
	//ErrFatalDisplayIf(MemHandleSize(block_buffer_array_handle)!=10*sizeof(block),"SiDocument test failed : ensure allocated handle size");

	Int16 b_b_index;
	block * b=get_line(9,b_b_index);

	ErrFatalDisplayIf(b==NULL,"SiDocument test failed: get_line");
	release_line(b_b_index);

#ifdef TEST_OBJECT_LOG

	log_entry("SiDocument::tests() 2");
#endif

	insert_block(8);
	ErrFatalDisplayIf(get_number_blocks()!=11,"SiDocument test failed: inserting block");
	//ErrFatalDisplayIf(MemHandleSize(block_buffer_array_handle)!=11*sizeof(block),"SiDocument test failed : insert block handle size");
	b=get_line(10,b_b_index);
	ErrFatalDisplayIf(b==NULL,"SiDocument test failed: inserting_block get_line");
	release_line(b_b_index);
	remove_block(7);

#ifdef TEST_OBJECT_LOG

	log_entry("SiDocument::tests() 3");
#endif

	ErrFatalDisplayIf(get_number_blocks()!=10,"SiDocument test failed: removing block");
	Int16 start_index;
	block_buffer * b_b=get_block_buffer(0,&start_index,b_b_index);
	Char error[100];
	StrPrintF(error,"SiDocument test failed: block buffer number of blocks=%i",b_b->n_lines);
	ErrFatalDisplayIf(b_b->n_lines!=10,error);


#ifdef TEST_OBJECT_LOG

	log_entry("SiDocument::tests() 4");
#endif

	free_text();
	ErrFatalDisplayIf(get_number_blocks()!=0,"SiDocument test failed: freeing text");
	ErrFatalDisplayIf(block_buffer_array_handle!=NULL,"SiDocument test failed : freeing text handle");

#ifdef DEBUG_LOG

	log_entry("SiDocument tests complete");
#endif

	return true;
}
#endif

