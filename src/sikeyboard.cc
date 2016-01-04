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

#include "sikeyboard.h"
#include "editor.h"
#include "form_pos.h"
#include "main.h"
#include "error.h"
#include "utility.h"
SiKeyboard * g_keyboard=NULL;

SiKeyboard::SiKeyboard(SiEditor * edit)
{
	m_editor=edit;
	current_keyboard=0;
	shift_pressed=false;
}


SiKeyboard::~SiKeyboard()
{}


Boolean keyboardEventLoop(EventPtr event)
{
	//event loop for file dialog (save/open)
	Boolean handled = false;

	FormPtr frmP;

	switch (event->eType)
	{
	case penDownEvent:
	case penMoveEvent:
	case penUpEvent:
	case sclRepeatEvent:
		if(event->screenY<0)
		{
			g_keyboard->handle_event(event);//event->eType,event->screenX,event->screenY+KEYBOARD_TOP);
			handled=true;
		}
		break;
	case keyDownEvent:
		g_keyboard->handle_event(event);//event->eType,event->screenX,event->screenY+KEYBOARD_TOP);
		break;
	case ctlSelectEvent:
		handled=g_keyboard->handle_keyboard_keypress(event);
		break;
	case frmLoadEvent:
		frmP=FrmInitForm(event->data.frmLoad.formID);
		FrmSetActiveForm(frmP);
		FrmSetEventHandler(frmP,(FormEventHandlerPtr) keyboardEventLoop);
		break;
	case frmOpenEvent:
		frmP = FrmGetActiveForm();
		/* initialize UI objects here */
		FrmDrawForm(frmP);
		handled = true;
		g_keyboard->set_key_selection();
		break;
	default:
		break;
	}
	return handled;
}
void SiKeyboard::handle_event(EventPtr event)//(eventsEnum type,Int16 x,Int16 y)
{
	FormPtr frm=FrmGetFormPtr(current_keyboard);
	FrmSetActiveForm(FrmGetFormPtr(ResformID_text));
	event->screenY+=KEYBOARD_TOP;

	FrmDispatchEvent(event);

	FrmSetActiveForm(frm);
}
void SiKeyboard::set_key_selection()
{

	FormPtr frm=FrmGetActiveForm();
	switch(current_keyboard)
	{
	case NumKeyboardDialog:
		FrmSetControlGroupSelection(frm,2,Num_select);
		break;
	case AlphaKeyboardDialog:
	case CapsAlphaKeyboardDialog:
		FrmSetControlGroupSelection(frm,2,Alpha_select);
		break;
	case IntlKeyboardDialog:
	case CapsIntlKeyboardDialog:
		FrmSetControlGroupSelection(frm,2,Intl_select);
		break;
	}

}

void SiKeyboard::display_keyboard(UInt16 dialog)
{
	current_keyboard=dialog;
	FrmGotoForm(dialog);
	set_key_selection();
}


