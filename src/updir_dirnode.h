#ifndef UPDIRDIRNODE_HPP
#define UPDIRDIRNODE_HPP
#include "dirnode.h"

class updir_SiDirNode : public SiDirNode
{
public:
	updir_SiDirNode() __attribute__ ((section ("diafns")));
	~updir_SiDirNode() __attribute__ ((section ("diafns")));
	virtual SiFile * get_file(Char * filename) __attribute__ ((section ("diafns")));
	virtual SiFile * create_file(Char * filename,UInt32 size) __attribute__ ((section ("diafns")));
	virtual SiDirNode* get_copy() __attribute__ ((section ("diafns")));
};
#endif
