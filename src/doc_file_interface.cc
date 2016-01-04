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

#include "doc_file_interface.h"
#include "error.h"
#include "file_plugin.h"



doc_SiFile::doc_SiFile(const Char * name,const Char * location,const UInt16 volRefNum):pdb_SiFile(name,location,volRefNum)
{
	//this constructor (with no size argument)
	//only attempts to open a file - if it doesn't exist already it won't create it

	curr_record_index=num_records=curr_record_size=curr_record_cursor=0;

	startup();
	open();
	m_type=DOC_FILE;

}

doc_SiFile::doc_SiFile(const Char * name,const Char * location,const UInt16 volRefNum,const UInt32 size):pdb_SiFile(name,location,volRefNum,size)
{
	//This constructor will open the named file if it exists, otherwise it creates one

	header_size=DOC_HEADER_SIZE;

	startup();
	num_records=curr_record_size=curr_record_cursor=0;

	create_database(size);
	m_type=DOC_FILE;

}

doc_SiFile::~doc_SiFile()
{
}

UInt32 doc_SiFile::get_size()
{
	//return the sizeof the data part of the file ie ignoring the DOC header
	return m_file_size-header_size;
}

Boolean doc_SiFile::create_database(const UInt32 size)
{
	open();
	if(!m_file_open)
	{
		m_file_size=0;
		Err err=DmCreateDatabase(THE_CARD,m_name,DOC_CREATOR,DOC_TYPE,false);

		if(err==errNone)
		{
			add_record(DOC_HEADER_SIZE);

			//set the backup bit for the database so that users
			//with no conduit can still copy files to the desktop easily
			UInt16 attr=dmHdrAttrBackup;
			DmSetDatabaseInfo(THE_CARD,m_id,NULL,&attr,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

		}
		m_id=DmFindDatabase(THE_CARD,m_name);

		//double check that the file has been created properly
		if(m_id==0)
			m_file_open=false;
	}
	if(m_file_open)
		resize(size);

	return m_file_open;
}

void doc_SiFile::set_access_mode(const Int16 mode)
{
	//change the access mode for the file - write/read
	unlock();

	if(m_file_open)
	{
		lock(1)
			;
	}
}

void doc_SiFile::resize(const UInt32 size)
{

	//resize the file so the data section is the size passed in
	UInt32 actual_size=size+header_size;

	if(m_trans!=NULL)
	{
		actual_size=m_trans->adjust_size(actual_size);

	}
	if(!m_file_open||actual_size==m_file_size)
		return;

	unlock();

	//shrinking file - remove records then resize last record
	if(actual_size<m_file_size)
	{
		UInt32 change=m_file_size-actual_size;
		UInt32 last_size=last_record_size();

		if(change<last_size)
		{
			resize_last_record(-(Int32)change);
		}
		else
			remove_records(change);

		change=m_file_size-actual_size;

		if(change>0)
			resize_last_record(-(Int32)change);

	}
	else if(actual_size>m_file_size)
	{

		UInt32 change=actual_size-m_file_size;
		UInt32 last_space=last_record_remaining_space();
		if(change<last_space)
		{
			resize_last_record(change);
			change=0;
		}
		else
		{
			resize_last_record(last_space);
			change-=last_space;
		}

		if(change>0)
			add_records(change);
	}
#ifdef DEBUG
	ErrFatalDisplayIf(get_size()!=size,"Incorrect resize");
#endif

	write_header();
}


UInt16 doc_SiFile::last_record_remaining_space()
{
	//return how much free space is available in the last record
	if(num_records<2)
	{

		return 0;
	}
	else
	{
		return (num_records-1)*RECORD_SIZE-get_size();
	}
}

UInt16 doc_SiFile::last_record_size()
{
	UInt16 l_number_records=get_size()/RECORD_SIZE;

	return get_size()-(l_number_records*RECORD_SIZE);
}

void doc_SiFile::resize_last_record(const Int32 size_to_add)
{

	Int32 re_lock_record;
	if(m_curr_handle!=NULL)
		re_lock_record=curr_record_index;
	else
		re_lock_record=-1;

	unlock();

	MemHandle last_handle=DmGetRecord(m_open_ref,num_records-1);
	UInt16 new_size=MemHandleSize(last_handle)+size_to_add;
	DmReleaseRecord(m_open_ref,num_records-1,false);
	m_file_size+=size_to_add;

	DmResizeRecord(m_open_ref,num_records-1,new_size);
	if(re_lock_record!=-1)
		lock(re_lock_record)
			;
}

Boolean doc_SiFile::add_records(UInt32 size_to_add)
{
	//!
	//! Add  the required number of records to the database
	//!

	Int16 number_of_records=size_to_add/RECORD_SIZE;

	for(Int16 i=0;i<number_of_records;++i)
		add_record(RECORD_SIZE);

	if(size_to_add%RECORD_SIZE!=0)
	{
		add_record(size_to_add%RECORD_SIZE);
	}

	return true;
}

void doc_SiFile::remove_last_record()
{
	m_file_size-=last_record_size();
	DmRemoveRecord(m_open_ref,num_records-1);

	--num_records;
}

Boolean doc_SiFile::remove_records(UInt32 size_to_remove)
{
#ifdef TEST_OBJECTS_LOG
	log_entry_number("removing records size=",size_to_remove);
#endif

	Int16 last_size=last_record_size();
	remove_last_record();
	size_to_remove-=last_size;

	Int16 number_of_records=size_to_remove/RECORD_SIZE;

	for(Int16 i=0;i<number_of_records;++i)
		remove_last_record();

	//m_file_size-=(size_to_remove+last_size);
	return true;
}

void doc_SiFile::open()
{

	if(!m_file_open)
	{
		m_curr_handle=NULL;
		m_curr_locked=NULL;

		if(!do_open())
		{

			m_file_size=0;
			m_id=NULL;
			m_file_open=false;
		}
		else
		{
			//make sure this is actually a DOC file
			if(num_records<2)
			{
				//There must be at least a header and one data record
				//if not then close and exit because this isn't a doc file
				close();
				return;
			}
			else
			{
				m_cursor=0;
				if(!read_header())
				{
					close();

					return;
				}

			}

		}


	}

	//lock(1);

}

void doc_SiFile::lock(const UInt16 text_record)
{
	//! Lock the relevant record reading for reading/writing
	//!

	if(!m_file_open)
	{
		return;
	}

	if(m_curr_handle==NULL)
	{
		m_curr_handle=DmGetRecord(m_open_ref,text_record);
		curr_record_size=MemHandleSize(m_curr_handle);

		curr_record_cursor=0;
		curr_record_index=text_record;
		m_curr_locked=(Char*)MemHandleLock(m_curr_handle);
	}
}

void doc_SiFile::write_direct(const SiMemChunk * const chunk)
{
#ifdef DEBUG
	ErrFatalDisplayIf(chunk->size>5000,"Over large block writing");
#endif
	//actually do the writing to the DOC file
	BlockInt amount_written=0;
	BlockInt amount_to_write=0;
	while(amount_written<chunk->size)
	{
#ifdef LOG_ENTRY
		log_entry_number("amount_written ",amount_written);
		log_entry_number("write_direct ",chunk->size);
#endif

		if(curr_record_cursor+chunk->size > curr_record_size+amount_written )
		{
#ifdef DEBUG
			ErrFatalDisplayIf(curr_record_size<curr_record_cursor,"Overwriting record");
#endif

			amount_to_write=curr_record_size-curr_record_cursor;

		}
		else
		{
#ifdef DEBUG
			ErrFatalDisplayIf(chunk->size<amount_written,"Overwriting");
#endif

			amount_to_write=chunk->size-amount_written;
		}

		DmWrite(m_curr_locked,curr_record_cursor,chunk->mem_ptr+amount_written,amount_to_write);

		curr_record_cursor+=amount_to_write;

		amount_written+=amount_to_write;
		m_cursor+=amount_to_write;
		if(curr_record_cursor==curr_record_size&&(curr_record_index+1)<(num_records))
		{
			lock_next_record();
		}
		else
		{

			break;
		}

	}
	m_changed=true;

}

Int16 doc_SiFile::read_direct(SiMemChunk * chunk)
{
	//copy data from a file/database to main memory
	BlockInt amount_read=0;
	BlockInt amount_to_read=0;

	while(amount_read<chunk->size)
	{
#ifdef DEBUG_LOG
		log_entry_number("amount_read ",amount_read);
		log_entry_number("read_direct ",chunk->size);
#endif

		if(chunk->size+curr_record_cursor> amount_read +curr_record_size )
		{
			amount_to_read=curr_record_size-curr_record_cursor;
		}
		else
		{
			amount_to_read=chunk->size-amount_read;
		}


		MemMove(chunk->mem_ptr+amount_read,m_curr_locked+curr_record_cursor,amount_to_read);

		curr_record_cursor+=amount_to_read;
		amount_read+=amount_to_read;
		m_cursor+=amount_to_read;

		if(curr_record_cursor==curr_record_size&&(curr_record_index+1)<num_records)
		{
			lock_next_record();
		}
		else
		{
#ifdef DEBUG_LOG
			if(chunk->size-amount_read>0)
				log_entry_number("Attempted over read = ",chunk->size-amount_read);
#endif

			break;
		}
	}
	return amount_read;
}

void doc_SiFile::lock_next_record()
{

	unlock();

	lock(curr_record_index+1)
		;
}

Boolean doc_SiFile::read_header()
{

	lock(HEADER_RECORD)
		;
	header_size=curr_record_size;
	doc_header header;
	SiMemChunk temp_chunk;
	temp_chunk.mem_ptr=(Char*)&header;
	temp_chunk.size=sizeof(doc_header);
	read_direct(&temp_chunk);

	move_cursor(header_size,MOVE_TO);
	unlock();
#ifdef DEBUG_LOG

	log_entry("Reading DOC Header");
	log_entry_number("number records ",header.num_records);
	log_entry_number("header_size ",header.size);
	log_entry_number("m_file_size ",get_size());
#endif
	//No support for compressed DOC files yet
	//TODO: IMPLEMENT SUPPORT!
	temp_chunk.mem_ptr=NULL;

	//only take note of the header number of records
	//count if it doesn't contradict the OS count
	//allows ignoring bookmark records
	//without crashing with corrupted files
	if(header.num_records+1<num_records)
		num_records=header.num_records+1;

	if(header.size<=get_size())
		m_file_size=header.size+header_size;
	if(header.version==COMPRESSED_DOC)
	{
		if(SiFile::display_warnings)
		{
			DisplayError(NO_COMPRESSED_SUPPORT,get_name());
		}
		return false;
	}

	return true;
}
Boolean doc_SiFile::write_header()
{
	//Write out all the standard DOC header record
	if(header_size>sizeof(doc_header))
	{
		DmResizeRecord(m_open_ref,0,DOC_HEADER_SIZE);
		header_size=DOC_HEADER_SIZE;

	}
	lock(HEADER_RECORD)
		;
	doc_header header;
	header.size=get_size();
	header.spare=0;
	header.version=UNCOMPRESSED_DOC;
	header.num_records=num_records-1;
	header.max_rec_size=RECORD_SIZE;

	header.curr_pos=0;
#ifdef DEBUG_LOG

	log_entry("Writing DOC Header");
	log_entry_number("number records ",header.num_records);
	log_entry_number("header_size ",header.size);
	log_entry_number("m_file_size ",get_size());
#endif

	SiMemChunk temp_chunk;
	temp_chunk.mem_ptr=(Char*)&header;
	temp_chunk.size=sizeof(doc_header);
	write_direct(&temp_chunk);
	unlock();
	return true;
}
void doc_SiFile::update_record_cursor(const UInt16 amount,const Int16 dir)
{
	//move the cursor within the current record.
	//Simplistic imlementation - assumes the cursor
	//won't move outside the current record
	if(m_curr_handle==NULL)
		return;

	switch(dir)
	{
	case FORWARDS:
		curr_record_cursor+=amount;
		break;
	case BACKWARDS:
		curr_record_cursor-=amount;
		break;
	case MOVE_TO:
		curr_record_cursor=0;
		break;
	}
}
void doc_SiFile::move_cursor(const Int32 amount,const Int16 dir)
{
	//move the current reading/writing position in the file
	//called after reading file type header and when filehandler detects a read has inished in
	//the middle of a two byte character

	if(amount==0&&dir!=MOVE_TO)
		return;

	switch(dir)
	{
	case FORWARDS:
		m_cursor+=amount;

		if(m_cursor>=m_file_size)
			m_cursor=m_file_size-1;
		break;
	case BACKWARDS:
		if((UInt32)amount>m_cursor)
			m_cursor=0;
		else
		{
			m_cursor-=amount;

		}
		break;
	case MOVE_TO:
		m_cursor=amount;
		break;
	}
	update_record_cursor(amount,dir);
}
