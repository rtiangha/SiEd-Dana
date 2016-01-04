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

#include "siscrollbar.h"
#include "editor.h"
#include "error.h"
#define SCROLL_BUTTON_SIZE ((UInt32)7)
#define MIN_SLIDER_LENGTH ((UInt32)7)
#define SLIDER_ACCURACY ((UInt32)100)
#define BUTTON_SPEED 2
#define BUTTON_DELAY 7
#define DRAG_DELAY 100
#define TROUGH_SPEED 5

SiScrollbar::SiScrollbar(SiEditor * edit,const UInt8 orientation)
{
	m_orientation=orientation;
	m_editor=edit;
	m_shown=false;
	m_scale_factor=0;
	m_slider_pos=0;
	m_max=1;
	m_page_size=m_editor->COMMAND_LINE;

	m_draw_position.topLeft.x=m_draw_position.topLeft.y=
		m_draw_position.extent.x=m_draw_position.extent.y=0;
	m_slider_draw_position.extent.x=m_slider_draw_position.extent.y=
		m_slider_draw_position.topLeft.x=m_slider_draw_position.topLeft.y=0;
	m_slider_prev_position.topLeft.x=m_slider_prev_position.topLeft.y=
		m_slider_prev_position.extent.x=m_slider_prev_position.extent.y=0;
	m_button_pressed=-1;
}

Boolean SiScrollbar::handle_tap(const Int16 x,const Int16 y)
{
	Boolean ret=true;
	if(RctPtInRectangle(x,y,&m_slider_draw_position))
	{

		handle_slider_tap(x,y);

	}
	else if(m_orientation==VERTICAL_SCROLL)
	{
		//maybe in the trough or in a button
		if(x>=m_draw_position.topLeft.x)
		{
			if(y<=m_draw_position.topLeft.y)
			{
				button_press(TOP_BUTTON);
			}
			else if(y>=m_draw_position.topLeft.y+m_draw_position.extent.y
					&&(UInt16)y<=(m_draw_position.topLeft.y+m_draw_position.extent.y+SCROLL_BUTTON_SIZE))
			{
				button_press(BOTTOM_BUTTON);
			}
			else if(y>=m_draw_position.topLeft.y
					&&y<=m_draw_position.topLeft.y+m_draw_position.extent.y)
			{
				trough_tap(y);
			}
			else
				ret=false;
		}
		else
			ret=false;
	}

	return ret;
}

void SiScrollbar::handle_slider_tap(const Int16 x,const Int16 y)
{
	Int16 x_offset=x-m_slider_draw_position.topLeft.x;
	Int16 y_offset=y-m_slider_draw_position.topLeft.y;
	Int16 l_x,l_y;
	//Int16 ticksPause=SysTicksPerSecond()/DRAG_DELAY;

	//tap is in slider
	Coord prev_x=x;
	Coord prev_y=y;
	Boolean isPenDown=true;
	while (isPenDown)
	{
		EvtGetPen(&l_x, &l_y, &isPenDown);

		if((abs(l_y-prev_y)>2)&&isPenDown)
		{
			move_slider(l_x-x_offset,l_y-y_offset);

			prev_x=l_x;
			prev_y=l_y;
		}
		//	  if(isPenDown)
		//	    SysTaskDelay(ticksPause);
	}
}

void SiScrollbar::button_press(const UInt8 direction)
{
	m_button_pressed=direction;

	draw_button(direction);
	if(direction==TOP_BUTTON)
	{
		do_quick_move(-m_page_size/BUTTON_SPEED);

	}
	else
	{
		do_quick_move(m_page_size/BUTTON_SPEED);

	}

	m_button_pressed=-1;
	draw_button(direction);
}

void SiScrollbar::do_quick_move(const Int16 step)
{
	Int16 ticksPause=SysTicksPerSecond()/BUTTON_DELAY;
	Boolean isPenDown=true;
	Int16 l_x,l_y;

	while (isPenDown)
	{
		EvtGetPen(&l_x, &l_y, &isPenDown);
		move_slider_value(step);
		SysTaskDelay(ticksPause);
	}

}

