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
#include "sections.h"
#ifndef DIRNODE_HH
#define DIRNODE_HH
#include <PalmOS.h>
#include <PalmTypes.h>
class SiFile;
#define TOP_LEVEL 4
#define CAN_BEAM_FILE 8
#define CREATE_DIRECTORIES 16
#define DISPLAY_FILENAME 32
enum {ACTION_CHANGE_DIR,ACTION_SELECT_FILE,ACTION_MOVE_UP_DIR};

#define STANDARD 0
#define EXCLUDE_LEAF_NODE 1
#define INCLUDE_CARD 2
#include "resources.h"

#define VFSVOL_POSITION 1
#define VFSDIR_POSITION 2
#define VFSFILE_POSITION 3
#define DOC_POSITION 4
#define PDB_POSITION 5

class SiDirNode
{
public:
	SiDirNode(Char * name,const UInt16 n_children);
	//	SiDirNode(SiDirNode* d);
	SiDirNode();
	virtual ~SiDirNode();
	virtual Char * get_path(const UInt16 type=STANDARD)  __attribute__ ((section("diafns")));
	virtual SiFile * get_file(Char * filename) __attribute__ ((section ("diafns")));
	virtual SiFile * create_file(Char * filename,UInt32 size) __attribute__ ((section ("diafns")));
	virtual void populate() __attribute__ ((section ("diafns")));
	virtual SiDirNode * get_copy() __attribute__ ((section ("diafns")));
	virtual UInt32 get_file_size() __attribute__ ((section ("diafns")));
	virtual UInt32 get_file_date() __attribute__ ((section ("diafns")));

	SiDirNode ** child_nodes;
	Boolean is_encrypted() __attribute__ ((section ("diafns")));
	void flush_children() __attribute__ ((section ("diafns"))) ;
	virtual void tidy()  __attribute__ ((section ("diafns")));
	virtual void draw_self(UInt16 x,const UInt16 y,const UInt16 max_width)  __attribute__ ((section ("diafns")));
	SiDirNode* add_child(SiDirNode * node) __attribute__ ((section ("diafns")));
	SiDirNode * get_child(Char * name) __attribute__ ((section ("diafns")));
	SiDirNode * get_child(UInt16 vol) __attribute__ ((section ("diafns")));
	virtual Int16 get_position_index() const __attribute__ ((section ("diafns")));
	void sort_children() __attribute__ ((section ("diafns")));
	//SiDirNode & operator =(SiDirNode & rhs) __attribute__ ((section ("diafns")));
	SiDirNode * parent;
	UInt16 volRefNum;
	LocalID dbID;
	UInt16 cardNo;
	UInt16 options;
	UInt16 select_action;
	UInt16 number_children;
	Char * name;
#ifdef TEST_OBJECT_UNIT

	void perform_tests() __attribute__ ((section ("diafns")));
#endif

protected:
	void swap_elements(SiDirNode ** partition,Int32 el_1,Int32 el_2) __attribute__ ((section ("diafns")));

	void sort_partition(SiDirNode** partition,const UInt16 begin, const UInt16 end) __attribute__ ((section ("diafns")));
	MemHandle children_handle;
	UInt16 m_is_encrypted;
	UInt32 image_id;
	UInt16 x_offset;
	UInt32 file_size;
	UInt32 file_date;
	FontID m_font;
};

Boolean compare_less(const SiDirNode * lhs,const SiDirNode * rhs) __attribute__ ((section ("diafns")));
Boolean compare_less_equal(const SiDirNode * lhs,const SiDirNode * rhs) __attribute__ ((section ("diafns")));
#endif
