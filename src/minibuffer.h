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

#include <PalmOS.h>
#include <PalmTypes.h>
#include "types.h"

#ifndef MINIBUFF_HPP
#define MINIBUFF_HPP

class SiMiniBuffer
{
public:
	SiMiniBuffer(Int16 x,Int16 y,Int16 max_width);
	~SiMiniBuffer();
	void give_key(WChar the_char);
	void set_title(Char * text);
	Char * get_text();
	void release_text();
protected:
	void erase_cursor();
	void draw_cursor();
	void copy_background();
	void restore_background();
	void draw_title();
	void redraw_on_insert_char(WChar the_char);
	void redraw_on_delete_char(WChar the_char);
	void redraw_on_scroll();
	Int16 get_left_x() const;
	Int16 m_curr_text_width;
	Int16 m_title_width;
	Int16 m_d_height;
	Char * m_title;
	MemHandle m_text_handle;
	Int16 m_x_offset;
	Int16 m_max_width;
	Int16 m_x;
	Int16 m_y;

	WinHandle m_backing_store;
};
#endif
