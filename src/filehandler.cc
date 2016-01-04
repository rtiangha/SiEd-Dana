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
#include "filehandler.h"
#include "resources.h"
#include "editor.h"
#include "texthandler.h"
#include "error.h"
#include "utility.h"

#include "doc_file_interface.h"
#include "vfs_file_interface.h"

#include "filedialog.h"
#include "file_plugin.h"

//SiFileHandler *curr_handler;


SiFileHandler::SiFileHandler(SiEditor* editor,SiDocument * document)
{
	m_editor = editor;

	curr_file=NULL;

	SiFileDialog::initialise();
	m_document=document;
	m_temporary_file=false;
}

SiFileHandler::~SiFileHandler()
{
	if(curr_file!=NULL)
	{
		delete curr_file;
		curr_file=NULL;
	}
	SiFileDialog::exit();

}
void SiFileHandler::set_curr_file(SiFile * file)
{
#ifdef LOG_ENTRY
	log_entry("SiFileHandler::set_curr_file");
	if(file!=NULL)
	{
		if(file->get_location()!=NULL)
			log_entry(file->get_location());
	}
#endif
	if(curr_file!=NULL)
		delete curr_file;

	curr_file=file;

}
Boolean SiFileHandler::save_doc_file(Char * name)
{

	curr_file=new doc_SiFile(name,name,THE_CARD,m_document->get_doc_length());
	return save_file();

}
Boolean SiFileHandler::open_doc_file(Char * name)
{
	SiFile * t_file=new doc_SiFile(name,name,THE_CARD);
	if(t_file!=NULL)
	{
		return open_file(t_file);
	}
	else
		return false;
}


Boolean SiFileHandler::save_file_as(Char * initial_dir,UInt16 vol)
{
	//Show the save as dialog and save current document to selected filename
	if(m_document->get_doc_length()==0)
		return false;
#ifdef LOG_ENTRY

	log_entry("SiFileHandler::save_file_as");
#endif

	const Char * init_filename=m_document->get_initial_filename();
	SiFile *t_file=SiFileDialog::ShowSaveFileDialog(init_filename,initial_dir,m_document->get_doc_length(),vol);

	if(init_filename!=NULL)
		MemPtrFree(const_cast<Char*>(init_filename));

	if(t_file==NULL)
	{
		return false;
	}
	else
	{
		set_curr_file(t_file);
		if(!save_file())
		{
			DisplayError(UNABLE_TO_SAVE,t_file->get_name());
			t_file->delete_file();
			//			delete t_file;
			set_curr_file(NULL);
			return false;
		}
		return true;
	}

}

Boolean  SiFileHandler::do_open_file(Char * initial_dir,UInt16 vol)
{
	//show the open file dialog and open selected file to current document
#ifdef LOG_ENTRY
	log_entry("SiFileHandler::do_open_file()");
#endif

	SiFile * t_file=SiFileDialog::ShowOpenFileDialog(initial_dir,vol);
	return open_file(t_file);

}

Boolean SiFileHandler::open_file(SiFile * t_file)
{

	if(t_file!=NULL)
	{

		if(!save_if_necessary())
		{
			delete t_file;
			return false;
		}


		if(!t_file->is_open())
		{
			return false;
		}
		else
		{

			set_curr_file(t_file);
			if(!read_file())
				return false;
		}

	}

	return true;
}

Boolean SiFileHandler::save_if_necessary()
{
	if (curr_file != NULL)
	{
		if(m_document->file_dirty())
		{
			if(!save_file())
			{
				m_editor->stop_waiting();
				return false;
			}
		}
	}
	else
	{
		if(!check_save_curr_file())
		{
			m_editor->stop_waiting();
			return false;
		}
	}
	return true;
}

Boolean SiFileHandler::do_file_manage(Char * location,UInt16 vol)
{
#ifdef LOG_ENTRY
	log_entry("file_handler::do_file_manage()");
#endif

	SiFile * t_file=SiFileDialog::ShowFileManageDialog(location,vol);
	if(t_file!=NULL)
		return open_file(t_file);
	else
		return false;
}