void SiScrollbar::move_slider(Int16 x,Int16 y)
{

	Int16 distance=m_slider_pos;
	if(x<m_draw_position.topLeft.x)
		x=m_draw_position.topLeft.x;
	if(y<m_draw_position.topLeft.y)
		y=m_draw_position.topLeft.y;
	else if((UInt16)y>m_draw_position.topLeft.y+m_scale_factor)
		y=m_draw_position.topLeft.y+m_scale_factor;

	if(m_orientation==VERTICAL_SCROLL)
	{
		
		m_slider_pos=((m_max-m_page_size)*(y-m_draw_position.topLeft.y))/m_scale_factor;
//		m_slider_draw_position.topLeft.y=y;
	}

	if(m_slider_pos+m_page_size>m_max)
		m_slider_pos=m_max-m_page_size;
	calc_slider_pos();
	m_editor->handle_scroll(distance,m_slider_pos);

	draw_slider(true);
}

void SiScrollbar::trough_tap(Int16 y)
{
	if(m_orientation==VERTICAL_SCROLL)
	{
		move_slider(0,y);

	}
}

void SiScrollbar::set
	(const UInt16 pos,const UInt16 max,const UInt16 page_length)
{

#ifdef DEBUG_LOG
	log_entry_number("SiScrollbar::Set pos=",pos);
#endif

	if(pos==m_slider_pos&&max==m_max&&page_length==m_page_size)
		return;
	else if(max==0)
	{
		m_max=0;
		hide();
	}
	else if(pos>max)
	{
		return;
	}
	else
	{
		m_slider_pos=pos;
		m_max=max;
		m_page_size=page_length;
		calc_scale_factors();
		calc_slider_pos();
		if(m_max>m_page_size)
		  show();
		else
		  hide();
		draw_slider(true);

	}

}


Boolean SiScrollbar::is_visible() const
{
	return m_shown;
}

void SiScrollbar::resize(const UInt16 top_x,const UInt16 top_y,const UInt16 x_ext,const UInt16 y_ext)
{
#ifdef DEBUG_LOG
	log_entry("SiScrollbar::resize");
#endif

	if(x_ext==0||y_ext==0)
		return;
	if(m_orientation==VERTICAL_SCROLL)
	{
		if(y_ext<=SCROLL_BUTTON_SIZE*2+MIN_SLIDER_LENGTH)
			return;
	}
	else
	{
		if(x_ext<=SCROLL_BUTTON_SIZE*2+MIN_SLIDER_LENGTH)
			return;
	}
	m_slider_prev_position.extent.y=0;
	clear_self();
	if(m_max>0)
	{
		if(m_orientation==VERTICAL_SCROLL)
		{
			m_slider_draw_position.topLeft.y=top_y+(m_scale_factor*m_slider_pos)/m_max+SCROLL_BUTTON_SIZE;
		}
		else
		{
			m_slider_draw_position.topLeft.x=top_y+m_scale_factor*m_slider_pos/m_max+SCROLL_BUTTON_SIZE;
		}
	}

	if(m_orientation==VERTICAL_SCROLL)
	{
		m_draw_position.topLeft.x=top_x;
		m_draw_position.topLeft.y=top_y+SCROLL_BUTTON_SIZE;

		m_draw_position.extent.y=y_ext-2*SCROLL_BUTTON_SIZE;

		m_draw_position.extent.x=x_ext;


		m_slider_draw_position.topLeft.x=top_x;
		m_slider_draw_position.extent.x=x_ext;
	}
	else
	{
		m_draw_position.topLeft.x=top_x+SCROLL_BUTTON_SIZE;
		m_draw_position.topLeft.y=top_y;

		m_draw_position.extent.y=y_ext;

		m_draw_position.extent.x=x_ext-2*SCROLL_BUTTON_SIZE;


		m_slider_draw_position.topLeft.y=top_y;
		m_slider_draw_position.extent.y=y_ext;

	}
	if(m_max>0)
	{
	  calc_scale_factors();
	  calc_slider_pos();
	  if(m_max<=m_page_size)
	    hide();
	  draw_self(false);
	}
}


