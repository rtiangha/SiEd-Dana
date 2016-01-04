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

#include "Window.h"
#include "textview.h"
#include "types.h"
#include "form_pos.h"
#include "error.h"
#include "texthandler.h"
#include "editor.h"
#include <Form.h>
#include "utility.h"
#include <PalmOSGlue.h>
#include "siscrollbar.h"
#define CURSOR_WIDTH 2
#define CURSOR_X_OFFSET 1
#define MINI_BUFFER_RIGHT 69
#define SMALL_FONT_LIMIT 8
#define SCROLLBAR_WIDTH 7


void SiTextView::initialise(SiEditor * editor, SiDocument * doc)
{

	//set up the textview state
#ifdef DEBUG_LOG
	log_entry("SiTextView::initialise");
#endif

	if(text_buff==NULL)
		text_buff = MemHandleNew(40);
	m_waiting_count=0;

	m_tidy_after=false;
	enable_screen_updates();
	small_return_rsc=return_rsc=NULL;
	dirty_state = false;
	cursor_backing_store = 0;
	prev_document_number_blocks=0;
	m_document = doc;
	m_document->set_view(this);
	m_editor = editor;	
	top_line=0;
	rp.topLeft.x = 0;
	rp.topLeft.y = 0;
	rp.extent.x = WIDTH-BORDER;
	rp.extent.y = WIDTH;
//	cursor_location.x = BORDER;
	//cursor_location.line = 0;
	src_rect.extent.x = CURSOR_WIDTH;
	src_rect.extent.y = line_height;
	cont_r.topLeft.x = m_editor->BOTTOM_Y - 8;
	cont_r.topLeft.y = m_editor->BOTTOM_Y - 8;
	cont_r.extent.x = 8;
	cont_r.extent.y = 8;
	focused = true;
	filename_dirty=true;
	cursor_drawn = false;
	prev_frame_selected_position.line=SIGNED_NONE;
	


}

SiTextView::SiTextView(SiEditor * editor, SiDocument * doc)
{
	m_vert_scrollbar = NULL;
	text_buff=NULL;
	scrollbar_visible=false;

	initialise(editor, doc);
}

void SiTextView::notify_font_changed()
{
	//reset variables and redraw the screen after the
	//user changes font
	erase_cursor();
	line_height=FntLineHeight();
	src_rect.extent.x = CURSOR_WIDTH;
	src_rect.extent.y = line_height;
	get_cursor_window();
	if(update_screen_enabled)
	{
		if(m_document->get_number_blocks()>0)
			redraw_display();

		draw_status_dis(true);
	}
}

void SiTextView::clear_screen()
{
	//clear just the text area

  rp.topLeft.x = 0;
  rp.topLeft.y = m_editor->TOP_Y;
  rp.extent.x = WIDTH;
  rp.extent.y = (m_editor->COMMAND_LINE)* line_height;

	WinEraseRectangle(&rp, 0);
	cursor_drawn = false;
}

void SiTextView::clear_self()
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::clear_self");
#endif
	//clear the entire area of the editor
	//including the status display
	rp.topLeft.x = 0;
	rp.topLeft.y = m_editor->TOP_Y;
	rp.extent.x = TOTAL_WIDTH;
	rp.extent.y = m_editor->HEIGHT+1;
	WinEraseRectangle(&rp, 0);
	cursor_drawn = false;
}
void SiTextView::get_cursor_window()
{
//	line_height=FntLineHeight();
	
	if(cursor_backing_store!=NULL)
	{
		WinDeleteWindow(cursor_backing_store, false);
		cursor_backing_store=NULL;
	}

	Err err;
	cursor_backing_store =
		WinCreateOffscreenWindow(CURSOR_WIDTH,line_height*2, screenFormat,&err);

	if (err != errNone)
		DisplayError(SCREEN_ERROR, NULL);
	RectangleType scr;
	scr.topLeft.x=0;
	scr.topLeft.y=m_editor->TOP_Y;
	scr.extent.x=CURSOR_WIDTH;
	scr.extent.y=line_height;
	WinCopyRectangle(NULL,cursor_backing_store,&scr,0,0,winPaint);

}

SiTextView::~SiTextView()
{
	if(text_buff!=NULL)
		MemHandleFree(text_buff);
	text_buff=NULL;
	WinDeleteWindow(cursor_backing_store, false);
	delete m_vert_scrollbar;
}

void SiTextView::initialise_display()
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::initialise_display");
#endif
	//Initialise the display
	//called when ever a new document is opened
	cursor_drawn=false;
	star_pos.topLeft.x = BORDER;
	fontID f=FntSetFont(stdFont);
	star_pos.extent.x = TxtGlueCharWidth((WChar)'*') ;
	star_pos.extent.y = STATUS_BAR_HEIGHT-1;
	FntSetFont(f);
	prev_frame_selected_position.line=SIGNED_NONE;
	prev_document_number_blocks=0;
	screen_size.topLeft.x=0;
	screen_size.topLeft.y=m_editor->TOP_Y;
	screen_size.extent.x=SiUtility::ACTIVE_SCREEN_WIDTH;
	screen_size.extent.y=m_editor->BOTTOM_Y;
	enable_screen_updates();
	m_waiting_count=0;
	line_height = FntLineHeight();
	//cursor_location.line=0;
	//cursor_location.x=0;
	clear_rect.extent.y = line_height;
	src_rect.extent.x=CURSOR_WIDTH;
	src_rect.extent.y=line_height;
	m_prev_read_only=false;
	top_line = 0;
	m_tidy_after=false;
	prev_dis_width = 0;
	clear_self();
	//	draw_status_dis(true);
	draw_status_lines();
	draw_status_controls();
	get_resources();
	prev_frame_selected_position.line=SIGNED_NONE;
	if(m_vert_scrollbar!=NULL)
	{
		delete m_vert_scrollbar;
	}
	m_vert_scrollbar=new SiScrollbar(m_editor,VERTICAL_SCROLL);
	m_vert_scrollbar->resize(SiUtility::SCREEN_WIDTH-SCROLLBAR_WIDTH,m_editor->TOP_Y,SCROLLBAR_WIDTH,m_editor->HEIGHT-STATUS_BAR_HEIGHT-BORDER-1);
#ifdef DEBUG_LOG

	log_entry("SiTextView::initialise done");
#endif
}


DrawPosition SiTextView::find_draw_position(Position pos)
{

	//find the screen position of the position in the text passed in

	//lock line pos.line
	DrawPosition p(BORDER,get_y(pos.line));
	if(m_document->get_number_blocks()==0||pos.line>=m_document->get_number_blocks()||pos.x<=0)
	{
		//p.line=m_document->get_number_blocks();
		return p;
	}

	Int16 b_b_index;

	block * b=m_document->get_line(pos.line,b_b_index);

	p.x+=SiUtility::CorrectCharsWidth_char(b->mem_chunk->mem_ptr,pos.x,NULL);
	//unlock line pos.line (const)
	m_document->release_line(b_b_index);

	return p;
}

void SiTextView::draw_sel_block(const DrawPosition p,const Int16 thick)
{
	//draw the selection background for whitespace characters other than space
	//tabs line feed etc
	// if(p.line>get_y(m_editor->COMMAND_LINE)||p.line<m_editor->TOP_Y)
	//return;


	if(thick>0)
	{
		clear_rect.extent.x = thick;
		clear_rect.extent.y=line_height;
		clear_rect.topLeft.y = p.line;
		clear_rect.topLeft.x = p.x;
		WinPushDrawState();
#ifdef DEBUG_LOG

		log_entry_number("Draw sel block y=",clear_rect.topLeft.y);
		log_entry_number("Draw sel block x=",clear_rect.topLeft.x);
		log_entry_number("Draw sel block ext x=",clear_rect.extent.x);
		log_entry_number("Draw sel block ext y=",clear_rect.extent.y);
#endif

		WinSetForeColor(SiUtility::get_back_color());
		WinPaintRectangle(&clear_rect, 0);
		WinPopDrawState();
	}
}

