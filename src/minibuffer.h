#include <PalmOS.h>
#include <PalmTypes.h>
#include "types.h"

#ifndef MINIBUFF_HPP
#define MINIBUFF_HPP

class SiMiniBuffer
{
public:
	SiMiniBuffer(Int16 x,Int16 y,Int16 max_width);
	~SiMiniBuffer();
	void give_key(WChar the_char);
	void set_title(Char * text);
	Char * get_text();
	void release_text();
protected:
	void erase_cursor();
	void draw_cursor();
	void copy_background();
	void restore_background();
	void draw_title();
	void redraw_on_insert_char(WChar the_char);
	void redraw_on_delete_char(WChar the_char);
	void redraw_on_scroll();
	Int16 get_left_x() const;
	Int16 m_curr_text_width;
	Int16 m_title_width;
	Int16 m_d_height;
	Char * m_title;
	MemHandle m_text_handle;
	Int16 m_x_offset;
	Int16 m_max_width;
	Int16 m_x;
	Int16 m_y;

	WinHandle m_backing_store;
};
#endif
