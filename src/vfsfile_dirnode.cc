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

#include "vfsfile_dirnode.h"
#include "vfs_file_interface.h"
#include "error.h"
vfsfile_SiDirNode::vfsfile_SiDirNode(Char * file_name):vfs_SiDirNode(file_name)
{
	options=CREATE_DIRECTORIES|DISPLAY_FILENAME;
	select_action=ACTION_SELECT_FILE;

}
SiFile * vfsfile_SiDirNode::get_file(Char * filename)
{
	Char * path=get_path(STANDARD);
	vfs_SiFile * t_file=new vfs_SiFile(name,path,volRefNum);
	MemPtrFree(path);
	if(t_file->is_open())
	{
		return t_file;
	}
	else
	{
		delete t_file;
		return NULL;
	}
}
Int16 vfsfile_SiDirNode::get_position_index() const
{
	return VFSFILE_POSITION;
}
SiDirNode * vfsfile_SiDirNode::get_copy()
{
	SiDirNode * ret_node=new vfsfile_SiDirNode(name);
	ret_node->volRefNum=volRefNum;
	ret_node->parent=parent;
	return ret_node;

}
