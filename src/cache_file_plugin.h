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
#include "file_plugin.h"
#ifndef CACHE_PLUGIN_HPP
#define CACHE_PLUGIN_HPP
#include "error.h"
class cache_SiFilePlugin: public SiFilePlugin
{
public:
	cache_SiFilePlugin(SiFile * file);
	virtual ~cache_SiFilePlugin();

protected:
	friend void SiFile::write_direct(const SiMemChunk * const chunk);
	virtual Boolean do_initialise(const Int16 access_mode) __attribute__ ((section ("filefns")));
	virtual void do_finalise() __attribute__ ((section ("filefns")));
	virtual void do_flush();
	virtual void process_data_to_storage(const SiMemChunk * const chunk);
	virtual void process_data_from_storage(SiMemChunk * chunk);
	void delete_cache() __attribute__ ((section ("filefns")));
	void initialise_cache() __attribute__ ((section ("filefns")));
	void copy_to_cache(const Char * data,const UInt32 size);
	UInt32 last_cache_pos;
	UInt32 cache_end;
	UInt32 cache_cursor_pos;
	Char * cache_data;
};

#endif
