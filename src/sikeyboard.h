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

#ifndef SIKEYBOARD_H
#define SIKEYBOARD_H
#include <PalmOS.h>
#include <PalmTypes.h>
/**
@author Benjamin Roe
*/
class SiEditor;

class SiKeyboard
{
public:
	SiKeyboard(SiEditor * edit);
	void display_virtual_keyboard(KeyboardType kbd);
	Boolean handle_keyboard_keypress(EventPtr e);
	void set_key_selection();
	~SiKeyboard();
	void handle_event(EventPtr event);//eventsEnum type,Int16 x,Int16 y);
	SiEditor * m_editor;
	UInt16 get_current_keyboard() const;
	void set_current_keyboard(const UInt16 k);
protected:


	void do_keyboard(KeyboardType kbd);
	void display_keyboard(UInt16 dialog);
	void exit_keyboard_dialog();
	void toggle_caps_status();
	//	Int16 prev_height;
	void popup_form(const Int16 form);
	UInt16 current_keyboard;
	Boolean shift_pressed;

};

#endif
