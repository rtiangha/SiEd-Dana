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

#include "updir_dirnode.h"
#include "resources.h"
#include "error.h"
const Char * dots="..";

updir_SiDirNode::updir_SiDirNode()
{
	name=const_cast<Char*>(dots);
	image_id=UP_ARROW_BITMAP;
	select_action=ACTION_MOVE_UP_DIR;
	x_offset=ICON_WIDTH;
	children_handle=NULL;
	child_nodes=NULL;
	number_children=0;
	m_font=stdFont;
	m_is_encrypted=false;
}
updir_SiDirNode::~updir_SiDirNode()
{
	name=NULL;
}

SiFile * updir_SiDirNode::get_file(Char * filename)
{
	return NULL;
}

SiFile *updir_SiDirNode::create_file(Char * filename,UInt32 size)
{
	return NULL;
}
SiDirNode * updir_SiDirNode::get_copy()
{
	return new updir_SiDirNode();
}
