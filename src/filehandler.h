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
#include <PalmOS.h>
#include <PalmTypes.h>
#include <Preferences.h>
#include "sections.h"
#include "resources.h"
#ifndef FILEHANDLER_HH
#define FILEHANDLER_HH
#include "sidocument.h"
class SiEditor;
class SiTextHandler;
class SiFile;
Boolean filelistloop(EventPtr event);

class SiFileHandler
{
public:
	SiFileHandler(SiEditor * editor,SiDocument * doc) __attribute__ ((section ("filefns")));
	~SiFileHandler() __attribute__ ((section ("filefns")));
	void write_to_scratch(const Int16 index) __attribute__ ((section ("filefns")));
	//SiFile* save_file() __attribute__ ((section ("filefns")));

	Boolean do_open_file(Char * initial_dir,UInt16 vol) __attribute__ ((section ("filefns")));
	Boolean do_file_manage(Char * location,UInt16 vol) __attribute__ ((section ("filefns")));
	void clear_open_filename() __attribute__ ((section ("filefns")));
	Boolean open_saved_filename() __attribute__ ((section ("filefns")));
	Boolean save_file_as(Char * initial_dir,UInt16 vol) __attribute__ ((section ("filefns")));
	Boolean save_doc_file(Char * name) __attribute__ ((section ("filefns")));
	Boolean open_doc_file(Char * name) __attribute__ ((section ("filefns")));
	static Boolean valid_file_name(const char *name)  __attribute__ ((section ("filefns")));
	void read_from_scratch(const Int16 index) __attribute__ ((section ("filefns")));
	Boolean save_file() __attribute__ ((section ("filefns")));
	Boolean read_file() __attribute__ ((section ("filefns")));
	Boolean read_file(Char *file,const Int16 type,UInt16 vol_ref) __attribute__ ((section ("filefns")));
	SiFile* get_file() __attribute__ ((section ("filefns")));
	Char * get_location() __attribute__ ((section ("filefns")));
	UInt16 get_vol_ref() __attribute__ ((section ("filefns")));
	void clear_curr_file() __attribute__ ((section ("filefns")));
	void delete_scratch_file(const Int16 index) __attribute__ ((section ("filefns")));
	Boolean file_is_read_only() __attribute__ ((section ("filefns")));
#ifdef DEBUG

	Boolean perform_tests() __attribute__ ((section ("filefns")));
#endif
protected:
	Boolean open_file(SiFile* t_file) __attribute__ ((section ("filefns")));
	Boolean check_save_curr_file() __attribute__ ((section ("filefns")));
	void set_curr_file(SiFile * file) __attribute__ ((section ("filefns")));
	Char * strip_path(Char * location) __attribute__ ((section ("filefns")));
	Boolean read_data(SiFile * src) __attribute__ ((section ("filefns")));
	Boolean save_if_necessary() __attribute__ ((section ("filefns")));
	Boolean handle_button_event(const UInt16 button) __attribute__ ((section ("filefns")));

	Int16 file_selected ;
	SiEditor * m_editor ;
	SiFile * curr_file;
	SiDocument * m_document;
	Boolean m_temporary_file;
};

#endif
