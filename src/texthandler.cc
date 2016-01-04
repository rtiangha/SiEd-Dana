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

#include "texthandler.h"
#include "error.h"
#include "types.h"
#include "editor.h"
#include "utility.h"
#include "memhandler.h"
#define SMALL_UNDO_BUFFER 4096
#define LARGE_UNDO_BUFFER 128000
UInt32 MAX_UNDO_BUFFER=SMALL_UNDO_BUFFER;

SiUndoAction::SiUndoAction()
{
	if(this==NULL)
		return;
	text=NULL;
	next=prev=NULL;
}

SiUndoAction::~SiUndoAction()
{
	if(text!=NULL)
	{
		SiMemHandler::free_chunk(text);
	}
}
//!
//! Initialise the texthandler
//!
SiTextHandler::SiTextHandler()
{
	UInt32 free_mem,max_chunk;
	MemHeapFreeBytes(0,&free_mem,&max_chunk);
	if(free_mem>SMALL_MEM_AMOUNT)
		MAX_UNDO_BUFFER=LARGE_UNDO_BUFFER;
	undo_action_list=curr_undo_action=NULL;
	m_document=new SiDocument();
	initialise();
}

SiDocument * SiTextHandler::get_document()
{
	return m_document;
}


//!
//! Tidy up on exiting the texthandler
//!
SiTextHandler::~SiTextHandler()
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiText::Destructor");
#endif

	clear_undo_history();
	delete m_document;
}

void SiTextHandler::initialise()
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiText::init");
#endif

	m_document->free_text();
	m_document->ready_for_change();
	invalidate_cache();
	//	insert_block_curr_width=NOT_CALC;
	clear_undo_history();

	last_undo_action=undo_action_list=curr_undo_action=NULL;
	undo_position_start.line=SIGNED_NONE;
	undo_action=SIGNED_NONE;
	enable_undo_history();
	m_document->clear_dirty_flag();
}

//!
//! Make sure that all the lines up to the index passed in exist
//! if they don't then allocate some more
Position SiTextHandler::find_next_word_boundary(Position start,const Boolean ignore_punctuation)
{
	if(m_document->get_number_blocks()==0)
		return start;
	Int16 b_b_index;

	BlockInt byte_offset;
	WChar the_char;
	block * curr_block;
	do
	{
		curr_block=m_document->get_line(start.line,b_b_index);
		if(curr_block->n_chars==0)
		{
			++(start.line);
		}
		else
			break;
		m_document->release_line(b_b_index);
		curr_block=NULL;
	}
	while(start.line<m_document->get_number_blocks());

	if(curr_block==NULL)
		return m_document->find_end_document();

	m_document->get_char_bound(start.line,curr_block->mem_chunk->mem_ptr,start.x,&byte_offset);

	while(curr_block!=NULL)
	{
		while(start.x<curr_block->n_chars)
		{
			byte_offset+=TxtGetNextChar(curr_block->mem_chunk->mem_ptr,byte_offset,&the_char);
			if(SiUtility::is_break_char(the_char,ignore_punctuation))
			{
				m_document->release_line(b_b_index);
				return start;
			}
			else
				++(start.x);
		}
		++start.line;
		byte_offset=start.x=0;

		m_document->release_line(b_b_index);
		curr_block=m_document->get_line(start.line,b_b_index);
	}

	curr_block=m_document->get_line(m_document->get_number_blocks()-1,b_b_index);
	start.line=m_document->get_number_blocks()-1;
	start.x=curr_block->n_chars;
	m_document->release_line(b_b_index);
	return start;
}

