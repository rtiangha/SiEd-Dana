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

/*
* 	AES encryption implemented using the freeware AES implementation AESLib from
*	Copera : http://www.copera.com/AESLib/
 
*/

#include "file_plugin.h"
#include "error.h"
#include "sha1.h"
#include "AESLib/AESLib.h"
#define MINIMUM_BUFFER_SIZE 16
#include "sections.h"
class encrypt_SiFilePlugin : public SiFilePlugin
{
public:
	encrypt_SiFilePlugin(SiFile * parent) __attribute__ ((section ("filefns")));
	virtual ~encrypt_SiFilePlugin() __attribute__ ((section ("filefns")));
	virtual UInt32 adjust_size(UInt32 size) const __attribute__ ((section ("filefns")));

protected:
	void remove_trailing_zeros(SiMemChunk * chunk) __attribute__ ((section ("filefns")));
	void pad_buffer(Char * data,Int16 start_pos) __attribute__ ((section ("filefns")));
	BlockInt append_to_cache(const SiMemChunk  * const chunk,Int16 &amount_done) __attribute__ ((section ("filefns")));

	Char * get_passphrase() __attribute__ ((section ("filefns")));
	Char * get_key(Char * passphrase) __attribute__ ((section ("filefns")));
	Char * get_salt() __attribute__ ((section ("filefns")));
	void get_hash(Char * conc,Int16 len) __attribute__ ((section ("filefns")));
	virtual Boolean do_initialise(const Int16 access_mode) __attribute__ ((section ("filefns")));
	virtual void do_finalise() __attribute__ ((section ("filefns")));
	void flush_cache() __attribute__ ((section ("filefns")));
	friend Int16 SiFile::write_direct(const SiMemChunk * const chunk);
	friend void SiFile::read_direct(SiMemChunk * chunk);
	virtual void process_data_to_storage(const SiMemChunk * const chunk) __attribute__ ((section ("filefns")));
	virtual void process_data_from_storage(SiMemChunk * chunk) __attribute__ ((section ("filefns")));
	void encrypt_data(SiMemChunk * chunk) __attribute__ ((section ("filefns")));
	void decrypt_data(SiMemChunk * chunk) __attribute__ ((section ("filefns")));
	void write_header() __attribute__ ((section ("filefns")));
	void read_header() __attribute__ ((section ("filefns")));
	void do_enc_write(SiMemChunk  *chunk) __attribute__ ((section ("filefns")));
	Char * pass_phrase;
	Char * cache_data;
	Char * m_salt;
	Char* m_key;
	Int16 cache_end;
	static UInt16 libRefNum;
	static UInt16 libRefCount;
	aes_ctx *m_ctx;
	static const UInt8 enc_file_type[FILE_TYPE_SIZE];
};