void SiTextView::clear_sel_block(const DrawPosition p,const Int16 thick)
{

	//clear the selection background for whitespace characters other than space
	//such as tabs, line feed etc
	//if(p.line>get_y(m_editor->COMMAND_LINE)||p.line<m_editor->TOP_Y)
	//return;

	if(thick>0)
	{
		clear_rect.extent.x = thick;
		clear_rect.topLeft.y = p.line;
		clear_rect.topLeft.x = p.x;
		clear_rect.extent.y=line_height;
		WinEraseRectangle(&clear_rect, 0);

		
	}
}


void SiTextView::notify_lost_focus()
{
	//redraw the status display etc after losing focus
	focused = false;
	if(update_screen_enabled)
	{
		erase_cursor();
		draw_status_lines();
	}
}
void SiTextView::redraw_text_view()
{

	if(!update_screen_enabled)
	{
		return;
	}

	clear_screen();

	if(m_document->get_number_blocks()==0)
		return;
	Position start(0,top_line);
	Position end;
	if(m_editor->COMMAND_LINE-1+top_line<m_document->get_number_blocks())
		end.line=m_editor->COMMAND_LINE-1+top_line;
	else
		end.line=m_document->get_number_blocks()-1;

	//lock line end.line
	Int16 b_b_index;

	end.x=m_document->get_line(end.line,b_b_index)->n_chars;
	//unlock line end.line
	m_document->release_line(b_b_index);
	notify_text_changed(start,end);
}

void SiTextView::redraw_text()
{
	Position start(0,top_line);
	Position end(0,m_editor->COMMAND_LINE+top_line+1);
	redraw_between(start,end);
}

void SiTextView::redraw_display(const Boolean scrollbar)
{
	//redraw everything on the screen

	if(!update_screen_enabled)
	{
		return;
	}

	clear_screen();
	FntSetFont(m_editor->get_font());

	draw_status_dis(true);

	if(m_document->get_number_blocks()==0)
		return;


	Position start(0,top_line);
	Position end;
	if(m_editor->COMMAND_LINE-1+top_line<m_document->get_number_blocks())
		end.line=m_editor->COMMAND_LINE-1+top_line;
	else
		end.line=m_document->get_number_blocks()-1;

	//lock line end.line
	Int16 b_b_index;

	end.x=m_document->get_line(end.line,b_b_index)->n_chars;
	//unlock line end.line
	m_document->release_line(b_b_index);
	notify_text_changed(start,end);
	cursor_drawn=false;
	redraw_cursor();

}

void SiTextView::notify_text_changed(Position start,Position end)
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::notify_text_changed");

#endif
#ifdef DEBUG_LOG

	log_entry_number("initial start.line=",start.line);
	log_entry_number("initial start.x=",start.x);

	log_entry_number("initial end.line=",end.line);
	log_entry_number("initial end.x=",end.x);
	log_entry_number("number of blocks=",m_document->get_number_blocks());
#endif
#ifdef DEBUG
	ErrFatalDisplayIf(start.line<0||end.line<0,"SiTextView::drawing off start of document");


#endif

	//draw all the text between the two positions specified
	//taking account of selected text

	if(!update_screen_enabled||start.line>top_line+m_editor->COMMAND_LINE||end.line<top_line||m_document->get_number_blocks()==0)
		return;

	BlockInt actual_nchars;
	Position min_pos,max_pos;

	if(end.line>=m_document->get_number_blocks())
		end=m_document->find_end_document();
	if(start.line>=m_document->get_number_blocks())
	{
		start.line=m_document->get_number_blocks()-1;
		start.x=0;
	}

	if(end.line<top_line)
		return;

	if(start.line<top_line)
	{
		start.line=top_line;
		start.x=0;
	}
	if(end.line>top_line+m_editor->COMMAND_LINE)
	{
		end.line=top_line+m_editor->COMMAND_LINE;
		Int16 b_b_index;
		end.x=m_document->get_line(end.line,b_b_index)->n_chars;
		m_document->release_line(b_b_index);
	}
#ifdef DEBUG
	ErrFatalDisplayIf(start.line<top_line,"start drawing off start of document");
	ErrFatalDisplayIf(end.line<top_line,"end drawing before start of document");
#endif
#ifdef DEBUG_LOG
	log_entry_number("start.line=",start.line);
	log_entry_number("start.x=",start.x);

	log_entry_number("end.line=",end.line);
	log_entry_number("end.x=",end.x);
#endif

	if(start<end)
	{
		min_pos=start;
		max_pos=end;
	}
	else
	{
		min_pos=end;
		max_pos=start;
	}

	//set the screen up ready for text drawing
	FntSetFont(m_editor->get_font());

	WinSetClip(&screen_size);

	DrawPosition draw_pos = find_draw_position(min_pos);

	Boolean done = false;
	Position text_pos;
	text_pos.x=min_pos.x;
	text_pos.line=min_pos.line;

	while ( (!done)
			&& (text_pos.line<m_document->get_number_blocks())
			&& (text_pos.line<top_line+m_editor->COMMAND_LINE ) )
	{

		if (text_pos.line==max_pos.line)
		{
			actual_nchars = max_pos.x;
			done = true;
		}
		else
		{
			actual_nchars =(m_document->find_end_line(text_pos.line)).x;
		}

		draw_block(draw_pos,text_pos,actual_nchars);

		text_pos.x=0;

		++(text_pos.line);
		(draw_pos.line)+=line_height;
		draw_pos.x=BORDER;
	}

	if(m_tidy_after)
		clear_last_lines();
	prev_document_number_blocks=m_document->get_number_blocks();
	m_tidy_after=false;
	WinResetClip();
#ifdef DEBUG_LOG
	log_entry("SiTextView::notify_text_changed done");
#endif
}

void SiTextView::clear_last_lines()
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::clear_last_lines");
#endif

	if(m_document->get_number_blocks()<prev_document_number_blocks&&m_document->get_number_blocks()>0)
		clear_after_line(m_document->get_number_blocks(),true);
}

