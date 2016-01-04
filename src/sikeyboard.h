//
// C++ Interface: sikeyboard
//
// Description:
//
//
// Author: Benjamin Roe <sied@benroe.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
