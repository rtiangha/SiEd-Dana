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
#include "pdb_dirnode.h"
#ifndef DOC_SIDIRNODE_HPP
#define DOC_SIDIRNODE_HPP


class doc_SiDirNode : public pdb_SiDirNode
{
public:
	doc_SiDirNode(Char * filename);
	virtual SiFile * get_file(Char * filename) __attribute__ ((section ("diafns")));
	virtual Int16 get_position_index() const __attribute__ ((section ("diafns")));
	virtual SiDirNode * get_copy() __attribute__ ((section ("diafns")));
};

#endif