Boolean SiFileHandler::check_save_curr_file()
{
	if(m_document->file_dirty())
	{
		if (FrmAlert(ConfirmOpenOver) == OpenOverSave)
		{
			return save_file_as(NULL,NON_VFS);
		}
	}
	return true;


}
Boolean SiFileHandler::read_file()
{
#ifdef BENCHMARK_FILE_OPEN
	Int16 start_time=TimGetTicks();
#endif
	//clear the state of the editor
	m_editor->initialise();
	m_editor->start_waiting();
	//display the waiting text

	if(!curr_file->is_open())
		curr_file->open();
	if(!curr_file->is_open())
		return false;

	//read the file into memory
	if(!read_data(curr_file))
	{
		DisplayError(FILE_ACCESS_ERROR,curr_file->get_location());
		set_curr_file(NULL);
		m_editor->stop_waiting();
		return false;
	}


	curr_file->close();
	m_document->finished_initial_opening();
	//and we're done

#ifdef BENCHMARK_FILE_OPEN

	Int16 total_time=TimGetTicks()-start_time;
	Char buff[50];
	StrPrintF(buff,"Ticks to open file: %i \n",total_time);
	DisplayError(DEBUG_MESSAGE,buff);
#endif

	m_editor->stop_waiting();
	return true;
}

Boolean SiFileHandler::read_data(SiFile * src)
{
	//do the actual file preparation and reading
	//check no errors on opening file
	if(!src->is_open())
		return false;
	src->set_access_mode(FILE_READ);
	if(!src->is_open())
		return false;
	SiFilePlugin * plug=SiFilePlugin::get_plugin_chain(src);
	if(plug!=NULL)
	{
		src->add_plugin(plug,AT_END);
	}

	src->initialise(FILE_READ);

	if(!src->is_open())
		return false;

	BlockInt n_chars;
	SiMemChunk* curr_chunk;
	Int16 line=0;
	Position start(0,0);
	Position end(0,0);
	m_document->set_opening_file_mode();
	do
	{
		//work out maximum number of bytes to read
		//n_chars is filled in with the number of characters in the chunk
		//damn multi-byte characters
		curr_chunk=src->read_data(n_chars);
		if(curr_chunk==NULL&&n_chars!=0)
		{
			//not enough memory for the data read in
			//so bail now
			DisplayError(FILE_TRUNCATION_ERROR,NULL);
			break;
		}

		//insert the block into current document
		//text handler handles all line sloshing
		line=m_document->get_number_blocks();
		if(!m_document->insert_block_at_end(curr_chunk,n_chars))
			break;

		if(line<=m_editor->COMMAND_LINE)
		{
			start.line=line;
			end.line=m_document->get_number_blocks();
			m_document->made_change(start,end);
		}
		//		else
		//			m_editor->redraw_chrome(false);

	}
	while(curr_chunk!=NULL);

	m_document->clear_dirty_flag();
	m_document->clear_opening_file_mode();
	return true;
}

SiFile * SiFileHandler::get_file()
{
	if(m_temporary_file)
		return NULL;
	return curr_file;
}

Char * SiFileHandler::get_location()
{
	SiFile * f=get_file();
	if(NULL==f)
		return NULL;
	else
		return f->get_location();
}

