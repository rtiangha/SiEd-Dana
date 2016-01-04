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

#include "doc_dirnode.h"

#include "doc_file_interface.h"
#include "error.h"
doc_SiDirNode::doc_SiDirNode(Char * filename):pdb_SiDirNode(filename)
{
#ifdef EN_LOG
	log_entry("doc_SiDirNode ctor");
#endif

	select_action=ACTION_SELECT_FILE;
	options=DISPLAY_FILENAME|CAN_BEAM_FILE;

}

SiFile * doc_SiDirNode::get_file(Char * filename)
{
	return new doc_SiFile(name,name,cardNo);
}
Int16 doc_SiDirNode::get_position_index() const
{
#ifdef EN_LOG
	log_entry("doc_SiDirNode::get_position_index");
#endif

	return DOC_POSITION;
}
SiDirNode * doc_SiDirNode::get_copy()
{
	SiDirNode * ret_node=new doc_SiDirNode(name);
	ret_node->volRefNum=volRefNum;
	ret_node->parent=parent;
	return ret_node;

}