void SiScrollbar::calc_scale_factors()
{
#ifdef DEBUG
	ErrFatalDisplayIf(m_max==0,"SiScrollbar:: Zero maximum on calculate");
#endif

	if(m_max==0)
	  {
	    return;
	  }
	if(m_orientation==VERTICAL_SCROLL)
	{
		m_slider_draw_position.extent.y=(m_draw_position.extent.y*m_page_size)/(m_max);
		if(m_slider_draw_position.extent.y<(Int16)MIN_SLIDER_LENGTH)
			m_slider_draw_position.extent.y=MIN_SLIDER_LENGTH;

		m_scale_factor=((m_draw_position.extent.y-m_slider_draw_position.extent.y));///(m_max);

	}
	else
	{
	}

#ifdef DEBUG
	//  ErrFatalDisplayIf(m_scale_factor==0&&m_shown,"Zero scale factor");
#endif

}


void SiScrollbar::calc_slider_pos()
{
#ifdef DEBUG
//	ErrFatalDisplayIf(m_slider_pos>m_max-m_page_size,"calc_slider_pos off bottom of trough");
#endif

	if(m_max==0)
		return;
	if(m_orientation==VERTICAL_SCROLL)
	{
		if(m_slider_pos==0)
			m_slider_draw_position.topLeft.y=m_draw_position.topLeft.y;
		else if(m_slider_pos>=m_max-m_page_size)
			m_slider_draw_position.topLeft.y=m_draw_position.topLeft.y+m_scale_factor;
		else
			m_slider_draw_position.topLeft.y=m_draw_position.topLeft.y+(m_slider_pos*m_scale_factor)/(m_max-m_page_size);

#ifdef DEBUG

	//	ErrFatalDisplayIf(m_slider_draw_position.topLeft.y+m_slider_draw_position.extent.y>m_draw_position.topLeft.y+m_draw_position.extent.y,"Calc_slider_pos: moved slider off end");
#endif

	}
	else
	{
		m_slider_draw_position.topLeft.x=m_draw_position.topLeft.x+(m_slider_pos*m_scale_factor)/m_max;
	}


}

void SiScrollbar::move_slider_value(const Int16 m)
{
	Int16 orig_pos=m_slider_pos;
	if(m<0)
	{
		if(abs(m)>=(Int32)m_slider_pos)
			m_slider_pos=0;
		else
			m_slider_pos+=m;
	}
	else
	{
		m_slider_pos+=m;
		if(m_slider_pos>m_max-m_page_size)
			m_slider_pos=m_max-m_page_size;
	}

	calc_slider_pos();
	m_editor->handle_scroll(orig_pos,m_slider_pos);
	draw_slider(true);
}

void SiScrollbar::set_position(const UInt16 pos)
{
	if(pos>m_max)
		m_slider_pos=m_max;
	else
		m_slider_pos=pos;
	calc_slider_pos();
	draw_slider(true);
}

void SiScrollbar::set_max(const UInt16 max)
{
	//  clear_self();
	m_max=max;
	calc_scale_factors();
	draw_slider(true);
}


void SiScrollbar::set_page_size(const UInt16 page)
{
	m_page_size=page;
	calc_scale_factors();
	draw_slider(true);
}

void SiScrollbar::hide()
{
	if(m_shown)
	{
		m_shown=false;
		clear_self();
	}
}

void SiScrollbar::show()
{
	if(!m_shown&&m_max>m_page_size)
	{
		m_shown=true;
		draw_self(false);
	}
}

void SiScrollbar::set_prev_slider_pos()
{
#ifdef DEBUG_LOG
	log_entry("SiScrollbar::set_prev_slider_pos");
#endif

	m_slider_prev_position.topLeft.x=m_slider_draw_position.topLeft.x;
	m_slider_prev_position.topLeft.y=m_slider_draw_position.topLeft.y;
	m_slider_prev_position.extent.x=m_slider_draw_position.extent.x;
	m_slider_prev_position.extent.y=m_slider_draw_position.extent.y;
}