Position SiTextHandler::find_prev_word_boundary(Position start,const Boolean ignore_punctuation)
{
	if(m_document->get_number_blocks()==0)
		return start;
	Int16 b_b_index;
	BlockInt byte_offset;
	WChar the_char;
	block * curr_block;
	do
	{
		curr_block=m_document->get_line(start.line,b_b_index);
		if(curr_block->n_chars==0)
		{
			--(start.line);
		}
		else
		{
			break;
		}

		m_document->release_line(b_b_index);
		curr_block=NULL;
	}
	while(start.line>=0);
	if(curr_block==NULL)
		return Position(0,0);
	m_document->get_char_bound(start.line,curr_block->mem_chunk->mem_ptr,start.x,&byte_offset);

	while(curr_block!=NULL)
	{
		while(start.x>0)
		{
#ifdef DEBUG
			ErrFatalDisplayIf(byte_offset==0,"SiText::find_prev_word_bound byte_offset==0");
#endif

			byte_offset-=TxtGetPreviousChar(curr_block->mem_chunk->mem_ptr,byte_offset,&the_char);
			if(SiUtility::is_break_char(the_char,ignore_punctuation))
			{
				m_document->release_line(b_b_index);
				m_document->check_newline_position(start,1);
				return start;
			}
			else
				--(start.x);
		}
		--start.line;

		m_document->release_line(b_b_index);
		if(start.line>=0)
		{
			curr_block=m_document->get_line(start.line,b_b_index);
			start.x=curr_block->n_chars;
			if(curr_block->mem_chunk!=NULL)
				byte_offset=curr_block->mem_chunk->size;
			else
				byte_offset=0;
		}
		else
			curr_block=NULL;
	}

	return Position(0,0);
}

//!
//! Insert the buffer passed in into the current document at position ch
//!
Boolean SiTextHandler::insert_chars(const Char* c,const BlockInt n_chars,const BlockInt bytes,Position ch,Position & f_cursor_pos)
{
#ifdef TEST_OBJECT_LOG
	log_entry("texthandler:insert_chars");
#endif

	invalidate_cache();

#ifdef DEBUG
	
	if(m_document->get_number_blocks()>0)
	  {
	    if(ch.line>=m_document->get_number_blocks())
	      {
		Char buff[100];
		StrPrintF(buff,"Writing off end of last line,ch.line=%i",ch.line);
		ErrFatalDisplay(buff);
	      }
	    Int16 t_b_in;
	    block * t_b=m_document->get_line(ch.line,t_b_in);
	    if(ch.x>t_b->n_chars)
	      {
		Char buff[100];
		StrPrintF(buff,"insert chars off end of line, ch.x=%u,t_b->n_chars=%u",ch.x,t_b->n_chars);	
		m_document->release_line(t_b_in);
		ErrFatalDisplay(buff);
	      }
	    m_document->release_line(t_b_in);
	   
	  }
 ErrFatalDisplayIf(n_chars<bytes,"Incorrect char count on insert_char");
#endif
	//No inserting zero length blocks
	if(bytes==0)
		return false;

	//Increase document length
	m_document->inserted_text(n_chars,bytes,1);

	m_document->ready_for_change();

	if(!m_document->ensure_allocated(ch.line))
		return false;

	Int16 b_b_index;
	//lock line ch.line
	block * b=m_document->get_line(ch.line,b_b_index);
#ifdef DEBUG

	ErrFatalDisplayIf(ch.x>b->n_chars,"Attempted write off end of block");
#endif
	//first insert the character in the block
	BlockInt offset;

	//allocate enough space in the block's chunk
	if(!b->ensure_space(bytes))
	{
		m_document->release_line(b_b_index);
		return false;
	}

	m_document->move_up_data(b,ch,bytes);

	//then insert the characters
	//find the byte offset

	b->n_chars+=n_chars;

	//find the insertion position
	if(b->n_chars>0)
		m_document->get_char_bound(ch.line,b->mem_chunk->mem_ptr,ch.x,&offset);
	else
		offset=0;

	//insert text
#ifdef DEBUG

	if(b->mem_chunk->size<offset+bytes)
	{
	#ifdef DEBUG_LOG
		log_entry_number("Error:chunk size=",b->mem_chunk->size);
		log_entry_number("Error:Writing to ",offset);
		log_entry_number("Error:size=",bytes);
		log_entry_number("Error:ch.x=",ch.x);
	#endif
		ErrFatalDisplay("Writing off end of block");
	}
#endif

	MemMove(b->mem_chunk->mem_ptr+offset,c,bytes);

	//	#ifdef USE_CHAR_CACHE
	//	m_document->check_cache(ch.line,ch.line);
	//	#endif
	m_document->release_line(b_b_index);

	//then re-slosh document
	//Position end is the last position that needs to be redrawn

	Position new_cursor_pos=ch;

	Position earliest_change;

#ifdef DEBUG

	ErrFatalDisplayIf(offset<ch.x,"Incorrect byte_offset, insert_char");
#endif

	Position end=m_document->tidy_lines(ch,offset,earliest_change,new_cursor_pos); //offset is the byte position of the first character inserted

	if(new_cursor_pos.line!=SIGNED_NONE)
		f_cursor_pos=new_cursor_pos;
	else
		f_cursor_pos=ch;

	if(!m_document->ensure_allocated(f_cursor_pos.line))
		return false;

	m_document->made_change(earliest_change,end);

	m_document->check_final_block_allocated();

	//move the cursor to the correct position - end of the inserted text
	m_document->move_position(f_cursor_pos,n_chars,true,false);

	add_undo_action(c,bytes,ch,f_cursor_pos,INSERTED_TEXT);

	if(new_cursor_pos.line!=SIGNED_NONE)
	{
		ch=new_cursor_pos;
	}

#ifdef DEBUG
	ErrFatalDisplayIf(ch.x>60000,"Dodgy-looking ch for undo action");
#endif
	m_document->correct_position(ch);
	change_undo_action_start(ch);

	return true;
}





