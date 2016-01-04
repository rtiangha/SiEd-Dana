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
#include <UI/Form.h>
#ifndef TEXTVIEW_HH
#define TEXTVIEW_HH
#define PERCENTAGE_LIST_LENGTH 5
#include "types.h"
#include "sections.h"
#include "sidocument.h"
class SiTextHandler;
class SiEditor;
class SiScrollbar;

class SiTextView
{
public:
	//SiTextView(SiEditor * editor, SiTextHandler * texthandle) ;
	SiTextView(SiEditor * editor, SiDocument * doc) __attribute__ ((section ("utilfns")));
	~SiTextView() __attribute__ ((section ("utilfns")));
	void initialise_display() __attribute__ ((section ("filefns")));

	void start_waiting() __attribute__ ((section ("utilfns")));
	void stop_waiting() __attribute__ ((section ("utilfns")));
	//notifications
	void notify_cursor_moved(const Position cursor_pos, Boolean erase) __attribute__ ((section ("utilfns")));
	void notify_block_deleted(const Int16 line) __attribute__ ((section ("utilfns")));
	void notify_block_inserted(const Int16 line) __attribute__ ((section ("utilfns")));
	void notify_text_deleted(const Int16 line,const BlockInt start_x)__attribute__ ((section ("utilfns")));
	void notify_text_changed(Position start,Position end) __attribute__ ((section ("utilfns")));
	Boolean notify_scrolled(Int16 a, Boolean scroll,Boolean quick=true) __attribute__ ((section ("utilfns")));
	void notify_size_changed() __attribute__ ((section ("utilfns")));
	void notify_saved() __attribute__ ((section ("utilfns")));
	void notify_lost_focus() __attribute__ ((section ("utilfns")));
	void notify_got_focus() __attribute__ ((section ("utilfns")));
	void notify_font_changed() __attribute__ ((section ("utilfns"))) ;
	void notify_selection_cleared() __attribute__ ((section ("utilfns")));

	//void notify_selection_cleared(Position min_pos,Position max_pos) __attribute__ ((section ("filefns")));

	//document data get functions

	void set_filename_dirty() __attribute__ ((section ("utilfns")));
	Position find_position(const Int16 x,const Int16 y) ;
	DrawPosition find_draw_position(Position p) ;

	Position find_vertical_cursor_position(Int16 line) ;

	Int16 get_top_line() ;

	//screen drawing controls
	void enable_screen_updates() __attribute__ ((section ("utilfns")));
	void disable_screen_updates() __attribute__ ((section ("utilfns")));
	void clear_screen() __attribute__ ((section ("utilfns")));
	void redraw_selection(Position prev_p,Position p) __attribute__ ((section ("utilfns")));
	void redraw_display(const Boolean redraw_scroll=false) __attribute__ ((section ("utilfns")));
	void redraw_text() __attribute__ ((section ("utilfns")));
	void redraw_chrome(Boolean force=true) __attribute__ ((section ("utilfns")));
	void ensure_visible(BlockInt line) __attribute__ ((section ("utilfns")));

	void erase_cursor() __attribute__ ((section ("utilfns")));
	void redraw_cursor() __attribute__ ((section ("utilfns")));
	Int16 get_line_height() const __attribute__ ((section ("utilfns")));
	Boolean point_in_save_star(Coord x,Coord y) __attribute__ ((section ("utilfns")));
	Boolean point_in_file_name(Coord x,Coord y) __attribute__ ((section ("utilfns")));
	Int16 point_in_line_number(Coord x,Coord y) __attribute__ ((section ("utilfns")));
	void make_top_line(BlockInt line,Boolean always) __attribute__ ((section ("utilfns")));
	Position get_last_redraw_position() __attribute__ ((section ("utilfns")));
	Boolean point_in_drag_area(const Int16 x,const Int16 y) __attribute__ ((section ("utilfns")));
	Int16 get_status_y() __attribute__ ((section ("utilfns")));
	void draw_status_line_number() __attribute__ ((section ("utilfns")));
	void clear_self() __attribute__ ((section ("utilfns")));
	void draw_status_read_only() __attribute__ ((section ("utilfns")));
	SiScrollbar * get_scrollbar() __attribute__ ((section ("utilfns")));
#ifdef TEST_OBJECT_UNIT

	void perform_tests() __attribute__ ((section ("utilfns")));
#endif

	UInt16 get_line_multiple_size(const UInt16 y) __attribute__ ((section ("utilfns")));
private:

