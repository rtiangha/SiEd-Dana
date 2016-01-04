#include "div_dirnode.h"
#include "utility.h"
#include "error.h"
div_SiDirNode::div_SiDirNode()
{
 
  select_action=NONE;
  name=NULL;
  image_id=UP_ARROW_BITMAP;
  
  x_offset=ICON_WIDTH;
  children_handle=NULL;
  child_nodes=NULL;
  number_children=0;
  m_font=stdFont;
  m_is_encrypted=false;
}


SiDirNode* div_SiDirNode::get_copy()
{
  return new div_SiDirNode();
}

#define MAIN_MEM_DIVIDER "Internal Memory"
void div_SiDirNode::draw_self(UInt16 x,const UInt16 y,const UInt16 max_width)
{

  FontID prev_font=FntSetFont(stdFont);
  UInt16 len=StrLen(MAIN_MEM_DIVIDER);
  UInt16 width=SiUtility::CorrectCharsWidth_byte(MAIN_MEM_DIVIDER,len,NULL);
  UInt16 height=FntLineHeight()/2;
  WinDrawChars(MAIN_MEM_DIVIDER,StrLen(MAIN_MEM_DIVIDER),x+(max_width-width)/2,y);
  WinDrawLine(x+3,y+height,x-3+(max_width-width)/2,y+height);
  WinDrawLine(x+1+(max_width-width)/2+width,y+height,x+max_width-5,y+height);
}
