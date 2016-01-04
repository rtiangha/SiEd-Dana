/*
 * SiEd - A text editor for PalmOS 3.5+ Copyright (C) 2003 Benjamin Roe
 * Contact Email:ben@benroe.com
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
#include <Chars.h>
#include <PalmOSGlue.h>
#include "types.h"
#include "sections.h"
typedef Int16 (*fntcall)(WChar ch);
extern fntcall get_char_width;
#ifndef SIUTIL_HH
#define SIUTIL_HH

class Position;

#define CorrectCharWidth(ch,pos) (((ch==chrHorizontalTabulation) ? (SiUtility::NextTabStop(pos)-pos):get_char_width(ch)))

//Int16 CorrectCharWidth(const WChar ch,const Int16 curr_width);

class SiUtility
{
public:
	static Int16 NextTabStop(const Int16 x) __attribute__ ((section ("utilfns")));
	static BlockInt calc_n_chars(const Char* const data,const BlockInt bytes) __attribute__ ((section ("utilfns")));
	static BlockInt calc_byte_length(const Char* data,const BlockInt n_chars) __attribute__ ((section ("utilfns")));
	static BlockInt last_char_boundary(MemHandle m,BlockInt *n_chars);
	static BlockInt last_char_boundary(Char * data,BlockInt *n_chars,BlockInt max_bytes) __attribute__ ((section ("utilfns")));

	static Int16 CorrectCharsWidth_byte(char * data,const BlockInt max_offset,Boolean * contains_line_feed,Int16 show_codes=0) __attribute__ ((section ("texfns")));
	static Int16 CorrectCharsWidth_char(char * data,const BlockInt max_offset,Boolean * contains_line_feed,Int16 show_codes=0) __attribute__ ((section ("texfns")));


	static void initialise_calls() __attribute__ ((section ("utilfns")));
	static Boolean is_break_char(const WChar ch) __attribute__ ((section ("utilfns")));
	static Boolean is_break_char(const WChar ch,Boolean ignore_punc) __attribute__ ((section ("utilfns")));
	static Boolean VFS_SUPPORT;
	static Coord SCREEN_WIDTH;

	static Coord ACTIVE_SCREEN_WIDTH;
	static Coord ACTIVE_SCREEN_HEIGHT;
	static Coord CurrentScreenHeight;
	static Boolean system_file(const char *file) __attribute__ ((section ("utilfns")));
	static Boolean has_colour_support() __attribute__ ((section ("utilfns")));
	static Boolean has_hires_support() __attribute__ ((section ("utilfns")));
	static IndexedColorType get_line_color() __attribute__ ((section ("utilfns")));
	static IndexedColorType get_status_text_color() __attribute__ ((section ("utilfns")));
	static void check_system_capabilities() __attribute__ ((section ("utilfns")));
	static Boolean encryption_support() __attribute__ ((section ("utilfns")));

	static IndexedColorType get_fore_color() __attribute__ ((section ("utilfns")));
	static IndexedColorType get_back_color() __attribute__ ((section ("utilfns")));
	static IndexedColorType get_cursor_color() __attribute__ ((section ("utilfns")));
	static void set_field_term(FieldType* text_field,MemHandle source_handle,Boolean set_selection) __attribute__ ((section ("utilfns")));
	static void set_field_term(FieldType* text_field,const Char * text,Boolean set_selection) __attribute__ ((section ("utilfns")));
	static void clear_field(FieldType * text_field) __attribute__ ((section ("utilfns")));
	static void invalidate_cache() __attribute__ ((section ("utilfns")));
	static Boolean has_dynamic_IA() __attribute__ ((section ("utilfns")));
#ifdef COLLECT_STATISTICS

	static	void add_width_cache_hit(const UInt16 size) __attribute__ ((section ("utilfns")));
	static void add_char_cache_hit(const UInt16 size) __attribute__ ((section ("utilfns")));
	static void add_width_cache_miss() __attribute__ ((section ("utilfns")));
	static 	void add_char_cache_miss() __attribute__ ((section ("utilfns")));
	static void display_stats() __attribute__ ((section ("utilfns")));
#endif
protected:
	static void check_dynamic_IA() __attribute__ ((section ("utilfns")));

	static void check_screen_capabilities() __attribute__ ((section ("utilfns")));
	static void check_vfs_support() __attribute__ ((section ("utilfns")));
	static UInt32 screen_version;
	static Boolean m_encryption_support;
	static Boolean m_colour_support;
	static IndexedColorType fore_text_color;
	static IndexedColorType back_text_color;
	static IndexedColorType cursor_color;
	static IndexedColorType line_color;
	static IndexedColorType status_color;
	static Boolean m_hires_support;
	static Boolean m_has_dynamic_IA;	
	static Coord SCREEN_HEIGHT;
#ifdef USE_WIDTH_CACHE

	static Int16 cached_text_width;
	static BlockInt cached_text_char;
	static BlockInt cached_text_byte;
	static Char * cached_text_data;
#endif
	#ifdef COLLECT_STATISTICS

	static UInt16 w_cache_hits;
	static 	UInt16 w_cache_misses;
	static 	UInt16 c_cache_hits;
	static 	UInt16 c_cache_misses;
	static 	UInt16 w_cache_hit_size;
	static 	UInt16 c_cache_hit_size;
#endif
};

Boolean operator ==(const Position & lhs,const Position & rhs);
//Int16 abs(const Int16 num) __attribute__ ((section ("utilfns")));

Boolean operator <(const Position & lhs,const Position & rhs);
Boolean operator <=(const Position & lhs,const Position & rhs);

char to_lower(WChar c) __attribute__ ((section ("utilfns")));



inline Boolean operator ==(const Position & lhs,const Position & rhs)
{
	if(lhs.line==rhs.line&&lhs.x==rhs.x)
		return true;
	else
		return false;
}


inline BlockInt SiUtility::last_char_boundary(MemHandle m,BlockInt * n_chars)
{
	Char * data=(Char*)MemHandleLock(m);
	Int16 ret=last_char_boundary(data,n_chars,(BlockInt)MemHandleSize(m));
	MemHandleUnlock(m);
	return ret;
}
#endif
