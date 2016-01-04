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

#ifndef SIEDITINT_HPP
#define SIEDITINT_HPP
#include "editor.h"
#include "preferences.h"
#include "simacrorecorder.h"

class SiEditInterface
{
public:
  SiEditInterface();
	~SiEditInterface();
	void initialise(Char * file_to_open);
	Boolean handle_event(EventPtr e);
	void register_for_resize();
	Boolean intercept_event(EventPtr e);

	void emit_redraw_signal(SiEditor * edit);
	Boolean split_screen();
	Boolean unsplit_screen();
	void change_font();
	void make_exclusive(SiEditor *m_editor);
	void release_exclusive();
	void focus_offscreen_field();
#ifdef PERFORM_TEST
	SiEditor *the_editor_active;
#endif

	Int32 get_event_delay();
	void do_quick_slosh();
#ifdef TEST_OBJECT_UNIT
	Boolean perform_tests() __attribute__ ((section("editfns")));
#endif
	void set_size_limits(UInt16 frm);
private:
	void resize_screen(const Int16 new_height);
	void change_active_editor(SiEditor * edit);
	Boolean is_macro_tap(const Int16 x,const Int16 y);
	Boolean view_menu_handle(const UInt16 id);
	SiEditor * get_editor_in_bounds(const Int16 x,const Int16 y);
	SiEditor *the_editor_top;
	SiEditor *the_editor_bottom;
#ifndef PERFORM_TEST

	SiEditor *the_editor_active;
#endif

	void check_resize_drag_start(const Int16 x,const Int16 y);
	Boolean do_drag_resize(const Int16 x,const Int16 y);
	SiMacroRecorder * macro_recorder;
	Boolean drag_resizing;
	//	Int16 drag_resizing_y;
	SiEditor * m_exclusive_editor;
#ifdef DEBUG

	Boolean screen_toggled;
#endif

};

#endif
