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

#include "vfs_file_interface.h"
#include "error.h"
#include "cache_file_plugin.h"

vfs_SiFile::vfs_SiFile(const Char * name,const Char * location,const UInt16 p_volRefNum):SiFile(name,location,p_volRefNum)
{
	volRefNum=p_volRefNum;
	m_open_mode=FILE_READ;
	init();
	open();
}

vfs_SiFile::vfs_SiFile(const Char * name,const Char * location,const UInt16 p_volRefNum,const UInt32 size):SiFile(name,location,p_volRefNum,size)
{
	volRefNum=p_volRefNum;
	m_open_mode=FILE_WRITE;
	init();
	open();
}

vfs_SiFile::~vfs_SiFile()
{
	close();
}

void vfs_SiFile::init()
{
	m_type=VFS_FILE;
	m_cursor=0;
}

void vfs_SiFile::open()
{
	if(!m_file_open)
	{
		Err err=VFSFileOpen(volRefNum,m_location,m_open_mode,&file_ref);
		if(err!=errNone)
		{
			m_file_open=false;
		}
		else
		{
			VFSFileSize(file_ref,&m_file_size);

			//check for read only
			UInt32 attr;
			VFSFileGetAttributes(file_ref,&attr);
			if(attr&vfsFileAttrReadOnly)
				m_is_read_only=true;
			VolumeInfoType vol_info;
			VFSVolumeInfo(volRefNum,&vol_info);

			if(vol_info.attributes&vfsVolumeAttrReadOnly)
				m_is_read_only=true;
			m_file_open=true;
			m_cursor=0;
		}
	}

}

void vfs_SiFile::close()
{
	if(m_file_open)
	{
		if(m_trans!=NULL)
			m_trans->finalise();
		VFSFileClose(file_ref);
		m_file_open=false;
	}

}

UInt32 vfs_SiFile::get_mod_time()
{
	//! Return the modication time of the file
	//! in seconds since 1904
	UInt32 create_time=0;
	if(m_file_open)
		VFSFileGetDate(file_ref,vfsFileDateModified,&create_time);

	return create_time;
}

void vfs_SiFile::set_access_mode(const Int16 mode)
{
	close();
	if(mode==FILE_WRITE)
	{
		SiFilePlugin * plug=new cache_SiFilePlugin(this);
		if(NULL!=plug)
		{
			add_plugin(plug,AT_START);
		}
	}
	m_open_mode=mode;
	open();
}

void vfs_SiFile::write_direct(const SiMemChunk * const chunk)
{
#ifdef DEBUG
	ErrFatalDisplayIf(!m_file_open,"Writing to closed VFS file");
#endif

	VFSFileWrite(file_ref,chunk->size,chunk->mem_ptr,NULL);
	m_cursor+=chunk->size;
}

Int16 vfs_SiFile::read_direct(SiMemChunk * chunk)
{
#ifdef DEBUG
	ErrFatalDisplayIf(!m_file_open,"Reading from closed VFS file");
#endif

	UInt32 bytes_read;
	VFSFileRead(file_ref,chunk->size,chunk->mem_ptr,&bytes_read);
	m_cursor+=bytes_read;
	return (Int16)bytes_read;
}

void vfs_SiFile::move_cursor(const Int32 amount,const Int16 dir)
{
	if(amount==0&&dir!=MOVE_TO)
		return;

	if(m_trans!=NULL)
		m_trans->flush();
	switch(dir)
	{
	case FORWARDS:
		m_cursor+=amount;
		if(m_cursor>=m_file_size)
			m_cursor=m_file_size-1;
		VFSFileSeek(file_ref,vfsOriginCurrent,amount);
		break;
	case BACKWARDS:
		if((UInt32)amount>m_cursor)
			m_cursor=0;
		else
			m_cursor-=amount;
		VFSFileSeek(file_ref,vfsOriginCurrent,-amount);
		break;
	case MOVE_TO:
		m_cursor=amount;
		VFSFileSeek(file_ref,vfsOriginBeginning,amount);
		break;
	}

}

void vfs_SiFile::resize(const UInt32 size)
{
	//resize is a NOP for VFS files as they are truncated and rewritten on every save`
}

void vfs_SiFile::rename(Char * new_name)
{
	Char * new_path_name;
	if(StrCompare(new_name,m_name)==0)
		return;

	new_path_name=get_file_dir(new_name);
	close();
	Err err=VFSFileRename(volRefNum,m_location,new_name);
	if(err!=errNone)
	{
		if(display_warnings)
			DisplayError(FILE_ACCESS_ERROR,new_name);
		return;
	}
	MemPtrFree(m_name);
	m_name=(Char*)MemPtrNew(StrLen(new_name)+1);
	StrCopy(m_name,new_name);
	MemPtrFree(m_location);
	m_location=(Char*)MemPtrNew(StrLen(new_path_name)+1);
	StrCopy(m_location,new_path_name);

}

Char * vfs_SiFile::get_file_dir(Char * new_file_name)
{
	//append new_file_name to the directory name of this file
	//copy it to a new string and return it
	if(m_location==NULL)
	{
		return NULL;
	}
	UInt16 path_length=StrLen(m_location)-StrLen(m_name);
	Char * dir=(Char*)MemPtrNew(path_length+StrLen(new_file_name)+1);

	MemMove(dir,m_location,path_length);
	StrCopy(dir+path_length,new_file_name);

	return dir;
}

void vfs_SiFile::delete_file()
{
	close();
	VFSFileDelete(volRefNum,m_location);

}
UInt16 vfs_SiFile::get_vol_ref()
{
	return volRefNum;
}