void SiTextView::draw_block(DrawPosition draw_pos,const Position text_pos,BlockInt max)
{
	//actually draw the specified block
	//boolean clear after flag specifies whether the blank space
	//after the text should be erased or not
#ifdef DEBUG_LOG
	log_entry("SiTextView::draw_block");
#endif

	//don't bother drawing offscreen
	if(!is_onscreen(text_pos.line))
	{
		return;
	}
	BlockInt byte_offset;
	Int16 b_b_index;

	//lock line text_pos.line (const)
	block * curr_block=m_document->get_line(text_pos.line,b_b_index);

	//don't draw an empty block
	if(curr_block->n_chars==0||max==0)
	{
		if(m_tidy_after)
			clear_line(text_pos.line);
		m_document->release_line(b_b_index);
		return;
	}

	if(max>curr_block->n_chars)
		max=curr_block->n_chars;

	Boolean inverted;

	m_document->get_char_bound(text_pos.line,curr_block->mem_chunk->mem_ptr,text_pos.x,&byte_offset);

	DrawPosition p=draw_pos;

	BlockInt prev_char=byte_offset;
	DrawPosition prev_pos=draw_pos;
	WChar the_char;
	Position min_sel_pos=m_editor->get_min_selected_position();
	Position max_sel_pos;
	Int16 curr_width=draw_pos.x;
	BlockInt curr_char_byte=byte_offset;
	//get the selection drawing correct
	if(min_sel_pos.line==SIGNED_NONE)
	{
		inverted=false;
		max_sel_pos.line=SIGNED_NONE;
	}
	else
	{
		max_sel_pos=m_editor->get_max_selected_position();
		if(text_pos<=max_sel_pos&&min_sel_pos<=text_pos)
			inverted=true;
		else
			inverted=false;
	}
	Boolean drawn_eol=false;
	Position curr_pos=text_pos;
	for(BlockInt c=text_pos.x;c<max;++c)
	{
		the_char=TxtGetChar(curr_block->mem_chunk->mem_ptr,byte_offset);

		if(curr_pos==min_sel_pos)
		{
			inverted=true;
			p.x=curr_width+FntCharsWidth(curr_block->mem_chunk->mem_ptr+curr_char_byte,byte_offset-curr_char_byte);
			curr_char_byte=byte_offset;
			if(byte_offset>prev_char)
			{
				WinDrawChars(curr_block->mem_chunk->mem_ptr+prev_char,byte_offset-prev_char,prev_pos.x,prev_pos.line);
			}
			prev_char=byte_offset;
			curr_width=prev_pos.x=p.x;

		}
		if(curr_pos==max_sel_pos)
		{
			inverted=false;
			p.x=curr_width+FntCharsWidth(curr_block->mem_chunk->mem_ptr+curr_char_byte,byte_offset-curr_char_byte);
			curr_char_byte=byte_offset;
			if(byte_offset>prev_char)
			{
				DrawSelectedChars(curr_block->mem_chunk->mem_ptr+prev_char,byte_offset-prev_char,prev_pos.x,prev_pos.line);
			}
			prev_char=byte_offset;
			curr_width=prev_pos.x=p.x;
		}
		++curr_pos.x;

		switch(the_char)
		{
		case chrLineFeed:
		case chrCarriageReturn:
		  p.x=curr_width+FntCharsWidth(curr_block->mem_chunk->mem_ptr+curr_char_byte,byte_offset-curr_char_byte);
		  curr_char_byte=byte_offset+1;
			//end of line so stop
			//prevent the end drawing bit drawing the line feed character
			if(byte_offset>prev_char)
			{
				if(inverted)
					DrawSelectedChars(curr_block->mem_chunk->mem_ptr+prev_char,byte_offset-prev_char,prev_pos.x,prev_pos.line);
				else
					WinDrawChars(curr_block->mem_chunk->mem_ptr+prev_char,byte_offset-prev_char,prev_pos.x,prev_pos.line);
			}


			p.x+=draw_EOL(p,inverted);
			drawn_eol=true;
			prev_char=byte_offset+1;
			curr_width=prev_pos.x=p.x;
			break;
		case chrHorizontalTabulation:
		  p.x=curr_width;
		  if(byte_offset>curr_char_byte)
		    p.x+=FntCharsWidth(curr_block->mem_chunk->mem_ptr+curr_char_byte,(byte_offset-curr_char_byte));
		  curr_char_byte=byte_offset+1;
			if(byte_offset>prev_char)
			{
				if(inverted)
					DrawSelectedChars(curr_block->mem_chunk->mem_ptr+prev_char,byte_offset-prev_char,prev_pos.x,prev_pos.line);
				else
					WinDrawChars(curr_block->mem_chunk->mem_ptr+prev_char,byte_offset-prev_char,prev_pos.x,prev_pos.line);
			}
			prev_char=byte_offset+1;

			prev_pos.x=SiUtility::NextTabStop(p.x)+1;
			if(inverted)
				draw_sel_block(p,prev_pos.x-p.x);
			else
				clear_sel_block(p,prev_pos.x-p.x);

			curr_width=p.x=prev_pos.x;
			
			break;
		
		}
		byte_offset+=TxtCharSize(the_char);
	}

	if(prev_char<byte_offset)
	{
		if(inverted)
			DrawSelectedChars(curr_block->mem_chunk->mem_ptr+prev_char,byte_offset-prev_char,prev_pos.x,prev_pos.line);
		else
			WinDrawChars(curr_block->mem_chunk->mem_ptr+prev_char,byte_offset-prev_char,prev_pos.x,prev_pos.line);


	}
	if(m_tidy_after&&!drawn_eol&&max==curr_block->n_chars&&text_pos.x<curr_block->n_chars&&curr_block->n_chars>0)
	{
	  p.x=curr_width+FntCharsWidth(curr_block->mem_chunk->mem_ptr+curr_char_byte,byte_offset-curr_char_byte);
		clear_line(p);
	}
	//unlock line text_pos.line
	m_document->release_line(b_b_index);

}
void SiTextView::get_resources()
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::get_resources");
#endif

	if(return_rsc==NULL)
		return_rsc = DmGetResource(bitmapRsc,RETURN_KEY_BMP);
	if(small_return_rsc==NULL)
		small_return_rsc=DmGetResource(bitmapRsc,SMALL_RETURN_BMP);
	get_cursor_window();


}
Int16 SiTextView::draw_EOL(DrawPosition p,const Boolean invert)
{
	Int16 ret=0;
	BitmapType* bitmapP=NULL ;
	Int16 y=p.line;

	//set draw position and bitmap correctly based on font size and screen resolution
	if(FntLineHeight()>SMALL_FONT_LIMIT||!SiUtility::has_hires_support())
	{
		if(SiPreferencesDatabase::get_show_codes()&EOL_CODES)
			bitmapP = (BitmapType*) MemHandleLock(return_rsc);
		y+=(FntLineHeight()-7)/2;
		ret=RETURN_BITMAP_WIDTH;
	}
	else
	{
		if(SiPreferencesDatabase::get_show_codes()&EOL_CODES)
			bitmapP = (BitmapType*) MemHandleLock(small_return_rsc);
		y+=(FntLineHeight()-3)/2;
		ret=SMALL_RETURN_BITMAP_WIDTH;
	}

	if(invert)
	{
		if(SiPreferencesDatabase::get_show_codes()&EOL_CODES)
		{
			draw_sel_block(p,ret);

		}
		else
		{

			draw_sel_block(p,CHR_SEL_THICK);
			p.x+=ret=CHR_SEL_THICK;
			ret=CHR_SEL_THICK;
		}

	}
	else
		clear_sel_block(p,CHR_SEL_THICK);

	if(SiPreferencesDatabase::get_show_codes()&EOL_CODES)
	{
		if(bitmapP!=NULL)
		{

			if(!invert)
				clear_sel_block(p,ret);

			WinDrawBitmap(bitmapP,p.x,y);


			if(FntLineHeight()>SMALL_FONT_LIMIT||!SiUtility::has_hires_support())
				MemHandleUnlock(return_rsc);
			else
				MemHandleUnlock(small_return_rsc);
			p.x+=ret;
		}
	}

	if(m_document->has_done_insert())
		clear_line(p);
	return ret;

}
void SiTextView::redraw_selection(Position prev_pos,Position sel_pos)
{
	//redraw the selection text
	//redraws the minimum amount required
	if(m_editor->get_min_selected_position().line!=SIGNED_NONE)
	{
		if(update_screen_enabled)
		{
			notify_text_changed(prev_pos,sel_pos);
		}
	}
}


/*
 * notifications
 */

void SiTextView::notify_selection_cleared()
{
	prev_frame_selected_position.line=SIGNED_NONE;

}

void SiTextView::redraw_between(Position &min_pos,Position &max_pos)
{
	//the editor has changed part of the text, so redraw it
	if(update_screen_enabled)
	{
#ifdef EN_LOG3
		log_entry_number("SiTextView:: redrawing between ",min_pos.line);
		log_entry_number("SiTextView:: and ",max_pos.line);
#endif

		erase_cursor();
		notify_text_changed(min_pos,max_pos);
	}
}
void SiTextView::notify_block_inserted(const Int16 line)
{
	//	if(update_screen_enabled&&line<top_line+m_editor->COMMAND_LINE)

	clear_after_line(line,false);
	update_scrollbar();

}

