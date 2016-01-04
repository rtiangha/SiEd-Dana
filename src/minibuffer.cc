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

#include "minibuffer.h"
#include "error.h"
#include <UI/Form.h>
#include "utility.h"
#define MAX_FIND_STRING_LENGTH 51

SiMiniBuffer::SiMiniBuffer(Int16 x,Int16 y,Int16 max_width)
{
	fontID font=FntSetFont(stdFont);
	m_d_height=FntLineHeight();
	m_x=x;
	m_y=y-m_d_height;
	m_max_width=max_width;
	m_title=NULL;
	m_title_width=NULL;
	m_backing_store=NULL;
	m_curr_text_width=0;
	m_x_offset=0;
	m_text_handle=NULL;

	FntSetFont(font);

	copy_background();

}

SiMiniBuffer::~SiMiniBuffer()
{
	restore_background();
	if(m_title!=NULL)
	{
		MemPtrFree(m_title);
		m_title=NULL;
	}
	if(m_text_handle!=NULL)
	{
		MemHandleFree(m_text_handle);
		m_text_handle=NULL;
	}
}

void SiMiniBuffer::copy_background()
{
	RectangleType rp;
	rp.topLeft.x = m_x;
	rp.topLeft.y = m_y;
	rp.extent.x = m_max_width;
	rp.extent.y = m_d_height;
	Err err;
	m_backing_store =WinCreateOffscreenWindow(m_max_width,m_d_height, nativeFormat,&err);

	if (err != errNone)
		DisplayError(SCREEN_ERROR, NULL);

	WinCopyRectangle(NULL,m_backing_store,&rp,0,0,winPaint);
	WinEraseRectangle(&rp,0);
}

void SiMiniBuffer::restore_background()
{
	RectangleType rp;
	rp.topLeft.x = 0;
	rp.topLeft.y = 0;
	rp.extent.x = m_max_width;
	rp.extent.y = m_d_height;
	WinCopyRectangle(m_backing_store, NULL, &rp, m_x,m_y, winPaint);
	WinDeleteWindow(m_backing_store,false);
}

void SiMiniBuffer::set_title(Char * text)
{

	Int16 len=StrLen(text);

	if(m_title!=NULL)
		MemPtrFree(m_title);

	m_title=(Char*)MemPtrNew(len+1);

	StrCopy(m_title,text);
	fontID font=FntSetFont(stdFont);
	m_title_width=SiUtility::CorrectCharsWidth_char(m_title,len,NULL);
	FntSetFont(font);
	draw_title();
	draw_cursor();
}
Int16 SiMiniBuffer::get_left_x() const
{
	return m_x+m_title_width+m_x_offset;
}
void SiMiniBuffer::erase_cursor()
{
	WinEraseLine(get_left_x()+m_curr_text_width,m_y+2,get_left_x()+m_curr_text_width,m_y+m_d_height-2);
}

void SiMiniBuffer::draw_cursor()
{
	WinDrawLine(get_left_x()+m_curr_text_width,m_y+2,get_left_x()+m_curr_text_width,m_y+m_d_height-2);
}

void SiMiniBuffer::draw_title()
{
	fontID font=FntSetFont(stdFont);
	WinDrawChars(m_title,StrLen(m_title),m_x_offset+m_x,m_y);
	FntSetFont(font);
}

void SiMiniBuffer::redraw_on_insert_char(WChar the_char)
{
	fontID font=FntSetFont(stdFont);
	erase_cursor();

	Int16 char_width=CorrectCharWidth(the_char,0);
	if(m_title_width+m_curr_text_width+char_width>m_max_width)
	{
		m_x_offset=m_max_width-(m_title_width+m_curr_text_width+char_width);
		redraw_on_scroll();
	}
	else
	{
		WinDrawChar(the_char,get_left_x()+m_curr_text_width,m_y);
	}
	m_curr_text_width+=char_width;
	draw_cursor();
	FntSetFont(font);
}

void SiMiniBuffer::redraw_on_scroll()
{
	RectangleType rp;
	rp.topLeft.x=m_x;
	rp.topLeft.y=m_y;
	rp.extent.x=m_max_width;
	rp.extent.y=m_d_height;
	WinEraseRectangle(&rp,0);
	draw_title();
	Char * text=(Char*)MemHandleLock(m_text_handle);
	WinDrawChars(text,StrLen(text),get_left_x(),m_y);
	MemHandleUnlock(m_text_handle);
}

void SiMiniBuffer::redraw_on_delete_char(WChar the_char)
{
	fontID font=FntSetFont(stdFont);
	erase_cursor();

	Int16 char_width=CorrectCharWidth(the_char,0);
	m_curr_text_width-=char_width;
	if(m_x_offset<0)
	{
		m_x_offset=m_max_width-(m_title_width+m_curr_text_width);
		if(m_x_offset>0)
			m_x_offset=0;
		redraw_on_scroll();
	}
	else
	{
		RectangleType rp;
		rp.topLeft.x=get_left_x()+m_curr_text_width;
		rp.topLeft.y=m_y;
		rp.extent.x=char_width;
		rp.extent.y=m_d_height;
		WinEraseRectangle(&rp,0);
	}


	draw_cursor();
	FntSetFont(font);
}


void SiMiniBuffer::give_key(WChar the_char)
{
	Char * term=NULL;
	Int16 initial_size=0;
	if(m_text_handle!=NULL)
		initial_size=MemHandleSize(m_text_handle);

	switch(the_char)
	{
	case chrBackspace:
		if(m_text_handle==NULL)
			return;
		else
		{
			if(MemHandleSize(m_text_handle)==1)
			{
				return;
			}
			term=(Char*)MemHandleLock(m_text_handle);
			Int16 w=TxtGetPreviousChar(term,initial_size-1,&the_char);
			MemHandleUnlock(m_text_handle);
			MemHandleResize(m_text_handle,initial_size-w);
			term=(Char*)MemHandleLock(m_text_handle);
			term[MemHandleSize(m_text_handle)-1]='\0';
			MemHandleUnlock(m_text_handle);
		}
		redraw_on_delete_char(the_char);
		break;
	default:
		if(!(TxtCharIsPrint(the_char)))
			return;
		if(m_text_handle!=NULL)
		{
			if(MemHandleSize(m_text_handle)>MAX_FIND_STRING_LENGTH)
				return;
		}
		if(m_text_handle==NULL)
		{
			m_text_handle=MemHandleNew(TxtCharSize(the_char)+1);
		}
		else
		{
			initial_size=MemHandleSize(m_text_handle)-1;
			MemHandleResize(m_text_handle,initial_size+TxtCharSize(the_char)+1);
		}
		term=(Char*)MemHandleLock(m_text_handle);
		MemMove(term+initial_size,((Char*)(&the_char))+(2-TxtCharSize(the_char)),TxtCharSize(the_char));
		term[MemHandleSize(m_text_handle)-1]='\0';
		MemHandleUnlock(m_text_handle);
		redraw_on_insert_char(the_char);
		break;
	}

}

Char * SiMiniBuffer::get_text()
{
	if(m_text_handle==NULL)
		return NULL;
	else
		return (Char*)MemHandleLock(m_text_handle);
}
void SiMiniBuffer::release_text()
{
	if(m_text_handle!=NULL)
		MemHandleUnlock(m_text_handle);
}
