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

#include "utility.h"
#include <Chars.h>
#include <PalmOSGlue.h>
#include "resources.h"
#include "types.h"
#include "error.h"
#include "main.h"
#include "VFSMgr.h"
#include "form_pos.h"
#include "AESLib/AESLib.h"
#include "Form.h"
Boolean SiUtility::VFS_SUPPORT=false;
Coord SiUtility::SCREEN_WIDTH=160;
Coord SiUtility::SCREEN_HEIGHT=160;
Coord SiUtility::ACTIVE_SCREEN_WIDTH=152;
Coord SiUtility::ACTIVE_SCREEN_HEIGHT=160;
Coord SiUtility::CurrentScreenHeight=160;
UInt32 SiUtility::screen_version=0;
Boolean SiUtility::m_encryption_support=false;
Boolean SiUtility::m_colour_support=false;
Boolean SiUtility::m_hires_support=false;
IndexedColorType SiUtility::fore_text_color=0;
IndexedColorType SiUtility::back_text_color=0;
IndexedColorType SiUtility::cursor_color=0;
IndexedColorType SiUtility::line_color=0;
IndexedColorType SiUtility::status_color=0;
Boolean SiUtility::m_has_dynamic_IA=false;
#ifdef USE_WIDTH_CACHE
Int16 SiUtility::cached_text_width=0;
BlockInt SiUtility::cached_text_char=0;
BlockInt SiUtility::cached_text_byte=0;
Char * SiUtility::cached_text_data=0;
#endif
#ifdef COLLECT_STATISTICS
UInt16 SiUtility::w_cache_hits=0;
UInt16 SiUtility::w_cache_misses=0;
UInt16 SiUtility::c_cache_hits=0;
UInt16 SiUtility::c_cache_misses=0;
UInt16 SiUtility::w_cache_hit_size=0;
UInt16 SiUtility::c_cache_hit_size=0;
#endif
#define CURSOR_RGB_R 213
#define CURSOR_RGB_G 60
#define CURSOR_RGB_B 45
#define LINE_RGB_R 50
#define LINE_RGB_G 57
#define LINE_RGB_B 186
#define STATUS_RGB_R 80
#define STATUS_RGB_G 80
#define STATUS_RGB_B 80

//IndexedColorType SiUtility::back_line_color=0;
fntcall get_char_width=NULL;
//fntcall SiUtility::get_char_width=NULL;
#define AESLibName "AESLib"

/*Int16 abs(const Int16 num)
{
	if(num<0)
		return -num;
	else
		return num;
}*/
void SiUtility::initialise_calls()
{
	get_char_width=(fntcall)SysGetTrapAddress(sysTrapFntWCharWidth);
	if(SysGetTrapAddress(sysTrapSysUnimplemented)==get_char_width)
	{
		get_char_width=FntGlueWCharWidth;
#ifdef DEBUG

		DisplayError(DEBUG_MESSAGE,"Falling back to Glue library");
#endif

	}
}
Boolean SiUtility::system_file(const char *file)
{
	//return true if the file is a system file
	if (StrNCompare(file, "sied-scratch", 12) == 0)
		return true;
	else if(StrNCompare(file,"sied-temp",9)==0)
		return true;
	else
		return false;
}

Int16 SiUtility::NextTabStop(const Int16 x)
{

	//  Int16 r=
	return BORDER+((x / TAB_WIDTH + 1) * TAB_WIDTH);
	//  if(r>ACTIVEWIDTH)
	//r=ACTIVEWIDTH;

	//  return r;
}
void SiUtility::check_system_capabilities()
{

	check_vfs_support();
	check_screen_capabilities();
	//check for the AES library
	UInt16 libRefNum=0;
	Err err=AESLib_OpenLibrary(&libRefNum);

	if(err!=sysErrLibNotFound)
	{
#ifdef TEST_OBJECTS_LOG
		log_entry("AES Support");
#endif

		m_encryption_support=true;
		err=AESLib_CloseLibrary(libRefNum);
	}
	else
		m_encryption_support=false;
	RGBColorType rgb;
	rgb.r=CURSOR_RGB_R;
	rgb.g=CURSOR_RGB_G;
	rgb.b=CURSOR_RGB_B;

	fore_text_color=UIColorGetTableEntryIndex(UIFieldTextHighlightForeground);
	back_text_color=UIColorGetTableEntryIndex(UIFieldTextHighlightBackground);
	cursor_color=WinRGBToIndex(&rgb);

	rgb.r=LINE_RGB_R;
	rgb.g=LINE_RGB_G;
	rgb.b=LINE_RGB_B;
	line_color=WinRGBToIndex(&rgb);
	rgb.r=STATUS_RGB_R;
	rgb.g=STATUS_RGB_G;
	rgb.b=STATUS_RGB_B;
	status_color=WinRGBToIndex(&rgb);
	CurrentScreenHeight=SCREEN_HEIGHT;
	check_dynamic_IA();

}
Boolean SiUtility::has_colour_support()
{
	return m_colour_support;
}