Boolean SiKeyboard::handle_keyboard_keypress(EventPtr e)
{
	UInt16 id=e->data.ctlSelect.controlID;

	Boolean handled=false;
	switch(id)
	  {
	  case Num_select:
	    if(current_keyboard!=NumKeyboardDialog)
	      {
		display_keyboard(NumKeyboardDialog);
		handled=true;
	      }
	    break;
	  case Alpha_select:
	    if(current_keyboard!=AlphaKeyboardDialog )
	      {
		display_keyboard(AlphaKeyboardDialog);
		handled=true;
	      }
	    break;
	  case Intl_select:
	    if(current_keyboard!=IntlKeyboardDialog)
	      {
		display_keyboard(IntlKeyboardDialog);
		handled=true;
	      }
	    break;
	  case CapsKey:
		toggle_caps_status();
		handled=true;
		break;
	  case ShiftKey:
		toggle_caps_status();
		shift_pressed=!shift_pressed;
		handled=true;
		break;
	  case KeyboardDoneButton:
		exit_keyboard_dialog();
		handled=true;
		break;
	  default:
		WChar the_char;
		FormPtr frm=FrmGetFormPtr(current_keyboard);
		ControlType * button=(ControlType *)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,id));
		CtlSetValue(button,0);

		FrmSetActiveForm(FrmGetFormPtr(ResformID_text));
		switch(e->data.ctlSelect.controlID)
		{
		case DeleteKey:
			m_editor->handle_key(chrBackspace,0);
			break;
		case SpaceKey:
			m_editor->handle_key(' ',0);
			break;
		case TabKey:
			m_editor->handle_key(chrHorizontalTabulation,0);
			break;
		case ReturnKey:
			m_editor->handle_key(chrLineFeed,0);
			break;
		case CursorKeyUp:
			m_editor->handle_cursor_movement(VERTICAL,-1,0);
			break;
		case CursorKeyDown:
			m_editor->handle_cursor_movement(VERTICAL,1,0);
			break;
		case CursorKeyLeft:
			m_editor->handle_cursor_movement(HORIZONTAL,-1,0);
			break;
		case CursorKeyRight:
			m_editor->handle_cursor_movement(HORIZONTAL,1,0);
			break;
		default:
			the_char=TxtGetChar(CtlGetLabel(button),0);
			m_editor->handle_key(the_char,0);
			break;
		}
		FrmSetActiveForm(frm);
		if(shift_pressed)
		{
			toggle_caps_status();
			shift_pressed=false;
		}

		handled=true;
		break;
	}

	return handled;

}
void SiKeyboard::toggle_caps_status()
{
	switch(current_keyboard)
	{
	case AlphaKeyboardDialog:
		display_keyboard(CapsAlphaKeyboardDialog);
		break;
	case CapsAlphaKeyboardDialog:
		display_keyboard(AlphaKeyboardDialog);
		break;
	case IntlKeyboardDialog:
		display_keyboard(CapsIntlKeyboardDialog);
		break;
	case CapsIntlKeyboardDialog:
		display_keyboard(IntlKeyboardDialog);
		break;
	default:
		break;
	}
}
void SiKeyboard::exit_keyboard_dialog()
{
	FrmReturnToForm(ResformID_text);

	//	m_editor->resize(prev_height,0);
	m_editor->return_to_original_size();

	edit_interface->emit_redraw_signal(NULL);

	current_keyboard=0;
}

void SiKeyboard::display_virtual_keyboard(KeyboardType kbd)
{

	if(current_keyboard!=0)
		return;


	m_editor->resize_for_dialog(KEYBOARD_TOP-1);


	m_editor->make_cursor_visible();
	do_keyboard(kbd);

}
void SiKeyboard::set_current_keyboard(const UInt16 keyb)
{
	if(current_keyboard!=0||keyb==0)
		return;

	current_keyboard=keyb;

	m_editor->resize_for_dialog(KEYBOARD_TOP-1);

	m_editor->redraw();

	g_keyboard=this;
	popup_form(current_keyboard);
}

void SiKeyboard::do_keyboard(KeyboardType kbd)
{

	switch(kbd)
	{
	case kbdAlpha:
	case kbdDefault:
		current_keyboard=AlphaKeyboardDialog;
		break;
	case kbdNumbersAndPunc:
		current_keyboard=NumKeyboardDialog;
		break;
	case kbdAccent:
		current_keyboard=IntlKeyboardDialog;
		break;
	}
	g_keyboard=this;

	//then actually display the keyboard
	popup_form(current_keyboard);

}
void SiKeyboard::popup_form(const Int16 form)
{
	RectangleType rp;
	rp.topLeft.x=0;
	rp.topLeft.y=KEYBOARD_TOP;
	rp.extent.x=SiUtility::SCREEN_WIDTH;
	rp.extent.y=SiUtility::CurrentScreenHeight-KEYBOARD_TOP;
	WinEraseRectangle(&rp,0);
	FrmPopupForm(form);
}
UInt16 SiKeyboard::get_current_keyboard() const
{
	return (current_keyboard);
}