void SiTextView::clear_after_line(const BlockInt line,Boolean tidy_end)
{
	if(!update_screen_enabled)
		return;
#ifdef DEBUG_LOG

	log_entry_number("SiTextView::clear after line ",line);
#endif
	//	RectangleType clear_rect;



	if(line<=top_line+m_editor->COMMAND_LINE)
	{
		if(tidy_end)
			tidy_end_line(line-1);
		clear_rect.topLeft.x = 0;
		clear_rect.topLeft.y = get_y(line);
		if(clear_rect.topLeft.y<m_editor->TOP_Y)
			clear_rect.topLeft.y=m_editor->TOP_Y;
		clear_rect.extent.y =(prev_document_number_blocks-line)*line_height;

		if(clear_rect.extent.y+clear_rect.topLeft.y>m_editor->BOTTOM_Y)
		{
			clear_rect.extent.y=m_editor->BOTTOM_Y-clear_rect.topLeft.y;
		}

		if(clear_rect.extent.y+clear_rect.topLeft.y>m_editor->TOP_Y)
		{
			clear_rect.extent.x = SiUtility::ACTIVE_SCREEN_WIDTH;
#ifdef DEBUG_LOG

			log_entry_number("clear after: x ",clear_rect.topLeft.x);
			log_entry_number("clear after: y ",clear_rect.topLeft.y);
			log_entry_number("clear after extent:x",clear_rect.extent.x);
			log_entry_number("clear after extent:y",clear_rect.extent.y);
#endif

			WinEraseRectangle(&clear_rect, 0);
		}
		m_tidy_after=false;
	}

	if(src_rect.topLeft.y+src_rect.extent.y>=clear_rect.topLeft.y)
	{
		cursor_drawn=false;
	}


}
void SiTextView::notify_block_deleted(const Int16 line)
{
#ifdef DEBUG_LOG
	log_entry_number("SiTextView:: clearing block ",line);
#endif

	if(!update_screen_enabled||line>top_line+m_editor->COMMAND_LINE)
		return;
	else
	{
#ifdef DEBUG_LOG
		log_entry("SiTextView:: doing clear");
#endif

		clear_line(line);
		m_tidy_after=true;
		update_scrollbar();

	}
}

void SiTextView::notify_text_deleted(const Int16 line,const BlockInt start_x)
{
#ifdef DEBUG_LOG
	log_entry("notify text deleted");

#endif

	if(!update_screen_enabled||!is_onscreen(line))
		return;
	else
	{
#ifdef DEBUG_LOG
		log_entry_number("SiTextView:: clearing deleted text ",line);
#endif

		WinSetClip(&screen_size);
		Int16 b_b_index;
		block * b=m_document->get_line(line,b_b_index);

		if(0==b->n_chars)
		{
			clear_line(line);
		}
		else
		{
			DrawPosition clear_pos(BORDER+SiUtility::CorrectCharsWidth_char(b->mem_chunk->mem_ptr,start_x,NULL),get_y(line));
			clear_line(clear_pos);
		}
		m_document->release_line(b_b_index);
		WinResetClip();
		//		m_tidy_after=false;
	}

}

void SiTextView::make_top_line(BlockInt line,Boolean always)
{
	if(!always&&top_line<m_document->get_number_blocks())
		return;
	else
		notify_scrolled((line-top_line),true,true);
}

void SiTextView::ensure_visible(BlockInt line)
{
	//if this is the top or only view make sure the selected text is visible
	Int16 y_offset=get_y(line+1)-REPLACE_FORM_TOP;
	if(y_offset)
	{
		if(m_editor->TOP_Y==0)
			notify_scrolled((line-top_line-1),true,false);
		else
			notify_scrolled((line-top_line),true,false);
	}

}

void SiTextView::notify_cursor_moved(const Position p, Boolean erase)
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::notify_cursor_moved");
	log_entry_number("cursor_pos.line=",p.line);
	log_entry_number("cursor_pos.x=",p.x);
#endif
	//redraw the cursor in its new position
	if(m_document->get_number_blocks()==0)
	{

		top_line=0;
		draw_cursor(DrawPosition(BORDER,m_editor->TOP_Y));
		draw_status_dis(false);
		update_scrollbar(0);
		return;
	}
	else if(top_line>m_document->get_number_blocks()-1)
		top_line=m_document->get_number_blocks()-1;

	if ((p.line - top_line) >= m_editor->COMMAND_LINE)
	{
		notify_scrolled(p.line - (top_line + m_editor->COMMAND_LINE)+1 ,true);
	}

	else if ((p.line < top_line))
	{
		notify_scrolled(p.line - top_line, true);
	}
	else
	{
		if(update_screen_enabled)
		{
			if (erase)
			{
				erase_cursor();
			}

		}

	}
#ifdef DEBUG
	#ifdef DEBUG_LOG
	log_entry_number("SiTextView cursor move p.line=",p.line);
	log_entry_number("SiTextView cursor move top_line=",top_line);
#endif

	//	ErrFatalDisplayIf(p.line<top_line,"Moving cursor off top of screen");
#endif

	if(update_screen_enabled)
	{
		draw_cursor(find_draw_position(p));
		draw_status_dis(false);
	}
	update_scrollbar(top_line);

}

void SiTextView::erase_cursor()
{
	//erase the cursor by painting the contents of the cursor backing
	//store over its current position

	if(!cursor_drawn)
		return;
	Coord topLeft_destx, topLeft_desty;
	topLeft_destx = src_rect.topLeft.x;
	topLeft_desty = src_rect.topLeft.y;//get_y(cursor_location.line);
	RectangleType copy_rect;
	copy_rect.topLeft.x = 0;
	copy_rect.topLeft.y = 0;
	copy_rect.extent.x =CURSOR_WIDTH;
	copy_rect.extent.y = line_height;

	//copy the background of the cursor
	//over the current cursor position
	WinCopyRectangle(cursor_backing_store, NULL, &copy_rect, topLeft_destx,
					 topLeft_desty, winPaint);


	cursor_drawn = false;
}

void SiTextView::redraw_cursor()
{

	
	if(src_rect.topLeft.y>=m_editor->TOP_Y&&src_rect.topLeft.y<=m_editor->BOTTOM_Y)
	{
		WinPushDrawState();
		if(SiUtility::has_colour_support())
			WinSetForeColor(SiUtility::get_cursor_color());
		WinSetDrawMode(winSwap);
		
		WinPaintRectangle(&src_rect,0);
		cursor_drawn = true;
		WinPopDrawState();
	}
}
void SiTextView::update_scrollbar()
{
	update_scrollbar(top_line);
}

