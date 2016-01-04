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
#include "dirnode.h"
#ifndef VFS_SIDIRNODE_HPP
#define VFS_SIDIRNODE_HPP
class vfs_SiDirNode : public SiDirNode
{
public:
	vfs_SiDirNode(Char * filename) __attribute__ ((section ("diafns")));

	virtual SiFile * get_file(Char * name) __attribute__ ((section ("diafns")));
	virtual SiFile * create_file(Char * name,UInt32 size) __attribute__ ((section ("diafns")));
	virtual void populate() __attribute__ ((section ("diafns")));
	virtual UInt32 get_file_size() __attribute__ ((section ("diafns")));
	virtual UInt32 get_file_date() __attribute__ ((section ("diafns")));
};

#endif
