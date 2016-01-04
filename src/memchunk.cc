#include "memchunk.h"

#define STATUS_INDEX 0
#define SIZE_INDEX 2
#define NEXT_CHUNK_INDEX 8
#define PREV_CHUNK_INDEX 4



SiMemChunk()
{}
virtual ~SiMemChunk()
{}
void SiMemChunk::page_in()
{}
void SiMemChunk::page_out()
{}
SiMemChunk * SiMemChunk::next_chunk()
{
	return (SiMemChunk*)mem_ptr[NEXT_CHUNK_INDEX];
}
SiMemChunk * SiMemChunk::prev_chunk()
{
	return (SiMemChunk*)mem_ptr[PREV_CHUNK_INDEX];
}
UInt8 SiMemChunk::status()
{
	return (SiMemChunk*)mem_ptr[STATUS_INDEX];
}
Int16 SiMemChunk::size()
{
	return (SiMemChunk*)mem_ptr[SIZE_INDEX];
}
Char * SiMemChunk::data(Int8 mode)
{}
void SiMemChunk::set_next_chunk(SiMemChunk * chunk)
{}
void SiMemChunk::set_prev_chunk(SiMemChunk * chunk)
{}
void SiMemChunk::set_size(Int16 size)
{}
void SiMemChunk::set_status(Int8 status)
{}