Boolean SiUtility::has_hires_support()
{
	//palmos 5 or better means hires
	return (m_hires_support);
}

void SiUtility::check_screen_capabilities()
{


	FtrGet(sysFtrCreator, sysFtrNumWinVersion, &screen_version);
	if(screen_version>=(UInt32)3)
	{
		//on OS 4 or better, check for colour
		WinScreenMode(winScreenModeGet,NULL,NULL,NULL,&m_colour_support);
		if(screen_version>=(UInt32)4)
		{
			UInt32 attr;
			WinScreenGetAttribute(winScreenDensity, &attr);

			if (attr == kDensityDouble)
			{
				m_hires_support=true;
			}
		}
	}


}
Boolean SiUtility::is_break_char(const WChar ch)
{
	switch(ch)
	{
	case chrApostrophe:
		return false;
	case chrLineFeed:
		return true;
	case chrHorizontalTabulation:
	  return true;
	default:
		return TxtCharIsSpace(ch);
	}

}
Boolean SiUtility::is_break_char(const WChar ch,Boolean ignore_punctuation)
{
	switch(ch)
	{
	case chrApostrophe:
		return false;
	case chrLineFeed:
		return true;
	case chrHorizontalTabulation:
		return true;
	default:
		if(TxtCharIsPunct(ch))
			return ignore_punctuation;
		else
			return TxtCharIsDelim(ch);
	}

}
BlockInt SiUtility::calc_n_chars(const Char* const data,const BlockInt bytes)
{
	//given the number of bytes of data at * data,
	//work out how many characters
	BlockInt n_chars=0;
	BlockInt byte_offset=0;

	while(byte_offset<bytes)
	{
		++n_chars;

#ifdef DEBUG

		Int16 w=TxtNextCharSize(data,byte_offset);
		byte_offset+=w;

	#ifdef DEBUG_LOG
		if(w>1)
			log_entry_number("Char width ",w);
	#endif
#else

		byte_offset+=TxtNextCharSize(data,byte_offset);
#endif

	}
	/*      while(byte_offset<bytes)
	                {
	                        switch(TxtByteAttr(data[byte_offset]))
	                        {
	                        case byteAttrFirst:
	                                ++n_chars;
	                                byte_offset+=2;
	                                break;
	                        case byteAttrLast:
	                                //++n_chars;
	                                ++byte_offset;
	                                break;
	                        case byteAttrSingle:
	                                n_chars+=2;
	                                byte_offset+=2;
	                                break;
	                        }
	                }
	                if(byte_offset==bytes+1)
	                        --n_chars;*/
#ifdef __DEBUG_LOG2__
	Char buff[30];
	StrPrintF(buff,"n_chars=%i",n_chars);
	log_entry(buff);
#endif

	return n_chars;
}

BlockInt SiUtility::calc_byte_length(const Char* data,const BlockInt n_chars)
{
	//given the number of characters, work out how many bytes
	BlockInt n_bytes=0;
	for(BlockInt c=0;c<n_chars;++c)
	{
		n_bytes+=TxtNextCharSize(data,n_bytes);
	}
	return n_bytes;
}

