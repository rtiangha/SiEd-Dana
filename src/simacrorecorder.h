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

#ifndef SIMACRORECORDER_H
#define SIMACRORECORDER_H
#include <PalmOS.h>
#include <PalmTypes.h>
#include "sections.h"
/**
@author Benjamin Roe
*/

class SiMacroRecorder
{
public:
	SiMacroRecorder() __attribute__ ((section ("diafns")));
	Boolean give_event(EventPtr e) __attribute__ ((section ("diafns")));
	~SiMacroRecorder() __attribute__ ((section ("diafns")));
	void start_playback() __attribute__ ((section ("diafns")));
	Boolean start_record() __attribute__ ((section ("diafns")));
	void stop_record() __attribute__ ((section ("diafns")));
	void draw_macro_name(const UInt16 num,RectangleType * pos);
	void delete_selected_macro() __attribute__ ((section ("diafns")));
	Boolean check_valid() __attribute__ ((section ("diafns")));
	void popup_macro_list() __attribute__ ((section ("diafns")));
	Boolean recording;
	Boolean playing;
protected:
	void do_quick_macro(Char * the_char) __attribute__ ((section ("diafns")));
	void draw_macro_active_rect() __attribute__ ((section ("diafns")));
	void draw_macro_active_char(Char * text) __attribute__ ((section ("diafns")));
	void erase_macro_active_rect() __attribute__ ((section ("diafns")));
	void prepare_quick_macro() __attribute__ ((section ("diafns")));
	Char * display_macro_select(Int16 & index,Int16 mode) __attribute__ ((section ("diafns")));
	void close_macro_database() __attribute__ ((section ("diafns")));
	Boolean open_macro_database() __attribute__ ((section ("diafns")));
	Boolean add_macro_record(Char * name) __attribute__ ((section ("diafns")));
	ListPtr initialise_list(FormPtr frm) __attribute__ ((section ("diafns")));
	Boolean initialise_macro_record() __attribute__ ((section ("diafns")));
	Boolean write_event_to_macro(EventPtr e) __attribute__ ((section ("diafns")));
	Boolean handle_tap(EventPtr e) __attribute__ ((section ("diafns")));

	Boolean play_macro(Int16 index) __attribute__ ((section ("diafns")));
	void copy_event(EventPtr target,EventPtr source) __attribute__ ((section ("diafns")));
	void initialise_macro_record(Char * name) __attribute__ ((section ("diafns")));
	Boolean open_macro(Int16 index) __attribute__ ((section ("diafns")));
	MemHandle macro_handle;
	Char * macro_name;

	UInt16 number_events;
	DmOpenRef open_ref;
	LocalID id;
	UInt16 curr_macro_index;
	UInt16 name_offset;
	UInt16 number_macros;
	Int16 dialog_mode;
	Boolean quick_macro_ready;
	Int16 list_draw_mode;

};

#endif