/*void SiTextHandler::remove_block(const Int16 line)
{
#ifdef TEST_OBJECT_LOG
	log_entry_number("SiText:remove block ",line);
#endif
 
	Int16 b_b_index;
	block * b=m_document->get_line(line,b_b_index);
	if(b->mem_chunk!=NULL)
	{
		append_text_undo_action(b->mem_chunk->mem_ptr,b->mem_chunk->size);
	}
 
	m_document->release_line(b_b_index);
 
#ifdef TEST_OBJECT_LOG
	log_entry("SiText:remove block done");
#endif
}*/
void SiTextHandler::add_block_delete_undo(const Int16 line)
{
	Int16 b_b_index;
	block * b=m_document->get_line(line,b_b_index);
	if(b->mem_chunk!=NULL)
	{
		append_text_undo_action(b->mem_chunk->mem_ptr,b->mem_chunk->size);
	}

	m_document->release_line(b_b_index);

}
Boolean SiTextHandler::remove_blocks(const Int16 start_line,const Int16 end_line)
{
	if(start_line>end_line)
	{
		return false;
	}
	Int16 l;
	for(l=start_line;l<=end_line;++l)
		add_block_delete_undo(l);

	for(l=end_line;l>=start_line;l--)
		m_document->remove_block(l);

	return true;
}
void SiTextHandler::invalidate_cache()
{
#ifdef USE_WIDTH_CACHE
	SiUtility::invalidate_cache();
#endif
  #ifdef USE_CHAR_CACHE

	m_document->invalidate_cache();
#endif
}
Position SiTextHandler::delete_chars(Position start,Position end)
{

	invalidate_cache();

	Position earliest_change,draw_end;

	Int16 b_b_index,b_b_index2;

	if(start.line>=m_document->get_number_blocks())
	{
		return Position (0,m_document->get_number_blocks()-1);
	}

	if(end.line>=m_document->get_number_blocks())
	{
		end.line=m_document->get_number_blocks()-1;
		block * b=m_document->get_line(end.line,b_b_index);
		end.x=b->n_chars;
		m_document->release_line(b_b_index);
	}
	if(start==end)
	{
		return start;
	}
	m_document->ready_for_change();

	//lock line end.line
	block * start_line=m_document->get_line(start.line,b_b_index2);
	block * end_line=m_document->get_line(end.line,b_b_index);

	if(end.x>end_line->n_chars)
	{
		end.x=end_line->n_chars;
	}

	Int16 start_rem=1;
	Int16 end_rem=0;
	add_undo_action(NULL,0,start,end,DELETED_TEXT);

	//if this is only text on one line, only remove the block if
	//the deleted text range includes the entire line
	Boolean increase_redraw=false;

	if(start.line==end.line)
	{

		if(end.x==end_line->n_chars&&start.x==0)
		{
			start_rem=start.line;
			end_rem=end.line;
			m_document->release_line(b_b_index);
			m_document->release_line(b_b_index2);
			add_block_delete_undo(start.line);
			m_document->remove_block(start.line);
			increase_redraw=true;
		}
		else
		{
			m_document->release_line(b_b_index);
			m_document->release_line(b_b_index2);


			remove_text_range(start.line,start.x,end.x);
		}
	}
	else
	{

		//		Int16 start_rem,end_rem;

		//remove the text on the first line
		if(start.x>0)
		{
			remove_text_range(start.line,start.x,start_line->n_chars);
			start_rem=start.line+1;
		}
		else
			start_rem=start.line;

		if(end.x<end_line->n_chars)
		{
			end_rem=end.line-1;
		}
		else
			end_rem=end.line;

		m_document->release_line(b_b_index);
		m_document->release_line(b_b_index2);

		//remove the blocks in the middle of the range
		remove_blocks(start_rem,end_rem);

		//remove the text on the last line
		if(end_rem==end.line-1)
		{
			remove_text_range(start_rem,0,end.x);
		}
		increase_redraw=true;
	}

	earliest_change=start;

	Position start_temp=start;
	if(start.line<m_document->get_number_blocks())
	{
		Position start_t(0,start.line);

		draw_end=m_document->tidy_lines(start_t,0,earliest_change,start_temp);
		if(start_temp.line!=SIGNED_NONE)
			start=start_temp;
	}

	if(increase_redraw)
	{
		draw_end=m_document->find_end_document();
	}
	else
	{
		if(m_document->get_number_blocks()==0)
		{
			draw_end.line=0;
			draw_end.x=0;
		}
	}


	m_document->made_change(earliest_change,draw_end);
	m_document->check_final_block_allocated();


	change_undo_action_start(start);
	return start;
}


