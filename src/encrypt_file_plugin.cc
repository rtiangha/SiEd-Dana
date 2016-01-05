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

#include "encrypt_file_plugin.h"

UInt16 encrypt_SiFilePlugin::libRefNum=0;
UInt16 encrypt_SiFilePlugin::libRefCount=0;

#define ENCRYPT_HEADER_SIZE 12
#define NUMBER_HASH_ITERATIONS 100
#define SALT_LENGTH 20
#define KEY_LENGTH 16


#define ENCRYPTED_FILE_TYPE {0,0,255,255}

const UInt8 encrypt_SiFilePlugin::enc_file_type[FILE_TYPE_SIZE]=ENCRYPTED_FILE_TYPE;

encrypt_SiFilePlugin::encrypt_SiFilePlugin(SiFile * parent): SiFilePlugin(parent)
{
#ifdef EN_LOG
	log_entry("encrypt_SiFilePlugin ctor");
#endif

	m_salt=m_key=pass_phrase=cache_data=NULL;
	cache_end=0;

	Err err=errNone;
	if(libRefCount==0)
	{
		err=AESLib_OpenLibrary(&libRefNum);

	}

	if(err==errNone)
		++libRefCount;
	else
		DisplayError(UNABLE_TO_LOAD_LIB,"opening encryption library AESLib");

	m_ctx = (aes_ctx*)MemPtrNew(sizeof(aes_ctx));
}

encrypt_SiFilePlugin::~encrypt_SiFilePlugin()
{

	if(cache_data!=NULL)
		MemPtrFree(cache_data);

	if(pass_phrase!=NULL)
	{
		MemSet(pass_phrase,StrLen(pass_phrase),0);
		MemPtrFree(pass_phrase);
		MemSet(m_key,KEY_LENGTH,0);
		MemPtrFree(m_key);
		MemPtrFree(m_salt);
	}

	if(m_ctx!=NULL)
		MemPtrFree(m_ctx);

	--libRefCount;

	if(libRefCount==0)
	{

		AESLib_CloseLibrary(libRefNum);
	}
}

void encrypt_SiFilePlugin::remove_trailing_zeros(SiMemChunk * chunk)
{
	BlockInt i=chunk->size;

	while(chunk->mem_ptr[i-1]==0)
	{
		--i;
	}
	if(i>0)
		chunk=SiMemHandler::resize_chunk(chunk,i,TO_START);
}

Char * encrypt_SiFilePlugin::get_key(Char * passphrase)
{
	Char* key=(Char*)MemPtrNew(KEY_LENGTH);

	Char* temp_hash_buff=(Char*)MemPtrNew(SALT_LENGTH);
	if(m_salt==NULL)
		m_salt=get_salt();

	UInt16 pass_length=StrLen(passphrase);
	if(pass_length>SALT_LENGTH)
		pass_length=SALT_LENGTH;
	else
	{
		//if required append the salt to the passphrase to make a big enough
		//input for the encryption
		MemMove(temp_hash_buff+pass_length,m_salt,SALT_LENGTH-pass_length);
	}

	//then copy the password in
	MemMove(temp_hash_buff,passphrase,pass_length);

	for(Int16 i=0;i<NUMBER_HASH_ITERATIONS;++i)
	{
		get_hash(temp_hash_buff,SALT_LENGTH);
	}

	MemMove(key,temp_hash_buff,KEY_LENGTH);
	MemPtrFree(temp_hash_buff);

	return key;
}

Char * encrypt_SiFilePlugin::get_salt()
{
	Char * salt=(Char*)MemPtrNew(SALT_LENGTH);
	for(Int16 i=0;i<SALT_LENGTH;++i)
	{
#ifdef DEBUG
		salt[i]=i;
#else

		salt[i]=(Int8)SysRandom(0);
#endif

	}
	return salt;
}

void encrypt_SiFilePlugin::get_hash(Char * conc,Int16 len)
{
	sha1_context ctx;
	sha1_starts(&ctx);
	sha1_update(&ctx,(unsigned char*)conc,len);
	sha1_finish(&ctx,(unsigned char*)conc);
}

Char * encrypt_SiFilePlugin::get_passphrase()
{
	FormPtr form=FrmInitForm(PasswordDialog);
	FrmSetFocus(form,FrmGetObjectIndex(form,PasswordField));
	Char * text=NULL;
	FieldPtr file_label=(FieldPtr)FrmGetObjectPtr(form,FrmGetObjectIndex(form,PasswordFileLabel));
	FldSetTextPtr(file_label,m_parent_file->get_location());

	Int16 ret=FrmDoDialog(form);

	Char * ret_text=NULL;
	FieldPtr field=(FieldPtr)FrmGetObjectPtr(form,FrmGetObjectIndex(form,PasswordField));
	text=FldGetTextPtr(field);

	if(ret==PasswordOK)
	{
		if(text!=NULL)
		{
			ret_text=(Char*)MemPtrNew(StrLen(text)+1);
			StrCopy(ret_text,text);
		}
	}

	FrmDeleteForm(form);
	return ret_text;
}