void SiScrollbar::draw_self(const Boolean clear)
{
#ifdef DEBUG_LOG
	log_entry("SiScrollbar::draw_self");
#endif

	if(m_shown)
	{
		draw_trough();
		draw_slider(clear);

		draw_button(TOP_BUTTON);
		draw_button(BOTTOM_BUTTON);


	}
}

void SiScrollbar::draw_trough()
{
#ifdef DEBUG_LOG
	log_entry("SiScrollbar::draw_trough");
#endif
	if(m_shown)
	{
		WinPushDrawState();
		WinSetPatternType(grayPattern);
#ifdef DEBUG
		if(m_orientation==(UInt16)VERTICAL_SCROLL)
		  ErrFatalDisplayIf(m_draw_position.topLeft.y<(Int16)SCROLL_BUTTON_SIZE,"Drawing trough off end");
		/*Char buff[100];
		StrPrintF(buff,"draw Trough %i",m_draw_position.extent.y);
		DisplayError(DEBUG_MESSAGE,buff);*/
#endif


		WinPaintRectangle(&m_draw_position,0);
		WinPopDrawState();
	}


}

void SiScrollbar::draw_slider(const Boolean clear)
{
	#ifdef DEBUG_LOG
	log_entry("SiScrollbar::draw_slider");
	#endif
	if(m_shown)
	{
	  if(clear)
	    clear_previous_slider_rect();
#ifdef DEBUG
		if(m_orientation==VERTICAL_SCROLL)
		{
			ErrFatalDisplayIf(m_slider_draw_position.topLeft.y<m_draw_position.topLeft.y,"Drawing slider off top end");
			if(m_slider_draw_position.topLeft.y+m_slider_draw_position.extent.y>m_draw_position.topLeft.y+m_draw_position.extent.y)
			  {
			    Char buff[100];
			    StrPrintF(buff,"Drawing slider off bottom end, y=%i",m_slider_draw_position.topLeft.y+m_slider_draw_position.extent.y);
			    ErrFatalDisplay(buff);
			  }
		}
#endif
		WinDrawRectangle(&m_slider_draw_position,0);

		set_prev_slider_pos();

	}

}

void SiScrollbar::clear_between(Int16 p1,Int16 p2)
{
#ifdef DEBUG_LOG
	log_entry_number("clear_between ",p1);
	log_entry_number(" and ",p2);
#endif

	//  ErrFatalDisplayIf(p1<m_draw_position.topLeft.y,"Clearing off top of trough");
	//  ErrFatalDisplayIf(p2>m_draw_position.topLeft.y+m_draw_position.extent.y,"Clearing off bottom of trough");
	if(p1<m_draw_position.topLeft.y)
		p1=m_draw_position.topLeft.y;
	if(p2>m_draw_position.topLeft.y+m_draw_position.extent.y)
		p2=m_draw_position.topLeft.y+m_draw_position.extent.y;
	RectangleType rp;
	if(m_orientation==VERTICAL_SCROLL)
	{
		rp.topLeft.y=p1;
		rp.topLeft.x=m_draw_position.topLeft.x;
		rp.extent.x=m_draw_position.extent.x;
		rp.extent.y=p2-p1;
	}
	else
	{}
	WinPushDrawState();
	WinSetPatternType(grayPattern);
	WinPaintRectangle(&rp,0);
	WinPopDrawState();
}

void SiScrollbar::clear_previous_slider_rect()
{
	//    RectangleType clear_rect;
	if(!m_shown)
		return;
	if(m_slider_prev_position.extent.y==0)
		return;
#ifdef DEBUG_LOG

	log_entry("Clearing previous slider rect");
#endif

	if(m_orientation==VERTICAL_SCROLL)
	{
		if(m_slider_prev_position.topLeft.y<m_slider_draw_position.topLeft.y)
		  clear_between(m_slider_prev_position.topLeft.y,m_slider_draw_position.topLeft.y);
		if(m_slider_prev_position.topLeft.y+m_slider_prev_position.extent.y>m_slider_draw_position.topLeft.y+m_slider_draw_position.extent.y)
		  clear_between(m_slider_draw_position.topLeft.y+m_slider_draw_position.extent.y,m_slider_prev_position.topLeft.y+m_slider_prev_position.extent.y);
	}
	else
	{}

}

