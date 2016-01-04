//
// C++ Interface: sidocument
//
// Description:
//
//
// Author: Benjamin Roe <sied@benroe.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SIDOCUMENT_H
#define SIDOCUMENT_H
#include <PalmOS.h>
#include <PalmTypes.h>
#include "types.h"
#include "sections.h"
#define NOT_CALC -1

class SiTextView;
/**
@author Benjamin Roe
*/
enum {TEXT_MODIFIED,TEXT_DELETED,LINE_DELETED,LINE_INSERTED};

class SiTextChange
{
public:
	Position start;
	Position end;
	Int16 type;
};

class SiDocument
{
public:
	SiDocument() __attribute__ ((section ("texfns")));

	~SiDocument() __attribute__ ((section ("texfns")));
	block * get_line(const Int16 line,Int16 & b_b_index) __attribute__ ((section ("texfns")));
	void release_line(Int16 b_b_index) __attribute__ ((section ("texfns")));

	Int16 get_number_blocks() const __attribute__ ((section ("texfns")));
	UInt32 get_doc_length() __attribute__ ((section ("texfns")));
	UInt32 get_number_chars() __attribute__ ((section ("texfns")));
	Boolean insert_block_at_end(SiMemChunk * chunk,BlockInt n_chars) __attribute__ ((section ("texfns")));
	void made_change(const Position start,const Position end) __attribute__ ((section ("texfns")));
	Int16 get_start_index(block_buffer * b_buffer)  __attribute__ ((section ("texfns")));
	//Cursor positioning
	void move_position(Position & pos,Int16 amount,Boolean new_line=true,Boolean is_cursor=true) __attribute__ ((section ("texfns")));
	void make_valid_pos(Position & p) __attribute__ ((section ("texfns")));
	void remove_block_buffer(block_buffer* b_buffer)  __attribute__ ((section ("texfns")));
	void move_to_prev_block(const Int16 line, block * src_block,block * dst_block,const BlockInt byte_offset,const BlockInt char_offset) __attribute__ ((section ("texfns")));
	inline Boolean wrap_back(Position  start_pos,Position & first_change_pos,Position & cursor_pos,Boolean & increase_redraw) __attribute__ ((section ("texfns")));
	void correct_position(Position & p) __attribute__ ((section ("texfns")));
	void remove_empty_lines() __attribute__ ((section ("texfns")));
	void move_up_data(block * b,const Position pos,const BlockInt bytes) __attribute__ ((section ("texfns")));
	void move_down_data(block * b,const Position pos,const BlockInt bytes) __attribute__ ((section ("texfns")));
	void check_eff_insert_line(Int16 start_line,BlockInt char_offset) __attribute__ ((section ("texfns")));
	Boolean move_to_next_block(const BlockInt char_offset,const BlockInt byte_offset,const Int16 start_line) __attribute__ ((section ("texfns")));
	void free_text()  __attribute__ ((section ("texfns")));

	Position tidy_lines(const Position start_pos,const BlockInt byte_offset,Position & first_change_pos,Position & cursor_pos) __attribute__ ((section ("texfns")));

	Boolean tidy_line_too_long(Position start_pos,BlockInt  byte_offset,Int16 & curr_width,Boolean & changed,Boolean * increase_redraw) __attribute__ ((section ("texfns")));

	Position tidy_line_too_short(const Int16 line,Int16 & curr_width,Boolean & changed,Boolean & increase_redraw) __attribute__ ((section ("texfns")));
	Boolean find_prev_delimiter(const Char * data, BlockInt& start_offset,BlockInt & char_offset,const Boolean before) __attribute__ ((section ("texfns")));
	Boolean find_delimiter(const SiMemChunk * chunk,BlockInt start_offset) __attribute__ ((section ("texfns")));
	void check_new_broken_word( Position start_pos,Position & cursor_pos,Boolean & changed,Position & first_change_pos) __attribute__ ((section ("texfns")));
	void check_final_block_allocated() __attribute__ ((section ("texfns")));
	Char * get_char_bound(const Int16 index,Char* data,const BlockInt char_offset,BlockInt *offset) __attribute__ ((section ("texfns")));
	WChar get_char(Int16 line,block *b,const BlockInt offset) __attribute__ ((section ("texfns")));
	Boolean inserted_text(const BlockInt n_chars,const BlockInt n_bytes,const Int8 grow) __attribute__ ((section ("texfns")));
	//void removed_block() __attribute__ ((section ("texfns")));
	//void added_block() __attribute__ ((section ("texfns")));

