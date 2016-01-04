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
#include "textview.h"
#include "texthandler.h"
#include "filehandler.h"
#include "file_interface.h"
#include <PalmOS.h>
#include <PalmTypes.h>
#include "sidocument.h"
#include "preferences.h"
#include "minibuffer.h"
#ifndef EDITOR_HH
#define EDITOR_HH
#define SIZE_TEST 100
#define NO_ITERATIONS 3000000
class SiKeyboard;
Boolean keyboardEventLoop(EventPtr event);
enum { VERTICAL,HORIZONTAL };

struct Position;
struct block;

class SiEditor
{
public:
	// SiEditor(const Int16 height, const Int16 top_y, const UInt16 index);
	SiEditor(const Int16 height, const Int16 top_y, const UInt16 index) __attribute__ ((section("editfns")));
	~SiEditor() __attribute__ ((section("editfns")));
	void resize(Int16 height, const Int16 top_y) __attribute__ ((section("editfns")));
	void set_ignore_events() __attribute__ ((section("editfns")));
	void clear_ignore_events() __attribute__ ((section("editfns")));

	//event handling
	Boolean handle_key(const WChar keycode, const UInt16 modifiers) __attribute__ ((section("editfns")));
	void handle_scroll(const Int16 old_v, const Int16 new_v) __attribute__ ((section("editfns")));
	void handle_menu_command(const UInt16 id)  __attribute__ ((section("editfns")));
	void handle_commandbar_popup()  __attribute__ ((section("editfns")));

	void give_focus() __attribute__ ((section("editfns")));
	void lost_focus() __attribute__ ((section("editfns")));
	
	const Char * get_initial_filename() __attribute__ ((section("editfns")));
	//notifications
	void notify_block_deleted(const Int16 line) __attribute__ ((section("editfns")));
	void redraw() __attribute__ ((section("editfns")));

	//event handlers
	void handle_tap(Coord x, Coord y,Int16 n_taps) __attribute__ ((section("editfns")));
	Position get_min_selected_position() const __attribute__ ((section("editfns")));
	Position get_max_selected_position() const __attribute__ ((section("editfns")));
	Char* get_filename() __attribute__ ((section("editfns")));
	fontID get_font() const __attribute__ ((section("editfns")));
	void initialise() __attribute__ ((section("editfns")));
	//void redraw();
	void handle_cursor_movement(const UInt16 direction,const Int16 amount, const UInt16 mod) __attribute__ ((section("editfns")));
	void delete_forward_cursor(const UInt16 mod) __attribute__ ((section("editfns")));
	void delete_backward_cursor(const UInt16 mod) __attribute__ ((section("editfns")));
	void redraw_chrome(Boolean force=true) __attribute__ ((section("editfns")));
	//public state for speed
	Int16 HEIGHT;
	Int16 TOP_Y, BOTTOM_Y;
	Int16 COMMAND_LINE;
	Boolean is_focus_tap(const Int16 x,const Int16 y)  __attribute__ ((section("editfns")));
	void toggle_find_whole_word() __attribute__ ((section("editfns")));
	void toggle_find_match_case() __attribute__ ((section("editfns")));
	void toggle_find_from_top() __attribute__ ((section("editfns")));
	Boolean point_in_drag_area(const Int16 x,const Int16 y) __attribute__ ((section("editfns")));
	void initialise_replace_fields() __attribute__ ((section("editfns")));
	Boolean finished_processing() const __attribute__ ((section("editfns")));
	void do_quick_slosh() __attribute__ ((section("editfns")));
	void handle_control(EventPtr event) __attribute__ ((section("editfns")));
	Int16 get_cursor_line() const __attribute__ ((section("editfns")));
#ifdef TEST_OBJECT_UNIT

	Boolean perform_tests() __attribute__ ((section("editfns")));
	void test_undo() __attribute__ ((section("editfns")));
	void test_replace() __attribute__ ((section("editfns")));
	void test_large_insert() __attribute__ ((section("editfns")));
	void test_char_insert() __attribute__ ((section("editfns")));
	void test_word_wrap() __attribute__ ((section("editfns")));
	void insert_test_text() __attribute__ ((section("editfns")));
	void test_reopen() __attribute__ ((section("editfns")));
	void test_find() __attribute__ ((section("editfns")));
	void test_clipboard() __attribute__ ((section("editfns")));
	void test_selection() __attribute__ ((section("editfns")));
	void test_redraw() __attribute__ ((section("editfns")));
	void test_controls() __attribute__ ((section("editfns")));
	void test_file_save() __attribute__ ((section("editfns")));
#endif

#ifdef BENCH_OBJECTS