BlockInt SiUtility::last_char_boundary(Char * data,BlockInt *n_chars,BlockInt max_bytes)
{

	//find the last safe position to break the buffer passed in
	//avoids looking at a possibly invalid split two-byte character
	if(max_bytes==0)
	{
		(*n_chars)=0;
		return 0;
	}
	if(max_bytes==1)
	{
		(*n_chars)=1;
		return 1;
	}
	UInt8 flags=TxtByteAttr((UInt8)data[max_bytes-1]);
	if((!(flags&byteAttrLast))&&(!(flags&byteAttrSingle)))
	{
		(*n_chars)=calc_n_chars(data,max_bytes-1);
#ifdef __DEBUG_LOG__

		log_entry("backing pointer up");
#endif

		return max_bytes-1;
	}
	else
	{
		(*n_chars)=calc_n_chars(data,max_bytes);
		return max_bytes;
	}
}



Boolean operator <=(const Position & lhs,const Position & rhs)
{
	if(lhs.line<rhs.line)
		return true;
	else if(lhs.line==rhs.line&&lhs.x<=rhs.x)
		return true;
	else
		return false;
}


void SiUtility::check_vfs_support()
{
	UInt32 Version;
	/*	FtrGet(sysFtrCreator,
		       sysFtrNumROMVersion, &Version);
		if(Version<sysMakeROMVersion(4,0,0,sysROMStageRelease,0))
			return false;*/

	if(FtrGet(sysFileCExpansionMgr,expFtrIDVersion, &Version)!=errNone)
		VFS_SUPPORT=false;
	else if(FtrGet(sysFileCVFSMgr,vfsFtrIDVersion, &Version)!=errNone)
		VFS_SUPPORT=false;
	else
		VFS_SUPPORT=true;
}
Boolean SiUtility::encryption_support()
{
	return m_encryption_support;
}
void SiUtility::invalidate_cache()
{
	cached_text_data=NULL;
}

Int16 SiUtility::CorrectCharsWidth_byte(char * data,const BlockInt max_byte,Boolean * contains_line_feed,Int16 show_codes)
{

	//calculate the width on the screen of the passed in text
	//properly ignoring line feeds and taking account of tabs
	Int16 width=0;
	BlockInt byte_offset=0;
	WChar the_char;

#ifdef USE_WIDTH_CACHE

	BlockInt max_char=0;
	if(cached_text_byte<=max_byte&&data==cached_text_data)
	{
#ifdef COLLECT_STATISTICS
		SiUtility::add_width_cache_hit(cached_text_byte);
#endif

		width=cached_text_width;
		byte_offset=cached_text_byte;
		max_char=cached_text_char;
	}
#ifdef COLLECT_STATISTICS
	else
	{
		SiUtility::add_width_cache_miss();
	}
#endif
	#endif

	while(byte_offset<max_byte)
	{
		byte_offset+=TxtGetNextChar(data,byte_offset,&the_char);
#ifdef USE_WIDTH_CACHE

		++max_char;
#endif

		switch(the_char)
		{
		case chrLineFeed:
			if(show_codes&EOL_CODES)
				width+=RETURN_BITMAP_WIDTH;
			//meant to be no break here
		case chrCarriageReturn:
			if(contains_line_feed!=NULL)
				(*contains_line_feed)=true;

			break;
		default:
			width+=CorrectCharWidth(the_char,width);
			break;
		}
	}
#ifdef USE_WIDTH_CACHE
	if( (cached_text_byte<=max_byte&&data==cached_text_data) || (data!=cached_text_data) )
	{
		cached_text_byte=byte_offset;
		cached_text_char=max_char;
		cached_text_width=width;
		cached_text_data=data;
	}
#endif
	return width;
}
Int16 SiUtility::CorrectCharsWidth_char(char * data,const BlockInt max_char,Boolean * contains_line_feed,Int16 show_codes)
{
	//calculate the width on the screen of the passed in text
	//properly ignoring line feeds and taking account of tabs
	Int16 width=0;
	BlockInt byte_offset=0;
	WChar the_char;
	BlockInt n_chars=0;
#ifdef USE_WIDTH_CACHE

	if(cached_text_char<=max_char&&data==cached_text_data)
	{

#ifdef COLLECT_STATISTICS
		SiUtility::add_width_cache_hit(cached_text_byte);
#endif

		width=cached_text_width;
		n_chars=cached_text_char;
		byte_offset=cached_text_byte;
	}
#ifdef COLLECT_STATISTICS
	else
	{
		SiUtility::add_width_cache_miss();
	}
#endif

#endif
	while(n_chars<max_char)
	{
		byte_offset+=TxtGetNextChar(data,byte_offset,&the_char);
		++n_chars;
		switch(the_char)
		{
		case chrLineFeed:
			if(show_codes&EOL_CODES)
				width+=RETURN_BITMAP_WIDTH;
			//meant to be no break here
		case chrCarriageReturn:
			if(contains_line_feed!=NULL)
				(*contains_line_feed)=true;

			break;
		default:
			width+=CorrectCharWidth(the_char,width);
			break;
		}
	}
#ifdef USE_WIDTH_CACHE
	if( (cached_text_char<=max_char&&data==cached_text_data) || (data!=cached_text_data) )
	{
		cached_text_byte=byte_offset;
		cached_text_char=max_char;
		cached_text_width=width;
		cached_text_data=data;
	}
#endif
	return width;
}
IndexedColorType SiUtility::get_status_text_color()
{
	return status_color;
}
IndexedColorType SiUtility::get_line_color()
{
	return line_color;
}
IndexedColorType SiUtility::get_fore_color()
{
	return fore_text_color;
}
IndexedColorType SiUtility::get_back_color()
{
	return back_text_color;
}

