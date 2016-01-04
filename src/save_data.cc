#include "main.h"
#include "error.h"

doc_non_virt_SiFile::doc_non_virt_SiFile(char * name,const UInt32 size)
{

	UInt16 len=StrLen(name)+1;
	m_name=(Char*)MemPtrNew(len);
	MemMove(m_name,name,len-1);
	m_name[len-1]='\0';
	m_curr_handle=NULL;
	m_curr_locked=NULL;
	m_file_open=false;
	create_database(size);
	write_header();
	lock(1)
		;
}

doc_non_virt_SiFile::~doc_non_virt_SiFile()
{
	unlock();
	close();
	MemPtrFree(m_name);
}

void doc_non_virt_SiFile::ensure_unique_name()
{
	LocalID d_id=0;

	Char * temp_c=NULL;
	UInt16 len=StrLen(m_name);
	Int16 count=0;
	do
	{

		d_id=DmFindDatabase(THE_CARD,m_name);
		if(d_id!=0)
		{
			if(temp_c==NULL)
			{
				temp_c=(Char*)MemPtrNew(len+4);

				MemMove(temp_c,m_name,len);
				MemPtrFree(m_name);
				m_name=temp_c;
			}
			StrPrintF(m_name+len,"_%i",count);

			++count;
		}
	}
	while(d_id!=0);


}

Boolean doc_non_virt_SiFile::create_database(const UInt32 size)
{

	ensure_unique_name();


	m_file_size=0;
	Err err=DmCreateDatabase(THE_CARD,m_name,DOC_CREATOR,DOC_TYPE,false);

	if(err==errNone)
	{
		open();
		add_record(DOC_HEADER_SIZE);
		add_records(size);
		//set the backup bit for the database so that users
		//with no conduit can still copy files to the desktop easily
		UInt16 attr=dmHdrAttrBackup;
		DmSetDatabaseInfo(THE_CARD,m_id,NULL,&attr,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

	}


	return m_file_open;
}

Boolean doc_non_virt_SiFile::add_records(UInt32 size_to_add)
{
	//!
	//! Add  the required number of records to the database
	//!

	Int16 number_of_records=size_to_add/RECORD_SIZE;

	for(Int16 i=0;i<number_of_records;++i)
		add_record(RECORD_SIZE);

	if(size_to_add%RECORD_SIZE!=0)
	{
		if(!add_record(size_to_add%RECORD_SIZE))
			return false;
	}

	return true;
}

void doc_non_virt_SiFile::open()
{


	m_curr_handle=NULL;
	m_curr_locked=NULL;

	m_id=DmFindDatabase(THE_CARD,m_name);
	if(m_id==0)
	{
		m_open_ref=NULL;
		m_file_open=false;
		return;
	}

	m_open_ref=DmOpenDatabase(THE_CARD,m_id,dmModeReadWrite);
	if(m_open_ref!=NULL)
	{
		m_file_open=true;
		DmDatabaseSize(THE_CARD,m_id,NULL,NULL,&m_file_size);
		num_records=DmNumRecords(m_open_ref);

		return;

	}
	else
	{
		m_file_open=false;
		return;
	}
	m_cursor=0;

}

void doc_non_virt_SiFile::lock(const UInt16 text_record)
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

void doc_non_virt_SiFile::write_data(Char * data,const UInt32 size)
{
	//actually do the writing to the DOC file
#ifdef DEBUG
	ErrFatalDisplayIf(m_curr_locked==NULL,"Writing to unlocked file");
#endif

	UInt32 amount_written=0;
	UInt32 amount_to_write=0;
	while(amount_written<size)
	{

		if(curr_record_cursor+size > curr_record_size+amount_written )
		{
			amount_to_write=curr_record_size-curr_record_cursor;

		}
		else
		{
			amount_to_write=size-amount_written;
		}

		DmWrite(m_curr_locked,curr_record_cursor,data+amount_written,amount_to_write);

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
}

void doc_non_virt_SiFile::lock_next_record()
{

	unlock();

	lock(curr_record_index+1)
		;
}

Boolean doc_non_virt_SiFile::write_header()
{
	//Write out all the standard DOC header record

	lock(HEADER_RECORD)
		;
	doc_header header;
	header.size=m_file_size-DOC_HEADER_SIZE;
	header.spare=0;
	header.version=UNCOMPRESSED_DOC;
	header.num_records=num_records-1;
	header.max_rec_size=RECORD_SIZE;

	header.curr_pos=0;

	write_data((Char*)&header,DOC_HEADER_SIZE);
	unlock();
	return true;
}

Boolean doc_non_virt_SiFile::add_record(const UInt16 size)
{
	UInt16 recordIndex=num_records;
	m_curr_handle=DmNewRecord(m_open_ref,&recordIndex,size);

	if(m_curr_handle==NULL)
	{
		close();
		DisplayError(UNABLE_TO_SAVE,"Not enough space to save file");
		return false;
	}
	DmReleaseRecord(m_open_ref,recordIndex,false);
	m_curr_handle=NULL;

	m_file_size+=size;
	++num_records;

	return true;

}

void doc_non_virt_SiFile::close()
{
	unlock();

	if(m_open_ref!=NULL)
	{
		DmCloseDatabase(m_open_ref);


		m_open_ref=NULL;

	}
	m_file_open=false;

}
void doc_non_virt_SiFile::startup()
{


	m_curr_handle=NULL;
	m_curr_locked=NULL;
	m_changed=false;
	m_open_ref=NULL;

}

void doc_non_virt_SiFile::unlock()
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
