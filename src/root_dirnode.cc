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

#include "root_dirnode.h"
#include "div_dirnode.h"

#include "doc_file_interface.h"

#include "utility.h"
#include "VFSMgr.h"

#include "vfsdir_dirnode.h"
#include "pdb_dirnode.h"
#include "doc_dirnode.h"

#include "vfsvol_dirnode.h"
#include "error.h"


root_SiDirNode::root_SiDirNode()
{
	//	name=FORWARD_SLASH;
	parent=NULL;
	name=NULL;

	options&=TOP_LEVEL;
}

Char * root_SiDirNode::get_path(UInt16 type)
{
	Char * ret=(Char*)MemPtrNew(StrLen(MAIN_DIRNODE_TEXT)+1);
	StrCopy(ret,MAIN_DIRNODE_TEXT);
	return ret;
}

SiFile * root_SiDirNode::get_file(Char * filename)
{
	//default is now doc file
	//this should never be called really
	//except to check if file exists
#ifdef EN_LOG
	log_entry("root_SiDirNode get_file");
#endif

	SiFile * t_file=new doc_SiFile(filename,filename,0);

	if(!t_file->is_open())
	{
		delete t_file;
		t_file=NULL;
	}
#ifdef EN_LOG
	log_entry("get_file done");
#endif

	return t_file;
}

SiFile * root_SiDirNode::create_file(Char * filename,UInt32 size)
{
	SiFile * t_file=new doc_SiFile(filename,filename,0,size);
	return t_file;
}

void root_SiDirNode::populate()
{

	//remove current children
	flush_children();


	if(SiUtility::VFS_SUPPORT)
		add_vfs_volumes();

	SiDirNode * div=new div_SiDirNode();

	add_child(div);

	add_doc_files();


}

void root_SiDirNode::add_vfs_volumes()
{

	UInt16 l_volRefNum;
	UInt32 volIterator = vfsIteratorStart;
	SiDirNode* child;//=new vfsvol_SiDirNode(NULL);

	Char buff[255];
	Err err;
	while (volIterator != vfsIteratorStop)
	{

		err = VFSVolumeEnumerate(&l_volRefNum, &volIterator);
		if (err == errNone)
		{
			// add the volume to the parent node
			buff[0]='\0';
			err=VFSVolumeGetLabel(l_volRefNum,buff,255);
			if(err==errNone)
			{
				if(StrLen(buff)==0)
				{
					StrPrintF(buff,"Card %u",volRefNum);
				}
				child=new vfsvol_SiDirNode(buff);
				//				child->name=(Char*)MemPtrNew(StrLen(buff)+1);

				//				StrCopy(child->name,buff);

				child->volRefNum=l_volRefNum;
				add_child(child);
			}

		}
		else
		{
			// handle error... break out
			break;
		}
	}
}

void root_SiDirNode::add_databases_by_type(SiDirNode * node,UInt32 creator,UInt32 type)
{
	Err err;
	Boolean newsearch = true;
	UInt16 card=THE_CARD;
	LocalID l_dbID=0;
	DmSearchStateType stateInfo;
	Char buff[70];
	SiDirNode * copy_node;
	do
	{
		err =
			DmGetNextDatabaseByTypeCreator(newsearch, &stateInfo,
										   type,
										   creator,
										   (Boolean) false, &card,
										   &l_dbID);

		if(err==0)
		{
			DmDatabaseInfo(card, l_dbID, buff, NULL, NULL, NULL, NULL, NULL,
						   NULL, NULL, NULL, NULL, NULL);

			if (!SiUtility::system_file(buff))
			{
				copy_node=node->get_copy();
				copy_node->name=(Char*)MemPtrNew(StrLen(buff)+1);
				copy_node->dbID=l_dbID;
				copy_node->cardNo=card;
				StrCopy(copy_node->name,buff);

				add_child(copy_node);
			}
			newsearch = false;
		}
	}
	while (err == 0);


}
void root_SiDirNode::add_doc_files()
{
#ifdef EN_LOG
	log_entry("add_doc_files 1");
#endif

	SiDirNode * node=new doc_SiDirNode(NULL);
#ifdef EN_LOG

	log_entry("add_doc_files 2");
#endif

	add_databases_by_type(node,0,DOC_TYPE);
#ifdef EN_LOG

	log_entry("add_doc_files 3");
#endif

	MemPtrFree(node);
}
SiDirNode* root_SiDirNode::get_copy()
{
	return new root_SiDirNode();
}
