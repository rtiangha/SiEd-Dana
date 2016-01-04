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

#include "file_interface.h"
#include "error.h"
#include "file_plugin.h"
#include "utility.h"
Boolean SiFile::display_warnings=false;

SiFile::SiFile(const Char * file_name,const Char * p_location,const UInt16 p_vol,const UInt32 size)
{
	//constructor for creating a file

	init(file_name,p_location);
	m_file_size=size;
}

SiFile::SiFile(const Char * file_name,const Char * p_location,const UInt16 p_vol)
{
	//constructor for opening an existing file
	init(file_name,p_location);
}

void SiFile::init(const Char * file_name,const Char * p_location)
{
	//initialise all common file members
	m_is_read_only=false;
	m_name=(Char*)MemPtrNew(StrLen(file_name)+1);
	StrCopy(m_name,file_name);
	if(p_location!=NULL)
	{
		m_location=(Char*)MemPtrNew(StrLen(p_location)+1);
		StrCopy(m_location,p_location);
	}
	else
		m_location=NULL;

	m_cursor=0;
	m_file_open=false;
	m_trans=NULL;
}


SiFile::~SiFile()
{
	//make sure the file is closed when object is deleted!
	close();
	if(m_name!=NULL)
		MemPtrFree(m_name);
	if(m_location!=NULL)
	{
		MemPtrFree(m_location);
	}


	if(m_trans!=NULL)
		delete m_trans;


}

//get functions
UInt32 SiFile::get_size()
{
	return m_file_size;
}
Char * SiFile::get_name()
{
	return m_name;
}
Char * SiFile::get_location()
{
	return m_location;
}
Boolean SiFile::is_open()
{
	return m_file_open;
}

//remove a plugin from the plugin chain
//not actually ever called at present, AFAIK
void SiFile::delete_plugin(const Int16 position)
{
#ifdef DEBUG_LOG
	log_entry("SiFile::Deleting plugin");
#endif
	//delete the plugin either AT_END or AT_START of the plugin chain
	if(m_trans==NULL)
		return;
	SiFilePlugin * curr_plug=m_trans;
	SiFilePlugin * prev_plug=NULL;
	if(position==AT_END)
	{
		while(curr_plug->next_plugin!=NULL)
		{
			prev_plug=curr_plug;
			curr_plug=curr_plug->next_plugin;

		}
	}
	else if(position==AT_START)
	{
		curr_plug=m_trans;
	}

	if(m_trans==curr_plug)
		m_trans=NULL;
	if(prev_plug!=NULL)
		prev_plug->next_plugin=curr_plug->next_plugin;
	delete curr_plug;
}

//add a plugin to the current chain
//either at end or beginning of chain
void SiFile::add_plugin(SiFilePlugin * p_trans,const Int16 position)
{
#ifdef DEBUG_LOG
	log_entry("SiFile::adding plugin");
#endif
	//add a plugin to the plugin chain
	if(m_trans==NULL)
	{
		m_trans=p_trans;
	}
	else
	{
		switch(position)
		{
		case AT_END:
			m_trans->add_plugin(p_trans);
			break;
		case AT_START:
			p_trans->add_plugin(m_trans);
			m_trans=p_trans;
			break;
		}
	}
}

void SiFile::change_location(const Char * new_location)
{
	//write a new location of the file
	//this is used as the path for VFS files
	if(m_location!=NULL)
		MemPtrFree(m_location);

	m_location=(Char*)MemPtrNew(StrLen(new_location)+1);
	StrCopy(m_location,new_location);
}

void SiFile::change_name(const Char * new_name)
{
	//change the name of the file as it appears in the file dialogs
	//and on the status bar
	if(m_name!=NULL)
		MemPtrFree(m_name);

	m_name=(Char*)MemPtrNew(StrLen(new_name)+1);
	StrCopy(m_name,new_name);
}

UInt32 SiFile::get_mod_time()
{
	return 0;
}

void SiFile::open()
{}

void SiFile::close()
{}

void SiFile::set_access_mode(const Int16 mode)
{
}

void SiFile::initialise(const Int16 file_mode)
{
	if(m_trans!=NULL)
	{
		if(!m_trans->initialise(file_mode))
		{
			close();
		}
	}
}
void SiFile::write_data(const SiMemChunk * const chunk)
{
#ifdef DEBUG
	ErrFatalDisplayIf(!m_file_open,"Writing to closed file");
#endif

	if(m_trans!=NULL)
	{
		m_trans->translate_to_storage(chunk);
	}
	else
	{
		write_direct(chunk);
	}

}

SiMemChunk * SiFile::read_data(BlockInt & n_chars)
{
	UInt32 bytes_to_read;
#ifdef DEBUG

	ErrFatalDisplayIf(!m_file_open,"Reading to closed file");
#endif

	if((m_cursor+SiMemHandler::MEMORY_CHUNK_SIZE)>m_file_size)
	{
		if(m_file_size<m_cursor)
			bytes_to_read=0;
		else
			bytes_to_read=m_file_size-m_cursor;
	}
	else
		bytes_to_read=SiMemHandler::MEMORY_CHUNK_SIZE;

	if(bytes_to_read==0)
	{
		//nothing left to do so return
		n_chars=0;
		return NULL;
	}

	SiMemChunk * curr_chunk=SiMemHandler::get_chunk_quick(bytes_to_read);

	//unable to allocate any memory so bail out
	if(curr_chunk==NULL)
	{
		return NULL;
	}

	BlockInt bytes=read_direct(curr_chunk);

	//if the read didn't complete fully, resize the chunk
	//to the number of bytes that were read
	if(bytes==0)
	{
		SiMemHandler::free_chunk(curr_chunk);
		n_chars=0;
		return NULL;
	}
	else if(bytes<curr_chunk->size)
	{
		curr_chunk=SiMemHandler::resize_chunk(curr_chunk,bytes,TO_START);

	}

	if(m_trans!=NULL)
		m_trans->translate_from_storage(curr_chunk);

	bytes_to_read=SiUtility::last_char_boundary(curr_chunk->mem_ptr,&n_chars,curr_chunk->size);
	if(bytes_to_read!=curr_chunk->size)
		curr_chunk=SiMemHandler::resize_chunk(curr_chunk,bytes_to_read,TO_START);

	//move the file cursor if required to be on an inter-character boundary
	move_cursor(curr_chunk->size-bytes_to_read,BACKWARDS);

	return curr_chunk;
}

void SiFile::move_cursor(const Int32 distance,const Int16 dir)
{
}

void SiFile::resize(const UInt32 size)
{
}

void SiFile::rename(Char * new_name)
{

}
void SiFile::delete_file()
{}

void SiFile::write_direct(const SiMemChunk * const chunk)
{
}

Int16 SiFile::read_direct(SiMemChunk * chunk)
{
	return 0;
}

UInt16 SiFile::get_type()
{
	return m_type;
}

UInt16 SiFile::get_vol_ref()
{
	return NON_VFS;
}

SiFilePlugin* SiFile::is_encrypted()
{
	if(m_location==NULL)
		return NULL;

	open();
	set_access_mode(FILE_READ);

	if(!is_open())
	{
		return NULL;
	}
	SiFilePlugin * plug=SiFilePlugin::get_plugin_chain(this);


	return plug;


}

Boolean SiFile::is_read_only()
{
	return m_is_read_only;
}