void SiTextHandler::remove_text_range(const Int16 line,const BlockInt start_x,BlockInt end_x)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiText:remove text range");
#endif

	if(line>=m_document->get_number_blocks()||start_x>=end_x)
		return;
	BlockInt start_offset,end_offset;
	Int16 b_b_index;

	//lock line line (const)
	block * b=m_document->get_line(line,b_b_index);

	m_document->get_char_bound(line,b->mem_chunk->mem_ptr,start_x,&start_offset);
	m_document->get_char_bound(line,b->mem_chunk->mem_ptr,end_x,&end_offset);

	append_text_undo_action(b->mem_chunk->mem_ptr+start_offset,end_offset-start_offset);
	m_document->release_line(b_b_index);
	m_document->remove_text_range(line,start_x,end_x);

}



Boolean SiTextHandler::validate_position(Position & p)
{
	if(p.line>=m_document->get_number_blocks()||p.line<0)
		return false;

	Int16 b_b_index;
	//lock line p.line
	const BlockInt x=m_document->get_line(p.line,b_b_index)->n_chars;
	//unlock line p.line
	m_document->release_line(b_b_index);
	if(p.x>x)
	{
		return false;
	}

	return true;
}
//! Move the passed in position so that a newline at the end of a line
//! is ignored correctly
//! (i.e insert text before it, move to next line if moving right over it)
Position SiTextHandler::get_next_pos(Position p)
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiTextHandler::get next Pos");
#endif

	if(m_document->get_number_blocks()==0)
		return Position(0,0);

	if(p.line>=m_document->get_number_blocks())
	{
		p.x=0;
	}
	else
	{
		++p.x;
		Int16 b_b_index;
		block * curr_block=m_document->get_line(p.line,b_b_index);
		if(p.x>curr_block->n_chars&&p.line<m_document->get_number_blocks()-1)
		{
			m_document->release_line(b_b_index);
			curr_block=m_document->get_line(p.line+1,b_b_index);
			if(curr_block->n_chars>0)
			{
				p.x=1;
				++(p.line);
			}
			else
			{
				--p.x;
			}
		}
		m_document->release_line(b_b_index);
	}
	return p;
}
//!
//! Find the position before the postion passed in in the text
//!
Position SiTextHandler::get_prev_pos(Position p)
{

	if(p.line<0)
		p.line=0;
	if(p.x==0)
	{
		if(p.line>0)
		{
			--(p.line);
			//lock line ret.line
			Int16 b_b_index;
			block * b=m_document->get_line(p.line,b_b_index);
			p.x=b->n_chars-1;

			//release line ret.line
			m_document->release_line(b_b_index);
		}

	}
	else
		--p.x;
	return p;
}
//!
//! Insert a new block *after* the line passed in
//!


void SiTextHandler::clear_undo_history()
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiText::clear_undo_history");
#endif

	SiUndoAction * curr_act=undo_action_list;
	SiUndoAction * temp_act;
	while(curr_act!=NULL)
	{
		temp_act=curr_act->next;
		delete curr_act;
		curr_act=temp_act;
	}
	undo_memory_usage=0;
	undo_action_list=NULL;
	last_undo_action=NULL;

}

