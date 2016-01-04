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