	Boolean perform_benchmark()  __attribute__ ((section("editfns")));
#endif
	Boolean keyboard_is_open()  __attribute__ ((section("editfns")));
	UInt8 m_index;
	void erase_not_found() __attribute__ ((section("editfns")));
	void resize_for_dialog(const Int16 height) __attribute__ ((section("editfns")));

	void return_to_original_size() __attribute__ ((section("editfns")));
	void finished_replace() __attribute__ ((section("editfns")));
	Boolean is_displaying_dialog() const __attribute__ ((section("editfns")));
	void reset_from_top_find() __attribute__ ((section("editfns")));
	void read_preferences(Char * file_to_open) __attribute__ ((section("editfns")));
	void make_cursor_visible() __attribute__ ((section("editfns")));

	//editor actions interface (callable from some mythical macro language one day)
	void clear_selection() __attribute__ ((section("editfns"))) ;
	Boolean delete_selection() __attribute__ ((section("editfns"))) ;
	void undo() __attribute__ ((section("editfns")));
	void redo() __attribute__ ((section("editfns")));
	void paste_clipboard() __attribute__ ((section("editfns"))) ;
	void move_cursor_to(Position pos) __attribute__ ((section("editfns"))) ;
	void save_current_file() __attribute__ ((section("editfns"))) ;
	void change_font(const fontID font) __attribute__ ((section("editfns")));
	void start_waiting() __attribute__ ((section("editfns")));
	void stop_waiting() __attribute__ ((section("editfns")));
	Boolean replace_text(Char * rep_text,Char * with_text,const Boolean show_end) __attribute__ ((section("editfns")));
	Boolean find_term(char *term) __attribute__ ((section("editfns")));
	void replace_all(Char * rep_text,Char * with_text) __attribute__ ((section("editfns")));
	void insert_chars(const Char * data, const BlockInt n_chars,const BlockInt n_bytes) __attribute__ ((section("editfns")));
	void delete_chars(const Position start,const Position end) __attribute__ ((section("editfns")));
	void set_selection(const Position start,const Position end) __attribute__ ((section("editfns")));
	Boolean get_read_only() __attribute__ ((section("editfns")));
	Int16 get_min_height() const __attribute__ ((section("editfns")));
	void hide() __attribute__ ((section("editfns")));
	void show() __attribute__ ((section("editfns")));
private:
	void update_control(FormPtr frm,UInt16 id) __attribute__ ((section("editfns")));
	void move_controls() __attribute__ ((section("editfns")));
	void set_viewport() __attribute__ ((section("editfns")));
	void show_replace_controls() __attribute__ ((section("editfns")));
	void hide_replace_controls() __attribute__ ((section("editfns")));

	void do_scroll(const Int16 amount,const Boolean scroll,const Boolean quick=true) __attribute__ ((section("editfns")));
	void scroll_page(const Int16 dir,const UInt16 modifiers) __attribute__ ((section("editfns")));
	Boolean handle_inc_search_key(WChar the_char) __attribute__ ((section("editfns")));
	void do_incremental_search() __attribute__ ((section("editfns")));
	void stop_inc_search() __attribute__ ((section("editfns")));
	void set_field_term(FieldType* text_field,MemHandle handle,Boolean set_selection);
	//	void copy_field_term(FieldType * text_field,MemHandle &handle);
	void check_scroll_position() __attribute__ ((section("editfns")));
	void do_open_file() __attribute__ ((section("editfns")));
	void do_save_file_as() __attribute__ ((section("editfns")));
	Boolean save_doc_file(Char * name) __attribute__ ((section("editfns")));
	Boolean open_doc_file(Char * name) __attribute__ ((section("editfns")));
	void do_file_manage() __attribute__ ((section("editfns")));
	void save_preferences() __attribute__ ((section("editfns")));
	void do_new_file() __attribute__ ((section("editfns")));
	void delete_line(const Int16 line) __attribute__ ((section("editfns")));
	void select_word(Coord x,Coord y) __attribute__ ((section("editfns"))) ;
	Boolean ui_tap(Coord x,Coord y) __attribute__ ((section("editfns"))) ;