	void draw_waiting() __attribute__ ((section ("utilfns")));
	void done_waiting() __attribute__ ((section ("utilfns")));
	void update_scrollbar(const Int16 line) __attribute__ ((section ("utilfns")));
	void update_scrollbar() __attribute__ ((section ("utilfns")));
	void clear_after_line(const BlockInt line,Boolean tidy_end) __attribute__ ((section ("utilfns")));
	void redraw_between(Position &min_pos,Position &max_pos) __attribute__ ((section ("utilfns")));
	Boolean is_onscreen(const Int16 line) const;
	//internal drawing functions
	//void draw_between(Position start,Position end) __attribute__ ((section ("utilfns")));
	void draw_block(DrawPosition draw_pos,const Position text_pos,BlockInt  max) __attribute__ ((section ("utilfns")));

	void fill_tab(const Int16 x, const Int16 curr_line,
				  const Int16 right_edge) __attribute__ ((section ("utilfns")));
	void clear_tab_space(const Int16 x, const Int16 curr_line,
						 const Int16 right_edge) __attribute__ ((section ("utilfns")));
	void tidy_end_line(Int16 line) __attribute__ ((section ("utilfns")));
	void scroll_up(const Int16 lines) __attribute__ ((section ("utilfns"))) ;
	void scroll_down(const Int16 lines) __attribute__ ((section ("utilfns"))) ;
	void clear_line(DrawPosition start) __attribute__ ((section ("utilfns")));
	void clear_line(Int16 line) __attribute__ ((section ("utilfns")));
	void clear_line(DrawPosition start, const Int16 right) __attribute__ ((section ("utilfns")));
	void draw_sel_block(const DrawPosition p,const Int16 thick) __attribute__ ((section ("utilfns"))) ;
	void quick_scroll_display(Int16 n_lines) __attribute__ ((section ("utilfns")));
	void get_resources() __attribute__ ((section ("utilfns")));
	void redraw_text_view() __attribute__ ((section ("utilfns")));
	void tidy_start_line(Int16 line) __attribute__ ((section ("utilfns")));
	Int16 get_y(Int16 text_line) __attribute__ ((section ("utilfns")));
	Int16 draw_EOL(DrawPosition p,const Boolean invert) __attribute__ ((section ("utilfns")));
	void DrawSelectedChars(const Char * chars,Int16 len,Coord x,Coord y) __attribute__ ((section ("utilfns")));
	//cursor drawing
	void get_cursor_window() __attribute__ ((section ("utilfns")));
	void draw_status_asterisk() __attribute__ ((section ("utilfns")));
	void draw_status_lines() __attribute__ ((section ("utilfns")));
	void draw_status_filename() __attribute__ ((section ("utilfns")));

	void draw_status_controls() __attribute__ ((section ("utilfns")));
	void draw_cursor(DrawPosition p) __attribute__ ((section ("utilfns")));
	void draw_status_dis(Boolean force) __attribute__ ((section ("utilfns")));
	void exit_display() __attribute__ ((section ("filefns")));
	void clear_sel_block(const DrawPosition p,const Int16 thick) __attribute__ ((section ("utilfns")));
	Boolean change_onscreen(SiTextChange * change) __attribute__ ((section ("utilfns")));
	Boolean focused;
	void initialise(SiEditor * editor, SiDocument * doc) __attribute__ ((section ("utilfns")));

	void clear_last_lines() __attribute__ ((section ("utilfns")));

	//internal screen state
	MemHandle text_buff;
	Int16 prev_dis_width;
	RectangleType rp;
	RectangleType cont_r;
	RectangleType clear_rect;
	Int16 line_height;
	Int32 top_line;
	Int16 last_offset;
	//DrawPosition cursor_location;
	Boolean filename_dirty;
	Int16 status_cursor_line;
	Int16 status_last_line;
	WinHandle cursor_backing_store;
	//WinHandle scroll_backing_store;
	Position prev_frame_selected_position;
	Int16 prev_document_number_blocks;
	SiDocument * m_document;
	SiEditor *m_editor;
	UInt16 m_scroll_index;
	RectangleType src_rect;

	Int16 last_line;
	Boolean cursor_drawn;
	Boolean dirty_state;
	UInt32 min_index;
	UInt32 max_index;
	RectangleType star_pos;
	Boolean update_screen_enabled;
	Boolean scrollbar_visible;
	MemHandle return_rsc;
	MemHandle small_return_rsc;
	RectangleType screen_size;
	RectangleType old_screen_size;
	Boolean m_tidy_after;
	SiScrollbar * m_vert_scrollbar;
	Int16 m_waiting_count;
	Boolean m_prev_read_only;
};

#endif