void SiTextView::update_scrollbar(const Int16 pos)
{

#ifdef DEBUG_LOG
	log_entry("SiTextView::update_scrollbar");
#endif

	//set the scrollbar to the current cursor position
	//and document size
	if(!update_screen_enabled||m_vert_scrollbar==NULL)
		return;

	if(m_document->get_number_blocks()<=m_editor->COMMAND_LINE&&top_line!=0)
	{
		make_top_line(0,true);
	}
	Int16 current_pos = pos;
	Int16 max_pos;

	if(m_document->get_apparent_number_blocks()>m_editor->COMMAND_LINE)
		max_pos=m_document->get_apparent_number_blocks();
	else
		max_pos=0;

	if(current_pos>max_pos)
		current_pos=max_pos;

#ifdef DEBUG_LOG

	log_entry_number("SiTextView::initial Setting scrollbar position ",current_pos);
	log_entry_number("SiTextView::initial Setting scrollbar max position ",max_pos);
#endif
#ifdef DEBUG

	ErrFatalDisplayIf(max_pos<current_pos,"Setting scrollbar off end of bar");
#endif

	m_vert_scrollbar->set
	(current_pos,max_pos,m_editor->COMMAND_LINE);
	/*
	#ifdef DEBUG_LOG
		log_entry("SiTextView::update_scrollbar");
	#endif
	 
		//set the scrollbar to the current cursor position
		//and document size
		if(!update_screen_enabled||m_scrollbar==NULL)
			return;
	 
		if(m_document->get_number_blocks()<=m_editor->COMMAND_LINE)
		{
			if(scrollbar_visible)
			{
	#ifdef DEBUG_LOG
				log_entry("SiTextView::Hiding scrollbar");
	#endif
	 
				FormPtr frm=FrmGetFormPtr(ResformID_text);
				FrmHideObject(frm,FrmGetObjectIndex(frm,m_scroll_index));
				scrollbar_visible=false;
				make_top_line(0,true);
				
			}
			
		}
		else if(m_document->get_number_blocks()>m_editor->COMMAND_LINE&&!scrollbar_visible)
		{
	#ifdef DEBUG_LOG
			log_entry("SiTextView::Showing scrollbar");
	#endif
	 
			FormPtr frm=FrmGetFormPtr(ResformID_text);
			FrmShowObject(frm,FrmGetObjectIndex(frm,m_scroll_index));
			scrollbar_visible=true;
		}
		if(scrollbar_visible)
		{
			Int16 current_pos = pos;
			Int16 max_pos;
	//	Int16 max, min, page;
		
			if(m_document->get_apparent_number_blocks()>m_editor->COMMAND_LINE)
				max_pos=m_document->get_apparent_number_blocks()-m_editor->COMMAND_LINE;
			else
				max_pos=0;
	 
			if(current_pos>max_pos)
				current_pos=max_pos;
		#ifdef DEBUG_LOG
			log_entry_number("SiTextView::initial Setting scrollbar position ",current_pos);
			log_entry_number("SiTextView::initial Setting scrollbar max position ",max_pos);
		#endif
		#ifdef DEBUG
			ErrFatalDisplayIf(max_pos<current_pos,"Setting scrollbar off end of bar");
		#endif
			SclSetScrollBar(m_scrollbar, current_pos, 0,max_pos,m_editor->COMMAND_LINE);
		}*/
}


void SiTextView::quick_scroll_display(Int16 n_lines)
{
	//scroll the display using the backing store in order
	//to avoid calling text redraw routines
	//only called when scrolling < height of editor lines
	if (n_lines == 0)
		return;

	Coord distance;
	WinDirectionType direction;
	//	RectangleType rp;
	rp.extent.y=m_editor->COMMAND_LINE*line_height;

	if(n_lines<0)
	{
		rp.topLeft.y=m_editor->TOP_Y;//-(n_lines*line_height);
		distance=(-n_lines)*line_height;
		direction=winDown;
	}
	else
	{
		rp.topLeft.y=m_editor->TOP_Y;
		distance=n_lines*line_height;
		direction=winUp;
	}
	rp.topLeft.x=0;
	rp.extent.x=ACTIVEWIDTH+1;
	RectangleType clear;

	//scroll the part of the screen that will be unchanged
	WinScrollRectangle(&rp,direction,distance,&clear);
	WinEraseRectangle(&clear,0);

	Position start(0,0);
	Position end(0,0);

	//then redraw the rest
	if (n_lines > 0)
	{
		//moving downwards
		start.line=m_editor->COMMAND_LINE - n_lines - 1 +top_line;
		if(start.line>=m_document->get_number_blocks())
			start.line=m_document->get_number_blocks()-1;
		end=get_last_redraw_position();
	}
	else if(n_lines<0)
	{
		//moving updwards
		start.line=top_line;
		if(start.line>=m_document->get_number_blocks())
			start.line=m_document->get_number_blocks()-1;

		end.line=top_line - n_lines;

		if(end.line>=m_document->get_number_blocks())
			end.line=m_document->get_number_blocks()-1;

		Int16 b_b_index2;
		end.x=m_document->get_line(end.line,b_b_index2)->n_chars;
		m_document->release_line(b_b_index2);

	}
	//lock line end.line
	WinResetClip();
	notify_text_changed(start,end);
}
Position SiTextView::get_last_redraw_position()
{
	if(m_document->get_number_blocks()==0)
		return Position(0,0);
	Position end;

	if(m_document->get_number_blocks()>top_line+m_editor->COMMAND_LINE)
	{
		end.line=top_line+m_editor->COMMAND_LINE;
	}
	else
		end.line=m_document->get_number_blocks()-1;

	Int16 b_b_index;
	end.x=m_document->get_line(end.line,b_b_index)->n_chars;
	m_document->release_line(b_b_index);
	return end;
}

Boolean SiTextView::notify_scrolled(Int16 a, Boolean scroll,Boolean quick)
{
	//! The cursor has moved off the screen or the user has moved the scroll bar (if scroll argument is false)
	//! Work out where to go and redraw the screen
#ifdef DEBUG_LOG
	log_entry("SiTextView::notify scrolled");
	log_entry_number("SiTextView::notify scrolled amount ",a);
#endif

	if ((top_line == 0 && a < 0))
		return false;
	if(quick)
		erase_cursor();
	//get rid of the cursor if the scrolling is going to be done by copying


	Int16 orig_top_line=top_line;
	top_line += a;

	if(top_line>m_document->get_number_blocks()-m_editor->COMMAND_LINE)
		top_line=m_document->get_number_blocks()-m_editor->COMMAND_LINE;

	if (top_line < 0)
	{
		a+=top_line;
		top_line = 0;
	}

	if(update_screen_enabled)
	{
		if(orig_top_line!=top_line)
		{
			if(abs(a)<m_editor->COMMAND_LINE&&quick)
			{
				quick_scroll_display(top_line-orig_top_line);
			}
			else
			{
				clear_screen();
				Position start(0,top_line);

				Position end=get_last_redraw_position();
				redraw_between(start,end);
			}
		}
	}
#ifdef DEBUG_LOG
	log_entry("SiTextView::notify scrolled 2");
#endif

	if (scroll)
	{
		update_scrollbar();
		if(update_screen_enabled)
			draw_status_dis(false);
	}
	else
		redraw_cursor();
#ifdef DEBUG_LOG

	log_entry("SiTextView::notify scrolled 3");
#endif

	return true;
}

void SiTextView::draw_cursor(DrawPosition p)
{
	//! Copy the current background to the passed in cursor position
	//! and then xor the cursor over the position.
	//! Save the position afterwards
	if (cursor_drawn||!focused)
		return;


	src_rect.topLeft.x = p.x-CURSOR_X_OFFSET;
	src_rect.topLeft.y = p.line;
	

	//copy the current background of the cursor
	WinCopyRectangle(NULL, cursor_backing_store, &src_rect,(Coord)0,(Coord)0, winPaint);

	//then draw the cursor on top
	redraw_cursor();

}

void SiTextView::set_filename_dirty()
{
	//the filename has changed since it was last drawn
	filename_dirty = true;
}
void SiTextView::draw_status_lines()
{
	IndexedColorType prev_color=0;
	if(SiUtility::has_colour_support())
		prev_color=WinSetForeColor(SiUtility::get_line_color());

	if (focused)
	{
		WinDrawLine(0,m_editor->HEIGHT+m_editor->TOP_Y-1,SiUtility::SCREEN_WIDTH,m_editor->HEIGHT+m_editor->TOP_Y-1);
		WinDrawLine(0,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2,SiUtility::SCREEN_WIDTH,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2);
	}
	else
	{
		WinEraseLine(0,m_editor->HEIGHT+m_editor->TOP_Y-1, TOTAL_WIDTH,m_editor->HEIGHT+m_editor->TOP_Y-1);
		WinEraseLine(0,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2, SiUtility::SCREEN_WIDTH,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2);
		WinDrawGrayLine(0,m_editor->HEIGHT+m_editor->TOP_Y-1,SiUtility::SCREEN_WIDTH,m_editor->HEIGHT+m_editor->TOP_Y-1);
		WinDrawGrayLine(0,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2, SiUtility::SCREEN_WIDTH,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2);
	}
	if(SiUtility::has_colour_support())
		WinSetForeColor(prev_color);
}