Boolean SiTextHandler::undo(Position & f_cursor_pos)
{
	if(undo_action_list==NULL||curr_undo_action==NULL)
		return false;
	else
	{
		perform_action(curr_undo_action,false,f_cursor_pos);
		last_undo_action=curr_undo_action;
		curr_undo_action=curr_undo_action->prev;
		return true;
	}
}

Boolean SiTextHandler::redo(Position & f_cursor_pos)
{
	if(undo_action_list==NULL||last_undo_action==NULL)
		return false;
	else
	{
		perform_action(last_undo_action,true,f_cursor_pos);
		curr_undo_action=last_undo_action;
		last_undo_action=last_undo_action->next;
		return true;
	}
}
void SiTextHandler::perform_action(SiUndoAction * action,Boolean reverse,Position & f_cursor_pos)
{
#ifdef DEBUG_LOG
	log_entry("Undo perform action");
#endif

	Position final_cursor_pos;
	disable_undo_history();
	m_document->ready_for_change();
	switch(action->status)
	{
	case INSERTED_TEXT:
		if(reverse)
		{
			BlockInt n_chars=SiUtility::calc_n_chars(action->text->mem_ptr,action->text->size);
			insert_chars(action->text->mem_ptr,n_chars,action->text->size,action->start_position,final_cursor_pos);
			f_cursor_pos=action->end_position;
		}
		else
		{
			delete_chars(action->start_undone_position,action->end_position);
			f_cursor_pos=action->start_position;
		}
		break;
	case DELETED_TEXT:
		if(reverse)
		{
			delete_chars(action->start_position,action->end_position);
			f_cursor_pos=action->start_undone_position;
		}
		else
		{

			BlockInt n_chars=SiUtility::calc_n_chars(action->text->mem_ptr,action->text->size);
			insert_chars(action->text->mem_ptr,n_chars,action->text->size,action->start_undone_position,f_cursor_pos);
			f_cursor_pos=action->end_position;
			//++(f_cursor_pos.x);
		}
		break;
	}

	enable_undo_history();
}

void SiTextHandler::change_undo_action_end(Position end_pos)
{
#ifdef LOG_ENTRY
	log_entry("SiText::change_undo_action_end");
#endif
	#ifdef DEBUG

	ErrFatalDisplayIf(end_pos.x>60000,"Dodgy-looking end_pos for change end undo action");
#endif

	if(curr_undo_action!=NULL&&undo_active)
		curr_undo_action->end_position=end_pos;
}

void SiTextHandler::change_undo_action_start(Position pos)
{
#ifdef DEBUG
	ErrFatalDisplayIf(pos.x>60000,"Dodgy-looking start_pos for change undo action start");
	ErrFatalDisplayIf(pos.line>m_document->get_number_blocks(),"Undo action start off end of document");
#endif

	if(curr_undo_action!=NULL&&undo_active)
		curr_undo_action->start_undone_position=pos;
}
SiUndoAction * SiTextHandler::create_undo_action()
{
#ifdef LOG_ENTRY
	log_entry("SiText::create_undo_action");
#endif

	SiUndoAction* action=NULL;
	while((action=new SiUndoAction())==NULL)
	{
		if(!remove_last_undo_action())
			break;
	}
	return action;
}

