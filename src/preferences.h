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

#include "types.h"
#ifndef SIPREFS_HPP
#define SIPREFS_HPP
#define customFontID ((FontID) fntAppFontCustomBase)
typedef struct
{
	UInt16 font;
	UInt8 selection_mode;
	UInt8 show_codes;
	UInt8 load_mode;
	Boolean word_wrap;
}
MainPrefsSavedType;

typedef struct
{
	Boolean split_screen;
	UInt16 keyboard_open;
	Position cursor_pos[2];
	Int16 file_type[2];
	UInt16 vol_ref[2];
	Int16 top_line[2];
	UInt8 focused_editor;
	UInt8 read_only[2];
	Int16 top_height;
	UInt16 location_offset;
	UInt16 location_size[2];
}MainPrefsUnSavedType;

class SiPreferencesDatabase
{
public:
	static void startup();
	static void shutdown();
	static fontID get_font() ;
	static Int16 get_selection_mode() ;
	static Boolean get_split_screen()  ;
	static UInt8 get_show_codes();
	static Int16 get_load_chunk_size();
	static Boolean get_word_wrap();
	static void set_word_wrap(const Boolean w);
	static void set_top_height(const Int16 y);
	static Int16 get_top_height();
	static Int16 get_top_line(const Int16 index);
	static void set_top_line(const Int16 index,const Int16 top_line);
	static void set_keyboard_open(const UInt16 o);
	static UInt16 get_keyboard_open();
	static void set_load_mode(UInt8 load_mode);
	static void set_font(const UInt16 font);
	static void set_selection_mode(const Int16 mode);
	static void set_split_screen(const Boolean split);
	static void set_show_codes(const UInt8 show);
	static void set_cursor_pos(const Int16 index,const Position pos);
	static void set_file_open(const Int16 index,Char * location, const Int16 ref,const Int16 type);
	static void set_focused_editor(const UInt8 f);
	static void set_read_only(const Int16 index,const Boolean only);
	static Boolean get_read_only(const Int16 index);
	static UInt8 get_focused_editor();
	static Char * get_location_data(const Int16 index);
	static Position get_cursor_pos(const Int16 index);
	static Int16 get_vol_ref(const Int16 index);
	static Int16 get_file_type(const Int16 index);
	static Boolean display_prefs_dialog();
	static void tidy();
	static void setup_font_controls(FormPtr frm);
	
		
private:
	static void write_saved_preferences();
	static void write_unsaved_preferences();
	static void write_preferences_data(void* data,UInt16 size,Boolean saved);
	static void load_file_location();
	static void load_unsaved_preferences();
	static void load_saved_preferences();
	static void * load_preferences_data(Boolean saved,UInt16 & size);
	static UInt16 load_font(UInt16 font);
	static void initialise_mime_list();
	static void check_mime_register(FormPtr frm,ListType * list);
	static void hide_mime_controls(FormPtr frm);
	static void tidy_mime_list();
	static void setup_prefs_dialog(FormPtr form);
	static void generate_saved_defaults();
	static void generate_unsaved_defaults();
	static void load_preferences();
	static void write_preferences();
	static void display_hires_controls();

	static void draw_font_text(FormPtr frm,UInt16 id,UInt16 font);
	static fontID lookup_font_id(UInt16 font);
	static MainPrefsUnSavedType *m_unsaved_prefs;
	static MainPrefsSavedType *m_saved_prefs;
	static Char * location_data[2];
	static UInt32 * crIDs;
	static Char * appNames;
	static UInt32  appCnt;
	static MemHandle m_name_hand;
	static UInt16 m_curr_loaded_font_id;
	static MemHandle m_curr_loaded_font_handle;
	static Boolean m_unsaved_prefs_changed;
	static Boolean m_saved_prefs_changed;
};

#endif