Boolean SiTextView::point_in_file_name(Coord x,Coord y)
{

	if((y>(m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT))&&(y<(m_editor->HEIGHT+m_editor->TOP_Y)))
		return ((x>0)&&(x<star_pos.topLeft.x));
	else
		return false;
}

void SiTextView::draw_status_filename()
{

	if(!update_screen_enabled)
		return;
	UInt16 next_width = BORDER;
	Int16 d_height=(STATUS_BAR_HEIGHT-FntLineHeight())/2;
	rp.topLeft.x = BORDER;
	rp.topLeft.y =m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-1;

	rp.extent.x = FILENAME_WIDTH+9;
	rp.extent.y = STATUS_BAR_HEIGHT-1;
	WinEraseRectangle(&rp, 0);
	Char *f = m_editor->get_filename();
	if(f==NULL)
	{
		f=const_cast<Char*>(TAP_TO_OPEN);
	}
	UInt16 len= StrLen(f);
	IndexedColorType prev_color=0;
	if(SiUtility::has_colour_support())
		prev_color=WinSetTextColor(SiUtility::get_status_text_color());

	WinDrawTruncChars(f,len, BORDER,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-1+d_height, FILENAME_WIDTH);

	if(SiUtility::has_colour_support())
		WinSetTextColor(prev_color);

	next_width = FntCharsWidth(f, len) + BORDER;

	if (next_width > FILENAME_WIDTH)
		next_width = FILENAME_WIDTH;

	star_pos.topLeft.x = next_width+1;
	star_pos.topLeft.y = m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT+d_height-1;
	if(m_editor->m_index==0&&m_editor->HEIGHT<SiUtility::CurrentScreenHeight)
	{
		MemHandle hRsc=DmGetResource(bitmapRsc,DRAG_ARROW_BITMAP);

		if(hRsc!=NULL)
		{
			BitmapType* bitmapP=(BitmapType*)MemHandleLock(hRsc);
			WinDrawBitmap(bitmapP,MacPopupRight-8,get_status_y());
			MemHandleUnlock(hRsc);
			DmReleaseResource(hRsc);
		}
	}
}

void SiTextView::draw_status_line_number()
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::draw_status_line_number()");
#endif

	if(text_buff!=NULL)
	{
		fontID prev_font=FntSetFont(stdFont);

		Int16 d_height=(STATUS_BAR_HEIGHT-FntLineHeight())/2;
		char *text = (char *) MemHandleLock(text_buff);

		status_cursor_line = m_editor->get_cursor_line();
		status_last_line=m_document->get_apparent_number_blocks();

		StrPrintF(text, "%i/%i", m_editor->get_cursor_line()+1,m_document->get_apparent_number_blocks());
		Int16 len = StrLen(text);
		Int16 width = FntCharsWidth(text, len);
		if(width>MAX_LINE_DISPLAY_WIDTH)
			width=MAX_LINE_DISPLAY_WIDTH;

		if (prev_dis_width > width)
		{
			rp.topLeft.x =LINE_DISPLAY_POS;
			rp.topLeft.y =
				m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-1;
			rp.extent.x = prev_dis_width;
			rp.extent.y = FntCharHeight();
			WinEraseRectangle(&rp, 0);
		}
		WinEraseLine(LINE_DISPLAY_POS+prev_dis_width,m_editor->HEIGHT+m_editor->TOP_Y-2,LINE_DISPLAY_POS+prev_dis_width,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-1);
		prev_dis_width = width;

		IndexedColorType prev_color=0;
		if(SiUtility::has_colour_support())
			prev_color=WinSetTextColor(SiUtility::get_status_text_color());
		WinDrawTruncChars(text, len, LINE_DISPLAY_POS,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-1+d_height,MAX_LINE_DISPLAY_WIDTH);

		if(SiUtility::has_colour_support())
		{
			WinSetTextColor(prev_color);
			prev_color=WinSetForeColor(SiUtility::get_line_color());
		}
		
		WinDrawLine(MacPopupRight,m_editor->HEIGHT+m_editor->TOP_Y-1,MacPopupRight,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2);
		if(width<MAX_LINE_DISPLAY_WIDTH)
		{
			WinDrawLine(LINE_DISPLAY_POS+width,m_editor->HEIGHT+m_editor->TOP_Y-1,LINE_DISPLAY_POS+width,m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2);
		}
		if(SiUtility::has_colour_support())
			WinSetForeColor(prev_color);
		MemHandleUnlock(text_buff);
		FntSetFont(prev_font);

	}

}

void SiTextView::draw_status_controls()
{
	if(m_editor->is_displaying_dialog())
		return;

	FormPtr frm=FrmGetFormPtr(ResformID_text);


	ControlType * ctl=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,PopupTriggerButton));
	CtlDrawControl(ctl);
	ctl=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,CopyButton));
	CtlDrawControl(ctl);
	ctl=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,PasteButton));
	CtlDrawControl(ctl);
	//IndexedColorType prev_color=0;
	/*if(m_editor->m_index==1||m_editor->m_index==0&&SiUtility::CurrentScreenHeight==m_editor->HEIGHT)
	{
		return;
		if(SiUtility::has_colour_support())
			prev_color=WinSetForeColor(SiUtility::get_line_color());
		WinPaintLine(MacPopupLeft-1,MacPopupTop,MacPopupLeft-1,MacPopupBottom);
		WinPaintLine(MacPopupRight,MacPopupTop,MacPopupRight,MacPopupBottom);
		if(SiUtility::has_colour_support())
			WinSetForeColor(prev_color);
	}*/



}

void SiTextView::draw_status_asterisk()
{

#ifdef DEBUG_LOG
	log_entry("SiTextView::draw_status_asterisk()");
#endif

	if(!update_screen_enabled)
		return;
	if (m_document->file_dirty())
	{
		IndexedColorType prev_color=0;
		if(SiUtility::has_colour_support())
			prev_color=WinSetTextColor(SiUtility::get_status_text_color());
		WinDrawChar('*', star_pos.topLeft.x, star_pos.topLeft.y);
		if(SiUtility::has_colour_support())
			WinSetTextColor(prev_color);
	}
	else
	{
		WinEraseRectangle(&star_pos, 0);
	}
	dirty_state=m_document->file_dirty();
}
void SiTextView::redraw_chrome(Boolean force)
{
	update_scrollbar();
	draw_status_dis(force);
	m_vert_scrollbar->draw_self(true);
}

#define READ_ONLY_X 149
#define READ_ONLY_ABBREV "RO"

void SiTextView::draw_status_read_only()
{
	if(m_editor->get_read_only()==m_prev_read_only)
		return;
	m_prev_read_only=m_editor->get_read_only();

	if(m_editor->get_read_only())
	{
		IndexedColorType prev_color=0;
		FontID prev_font=FntSetFont(stdFont);
		if(SiUtility::has_colour_support())
			prev_color=WinSetTextColor(SiUtility::get_status_text_color());
		WinDrawChars(READ_ONLY_ABBREV,StrLen(READ_ONLY_ABBREV),READ_ONLY_X,get_status_y());
		if(SiUtility::has_colour_support())
			WinSetTextColor(prev_color);

		FntSetFont(prev_font);
	}
	else
	{
		FontID prev_font=FntSetFont(stdFont);
		WinEraseChars(READ_ONLY_ABBREV,StrLen(READ_ONLY_ABBREV),READ_ONLY_X,get_status_y());
		FntSetFont(prev_font);
	}
}
void SiTextView::draw_status_dis(Boolean force)
{
	// draws the little display at the bottom of the screen
	// uses the dirty flags to minimise the amount of drawing done
#ifdef DEBUG_LOG
	log_entry("SiTextView::draw_status_dis(");
#endif

	if(!update_screen_enabled)
		return;
	fontID curr_font=FntSetFont(stdFont);

	if(force)
	{
		draw_status_lines();
		draw_status_read_only();
	}

	if (status_cursor_line != m_editor->get_cursor_line() || force || status_last_line!=m_document->get_number_blocks())
		draw_status_line_number();

	if(filename_dirty||force)
		draw_status_filename();

	if ((m_document->file_dirty() != dirty_state)||filename_dirty||force)
		draw_status_asterisk();

	filename_dirty=false;

	if(force)
		draw_status_controls();

	FntSetFont(curr_font);

}

