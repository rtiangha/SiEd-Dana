//
// C++ Interface: simacrorecorder
//
// Description:
//
//
// Author: Benjamin Roe <sied@benroe.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
