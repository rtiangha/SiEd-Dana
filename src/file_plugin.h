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
#ifndef SITRANSLATOR_HPP
#define SITRANSLATOR_HPP

#include <PalmOS.h>
#include <PalmTypes.h>

#include "file_interface.h"
#define FILE_TYPE_SIZE 4
#define ENCRYPTED_FILE 65535
class SiFilePlugin
{
public:
	SiFilePlugin(SiFile* parent_file);
	virtual ~SiFilePlugin();
	Boolean initialise(const Int16 access_mode) __attribute__ ((section ("diafns")));
	void translate_to_storage(const SiMemChunk * const chunk);
	void translate_from_storage(SiMemChunk * chunk);
	void flush();
	void finalise() __attribute__ ((section ("diafns")));
	void add_plugin(SiFilePlugin* n_plug) __attribute__ ((section ("diafns")));
	SiFilePlugin * next_plugin;

	//write bypassing plugins
	virtual void process_data_to_storage(const SiMemChunk * const chunk);
	virtual void process_data_from_storage(SiMemChunk * chunk);
	virtual UInt32 adjust_size(UInt32 size) const;
	static SiFilePlugin * get_plugin_chain(SiFile * file) __attribute__ ((section ("diafns")));

protected:

	virtual Boolean do_initialise(const Int16 access_mode);
	virtual void do_finalise();
	virtual void do_flush();


	SiFile * m_parent_file;

};

#endif