Boolean encrypt_SiFilePlugin::do_initialise(const Int16 access_mode)
{
	if(libRefCount==0)
		return false;

	if(pass_phrase==NULL)
	{
		pass_phrase=get_passphrase();

		if(pass_phrase==NULL)
			return false;

		if(access_mode==FILE_READ)
			read_header();

		m_key=get_key(pass_phrase);
	}

	//prepare the encrypt/decrypt key
	MemSet(m_ctx, sizeof(aes_ctx), (UInt8) 0);
	if(access_mode==FILE_WRITE)
	{
		//writing means encryption
		AESLibEncKey(libRefNum, (unsigned char*)m_key, KEY_LENGTH,m_ctx);
	}
	else
	{
		//else it's a decryption key
		AESLibDecKey(libRefNum, (unsigned char*)m_key, KEY_LENGTH, m_ctx);
	}

	cache_end=0;

	if(access_mode==FILE_WRITE)
	{
		if(cache_data==NULL)
			cache_data=(Char *)MemPtrNew(MINIMUM_BUFFER_SIZE);
		write_header();
	}
	return true;
}

void encrypt_SiFilePlugin::do_finalise()
{
#ifdef EN_LOG
	log_entry("encrypt_SiFilePlugin finalise()");
#endif
	//ensure all data in the cache is written to storage
	if(cache_data!=NULL)
	{

		flush_cache();

		MemPtrFree(cache_data);
		cache_data=NULL;
	}

}

void encrypt_SiFilePlugin::flush_cache()
{

	if(cache_end==0)
		return;

	//if necessary pad the cache to the minimum length
	//only required for last line
	if(cache_end<MINIMUM_BUFFER_SIZE)
		pad_buffer(cache_data,cache_end);

	SiMemChunk chunk;
	chunk.mem_ptr=cache_data;
	chunk.size=MINIMUM_BUFFER_SIZE;
	do_enc_write(&chunk);

	cache_end=0;
}

void encrypt_SiFilePlugin::process_data_to_storage(const SiMemChunk  * const chunk)
{
	SiMemChunk temp_chunk;

	Int16 number_buffers=chunk->size/MINIMUM_BUFFER_SIZE;
	Int16 amount_to_strip=chunk->size-(number_buffers*MINIMUM_BUFFER_SIZE);

	if(amount_to_strip>0)
	{
		BlockInt amount_initial_append=append_to_cache(chunk,amount_to_strip);
#ifdef EN_LOG

		log_entry_number("amount_initial_append=",amount_initial_append);
		log_entry_number("amount_to_strip=",amount_to_strip);

		ErrFatalDisplayIf(chunk->size<(amount_to_strip+amount_initial_append),"Too large strip");
#endif

		temp_chunk.size=chunk->size-(amount_to_strip+amount_initial_append);
		temp_chunk.mem_ptr=chunk->mem_ptr+amount_initial_append;
	}
	else
	{
		temp_chunk.mem_ptr=chunk->mem_ptr;
		temp_chunk.size=chunk->size;
	}

	if(temp_chunk.size>0)
	{
		//create an encrypted copy of the data
		//and write it to storage
		do_enc_write(&temp_chunk);
	}

}

void encrypt_SiFilePlugin::process_data_from_storage(SiMemChunk * chunk)
{
	decrypt_data(chunk);
}

void encrypt_SiFilePlugin::pad_buffer(Char * data,Int16 start_pos)
{

	for(UInt8 i=start_pos;i<MINIMUM_BUFFER_SIZE;++i)
		data[i]=0;
}

void encrypt_SiFilePlugin::do_enc_write(SiMemChunk * chunk)
{
#ifdef EN_LOG
	ErrFatalDisplayIf(chunk->size==0,"encrypting zero size chunk");
#endif

	encrypt_data(chunk);

	if(next_plugin==NULL)
	{
		m_parent_file->write_direct(chunk);
	}
	else
	{
		next_plugin->process_data_to_storage(chunk);
	}

	MemPtrFree(chunk->mem_ptr);
}

