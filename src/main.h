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
#include "sections.h"
#include "edit_interface.h"

// Resize Support
#include "presize/resize.h"

// Include AlphaSmart libraries
#include <WideTallApp.h>
#include <WideTallAppChars.h>
#include <Screen.h>


extern SiEditInterface* edit_interface;
#ifndef MAIN_HPP
#define MAIN_HPP


UInt16 StartApplication() __attribute__ ((section ("filefns")));
void EventLoop();
void StopApplication() ;

Boolean texteditloop(EventPtr event);


UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags);

Boolean texteditloop(EventPtr event);
class test
{
public:
	test();
private:
	Int16 m_test;
};

class doc_non_virt_SiFile
{
public:
	doc_non_virt_SiFile(Char * name,const UInt32 size) ;
	~doc_non_virt_SiFile() ;
	void write_data(Char * data,const UInt32 size) ;
	Char * m_name;
	LocalID m_id;
private:

	Boolean create_database() ;
	void unlock() ;
	void close() ;
	Boolean resize(const UInt32 size) ;
	void open() ;

	Boolean create_database(const UInt32 size) ;
	Boolean add_record(const UInt16 size) ;
	Boolean add_records(const UInt32 size) ;
	void startup() ;
	void lock(UInt16 rec)
		;
	void lock_next_record() ;
	Boolean write_header() ;
	void ensure_unique_name();
	DmOpenRef m_open_ref;
	MemHandle m_curr_handle;
	Char * m_curr_locked;
	Boolean m_changed;

	UInt16 curr_record_index;
	UInt16 num_records;
	UInt32 m_file_size;
	UInt32 m_cursor;
	UInt16 curr_record_cursor;
	UInt16 curr_record_size;


	Boolean m_file_open;
};

#endif
