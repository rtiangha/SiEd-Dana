#include <PalmOS.h>
#include <PalmTypes.h>

enum {READ_ONLY,READ_WRITE};
enum {MAIN_MEMORY,STORAGE};
//MemHandle GetSwapSpace(const Int16 size);
class SiMemChunk
{
public:
	SiMemChunk();
	virtual ~SiMemChunk();
	void page_in();
	void page_out();
	SiMemChunk * next_chunk();
	SiMemChunk * prev_chunk();
	void set_next_chunk(SiMemChunk * chunk);
	void set_prev_chunk(SiMemChunk * chunk);
	void set_size(Int16 size);
	void set_status(Int8 status);
	UInt8 status();
	Int16 size();
	Char * data(Int8 mode);
protected:
	Char * mem_ptr;
};

class SiMasterMemChunk: public SiMemChunk
{
public:
	SiMasterMemChunk();
	virtual ~SiMasterMemChunk();
protected:
	MemHandle swap_handle;
};
