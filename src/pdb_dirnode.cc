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
#include "VFSMgr.h"

#include "error.h"
pdb_SiDirNode::pdb_SiDirNode(Char * filename):SiDirNode(filename,0)
{
#ifdef EN_LOG
	log_entry("pdb_SiDirNode ctor");
#endif

	select_action=ACTION_SELECT_FILE;
	options=DISPLAY_FILENAME|CAN_BEAM_FILE;
}

UInt32 pdb_SiDirNode::get_file_size()
{
	UInt32 size;
	DmDatabaseSize(cardNo, dbID, NULL, NULL, &size);
	return size;
}

UInt32 pdb_SiDirNode::get_file_date()
{
	if(file_date==0)
	{

		DmDatabaseInfo(cardNo, dbID, NULL, NULL, NULL, NULL, &file_date, NULL,
					   NULL, NULL, NULL, NULL, NULL);
	}
	return file_date;
}

/*SiFile * pdb_SiDirNode::get_file(Char * filename)
{
	return new sied_SiFile(name,name,cardNo);
}*/

SiFile * pdb_SiDirNode::create_file(Char * filename,UInt32 size)
{
	return NULL;
}

SiDirNode * pdb_SiDirNode::get_copy()
{
	SiDirNode * ret_node=new pdb_SiDirNode(name);
	ret_node->volRefNum=volRefNum;
	ret_node->parent=parent;
	return ret_node;
}
Int16 pdb_SiDirNode::get_position_index()
{
	return PDB_POSITION;
}