BlockInt encrypt_SiFilePlugin::append_to_cache(const SiMemChunk * const chunk,Int16 & amount_to_do)
{
	//first take the start of the text and try to fill up the cache with it

	BlockInt initial_amount=0;

	if(cache_end>0)
	{
		if(amount_to_do+cache_end<MINIMUM_BUFFER_SIZE)
		{
			initial_amount=MINIMUM_BUFFER_SIZE-cache_end;
			if(initial_amount>chunk->size)
				initial_amount=chunk->size;
		}
		else
			initial_amount=MINIMUM_BUFFER_SIZE-cache_end;

		if(initial_amount>0)
			MemMove(cache_data+cache_end,chunk->mem_ptr,initial_amount);

		cache_end+=initial_amount;
#ifdef EN_LOG

		ErrFatalDisplayIf(cache_end>MINIMUM_BUFFER_SIZE,"Over end of cache");
#endif

		if(cache_end==MINIMUM_BUFFER_SIZE)
			flush_cache();

		//then resize the chunk so as to make it a multiple of MINIMUM_BUFFER_SIZE again
		Int16 number_buffers=(chunk->size-initial_amount)/MINIMUM_BUFFER_SIZE;
		amount_to_do=(chunk->size-initial_amount)-(number_buffers*MINIMUM_BUFFER_SIZE);

#ifdef EN_LOG

		ErrFatalDisplayIf(amount_to_do<0,"Initial amount too great");
#endif

	}

	if(amount_to_do>0)
	{
		//else copy the remaining amount to strip from the end of the chunk
#ifdef EN_LOG
		log_entry_number("adding to cache ",amount_to_do);
		ErrFatalDisplayIf(cache_end>0,"Cache not flushed");

		ErrFatalDisplayIf(amount_to_do>=MINIMUM_BUFFER_SIZE,"Too large cache append 2");
#endif

		MemMove(cache_data,chunk->mem_ptr+chunk->size-amount_to_do,amount_to_do);
		cache_end+=amount_to_do;
	}

	return initial_amount;
}

void encrypt_SiFilePlugin::encrypt_data(SiMemChunk * chunk)
{

	//adjust the size of the chunk passed in to the amount that will actually be
	//encrypted, leaving the remainder to be cached or padded

	UInt32 length=chunk->size;
	Char * out_blk=(Char *)MemPtrNew(chunk->size);

	AESLibEncBigBlk(libRefNum, (unsigned char*)chunk->mem_ptr+chunk->size-length, (unsigned char*)out_blk,&length,false,NULL,m_ctx);
	//MemMove(out_blk,chunk->mem_ptr,chunk->size);

#ifdef EN_LOG

	ErrFatalDisplayIf(length!=chunk->size,"Incorrect encryption size");
#endif

	chunk->mem_ptr=out_blk;
}

void encrypt_SiFilePlugin::decrypt_data(SiMemChunk * chunk)
{
	UInt32 length=chunk->size;

	AESLibDecBigBlk(libRefNum, (unsigned char*)chunk->mem_ptr, (unsigned char*)chunk->mem_ptr,&length,false,NULL,m_ctx);

	remove_trailing_zeros(chunk);
#ifdef EN_LOG

	log_entry_number("Bytes decrypted ",length);
#endif
}

UInt32 encrypt_SiFilePlugin::adjust_size(UInt32 size) const
{
	//make sure that the parent file leaved enough space for the padding and header required
	UInt32 new_size=(MINIMUM_BUFFER_SIZE)*(size/MINIMUM_BUFFER_SIZE)+SALT_LENGTH+FILE_TYPE_SIZE;
	if(size%MINIMUM_BUFFER_SIZE!=0)
		new_size+=MINIMUM_BUFFER_SIZE;
	if(next_plugin!=NULL)
		new_size=next_plugin->adjust_size(new_size);

	return new_size;
}

void encrypt_SiFilePlugin::read_header()
{
#ifdef EN_LOG
	log_entry("Reading header");
#endif
	//	SiMemChunk *temp_chunk=get_chunk(NULL,SALT_LENGTH,NON_TEXT_HEAP);
	SiMemChunk temp_chunk;
	temp_chunk.mem_ptr=m_salt=(Char*)MemPtrNew(SALT_LENGTH);
	temp_chunk.size=SALT_LENGTH;
	m_parent_file->read_direct(&temp_chunk);
	//free_chunk(temp_chunk);
}

void encrypt_SiFilePlugin::write_header()
{
#ifdef EN_LOG
	log_entry("Writing Header");
#endif

	//header is FILE_TYPE(4 bytes, identifies SiEd-Dana encrytped) followed by the salt used, 20 bytes

	//Char * buff=(Char*)MemPtrNew(FILE_TYPE_SIZE+SALT_LENGTH);
	SiMemChunk *temp_chunk=SiMemHandler::get_chunk(NULL,FILE_TYPE_SIZE+SALT_LENGTH,NON_TEXT_HEAP);
	MemMove(temp_chunk->mem_ptr,enc_file_type,FILE_TYPE_SIZE);

	MemMove(temp_chunk->mem_ptr+FILE_TYPE_SIZE,m_salt,SALT_LENGTH);

	if(next_plugin==NULL)
		m_parent_file->write_direct(temp_chunk);
	else
	{
		next_plugin->process_data_to_storage(temp_chunk);
	}
#ifdef EN_LOG
	log_entry("Header Done");
#endif

	SiMemHandler::free_chunk(temp_chunk);
}