UInt16 SiFileHandler::get_vol_ref()
{
	SiFile * f=get_file();
	if(NULL==f)
		return NON_VFS;
	else
		return f->get_vol_ref();
}
Boolean SiFileHandler::save_file()
{
#ifdef LOG_ENTRY
	log_entry("SiFileHandler::save_file");
#endif
#ifdef BENCHMARK_FILE_OPEN
	Int16 start_time=TimGetTicks();
#endif
	if(m_document->get_doc_length()==0||curr_file==NULL)
		return false;

	m_editor->start_waiting();

	//switch to writing mode on file

	curr_file->open();

	//if the file isn't open now, something is wrong so stop
	if(!curr_file->is_open())
	{
		m_editor->stop_waiting();
		return false;
	}

	curr_file->resize(m_document->get_doc_length());
	curr_file->set_access_mode(FILE_WRITE);
	curr_file->initialise(FILE_WRITE);
	if(!curr_file->is_open())
	{
		//	curr_file=NULL;
		m_editor->stop_waiting();
		return false;
	}

	block *b;

	Int16 b_b_index;
	//write the document out line by line
	for(Int16 i=0;i<m_document->get_number_blocks();++i)
	{
		b=m_document->get_line(i,b_b_index);
		if (b->mem_chunk!= NULL)
		{
			curr_file->write_data(b->mem_chunk);
		}
		m_document->release_line(b_b_index);
	}

	//tidy up and return
	curr_file->close();
	m_document->clear_dirty_flag();
	m_editor->stop_waiting();
#ifdef BENCHMARK_FILE_OPEN

	Int16 total_time=TimGetTicks()-start_time;
	Char buff[50];
	StrPrintF(buff,"Ticks to save file: %i \n",total_time);
	DisplayError(DEBUG_MESSAGE,buff);
#endif
	return true;
}

void SiFileHandler::delete_scratch_file(const Int16 index)
{

	char buff[16];
	StrPrintF(buff, "sied-scratch%u", index);
	LocalID id=DmFindDatabase(THE_CARD,buff);
	if(id!=0)
		DmDeleteDatabase(THE_CARD,id);
}

void SiFileHandler::write_to_scratch(const Int16 index)
{
#ifdef LOG_ENTRY
	log_entry("SiFileHandler::write_to_scratch");
#endif

	//save to temporary storage when user exits without saving
	m_temporary_file=true;
	char buff[16];
	StrPrintF(buff, "sied-scratch%u", index);

	SiFile * temp_file=curr_file;
	curr_file=new doc_SiFile(buff,buff,0,m_document->get_doc_length());
	save_file();
	set_curr_file(temp_file);
	m_temporary_file=false;
}

void SiFileHandler::read_from_scratch(const Int16 index)
{
#ifdef LOG_ENTRY
	log_entry("SiFileHandler::read_from_scratch");
#endif

	m_temporary_file=true;
	//check if a temporary file has been saved and if so open it
	char buff[17];
	StrPrintF(buff, "sied-scratch%u", index);
	SiFile * temp_file=curr_file;
	curr_file=new doc_SiFile(buff,buff,0);
	if(curr_file->is_open())
		read_file();
	set_curr_file(temp_file);
	m_temporary_file=false;
}

Char * SiFileHandler::strip_path(Char * location)
{
	for(Int16 len=StrLen(location);len>=0;--len)
	{
		if(location[len]=='/')
			return location+len+1;
	}
	return location;
}

Boolean SiFileHandler::read_file(Char *file,const Int16 type,UInt16 vol_ref)
{

	SiFile * ret_file=NULL;
	Char * name=NULL;
	Boolean ret=true;
	switch(type)
	{
	case VFS_FILE:
		name=strip_path(file);
		ret_file=new vfs_SiFile(name,file,vol_ref);
		break;
	case DOC_FILE:
		ret_file=new doc_SiFile(file,file,vol_ref);
		break;

		//obsolete and requiring too much maintenance - removing support
		/*	case SIED_FILE:
		 
				ret_file=new sied_SiFile(file,file,vol_ref);
				break;*/
	default:
		ret_file=NULL;
		break;
	}
	if(ret_file==NULL)
		return false;

	curr_file=ret_file;
	if(ret_file->is_open())
	{
		if(!read_file())
		{

			set_curr_file(NULL);
			ret=false;
		}

	}
	else
	{
		set_curr_file(NULL);
		ret=false;
	}

	return ret;
}

void SiFileHandler::clear_curr_file()
{
	set_curr_file(NULL);
}
Boolean SiFileHandler::file_is_read_only()
{
	if(curr_file==NULL)
		return false;
	else
		return curr_file->is_read_only();
}
#ifdef DEBUG
Boolean SiFileHandler::perform_tests()
{

	return true;
}
#endif