void SiScrollbar::clear_self()
{

#ifdef DEBUG_LOG
	log_entry_number("clearing scroll bar x",m_draw_position.topLeft.x);
	log_entry_number("clearing scroll bar y",m_draw_position.topLeft.y);
	log_entry_number("clearing scroll bar ext x",m_draw_position.extent.x);
	log_entry_number("clearing scroll bar ext y",m_draw_position.extent.y);

#endif

	m_slider_prev_position.extent.y=m_slider_prev_position.extent.x=0;

	if(m_shown)
	{
#ifdef DEBUG_LOG
		log_entry("clearing scroll bar shown");
#endif

		m_draw_position.extent.y+=SCROLL_BUTTON_SIZE*2;
		m_draw_position.topLeft.y-=SCROLL_BUTTON_SIZE;
		WinEraseRectangle(&m_draw_position,0);
		m_draw_position.extent.y-=SCROLL_BUTTON_SIZE*2;
		m_draw_position.topLeft.y+=SCROLL_BUTTON_SIZE;
	}
}


void SiScrollbar::draw_button(const UInt8 b)
{

	if(m_shown)
	{
		MemHandle hRsc=NULL;
		BitmapType* bitmapP=NULL;
		Int16 x=0;
		Int16 y=0;
		Int16 image_id=0;
		if(m_orientation==VERTICAL_SCROLL)
		{
			x=m_draw_position.topLeft.x;
			if(b==TOP_BUTTON)
			{
				if(m_button_pressed!=b)
					image_id=SCROLL_UP_ARROW_BITMAP;
				else
					image_id=UP_ARROW_PRESSED_BITMAP;
#ifdef DEBUG

				ErrFatalDisplayIf(m_draw_position.topLeft.y<(Int16)SCROLL_BUTTON_SIZE,"Attempting to draw scrollbar offscreen");
#endif

				y=m_draw_position.topLeft.y-SCROLL_BUTTON_SIZE;

			}
			else
			{
				y=m_draw_position.topLeft.y+m_draw_position.extent.y;

				if(m_button_pressed!=b)
					image_id=DOWN_ARROW_BITMAP;
				else
					image_id=DOWN_ARROW_PRESSED_BITMAP;
			}
		}
		else
		{
			//horizontal stuff here
		}

		hRsc = DmGetResource(bitmapRsc,image_id);
		if(hRsc!=NULL)
		{
			bitmapP = (BitmapType*) MemHandleLock(hRsc);
			if(bitmapP!=NULL)
			{
				WinDrawBitmap(bitmapP,x,y);
				MemHandleUnlock(hRsc);
			}
			DmReleaseResource(hRsc);
		}
	}
}
#ifdef TEST_OBJECT_UNIT

void SiScrollbar::perform_tests()
{
	resize(0,0,10,100);
	for(Int16 m=2;m<100;m+=3)
	{
		set(m,5*m,(m/3)+1);
		ErrFatalDisplayIf(m_slider_pos!=m,"Incorrect slide_pos scrollbar");
		ErrFatalDisplayIf(m_max!=5*m,"Incorrect m_max scrollbar");
		ErrFatalDisplayIf(m_page_size!=(m/3)+1,"Incorrect m_page_size scrollbar");
	}
	set(100,10,10);
	ErrFatalDisplayIf(m_max<m_slider_pos,"Slider pos off end of bar");
	set(0,100,10);

	for(Int16 s=SCROLL_BUTTON_SIZE*2+MIN_SLIDER_LENGTH+1;s<100;++s)
	{
		resize(s,s,s,s);
		ErrFatalDisplayIf(m_draw_position.topLeft.x!=s||m_draw_position.topLeft.y-SCROLL_BUTTON_SIZE!=s,"Incorrect position on resize");
		ErrFatalDisplayIf(m_draw_position.extent.x!=s||m_draw_position.extent.y!=s-2*SCROLL_BUTTON_SIZE,"Incorrect size on resize");
	}
#ifdef DEBUG_LOG
	log_entry("SiScrollbar tests complete");
#endif
}

#endif