Boolean SiTextHandler::add_undo_action(const Char * text,BlockInt n_bytes,Position start_pos,Position end_pos,UInt8 type)
{
#ifdef LOG_ENTRY
	log_entry("SiText::add_undo_action");
ErrFatalDisplayIf(ch.line>=m_document->get_number_blocks(),"Writing off end of last line");
	Int16 t_b_in;
	block * t_b=m_document->get_line(start_pos.line,t_b_in);
	if(start_pos.x>t_b->n_chars)
	  {
	    Char buff[100];
	    StrPrintF(buff,"insert chars off end of line, ch.x=%u,t_b->n_chars=%u",start_pos.x,t_b->n_chars);	
	    m_document->release_line(t_b_in);
	    ErrFatalDisplay(buff);
	  }
	m_document->release_line(t_b_in);

#endif

	if(!undo_active)
		return false;

	if(n_bytes>MAX_UNDO_BUFFER)
	{
		FrmAlert(UnableToUndoAlert);
		return false;
	}

	if(!ensure_undo_space(n_bytes+sizeof(SiUndoAction)))
		return false;

	SiUndoAction * action=create_undo_action();
	if(NULL==action)
		return false;

	if(text!=NULL)
	{
		action->text=SiMemHandler::get_chunk(NULL,n_bytes,NON_TEXT_HEAP);
		if(action->text==NULL)
		{
			delete action;
			FrmAlert(UnableToUndoAlert);
			return false;
		}
#ifdef DEBUG
		ErrFatalDisplayIf(action->text->size<n_bytes,"Writing action off end of block");
#endif

		MemMove(action->text->mem_ptr,text,n_bytes);
	}


	undo_memory_usage+=sizeof(SiUndoAction)+n_bytes;
#ifdef DEBUG

	ErrFatalDisplayIf(start_pos.x>60000,"Dodgy-looking start_pos for undo action");
	ErrFatalDisplayIf(end_pos.x>60000,"Dodgy-looking end_pos for undo action");
#endif

	action->start_position=start_pos;
	action->end_position=end_pos;
	action->status=type;

	if(curr_undo_action!=NULL)
	{
		clear_actions_after(curr_undo_action);
		curr_undo_action->next=action;
	}
	else if(last_undo_action!=NULL)
	{
		if(last_undo_action==undo_action_list)
			undo_action_list=NULL;
		delete last_undo_action;

	}
	last_undo_action=NULL;
	if(undo_action_list==NULL)
		undo_action_list=action;
	action->prev=curr_undo_action;
	curr_undo_action=action;

	return true;
}

Boolean SiTextHandler::ensure_undo_space(const BlockInt n_bytes)
{
#ifdef LOG_ENTRY
	log_entry("SiText::ensure_undo_space");
#endif

	while(undo_memory_usage+n_bytes>MAX_UNDO_BUFFER)
	{
		if(!remove_last_undo_action())
		{
			FrmAlert(UnableToUndoAlert);
			return false;
		}
		if(NULL==curr_undo_action)
		{
			FrmAlert(UnableToUndoAlert);
			return false;
		}
	}
	return true;
}

Boolean SiTextHandler::append_text_undo_action(const Char * text,BlockInt n_bytes)
{

	if(!undo_active)
		return false;


	if(curr_undo_action!=NULL)
	{
		if(!ensure_undo_space(n_bytes))
			return false;


		if(n_bytes>0)
		{
			BlockInt initial_size=0;
			if(curr_undo_action->text==NULL)
			{
				curr_undo_action->text=SiMemHandler::get_chunk(NULL,n_bytes,NON_TEXT_HEAP);
			}
			else
			{
				initial_size=curr_undo_action->text->size;
				curr_undo_action->text=SiMemHandler::resize_chunk(curr_undo_action->text,curr_undo_action->text->size+n_bytes,TO_START);
			}
			if(curr_undo_action->text==NULL)
			{
				SiUndoAction* temp_action=curr_undo_action->prev;
				delete curr_undo_action;
				curr_undo_action=temp_action;
				FrmAlert(UnableToUndoAlert);
				return false;
			}
			else
			{
#ifdef DEBUG
				ErrFatalDisplayIf(curr_undo_action->text->size<initial_size+n_bytes,"Appending undo off end of block");
#endif

				MemMove(curr_undo_action->text->mem_ptr+initial_size,text,n_bytes);

				undo_memory_usage+=n_bytes;
				return true;
			}
		}
	}
	return false;
}

Boolean SiTextHandler::remove_last_undo_action()
{
#ifdef LOG_ENTRY
	log_entry("SiText::remove_last_undo_action");
#endif

	if(undo_action_list==NULL)
		return false;
	else
	{
		SiUndoAction * temp_action=undo_action_list->next;

		if(undo_action_list->text!=NULL)
			undo_memory_usage-=(sizeof(SiUndoAction)+undo_action_list->text->size);
		else
			undo_memory_usage-=(sizeof(SiUndoAction));

		if(last_undo_action==undo_action_list)
			last_undo_action=NULL;
		if(curr_undo_action==undo_action_list)
			curr_undo_action=temp_action;
		delete undo_action_list;

		undo_action_list=temp_action;
		return true;
	}
}