void SiTextView::notify_got_focus()
{
	//the user has selected this editor, so redraw the status display appropriately
	focused = true;
	if(update_screen_enabled)
	{
		draw_status_lines();
		redraw_cursor();
	}

	update_scrollbar();
}

/*void SiTextView::set_scrollbar(ScrollBarType * sb,UInt16 index)
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::Setting scrollbar");
#endif
	//pass in the scrollbar pointer
	m_scrollbar = sb;
	m_scroll_index=index;
	scrollbar_visible=false;
	update_scrollbar();
	//	FormPtr frm=FrmGetFormPtr(ResformID_text);
	//	FrmShowObject(frm,FrmGetObjectIndex(frm,m_scroll_index));
}*/

void SiTextView::notify_saved()
{
	//redraw the status display after saving
	filename_dirty = true;
	if(update_screen_enabled)
		draw_status_dis(false);
}

Position SiTextView::find_position(const Int16 x,const Int16 y)
{
	//convert the screen location passed in into the position in the text
	//used for handling taps on the screen etc
#ifdef DEBUG_LOG
	log_entry("SiTextView::find_position");
	ErrFatalDisplayIf(y<m_editor->TOP_Y,"SiTextView:: finding position off screen");
#endif

	Position p(0,0);
	p.line=(y-m_editor->TOP_Y)/line_height+top_line;

	if(p.line>=m_document->get_number_blocks()||p.line<0)
	{
		if(m_document->get_number_blocks()>0)
			p.line=m_document->get_number_blocks()-1;
		else
		{
			//return a zero position
			return p;
		}
	}
	if(y==0)
		return p;
	//lock line p.line
	Int16 b_b_index;
	block * curr_block=m_document->get_line(p.line,b_b_index);
	Int16 curr_width=0;
	Int16 byte_offset=0;
	WChar the_char;
	p.x=0;

	Int16 this_char_width;
	while(p.x<curr_block->n_chars)
	{
		byte_offset+=TxtGetNextChar(curr_block->mem_chunk->mem_ptr,byte_offset,&the_char);
		this_char_width=CorrectCharWidth(the_char,curr_width);
		if(curr_width>=x)
			break;
		else if(x>=curr_width&&x<=curr_width+this_char_width/2)
			break;

		++p.x;
		curr_width+=this_char_width;
	}

	//unlock line p.line

	m_document->release_line(b_b_index);

	m_document->check_newline_position(p,-1);

	return p;
}

Position SiTextView::find_vertical_cursor_position(Int16 line)
{
	//find the position on the line passed in that the cursor should be moved to
	//when the up/down/page up/page down cursor keys are pressed
	if(m_document->get_number_blocks()==0)
		return Position(0,0);

	Int16 b_b_index;
	if(line>=m_document->get_number_blocks())
	{
		block * b=m_document->get_line(m_document->get_number_blocks()-1,b_b_index);
		if(b->n_chars==0)
		{
			m_document->release_line(b_b_index);
			return Position(0,m_document->get_number_blocks()-1);
		}
		/*if(m_document->get_char(line,b,b->n_chars-1)==chrLineFeed)
		{
			m_document->release_line(b_b_index);
			return Position(0,m_document->get_number_blocks());
		}*/
		else
		{
			m_document->release_line(b_b_index);
			line=m_document->get_number_blocks()-1;
		}


	}
	else if(line<0)
		line=0;

	block * b=m_document->get_line(line,b_b_index);
	Int16 curr_width=0;
	Int16 byte_offset=0;
	WChar the_char;
	Position p(0,line);
	while(curr_width<=src_rect.topLeft.x&&p.x<b->n_chars)
	{
		byte_offset+=TxtGetNextChar(b->mem_chunk->mem_ptr,byte_offset,&the_char);
		curr_width+=CorrectCharWidth(the_char,curr_width);

		if(curr_width<=src_rect.topLeft.x)
			++p.x;
	}


	//unlock line p.line
	m_document->release_line(b_b_index);

	return p;
}

void SiTextView::notify_size_changed()
{
	//called when the view is split or unsplit

	screen_size.topLeft.x=BORDER;
	screen_size.topLeft.y=m_editor->TOP_Y;
	screen_size.extent.x=SiUtility::ACTIVE_SCREEN_WIDTH-BORDER;
	screen_size.extent.y=m_editor->BOTTOM_Y-m_editor->TOP_Y;

	if(update_screen_enabled)
	{
		clear_self();
		redraw_display();
		draw_status_dis(true);
	}
	//update_scrollbar(0,false);
	m_vert_scrollbar->resize(SiUtility::SCREEN_WIDTH-SCROLLBAR_WIDTH,m_editor->TOP_Y,SCROLLBAR_WIDTH,m_editor->HEIGHT-STATUS_BAR_HEIGHT-1);
}

/*Int16 SiTextView::get_c_line(const Int16 offset) const
{
	return (cursor_location.line-top_line)+m_editor->TOP_Y+offset*line_height;
}
 
Int16 SiTextView::get_c_x() const
{
	return cursor_location.x;
}*/

Int16 SiTextView::get_top_line()
{
	return top_line;
}

/*Int16 SiTextView::get_line_height()
{
	return line_height;
}*/

void SiTextView::enable_screen_updates()
{
	update_screen_enabled=true;

}

void SiTextView::disable_screen_updates()
{
	update_screen_enabled=false;
}
#define BUTTON_LEFT 137
Boolean SiTextView::point_in_drag_area(const Int16 x,const Int16 y)
{

	if(y>=get_status_y()&&y<=m_editor->HEIGHT+m_editor->TOP_Y)
	{

		if(x>=BUTTON_LEFT&&x<=SiUtility::SCREEN_WIDTH)
		{

		    return true;
		}
		else if(m_document->file_dirty())
		  {
		    if((x>star_pos.topLeft.x&&(x<MacPopupRight)))
			return true;
		  }
		else
		  {
		     if((x>(star_pos.topLeft.x+star_pos.extent.x)&&(x<MacPopupRight)))
			return true;
		  }
	}
	return false;
}
Int16 SiTextView::get_line_height() const
{
	return line_height;
}
Boolean SiTextView::point_in_save_star(Coord x,Coord y)
{
	return RctPtInRectangle(x,y,&star_pos);
}

Int16 SiTextView::point_in_line_number(Coord x,Coord y)
{

	if(x>=LINE_DISPLAY_POS&&x<=LINE_DISPLAY_POS+prev_dis_width)
	{
		//Int16 d_height=FntLineHeight();
		if(y<=m_editor->HEIGHT+m_editor->TOP_Y-1&&y>=m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-2)
		{
			FormPtr frm=FrmGetFormPtr(ResformID_text);
			ListType * lst;
			if(m_editor->TOP_Y==0&&m_editor->HEIGHT<SiUtility::CurrentScreenHeight)
			{
				lst=(ListType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,PopupPositionListTop));
			}
			else
			{
				lst=(ListType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,PopupPositionListBottom));
			}
			Int16 curr_selection=(1+(top_line+m_editor->COMMAND_LINE)*PERCENTAGE_LIST_LENGTH)/m_document->get_apparent_number_blocks();

			//just make absolutely sure we don't end up with an invalid list selection
			if(m_document->get_apparent_number_blocks()<m_editor->COMMAND_LINE)
				curr_selection=0;
			else if(curr_selection>PERCENTAGE_LIST_LENGTH)
				curr_selection=PERCENTAGE_LIST_LENGTH;
			LstSetSelection(lst,curr_selection);
			return LstPopupList(lst);
		}
	}
	return noListSelection;
}