	void update_word_count() __attribute__ ((section("editfns"))) ;
	void draw_stats_data() __attribute__ ((section("editfns"))) ;
	void revert_curr_file() __attribute__ ((section("editfns"))) ;
	Boolean point_in_line_number(Coord x,Coord y) __attribute__ ((section("editfns"))) ;
	void select_current_word() __attribute__ ((section("editfns"))) ;
	Boolean ensure_tidied(const Int16 line) __attribute__ ((section("editfns"))) ;
	//search and replace
	Boolean do_replace_replace_next(Boolean show_end) __attribute__ ((section("editfns"))) ;
	void do_replace_find_next() __attribute__ ((section("editfns")));
	void do_replace_replace_all() __attribute__ ((section("editfns")));

	friend Boolean replaceEventLoop(EventPtr event) __attribute__ ((section("editfns")));
	friend Boolean stats_dialog_loop(EventPtr event) __attribute__ ((section("editfns")));
	//editor state data get functions
	void set_open_file() __attribute__ ((section("editfns"))) ;
	//Position get_start_selected_position() const;
	//Position get_end_selected_position() const;
	Char * get_prev_search_term() __attribute__ ((section("editfns"))) ;
	Position get_c_pos() const __attribute__ ((section("editfns")));

	Boolean check_whole_word_match(Position, Position) __attribute__ ((section("editfns"))) ;
	Boolean over_threshold(const Int16 x,const Int16 y) __attribute__ ((section("editfns"))) ;
	void make_visible_selection(Position start_pos, Position end_pos) __attribute__ ((section("editfns")));
	void popup_menu() __attribute__ ((section("editfns")));
	void reopen() __attribute__ ((section("editfns")));


	void display_stats_dialog() __attribute__ ((section("editfns")));
	void update_cursor_pos(const Position& p,const Boolean erase) __attribute__ ((section("editfns")));

	// cursor insertion/deletion functions
	void insert_ch_cursor(const WChar c) __attribute__ ((section("editfns")));
	void clear_control(const UInt16 id)__attribute__ ((section("editfns")));




	void notify_selection_changed() __attribute__ ((section("editfns"))) ;
	void set_sizes(const fontID font_id) __attribute__ ((section("editfns")));

	// selection/clipboard

	Boolean copy_selection_to_clipboard() __attribute__ ((section("editfns"))) ;

	void set_end_selected_position(Position p,Boolean check_newline=true) __attribute__ ((section("editfns"))) ;
	void set_start_selected_position(Position P) __attribute__ ((section("editfns"))) ;
	void close_file(Boolean ask=false) __attribute__ ((section("editfns"))) ;
	//dialogs
	//void do_find_dialog() __attribute__ ((section("editfns"))) ;
	void do_replace_dialog() __attribute__ ((section("editfns"))) ;
	void show_not_found() __attribute__ ((section("editfns"))) ;
	void toggle_read_only_mode() __attribute__ ((section("editfns")));
	void check_gsi_status() __attribute__ ((section("editfns")));
#ifdef NEW_LINE_OPTIONS

	Boolean do_new_line_insert(const WChar c) __attribute__ ((section("editfns")));
#endif

	Boolean valid_tap(const Int16 x, const Int16 y) __attribute__ ((section("editfns")));
#ifdef REPORT_MEM_USAGE

	void show_mem_usage() __attribute__ ((section("editfns"))) ;
#endif

	void do_typing() __attribute__ ((section("editfns")));
#ifdef COLLECT_STATISTICS

	void detailed_benchmarks() __attribute__ ((section("editfns")));
#endif
	void display_keyboard(const KeyboardType k) __attribute__ ((section("editfns")));

	//internal state data
	Position cursor_pos;
	Int16 preferred_cursor_x;

	Position start_selected_position;
	Position end_selected_position;

	Boolean ignore_events;
	SiTextView *m_text_view;
	SiFileHandler *m_file_handler;
	SiTextHandler *m_text_handler;
	//	MemHandle prev_search_term;
	//	MemHandle prev_replace_term;
	// find options
	Boolean find_option_match_case;
	Boolean find_option_whole_word;
	Boolean find_option_from_top;
	Boolean first_find;
	Boolean exiting;
	UInt8 selection_mode;
	SiKeyboard * m_keyboard;

	Boolean m_active;
	FieldPtr Altfield;
	FormPtr mfrm;
	fontID m_font_id;
	Int16 prev_height;

	SiDocument * m_document;
	Boolean m_incremental_search;

	SiMiniBuffer * m_mini_buffer;
	Boolean m_not_found;
	IndexedColorType m_not_found_colour;
	Int16 m_initial_height;
	Int16 m_initial_top;

	Boolean m_displaying_dialog;
	Boolean m_has_found_term;
	Boolean m_read_only;
};

#endif
