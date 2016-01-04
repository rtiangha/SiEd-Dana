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
#ifndef SISCROLL_HH
#define SISCROLL_HH

#include <PalmOS.h>
#include <PalmTypes.h>
#include "sections.h"
class SiEditor;

enum {HORIZONTAL_SCROLL,VERTICAL_SCROLL};
enum {TOP_BUTTON,BOTTOM_BUTTON};

class SiScrollbar
{
public:
	SiScrollbar(SiEditor * edit,const UInt8 orientation);
	void resize(const UInt16 top_x,const UInt16 top_y,const UInt16 length,const UInt16 width) __attribute__ ((section ("utilfns")));
	void set_position(const UInt16 pos) __attribute__ ((section ("utilfns")));
	void set_max(const UInt16 max) __attribute__ ((section ("utilfns")));
	void set
		(const UInt16 current_pos,const UInt16 max,const UInt16 page_length) __attribute__ ((section ("utilfns")));

	void set_page_size(const UInt16 page) __attribute__ ((section ("utilfns")));
	Boolean handle_tap(const Int16 x,const Int16 y) __attribute__ ((section ("utilfns")));
	void draw_self(const Boolean clear) __attribute__ ((section ("utilfns")));
#ifdef TEST_OBJECT_UNIT

	void perform_tests();
#endif

private:
	Int16 round_to_size(Int16 y) __attribute__ ((section ("utilfns")));
	void handle_slider_tap(const Int16 x,const Int16 y) __attribute__ ((section ("utilfns")));
	void clear_self() __attribute__ ((section ("utilfns")));

	void calc_slider_pos();
	void show() __attribute__ ((section ("utilfns")));
	Boolean is_visible() const __attribute__ ((section ("utilfns")));

	void hide() __attribute__ ((section ("utilfns")));

	void draw_slider(const Boolean clear) __attribute__ ((section ("utilfns")));
	void draw_trough() __attribute__ ((section ("utilfns")));
	void clear_slider() __attribute__ ((section ("utilfns")));
	void draw_button(const UInt8 b) __attribute__ ((section ("utilfns")));
	void move_slider(Int16 x,Int16 y) __attribute__ ((section ("utilfns")));
	void calc_scale_factors() __attribute__ ((section ("utilfns")));
	void set_prev_slider_pos() __attribute__ ((section ("utilfns")));
	void clear_previous_slider_rect() __attribute__ ((section ("utilfns")));
	void clear_between(Int16 p1,Int16 p2) __attribute__ ((section ("utilfns")));
	void button_press(const UInt8 direction);
	void trough_tap( Int16 y);
	void move_slider_value(const Int16 m);
	void do_quick_move(const Int16 step);
	UInt32 m_slider_pos;
	UInt32 m_page_size;
	UInt32 m_max;
	UInt8 m_orientation;
	RectangleType m_draw_position;
	RectangleType m_slider_draw_position;
	RectangleType m_slider_prev_position;
	UInt32 m_scale_factor;
	Int16 m_button_pressed;
	SiEditor * m_editor;
	Boolean m_shown;

};
#endif
