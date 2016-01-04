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
#include "file_interface.h"
#include "VFSMgr.h"
#ifndef VFSSIFILE_HPP
#define VFSSIFILE_HPP
class vfs_SiFile : public SiFile
{
public:
	vfs_SiFile(const Char * name,const Char * location,const UInt16 volRefNum);
	vfs_SiFile(const Char * name,const Char * location,const UInt16 volRefNum,const UInt32 size);
	virtual ~vfs_SiFile();
	virtual UInt32 get_mod_time() __attribute__ ((section ("diafns")));
	virtual void open() __attribute__ ((section ("diafns")));
	virtual void close() __attribute__ ((section ("diafns")));
	virtual void set_access_mode(const Int16 mode)  __attribute__ ((section ("diafns")));
	virtual void move_cursor(const Int32 distance,const Int16 dir);
	virtual void resize(const UInt32 size) __attribute__ ((section ("diafns")));
	virtual void rename(Char * new_name) __attribute__ ((section ("diafns")));
	virtual void delete_file() __attribute__ ((section ("diafns")));
	virtual UInt16 get_vol_ref() __attribute__ ((section ("diafns")));
protected:
	void init();
	Char * get_file_dir(Char * new_file_name) __attribute__ ((section ("diafns")));

	virtual void write_direct(const SiMemChunk * const chunk) __attribute__ ((section ("diafns")));
	virtual Int16 read_direct(SiMemChunk * chunk) __attribute__ ((section ("diafns")));
	UInt16 volRefNum;
	FileRef file_ref;

};

#endif
