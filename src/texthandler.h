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
#include "sections.h"
#include "sidocument.h"

#ifndef TEXTHANDLER_HH
#define TEXTHANDLER_HH


enum {INSERTED_TEXT,DELETED_TEXT};

class SiUndoAction
{
public:
	SiUndoAction() __attribute__ ((section ("texfns")));
	~SiUndoAction() __attribute__ ((section ("texfns")));
	SiMemChunk * text;
	Position start_position;
	Position start_undone_position;
	Position end_position;
	UInt8 status;
	SiUndoAction * next;
	SiUndoAction * prev;
};

//forward declare SiEditor here
class SiEditor;

class SiTextHandler
{
public:
	SiTextHandler()  __attribute__ ((section ("texfns")));
	~SiTextHandler()  __attribute__ ((section ("texfns")));
	void initialise()  __attribute__ ((section ("texfns")));

	//document editing functions
	Boolean insert_chars(const Char* c,const BlockInt n_chars,const BlockInt n_bytes,Position  ch,Position & f_cursor_pos) __attribute__ ((section ("texfns")));
	Position delete_chars(Position start,Position end) __attribute__ ((section ("texfns")));

	//Undo functions
	Boolean undo(Position & f_cursor_pos) __attribute__ ((section ("texfns")));
	Boolean redo(Position & f_cursor_pos) __attribute__ ((section ("texfns"))) ;

	//Document data get functions

	Position get_prev_pos(Position p) __attribute__ ((section ("texfns")));
	Position get_next_pos(Position p) __attribute__ ((section ("texfns")));
	UInt32 get_number_words(UInt32 & alphanum_chars) __attribute__ ((section ("texfns")));

	Position find_prev_word_boundary(Position start,const Boolean ignore_punctuation=true) __attribute__ ((section ("texfns")));
	Position find_next_word_boundary(Position start,const Boolean ignore_punctuation=true) __attribute__ ((section ("texfns")));

	WChar get_charAtpos(Position &p) __attribute__ ((section ("texfns")));
	WChar get_charBeforepos(Position &p) __attribute__ ((section ("texfns"))) ;
	SiDocument * get_document() __attribute__ ((section ("texfns")));

#ifdef DEBUG

	Boolean perform_tests() __attribute__ ((section ("texfns")));
#endif

	void enable_undo_history() __attribute__ ((section ("texfns")));
	void disable_undo_history() __attribute__ ((section ("texfns")));


private:
	SiUndoAction * create_undo_action() __attribute__ ((section ("texfns")));
	Boolean ensure_undo_space(const BlockInt n_bytes) __attribute__ ((section ("texfns")));
	//BlockInt paste_efficiency_hack(const BlockInt line,const BlockInt n_chars) __attribute__ ((section ("texfns")));
	Boolean find_delimiter(const SiMemChunk * chunk,BlockInt start_offset)__attribute__ ((section ("texfns")));
	void free_text() __attribute__ ((section ("texfns")));
	Boolean validate_position(Position & p) __attribute__ ((section ("texfns")));

	void remove_text_range(const Int16 line,const BlockInt  start_x,BlockInt  end_x) __attribute__ ((section ("texfns")));
	Boolean remove_blocks(const Int16 start_line,const Int16 end_line) __attribute__ ((section ("texfns")));
	block* insert_new_blocks(const Int16 num) __attribute__ ((section ("texfns")));


	//line handling
	//	void remove_block(const Int16 line) __attribute__ ((section ("texfns")));


	Boolean add_undo_action(const Char * text,const BlockInt bytes,Position start_pos,Position end_pos,UInt8 type) __attribute__ ((section ("texfns")));
	Boolean append_text_undo_action(const Char * text,const BlockInt bytes) __attribute__ ((section ("texfns")));
	Boolean remove_last_undo_action() __attribute__ ((section ("texfns")));

	void clear_undo_history() __attribute__ ((section ("texfns")));
	void clear_actions_after(SiUndoAction * action) __attribute__ ((section ("texfns")));
	void perform_action(SiUndoAction * action,Boolean reverse,Position & f_cursor_pos) __attribute__ ((section ("texfns")));
	void change_undo_action_end(Position end_pos) __attribute__ ((section ("texfns")));
	void change_undo_action_start(Position pos) __attribute__ ((section ("texfns")));
	//sloshing
	void invalidate_cache() __attribute__ ((section ("texfns")));
	void add_block_delete_undo(const Int16 line) __attribute__ ((section ("texfns")));
	//Internal state data

	Position undo_position_start;
	Position undo_position_end;
	Boolean undo_undone;
	Int16 undo_action;
	Boolean undo_active;
	//	Int16 insert_block_curr_width;

	//document data storage
	//MemHandle block_buffer_array_handle;
	SiUndoAction * undo_action_list;
	SiUndoAction * curr_undo_action;
	SiUndoAction * last_undo_action;
	SiDocument * m_document;
	UInt16 undo_memory_usage;

	DmOpenRef m_database;
	LocalID m_dbID;


};
#endif
