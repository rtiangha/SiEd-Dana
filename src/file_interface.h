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
#include <PalmOS.h>
#include <PalmTypes.h>
#ifndef SIFILE_HPP
#define SIFILE_HPP
#include <Preferences.h>
#include "resources.h"
#include "dirnode.h"
#include "memhandler.h"
#include <VFSMgr.h>
class SiFilePlugin;

#define FILE_WRITE (vfsModeWrite|vfsModeTruncate|vfsModeCreate)
#define FILE_READ vfsModeRead

enum {FORWARDS,BACKWARDS,MOVE_TO};
enum {AT_END,AT_START};
#define NON_VFS 65000

class SiFile
{
public:
	SiFile(const Char * name,const Char * location,const UInt16 volRefNum)  __attribute__ ((section ("filefns")));
	SiFile(const Char * name,const Char * location,const UInt16 volRefNum,const UInt32 size) __attribute__ ((section ("filefns")));

	virtual ~SiFile();
	void add_plugin(SiFilePlugin * trans,const Int16 position)  __attribute__ ((section ("filefns")));
	void delete_plugin(const Int16 position) __attribute__ ((section ("filefns")));
	Char * get_name()  __attribute__ ((section ("filefns")));
	Char * get_location()  __attribute__ ((section ("filefns")));
	virtual UInt32 get_size()  __attribute__ ((section ("filefns")));
	Boolean is_open()  __attribute__ ((section ("filefns")));
	UInt16 get_type()  __attribute__ ((section ("filefns")));

	virtual UInt32 get_mod_time() __attribute__ ((section ("filefns")));
	virtual void open() __attribute__ ((section ("filefns")));
	virtual void close() __attribute__ ((section ("filefns")));
	virtual void set_access_mode(const Int16 mode) __attribute__ ((section ("filefns")));

	void write_data(const SiMemChunk* const chunk) __attribute__ ((section ("filefns")));
	SiMemChunk* read_data(BlockInt &n_chars) __attribute__ ((section ("filefns")));

	virtual void move_cursor(const Int32 distance,const Int16 dir) __attribute__ ((section ("filefns")));
	virtual void resize(const UInt32 size) __attribute__ ((section ("filefns")));
	virtual void rename(Char * new_name) __attribute__ ((section ("filefns")));
	virtual void delete_file() __attribute__ ((section ("filefns")));

	//files actions bypassing plugins
	//should only be called by plugin classes
	virtual UInt16 get_vol_ref() __attribute__ ((section ("filefns")));
	virtual void initialise(const Int16 file_mode) __attribute__ ((section ("filefns")));
	virtual Int16 read_direct(SiMemChunk * chunk) __attribute__ ((section ("filefns")));
	virtual void write_direct(const SiMemChunk * const chunk) __attribute__ ((section ("filefns")));
	SiFilePlugin * is_encrypted() __attribute__ ((section ("filefns")));
	static Boolean display_warnings;
	Boolean is_read_only();
protected:
	void change_name(const Char * new_name)  __attribute__ ((section ("filefns")));
	void change_location(const Char * new_location)  __attribute__ ((section ("filefns")));
	void init(const Char * name,const Char * p_location)  __attribute__ ((section ("filefns")));

	UInt32 m_file_size;
	UInt16 m_open_mode;
	Boolean m_is_read_only;
	UInt32 m_cursor;
	Char * m_name;
	Char * m_location;
	Boolean m_file_open;
	SiFilePlugin * m_trans;
	UInt16 m_type;

};
#endif
