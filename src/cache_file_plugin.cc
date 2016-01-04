#include "cache_file_plugin.h"
#include "error.h"
#define CACHE_SIZE 16384

cache_SiFilePlugin::cache_SiFilePlugin(SiFile * file): SiFilePlugin(file)
{
#ifdef EN_LOG
	log_entry("Creating file write cache");
#endif

	cache_data=NULL;
	cache_cursor_pos=0;
}

Boolean cache_SiFilePlugin::do_initialise(const Int16 access_mode)
{
	if(access_mode==FILE_WRITE)
		initialise_cache();
	return true;
}

void cache_SiFilePlugin::do_finalise()
{
	delete_cache();
}

cache_SiFilePlugin::~cache_SiFilePlugin()
{
	delete_cache();
}

void cache_SiFilePlugin::process_data_to_storage(const SiMemChunk * const chunk)
{
	if(chunk->size+cache_cursor_pos>cache_end||cache_data==NULL)
	{
		Int32 amount_copied=cache_end-cache_cursor_pos;
		if(amount_copied>0)
			copy_to_cache(chunk->mem_ptr,amount_copied);
		do_flush();
		copy_to_cache(chunk->mem_ptr+amount_copied,chunk->size-amount_copied);
	}
	else
		copy_to_cache(chunk->mem_ptr,chunk->size);

}

void cache_SiFilePlugin::process_data_from_storage(SiMemChunk * chunk)
{}

void cache_SiFilePlugin::do_flush()
{
	if(cache_cursor_pos>0&&cache_data!=NULL)
	{
		SiMemChunk chunk;
		chunk.size=cache_cursor_pos;
		chunk.mem_ptr=cache_data;
		if(next_plugin==NULL)
		{
			m_parent_file->write_direct(&chunk);
		}
		else
		{
			next_plugin->translate_to_storage(&chunk);
		}
		cache_cursor_pos=0;
	}
}

void cache_SiFilePlugin::initialise_cache()
{
	//clear any existing cache
	do_flush();
	//create the write-combining cache for writes to VFS volumes
	if(cache_data==NULL)
	{
		UInt16 mult=1;
		while((cache_data=(Char*)MemPtrNew(CACHE_SIZE/mult))==NULL&&mult<10)
		{
			++mult;
		}

		cache_end=CACHE_SIZE/mult;

	}
	cache_cursor_pos=0;
}

void cache_SiFilePlugin::delete_cache()
{
	//remove the cache
	do_flush();
	if(cache_data!=NULL)
	{
		MemPtrFree(cache_data);
		cache_data=NULL;
		cache_cursor_pos=0;
	}
}

void cache_SiFilePlugin::copy_to_cache(const Char * data,const UInt32 bytes)
{
	//copy the data to the cache
	MemMove(cache_data+cache_cursor_pos,data,bytes);
	//move the cursor up
	cache_cursor_pos+=bytes;
}