void SiTextHandler::clear_actions_after(SiUndoAction * action)
{
	action=action->next;
	SiUndoAction * temp_action;
	while(action!=NULL)
	{
		temp_action=action->next;
		if(action->text!=NULL)
			undo_memory_usage-=(sizeof(SiUndoAction)+action->text->size);
		else
			undo_memory_usage-=(sizeof(SiUndoAction));
		delete action;
		action=temp_action;
	}
}

void SiTextHandler::enable_undo_history()
{
	undo_active=true;
}

void SiTextHandler::disable_undo_history()
{
	undo_active=false;
}
UInt32 SiTextHandler::get_number_words(UInt32 &alphanum_chars)
{
	Boolean in_word=false;
	UInt32 n_words=1;
	block * b;
	Int16 b_b_index;
	BlockInt c;
	Int16 byte_offset;
	WChar the_char;
	alphanum_chars=0;
	for(Int16 n=0;n<m_document->get_number_blocks();++n)
	{
		b=m_document->get_line(n,b_b_index);
		byte_offset=0;
		for(c=0;c<b->n_chars;++c)
		{
			byte_offset+=TxtGetNextChar(b->mem_chunk->mem_ptr,byte_offset,&the_char);
			if(SiUtility::is_break_char(the_char,true))
			{
				if(in_word)
				{
					++n_words;
					in_word=false;
				}
			}
			else
			{
				++alphanum_chars;
				in_word=true;

			}
		}

		m_document->release_line(b_b_index);
	}
	return n_words;
}

//function from Tim Brown
WChar SiTextHandler::get_charAtpos(Position &p)
{

	Int16 b_b_index;
	block * b=m_document->get_line(p.line,b_b_index);
	/*Int16 xpos = p.x;
	if (xpos>0)
		xpos--;*/

	WChar retc= m_document->get_char(p.line,b,p.x);
	m_document->release_line(b_b_index);

	return(retc);
}
WChar SiTextHandler::get_charBeforepos(Position &p)
{

	Int16 b_b_index;
	block * b=m_document->get_line(p.line,b_b_index);

	WChar retc= m_document->get_char(p.line,b,p.x-1);
	m_document->release_line(b_b_index);

	return(retc);
}

#ifdef DEBUG
Boolean SiTextHandler::perform_tests()
{
#ifdef TEST_OBJECT_LOG
	log_entry("SiTextHandler::tests()");
#endif

	const Char* buff="1234567890 1234567890 ";
	Position pos,new_cursor_pos;
	pos.line=0;
	pos.x=0;
	insert_chars(buff,StrLen(buff),StrLen(buff),pos,new_cursor_pos);
	ErrFatalDisplayIf(m_document->get_doc_length()!=StrLen(buff),"SiTextHandler test fail: document byte length");
	ErrFatalDisplayIf(m_document->get_number_chars()!=StrLen(buff),"SiTextHandler test fail: document char length");

#ifdef TEST_OBJECT_LOG

	log_entry("SiTextHandler::tests() 1");
#endif

	ErrFatalDisplayIf(new_cursor_pos.line!=(UInt16)0||new_cursor_pos.x!=StrLen(buff),"SiTextHandler test fail: cursor position");
	Position diff_cursor_pos=new_cursor_pos;
	insert_chars(buff,StrLen(buff),StrLen(buff),new_cursor_pos,diff_cursor_pos);
#ifdef TEST_OBJECT_LOG

	log_entry("SiTextHandler::tests() 2");
#endif

	ErrFatalDisplayIf(diff_cursor_pos.x!=StrLen(buff)||diff_cursor_pos.line!=(UInt16)1,"SiTextHandler test fail : cursor pos line wrap");

	Position start_del(11,0);
	Position end_del(10,1);

	delete_chars(start_del,end_del);
#ifdef TEST_OBJECT_LOG

	log_entry("SiTextHandler::tests() 2.6");



#endif

	initialise();
	Int16 b_b_index;
	ErrFatalDisplayIf(m_document->get_doc_length()!=0,"SiTextHandler test fail:initialise()");
	ErrFatalDisplayIf(m_document->get_line(0,b_b_index)!=NULL,"SiTextHandler test fail: initialise2()");

	#ifdef DEBUG_LOG
	log_entry("SiTextHandler tests complete");
	#endif



	return true;
}
#endif
