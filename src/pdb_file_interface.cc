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

#include "pdb_file_interface.h"
#include "error.h"
#include "file_plugin.h"

pdb_SiFile::pdb_SiFile(const Char * name,const Char * location,const UInt16 volRefNum):SiFile(name,location,volRefNum)
{
#ifdef EN_LOG
	log_entry("pdb_SiFile ctor");
#endif

}

pdb_SiFile::pdb_SiFile(const Char * name,const Char * location,const UInt16 volRefNum,const UInt32 size):SiFile(name,location,volRefNum,size)
{}

pdb_SiFile::~pdb_SiFile()
{
	close();
}

Boolean pdb_SiFile::create_database(const UInt32 size)
{
	return false;
}

void pdb_SiFile::startup()
{
#ifdef EN_LOG
	log_entry("pdb_SiFile startup");
#endif

	m_curr_handle=NULL;
	m_curr_locked=NULL;
	m_changed=false;
	m_open_ref=NULL;

}

void pdb_SiFile::close()
{
#ifdef DEBUG
	/*  if(m_id!=NULL)
	  {
	    UInt32 temp_size;
	    DmDatabaseSize(THE_CARD,m_id,NULL,NULL,&temp_size);
	    Char buff[100];
	    StrPrintF(buff,"Actual size=%lu, m_file_size=%lu",temp_size,m_file_size);
	    ErrFatalDisplayIf(m_file_size!=temp_size,buff);
	    }*/
#endif

	if(m_trans!=NULL)
		m_trans->finalise();

	unlock();

	if(m_open_ref!=NULL)
	{
		DmCloseDatabase(m_open_ref);

		m_open_ref=NULL;

	}
	m_file_open=false;

}

Boolean pdb_SiFile::do_open()
{
	m_id=DmFindDatabase(THE_CARD,m_name);
	if(m_id==0)
	{
		m_open_ref=NULL;
		return false;
	}

	m_open_ref=DmOpenDatabase(THE_CARD,m_id,dmModeReadWrite);
	if(m_open_ref!=NULL)
	{
		m_file_open=true;
		DmDatabaseSize(THE_CARD,m_id,NULL,NULL,&m_file_size);
		num_records=DmNumRecords(m_open_ref);
		UInt16 attr;
		DmDatabaseInfo(THE_CARD,m_id,NULL,&attr,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
		//check for read only database

		if(attr&dmHdrAttrReadOnly)
			m_is_read_only=true;
		return true;

	}
	else
	{
		m_file_open=false;
		return false;
	}
}

void pdb_SiFile::unlock()
{
	if(m_curr_locked!=NULL)
	{
		MemHandleUnlock(m_curr_handle);
		m_curr_locked=NULL;
	}

	if(m_curr_handle!=NULL)
	{
		DmReleaseRecord(m_open_ref,curr_record_index,m_changed);
		m_curr_handle=NULL;
	}
}

void pdb_SiFile::rename(Char * new_name)
{

#ifdef EN_LOG
	log_entry("pdb_SiFile::renaming file");
	log_entry(new_name);
#endif
       #ifdef DEBUG

	ErrFatalDisplayIf(m_id==NULL,"Renaming non-existent file");
#endif

	Err err=DmSetDatabaseInfo(THE_CARD,m_id,new_name,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

	if(err!=errNone)
	{
		if(display_warnings)
			DisplayError(UNABLE_TO_SAVE,new_name);
	}
	else
	{
		MemPtrFree(m_name);
		m_name=(Char*)MemPtrNew(StrLen(new_name)+1);
		StrCopy(m_name,new_name);
	}
}

Boolean pdb_SiFile::add_record(const UInt16 size)
{
#ifdef EN_LOG
	log_entry_number("PDB add_record",size);
#endif

	if(!m_file_open)
		do_open();

	UInt16 recordIndex=num_records;
	m_curr_handle=DmNewRecord(m_open_ref,&recordIndex,size);

	if(m_curr_handle==NULL)
	{
		close();
		if(display_warnings)
			DisplayError(UNABLE_TO_SAVE,"Not enough space to save file");
		return false;
	}
	DmReleaseRecord(m_open_ref,recordIndex,false);
	m_curr_handle=NULL;

	m_file_size+=size;
	++num_records;

	return true;
}

UInt32 pdb_SiFile::get_mod_time()
{
	//return the modication time of the file
	//in seconds since 1904
	UInt32 create_time=0;
	if(m_file_open)
	{
		DmDatabaseInfo(THE_CARD,m_id,NULL,NULL,NULL,&create_time,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

		return create_time;
	}
	return create_time;
}

void pdb_SiFile::delete_file()
{
#ifdef DEBUG
	ErrFatalDisplayIf(m_id==NULL,"Deleting non-existent file");
#endif

	close();
	if(m_id!=0)
	{
		DmDeleteDatabase(THE_CARD,m_id);
		m_id=NULL;
	}
}

