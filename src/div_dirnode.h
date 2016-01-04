#ifndef DIV_DIRNODE_HPP
#define DIV_DIRNODE_HPP
#include "dirnode.h"

class div_SiDirNode : public SiDirNode
{
 public:
	div_SiDirNode() __attribute__ ((section ("diafns")));
	virtual void draw_self(UInt16 x,const UInt16 y,const UInt16 max_width)  __attribute__ ((section ("diafns")));
	virtual SiDirNode* get_copy() __attribute__ ((section ("diafns")));
};

#endif
