#include "file_interface.h"
#include "file_plugin.h"
#include "error.h"
#include "encrypt_file_plugin.h"


SiFilePlugin::SiFilePlugin(SiFile * parent_file)
{
	m_parent_file=parent_file;
	next_plugin=NULL;
}

SiFilePlugin::~SiFilePlugin()
{

	finalise();
	if(next_plugin!=NULL)
		delete next_plugin;
}

Boolean SiFilePlugin::initialise(const Int16 access_mode)
{
	//! Called by the parent file from SiFile->set_access_mode
	//! Sets up the plugin ready for reading/writing
	if(!do_initialise(access_mode))
		return false;

	if(next_plugin!=NULL)
	{
		if(!next_plugin->initialise(access_mode))
			return false;
	}
	return true;
}

void SiFilePlugin::flush()
{
	do_flush();
	if(next_plugin!=NULL)
		next_plugin->flush();
}
void SiFilePlugin::do_flush()
{}
void SiFilePlugin::do_finalise()
{}

Boolean SiFilePlugin::do_initialise(const Int16 access_mode)
{
	return true;
}

void SiFilePlugin::add_plugin(SiFilePlugin* t_plug)
{
	//! Put the passed plugin at the end of the chain
	//! of plugins
	if(next_plugin==NULL)
		next_plugin=t_plug;
	else
		next_plugin->add_plugin(t_plug);
}

void SiFilePlugin::translate_to_storage(const SiMemChunk * const chunk)
{
	process_data_to_storage(chunk);
	/*	if(next_plugin!=NULL)
		{
			DisplayError(UNABLE_TO_SAVE,"file_plugin calling translate");
			next_plugin->translate_to_storage(chunk);
		}*/

}
void SiFilePlugin::translate_from_storage(SiMemChunk * chunk)
{
	process_data_from_storage(chunk);
	if(next_plugin!=NULL)
	{
		next_plugin->translate_from_storage(chunk);
	}
}

void SiFilePlugin::finalise()
{
	do_finalise();
	if(next_plugin!=NULL)
		next_plugin->finalise();
}

void SiFilePlugin::process_data_to_storage(const SiMemChunk * const chunk)
{}
void SiFilePlugin::process_data_from_storage(SiMemChunk * chunk)
{}
UInt32 SiFilePlugin::adjust_size(UInt32 size) const
{
	return size;
}

SiFilePlugin * SiFilePlugin::get_plugin_chain(SiFile * file)
{
	//! Reads the first two or four bytes of the file to determine
	//! if it is a file that requires plugins to read
	//! if so, it returns the plugins required as a linked list

	UInt16 file_header;
	SiMemChunk temp_chunk;

	//read data into file_header
	temp_chunk.mem_ptr=(Char*)&file_header;
	temp_chunk.size=FILE_TYPE_SIZE/2;

	if(file->get_size()<(UInt32)temp_chunk.size)
		return NULL;
	file->read_direct(&temp_chunk);
	SiFilePlugin * plugin=NULL;

	//all file headers start 00, so file_header is zero if there is a file type present
	if(file_header==0)
	{

		file->read_direct(&temp_chunk);

		switch (file_header)
		{
		case ENCRYPTED_FILE:
			plugin=new encrypt_SiFilePlugin(file);
			break;
			//TODO: More file types here!
		default:
			file->move_cursor(2,BACKWARDS);
			break;
		}
	}
	else
	{
		//reset the file cursor if no header present
		//as that means it's just a plain text file
		file->move_cursor(2,BACKWARDS);
	}

	return plugin;
}