IndexedColorType SiUtility::get_cursor_color()
{
	return cursor_color;
}

void SiUtility::set_field_term(FieldType* text_field,MemHandle source_handle,Boolean set_selection)
{
	if(source_handle==NULL)
		return;
	Char * prev=(Char*)MemHandleLock(source_handle);

	set_field_term(text_field,prev,set_selection);
	MemHandleUnlock(source_handle);
}

void SiUtility::set_field_term(FieldType* text_field,const Char * prev,Boolean set_selection)
{
	//set the text of the field to the string passed in
	//if set_selection is true, selects the entire string
	//  DisplayError(DEBUG_MESSAGE,prev);
	if(prev==NULL)
		return;

	FldEraseField(text_field);
	clear_field(text_field);

	FldInsert(text_field,prev,StrLen(prev));

	FldDrawField(text_field);

	if(set_selection)
		FldSetSelection(text_field,0,StrLen(prev));
}

void SiUtility::clear_field(FieldType * text_field)
{
	UInt16 length=FldGetTextLength(text_field);
	FldDelete(text_field,0,length);
	FldSetInsertionPoint(text_field,0);
}

Boolean SiUtility::has_dynamic_IA()
{
		return m_has_dynamic_IA;
}

void SiUtility::check_dynamic_IA()
{
	UInt32 version;
	Err err = FtrGet(pinCreator, pinFtrAPIVersion, &version); 
	if (!err && version)
	{ 
   		m_has_dynamic_IA=true;
	}
}

#ifdef COLLECT_STATISTICS
void SiUtility::add_width_cache_hit(const UInt16 size)
{
	++w_cache_hits;
	w_cache_hit_size+=size;
}
void SiUtility::add_char_cache_hit(const UInt16 size)
{
	++c_cache_hits;
	c_cache_hit_size+=size;
}
void SiUtility::add_width_cache_miss()
{
	++w_cache_misses;
}

void SiUtility::add_char_cache_miss()
{
	++c_cache_misses;
}

void SiUtility::display_stats()
{
	Char buff[300];
	if(w_cache_hits==0)
		w_cache_hits=1;
	if(c_cache_hits==0)
		c_cache_hits=1;
	Int16 w_percent=(w_cache_hits*100)/(w_cache_hits+w_cache_misses);
	Int16 c_percent=(c_cache_hits*100)/(c_cache_hits+c_cache_misses);

	Int16 w_i_saved=w_cache_hit_size/w_cache_hits;
	Int16 c_i_saved=c_cache_hit_size/c_cache_hits;
	StrPrintF(buff,"Width cache hit percent:%i , Average iterations saved:%i, Char cache hit percent: %i , average iterations saved:%i",w_percent,w_i_saved,c_percent,c_i_saved);
	DisplayError(DEBUG_MESSAGE,buff);
}

#endif