Int16 SiTextView::get_y(Int16 text_line)
{
	return (text_line-top_line)*line_height+m_editor->TOP_Y;
}

void SiTextView::tidy_end_line(Int16 line)
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::tidy_end_line");
#endif
	//erase the rectangle to the right of the text on the passed in line
	if(m_document->get_number_blocks()==0)
	{
		clear_line(0);
		return;
	}

	//don't bother if the line doesn't exist or is offscreen
	if(line<0||line>=m_document->get_number_blocks()||!is_onscreen(line))
		return;
	Int16 index;
	DrawPosition draw_pos;
	draw_pos.line=(line-top_line)*line_height+m_editor->TOP_Y;
	block * b=m_document->get_line(line,index);
	if(b->n_chars==0)
		draw_pos.x=0;
	else
		draw_pos.x=BORDER+SiUtility::CorrectCharsWidth_char(b->mem_chunk->mem_ptr,b->n_chars,NULL,SiPreferencesDatabase::get_show_codes());

	clear_line(draw_pos);

	m_document->release_line(index);
}

void SiTextView::tidy_start_line(Int16 line)
{
#ifdef DEBUG_LOG
	log_entry("SiTextView::tidy_end_line");
#endif

	if(line<0||line>=m_document->get_number_blocks())
		return;
	Int16 index;
	DrawPosition draw_pos;
	draw_pos.line=get_y(line);//(line-top_line)*line_height+m_editor->TOP_Y;
	block * b=m_document->get_line(line,index);

	draw_pos.x=0;
	Int16 max=BORDER+SiUtility::CorrectCharsWidth_char(b->mem_chunk->mem_ptr,b->n_chars,NULL,SiPreferencesDatabase::get_show_codes());

	clear_line(draw_pos,max);

	m_document->release_line(index);
}
void SiTextView::clear_line(Int16 line)
{

	//clear the line between the Position p and the RHS of the screen
#ifdef DEBUG_LOG
	log_entry_number("SiTextView::clear line ",line);
#endif

	if(is_onscreen(line))
	{

		clear_rect.topLeft.y = get_y(line);


		clear_rect.topLeft.x = 0;
		clear_rect.extent.x = SiUtility::ACTIVE_SCREEN_WIDTH+1;
		clear_rect.extent.y = line_height;

		WinEraseRectangle(&clear_rect, 0);
	}

}

void SiTextView::clear_line(const DrawPosition p)
{

	//clear the line between the Position p and the RHS of the screen
	if(p.line>(m_editor->BOTTOM_Y-line_height)||p.line<m_editor->TOP_Y)
		return;
	clear_rect.topLeft.y = p.line;
	clear_rect.topLeft.x = p.x;
	clear_rect.extent.x = SiUtility::ACTIVE_SCREEN_WIDTH - p.x+1;
	clear_rect.extent.y = line_height;
	if (clear_rect.extent.x > 0)
		WinEraseRectangle(&clear_rect, 0);
}

void SiTextView::clear_line(const DrawPosition p,const Int16 right)
{

	//clear the text line between the Position p and the x position passed in
	if((p.line>m_editor->BOTTOM_Y-line_height)||(p.line<m_editor->TOP_Y))
		return;
	clear_rect.extent.x = right - p.x;
	if(right - p.x>=0)
	{
		clear_rect.topLeft.y = p.line;
		clear_rect.topLeft.x = p.x;
		WinEraseRectangle(&clear_rect, 0);
	}
}

Boolean SiTextView::is_onscreen(const Int16 line) const
{
	if(line>=top_line&&line<m_editor->COMMAND_LINE+top_line)
		return true;
	else
		return false;
}

void SiTextView::DrawSelectedChars(const Char * chars,Int16 len,Coord x,Coord y)
{
	WinPushDrawState();

	WinSetBackColor(SiUtility::get_back_color());
	WinSetTextColor(SiUtility::get_fore_color());

	WinDrawChars(chars,len,x,y);
	WinPopDrawState();

}

void SiTextView::start_waiting()
{
	if(0==m_waiting_count)
		draw_waiting();
	++m_waiting_count;
}

void SiTextView::stop_waiting()
{

	if(m_waiting_count>0)
	{
		--m_waiting_count;
		if(0==m_waiting_count)
		{
			fontID f=FntSetFont(stdFont);
			done_waiting();
			draw_status_filename();
			draw_status_asterisk();
			FntSetFont(f);
		}
	}


}

Int16 SiTextView::get_status_y()
{

	Int16 y=m_editor->HEIGHT+m_editor->TOP_Y-STATUS_BAR_HEIGHT-1;

	return y;
}
void SiTextView::draw_waiting()
{
	RectangleType bounds;
	FormPtr prev_form=FrmGetActiveForm();
	FormPtr main_form=FrmGetFormPtr(ResformID_text);
	FrmSetActiveForm(main_form);
	FrmGetFormBounds(main_form,&bounds);

	fontID curr_font=FntSetFont(stdFont);
	//	Int16 text_width=FntCharsWidth(WAITING_TEXT,StrLen(WAITING_TEXT));
	Int16 text_height=(STATUS_BAR_HEIGHT-FntLineHeight())/2;
	//	RectangleType rp;
	rp.topLeft.x=0;
	rp.topLeft.y=get_status_y();
	rp.extent.x=MacPopupLeft-1;
	rp.extent.y=STATUS_BAR_HEIGHT;
	WinDrawRectangle(&rp,0);
	WinDrawInvertedChars(WAITING_TEXT,StrLen(WAITING_TEXT),WAITING_X,get_status_y()+text_height);
	FntSetFont(curr_font);
	FrmSetActiveForm(prev_form);
	//FrmDrawForm(FrmGetActiveForm());
}
void SiTextView::done_waiting()
{
	RectangleType bounds;
	FormPtr prev_form=FrmGetActiveForm();
	FormPtr main_form=FrmGetFormPtr(ResformID_text);
	FrmSetActiveForm(main_form);
	FrmGetFormBounds(main_form,&bounds);

	//	Int16 text_width=FntCharsWidth(WAITING_TEXT,14);


	//	RectangleType rp;
	rp.topLeft.x=0;
	rp.topLeft.y=get_status_y();
	rp.extent.x=MacPopupLeft-1;
	rp.extent.y=STATUS_BAR_HEIGHT;
	WinEraseRectangle(&rp,0);
	FrmSetActiveForm(prev_form);
	//	emit_redraw_signal(NULL);
	//FrmDrawForm(FrmGetActiveForm());
}
SiScrollbar * SiTextView::get_scrollbar()
{
	return m_vert_scrollbar;
}
UInt16 SiTextView::get_line_multiple_size(const UInt16 y)
{
#ifdef DEBUG
	ErrFatalDisplayIf(y+m_editor->TOP_Y>SiUtility::CurrentScreenHeight,"get_line_multiple_size off screen");
#endif

	if(m_editor->TOP_Y+y==SiUtility::CurrentScreenHeight)
		return y;
	else
	  {
	    Int16 h=STATUS_BAR_HEIGHT+(line_height*((y-STATUS_BAR_HEIGHT)/line_height));
	    if(h<m_editor->get_min_height())
	      h=m_editor->get_min_height()+1;
	    return h;
	  }
				       

}
#ifdef TEST_OBJECT_UNIT
void SiTextView::perform_tests()
{
	m_vert_scrollbar->perform_tests();
}
#endif