	Boolean file_dirty() const  __attribute__ ((section ("texfns")));
	void clear_dirty_flag()  __attribute__ ((section ("texfns")));
	void set_dirty_flag()  __attribute__ ((section ("texfns")));
	Position find_end_line(const Int16 line) __attribute__ ((section ("texfns")));
	void check_newline_position(Position& p,const Int16 dir)  __attribute__ ((section ("texfns")));
	const Char * get_stripped_text(const Position start,const Position end, const BlockInt max_chars) __attribute__ ((section ("texfns")));
	void insert_block(const Int16 prev_line) __attribute__ ((section ("texfns")));

	void remove_block(const Int16 line) __attribute__ ((section ("texfns")));
	void remove_text_range(const Int16 line,const BlockInt  start_x,BlockInt  end_x) __attribute__ ((section ("texfns")));
	Position find_end_document()  __attribute__ ((section ("texfns")));
	Boolean has_done_insert() const __attribute__ ((section ("texfns")));
	void ready_for_change() __attribute__ ((section ("texfns")));

	void invalidate_cache() __attribute__ ((section ("texfns")));
	//	void check_cache(const Int16 start_line,const Int16 end_line) __attribute__ ((section ("texfns")));

	void set_last_tidied_line(const Int16 line)  __attribute__ ((section ("texfns")));
	Boolean ensure_allocated(Int16 index) __attribute__ ((section ("texfns")));

	void clear_changes() __attribute__ ((section ("texfns")));
	SiTextChange * get_next_change() __attribute__ ((section ("texfns")));

	Boolean get_done_insert() const __attribute__ ((section ("texfns")));
	void set_view(SiTextView * view) __attribute__ ((section ("texfns")));
	const Char * get_initial_filename() __attribute__ ((section ("texfns")));
	void finished_initial_opening() __attribute__ ((section ("texfns")));
#ifdef TEST_OBJECT_UNIT

	Boolean perform_tests() __attribute__ ((section ("texfns")));
#endif

	void set_opening_file_mode() __attribute__ ((section ("texfns")));
	void clear_opening_file_mode() __attribute__ ((section ("texfns")));
	Int16 get_apparent_number_blocks() const __attribute__ ((section ("texfns")));
	void set_apparent_number_blocks(const Int16 num) __attribute__ ((section ("texfns")));
	Boolean ensure_tidied(const Int16 line) __attribute__ ((section ("texfns")));
	void tidy_up_lines(Int16 line) __attribute__ ((section ("texfns")));
	Int16 last_line_tidied;
	Boolean has_had_memory_error() const __attribute__ ((section ("texfns")));
	void set_memory_error(Boolean m) __attribute__ ((section ("texfns")));
private:

	Boolean line_ends_with_line_feed(const Int16 l) __attribute__ ((section ("texfns")));


	Boolean ends_in_line_feed(const Int16 l) __attribute__ ((section ("texfns")));
	void allocate_new_block_buffers(const Int16 num) __attribute__ ((section ("texfns")));
	block_buffer* get_block_buffer(const Int16 line,Int16 * start_index,Int16 &b_b_index) __attribute__ ((section ("texfns")));
	//internal state
	SiTextChange * text_change_list;
	MemHandle text_change_handle;
	UInt16 number_changes;
	UInt16 curr_change;
	Boolean dirty_flag;
	UInt32 max_char;
	UInt32 document_length;
	Int16 number_blocks;
	Int16 number_block_buffers;

	//data
	MemHandle block_buffer_array_handle;
	block_buffer* block_buffer_array;

	//cache
	block_buffer* prev_locked_block_buffer;
	Int16 prev_locked_start_index;
	//Int16 cached_get_line_index;
	//nt16 cached_line_block_buffer_index;
	//Int16 cached_line_start_index;
	//Int16 prev_locked_line;
	SiTextView * m_text_view;

#ifdef USE_CHAR_CACHE

	Int16 cached_get_char_block_index;
	BlockInt cached_bound_offset;
	BlockInt cached_bound_byte_offset;
#endif

	Boolean allow_change_add;
	Boolean done_insert;
#ifdef USE_LINE_CACHE

	Int16 cached_get_line_index;
	Int16 cached_line_block_buffer_index;
	Int16 cached_line_start_index;
#endif

	Boolean m_opening_file;
	Int16 apparent_number_blocks;
	Int32 line_width_estimate;
	Int32 n_lines_estimated;
	Boolean m_memory_error_occurred;
	Boolean m_background_tidy;

};

#endif
