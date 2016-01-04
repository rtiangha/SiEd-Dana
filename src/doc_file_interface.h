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
#ifndef DOCFILE_HPP
#define DOCFILE_HPP
#include "pdb_file_interface.h"



class doc_SiFile: public pdb_SiFile
{
public:
	doc_SiFile(const Char * name,const Char * location,const UInt16 volRefNum);
	doc_SiFile(const Char * name,const Char * location,const UInt16 volRefNum,const UInt32 size);
	virtual ~doc_SiFile();

	virtual void open() ;
	virtual void resize(const UInt32 size) __attribute__ ((section ("filefns")));

	virtual UInt32 get_size() __attribute__ ((section ("filefns")));
	virtual void move_cursor(const Int32 amount,const Int16 dir) __attribute__ ((section ("filefns")));
	virtual void set_access_mode(const Int16 mode) __attribute__ ((section ("filefns")));
protected:
	void remove_last_record() __attribute__ ((section ("filefns")));
	UInt16 last_record_remaining_space() __attribute__ ((section ("filefns")));
	UInt16 last_record_size() __attribute__ ((section ("filefns")));
	void update_record_cursor(const UInt16 amount,const Int16 dir) __attribute__ ((section ("filefns")));
	void resize_last_record(const Int32 size_to_add) __attribute__ ((section ("filefns")));
	Boolean read_header() __attribute__ ((section ("filefns")));
	Boolean write_header() __attribute__ ((section ("filefns")));

	Boolean add_records(const UInt32 total_size) __attribute__ ((section ("filefns")));
	Boolean remove_records(const UInt32 total_size) __attribute__ ((section ("filefns")));
	virtual Boolean create_database(const UInt32 size) __attribute__ ((section ("filefns")));

	virtual void write_direct(const SiMemChunk * const chunk);
	virtual Int16 read_direct(SiMemChunk * chunk);
	void lock_next_record() __attribute__ ((section ("filefns")));
	void lock(const UInt16 text_record)
		__attribute__ ((section ("filefns")));

	UInt16 header_size;
	UInt16 curr_record_size;
	UInt16 curr_record_cursor;
};
#endif
