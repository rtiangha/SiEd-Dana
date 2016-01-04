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

#include "edit_interface.h"
#include "utility.h"
#include "preferences.h"
#include "error.h"
#include "form_pos.h"
#include "test.h"

SiEditInterface::SiEditInterface()
{
#ifdef DEBUG_LOG
	log_entry("SiEditInterface::SiEditInterface()");
	log_entry_number("MemHandle size=",sizeof(MemHandle));
#endif

	m_exclusive_editor=the_editor_top=the_editor_active=the_editor_bottom=NULL;

	SiUtility::initialise_calls();
}

void SiEditInterface::initialise(Char * file_to_open)
{

	macro_recorder=new SiMacroRecorder();

	if(SiPreferencesDatabase::get_split_screen()&&NULL==file_to_open)
		split_screen();
	else
	{
		the_editor_active=the_editor_top=new SiEditor(SiUtility::CurrentScreenHeight,0,0);
		the_editor_active->read_preferences(file_to_open);
	}

	if(SiPreferencesDatabase::get_focused_editor()==0)
	{
		change_active_editor(the_editor_top);
	}
	else
	{
		change_active_editor(the_editor_bottom);
	}
	
	SiPreferencesDatabase::tidy();
	//	drag_resizing=false;
	//	drag_resizing_y=0;
	focus_offscreen_field();
	register_for_resize();
#ifdef DEBUG

	screen_toggled=false;
#endif
	#ifdef TEST_OBJECT_UNIT

	unsplit_screen();
	the_editor_top->initialise();

	perform_tests();

	perform_object_tests();
#endif
}

void SiEditInterface::register_for_resize()
{
	if(SiUtility::has_dynamic_IA())
	{
		DmSearchStateType searchState;
		LocalID dbID;
		UInt16 cardNo;
		Err err = DmGetNextDatabaseByTypeCreator(true,&searchState,sysFileTApplication, AppID, true, &cardNo, &dbID);

		if (!err)
		{
			SysNotifyRegister(cardNo,dbID,sysNotifyDisplayResizedEvent,
			NULL,sysNotifyNormalPriority,NULL);
		}
	}
	
}
void SiEditInterface::focus_offscreen_field()
{
	FormPtr frm=FrmGetFormPtr(ResformID_text);
	FrmSetFocus(frm, FrmGetObjectIndex(frm, 1112));
}
void SiEditInterface::set_size_limits(UInt16 formID)
{
	if(SiUtility::has_dynamic_IA())
	{
		#ifdef DEBUG_LOG
		if(formID==ResFileDialog)
		{
			DisplayError(DEBUG_MESSAGE,"DIA SET SIZE");
		}
		#endif
		FormPtr frmP=FrmGetFormPtr(formID);
		// Set the same policy for each form in application.  
		Err err;
 
   		// Enable the input trigger 

		// Set the event handler for the form, and set each form's  
		// size requirements.  
		WinHandle formWinH = FrmGetWindowHandle (frmP); 
   		switch (formID)
		{ 
      		case ResformID_text:
		case ResFileDialog:
		case ResPrefsDialog:
		case ResStatsDialog:
		case PasswordDialog:
		case AlphaKeyboardDialog:
		case NumKeyboardDialog:
		case IntlKeyboardDialog:
		  err = FrmSetDIAPolicyAttr(frmP, frmDIAPolicyCustom); 
		  err = PINSetInputTriggerState(pinInputTriggerEnabled);    				
         		WinSetConstraintsSize(formWinH, 80, 160, 1000,160, 160, 160); 
         		break;
 
		default:
		  //WinSetConstraintsSize(formWinH, 160,160,160,160,160,160);
		  break;
		}
   } 
}

SiEditInterface::~SiEditInterface()
{
	
#ifdef DEBUG_LOG
	log_entry("SiEditInterface::~SiEditInterface()");
#endif

	if(the_editor_top!=NULL)
	{
		delete the_editor_top;
		the_editor_top=NULL;
	}
	if(the_editor_bottom!=NULL)
	{
		delete the_editor_bottom;
		the_editor_bottom=NULL;
	}

#ifdef MACRO
	if(macro_recorder!=NULL)
	{
		delete macro_recorder;
		macro_recorder=NULL;
	}
#endif
}

SiEditor * SiEditInterface::get_editor_in_bounds(const Int16 x,const Int16 y)
{
#ifdef DEBUG_LOG
	log_entry("SiEditInterface::get_editor_in_bounds()");
#endif
	if(m_exclusive_editor!=NULL)
	  return m_exclusive_editor;
	SiEditor *ret;
	if (the_editor_top == NULL)
		ret = the_editor_bottom;
	else if (the_editor_bottom == NULL)
		ret = the_editor_top;
	else if(the_editor_top->is_focus_tap(x,y))
	  ret=the_editor_top;
	else if(the_editor_bottom->is_focus_tap(x,y))
	  ret=the_editor_bottom;
	else
	  ret=the_editor_active;

	if (ret != the_editor_active)
	{
		the_editor_active->lost_focus();
		SiPreferencesDatabase::set_focused_editor(ret->m_index);
		ret->give_focus();
	}
	return ret;
}
Int32 SiEditInterface::get_event_delay()
{

	if(the_editor_top!=NULL)
		if(!the_editor_top->finished_processing())
		{
			return PROCESSING_INTERVAL;
		}
	if(the_editor_bottom!=NULL)
		if(!the_editor_bottom->finished_processing())
			return PROCESSING_INTERVAL;

	return evtWaitForever;
}
void SiEditInterface::do_quick_slosh()
{
	if(the_editor_top!=NULL)
		if(!the_editor_top->finished_processing())
			the_editor_top->do_quick_slosh();
	if(the_editor_bottom!=NULL)
		if(!the_editor_bottom->finished_processing())
			the_editor_bottom->do_quick_slosh();
}

void SiEditInterface::change_active_editor(SiEditor * edit)
{
	if(the_editor_active!=NULL)
		the_editor_active->lost_focus();
	the_editor_active=edit;
	the_editor_active->give_focus();
	SiPreferencesDatabase::set_focused_editor(edit->m_index);
}

Boolean SiEditInterface::split_screen()
{
#ifdef DEBUG_LOG
	log_entry("SiEditInterface::split_screen()");
#endif

	Boolean start_up=false;
	if(the_editor_top==NULL)
	{
		the_editor_top=new SiEditor(SiPreferencesDatabase::get_top_height(),0,0);

		//		the_editor_top->give_focus();
		the_editor_active=the_editor_top;
		start_up=true;
	}

	if (the_editor_bottom == NULL)
	{

	  
		the_editor_top->set_ignore_events();
		the_editor_top->resize(SiPreferencesDatabase::get_top_height(), 0);


		the_editor_bottom =
			new SiEditor(SiUtility::CurrentScreenHeight-SiPreferencesDatabase::get_top_height()-1,
						 SiPreferencesDatabase::get_top_height()+1, 1);
		//set the scrollbar on the new editor only if it hasn't already done it
		//which happens if it has popped up a keyboard or something


		if(!start_up)
		{
			change_active_editor(the_editor_bottom);
			the_editor_bottom->read_preferences(NULL);
		}
		else
		{
			the_editor_top->read_preferences(NULL);
			the_editor_bottom->read_preferences(NULL);
			the_editor_bottom->lost_focus();
		}
		the_editor_top->clear_ignore_events();
		SiPreferencesDatabase::set_split_screen(true);
		return true;
	}
	else
		return false;
}

Boolean SiEditInterface::unsplit_screen()
{
#ifdef DEBUG_LOG
	log_entry("SiEditInterface::unsplit_screen()");
#endif

	if (the_editor_bottom != NULL)
	{
		the_editor_bottom->set_ignore_events();
		the_editor_top->set_ignore_events();
		SiEditor * temp_ed=the_editor_bottom;
		if(the_editor_active==the_editor_bottom)
			the_editor_active=NULL;
		the_editor_bottom=NULL;
		delete temp_ed;

		change_active_editor(the_editor_top);
		the_editor_top->resize(SiUtility::CurrentScreenHeight, 0);

		the_editor_top->clear_ignore_events();

		emit_redraw_signal(NULL);

		SiPreferencesDatabase::set_split_screen(false);

		return true;
	}
	else
		return false;
}

Boolean SiEditInterface::view_menu_handle(const UInt16 id)
{
	Boolean handled = false;

	switch (id)
	{
	case ViewMenuSplit:
		handled = split_screen();

		break;
	case ViewMenuUnsplit:

		handled=unsplit_screen();
		break;
#ifdef DEBUG

	case 3999:
		if(screen_toggled)
			resize_screen(160);
		else
			resize_screen(90);
		screen_toggled=!screen_toggled;
		break;
#endif

	}
	return handled;
}

Boolean SiEditInterface::handle_event(EventPtr event)
{
#ifdef DEBUG
	//  log_entry("Event");
#endif

	FormPtr form=NULL;
	Boolean handled = false;
#ifdef MACRO

	if(macro_recorder->give_event(event))
		return true;
#endif

	switch (event->eType)
	{
	case winDisplayChangedEvent:
		RectangleType displayBounds;
		form=FrmGetActiveForm();
		WinGetBounds(WinGetDisplayWindow(),&displayBounds);
		WinSetBounds(FrmGetWindowHandle(form), &displayBounds);
		//		FrmDrawForm(form);
		resize_screen(displayBounds.extent.y);



		//		
		handled=true;
		break;
	case winExitEvent:
		form=FrmGetFormPtr(ResformID_text);
		if (event->data.winExit.exitWindow == (WinHandle) form)
		{
			if (the_editor_top != NULL)
				the_editor_top->set_ignore_events();
			if (the_editor_bottom != NULL)
				the_editor_bottom->set_ignore_events();
		}
		break;
	case winEnterEvent:
		form=FrmGetFormPtr(ResformID_text);
		if (event->data.winEnter.enterWindow == (WinHandle) form)
		{
			if (the_editor_top != NULL)
				the_editor_top->clear_ignore_events();
			if (the_editor_bottom != NULL)
				the_editor_bottom->clear_ignore_events();

			focus_offscreen_field();
		}
		break;
	case menuEvent:
		if (!view_menu_handle(event->data.menu.itemID))
			the_editor_active->handle_menu_command(event->data.menu.itemID);

		handled = true;
		break;
	case menuCmdBarOpenEvent:
		the_editor_active->handle_commandbar_popup();
		event->data.menuCmdBarOpen.preventFieldButtons = true;

		break;
	case penDownEvent:
		if(!is_macro_tap(event->screenX,event->screenY))
		{
			the_editor_active = get_editor_in_bounds(event->screenX,event->screenY);
			if(NULL==m_exclusive_editor)
			  check_resize_drag_start(event->screenX,event->screenY);
			 
			if (event->screenY <= SiUtility::CurrentScreenHeight&&event->screenX<=SiUtility::SCREEN_WIDTH)
			{
			  if(m_exclusive_editor!=NULL)
			    m_exclusive_editor->handle_tap(event->screenX,event->screenY,event->tapCount);
			  else
			    the_editor_active->handle_tap(event->screenX,event->screenY,event->tapCount);
			  handled=false;
			}
		}
		
		break;

	case keyDownEvent:
		handled =the_editor_active->handle_key(event->data.keyDown.chr,event->data.keyDown.modifiers);
		break;
	case sclRepeatEvent:
		switch (event->data.sclRepeat.scrollBarID)
		{
		case ScrollBarMain:
		case ScrollBarTop:
			the_editor_top->handle_scroll(event->data.sclRepeat.value,event->data.sclRepeat.newValue);
			break;
		case ScrollBarBottom:
			the_editor_bottom->handle_scroll(event->data.sclRepeat.value,event->data.sclRepeat.newValue);
			break;
		}
		break;
	case ctlSelectEvent:
		if(event->data.ctlSelect.controlID==PopupTriggerButton)
		{
			macro_recorder->popup_macro_list();
			handled=true;
		}
		else
		  {
		    the_editor_active->handle_control(event);
		    handled=true;
		  }
		break;

	case nilEvent:
		do_quick_slosh();
		handled=true;
		break;
	case frmUpdateEvent:
		if(ResformID_text==event->data.frmUpdate.formID)
		  emit_redraw_signal(NULL);
		break;
	default:
		break;
	}


	return handled;
}
void SiEditInterface::check_resize_drag_start(const Int16 x,const Int16 y)
{
  #ifdef DEBUG
  ErrFatalDisplayIf(m_exclusive_editor!=NULL,"doing drag resize in exclusive mode");
  #endif
	if(the_editor_top!=NULL&&the_editor_bottom!=NULL)
	{
		if(the_editor_top->point_in_drag_area(x,y))
		{
		  //			drag_resizing=true;
		  //			drag_resizing_y=y;
			Int16 x_p=x;
			Int16 y_p=y;

			Coord prev_y=y;
			Boolean isPenDown=true;

			while (isPenDown)
			  {
			    //save battery life by putting a delay
			    //before checking the pen status again
			   
			    EvtGetPen(&x_p, &y_p, &isPenDown);
			    
			    if((abs(prev_y-y_p)>2)&&isPenDown)
			      {
				do_drag_resize(x_p,y_p);
				prev_y=y_p;
			      }

			      SysTaskDelay(10);
			  }
		}
	}


}

Boolean SiEditInterface::do_drag_resize(const Int16 x,Int16 y)
{
	if(the_editor_bottom==NULL)
		return false;
		if(y<the_editor_top->get_min_height())
		  y=the_editor_top->get_min_height()+1;
		if(SiUtility::CurrentScreenHeight-y<the_editor_bottom->get_min_height())
		  y=SiUtility::CurrentScreenHeight-the_editor_bottom->get_min_height();

		if(y==the_editor_top->HEIGHT)
		  return false;
		WinScreenLock(winLockDontCare);
		
		the_editor_top->resize(y,0);
		the_editor_bottom->resize(SiUtility::CurrentScreenHeight-the_editor_top->HEIGHT-1,the_editor_top->HEIGHT+1);
		
		emit_redraw_signal(NULL);
		
		//		drag_resizing_y=the_editor_top->HEIGHT;

		WinScreenUnlock();


		return true;

}

void SiEditInterface::emit_redraw_signal(SiEditor* source)
{
  if(m_exclusive_editor!=NULL)
    m_exclusive_editor->redraw();
  else if(the_editor_top!=NULL&&the_editor_top!=source)
		the_editor_top->redraw();
  else if(the_editor_bottom!=NULL&&the_editor_bottom!=source)
		the_editor_bottom->redraw();
}


Boolean SiEditInterface::intercept_event(EventPtr e)
{
#ifdef DEBUG_LOG

#endif

	if(macro_recorder==NULL)
		return false;
	if(e->eType==keyDownEvent)
	{
		if(the_editor_active!=NULL)
		{
			if(!macro_recorder->recording)
				if(e->data.keyDown.chr==vchrKeyboardAlpha||e->data.keyDown.chr==vchrKeyboardNumeric)
					return the_editor_active->handle_key(e->data.keyDown.chr,e->data.keyDown.modifiers);
		}
	}

	return false;
}


void SiEditInterface::change_font()
{
	if(the_editor_top!=NULL)
		the_editor_top->change_font(SiPreferencesDatabase::get_font());
	if(the_editor_bottom!=NULL)
		the_editor_bottom->change_font(SiPreferencesDatabase::get_font());

	do_drag_resize(0,the_editor_top->HEIGHT);
}

void SiEditInterface::make_exclusive(SiEditor * edit)
{
  m_exclusive_editor=edit;
}

void SiEditInterface::release_exclusive()
{
  m_exclusive_editor=NULL;
	focus_offscreen_field();
}

Boolean SiEditInterface::is_macro_tap(const Int16 x,const Int16 y)
{
	if(x>=72&&x<=95)
		if(y>=149&&y<=158)
			return true;

	return false;
}

#define RESIZE_MULT 1000
void SiEditInterface::resize_screen(const Int16 new_height)
{
  if(new_height==SiUtility::CurrentScreenHeight)
    return;
  /*  Char buff[100];
  StrPrintF(buff,"new_height=%i",new_height);
  DisplayError(DEBUG_MESSAGE,buff);*/
  UInt32 current_ratio;

  if(the_editor_bottom==NULL)
    current_ratio=RESIZE_MULT;
  else
    current_ratio=(UInt32)RESIZE_MULT*((UInt32)the_editor_top->HEIGHT)/(UInt32)SiUtility::CurrentScreenHeight;

	SiUtility::CurrentScreenHeight=new_height;
	UInt32 actual_new_height=((UInt32)current_ratio*(UInt32)new_height)/(UInt32)RESIZE_MULT;
	if(new_height<SiUtility::CurrentScreenHeight)
	{
		the_editor_top->resize(actual_new_height,0);
		if(the_editor_bottom!=NULL)
		{
			the_editor_bottom->resize(new_height-(the_editor_top->HEIGHT),the_editor_top->HEIGHT);
		}
	}
	else
	{
		the_editor_top->resize(actual_new_height,0);
		if(the_editor_bottom!=NULL)
		{
			the_editor_bottom->resize(new_height-(the_editor_top->HEIGHT),the_editor_top->HEIGHT);
		}
	}
	emit_redraw_signal(NULL);

#ifdef DEBUG
	Int16 total_height=the_editor_top->HEIGHT;
	if(the_editor_bottom!=NULL)
		total_height+=the_editor_bottom->HEIGHT;

	ErrFatalDisplayIf(new_height!=total_height,"Incorrect screen resize");
#endif
}
#ifdef TEST_OBJECT_UNIT
Boolean SiEditInterface::perform_tests()
{
	return true;
	for(Int16 i=0;i<15;++i)
	{

		split_screen();
		ErrFatalDisplayIf(the_editor_top==NULL||the_editor_bottom==NULL,"Edit interface not splitting");
		ErrFatalDisplayIf(the_editor_top->HEIGHT!=SiUtility::ACTIVE_SCREEN_HEIGHT/2||the_editor_top->TOP_Y!=0,"Incorrect position on split screen: top editor");
		ErrFatalDisplayIf(the_editor_bottom->HEIGHT!=SiUtility::ACTIVE_SCREEN_HEIGHT/2||the_editor_bottom->TOP_Y!=SiUtility::ACTIVE_SCREEN_HEIGHT/2,"Incorrect position on split screen: bottom editor");
		unsplit_screen();
		ErrFatalDisplayIf(the_editor_bottom!=NULL,"Edit Interface not unsplitting");
		ErrFatalDisplayIf(the_editor_top==NULL,"Incorrect editor being deleted on unsplit");
		ErrFatalDisplayIf(the_editor_top->HEIGHT!=SiUtility::ACTIVE_SCREEN_HEIGHT,"Incorrect height on unsplit");

		ErrFatalDisplayIf(the_editor_top->TOP_Y!=0,"Incorrect TOP_Y on unsplit");
	}
#ifdef DEBUG_LOG
	log_entry("SiEditInterface tests complete");
#endif

	return true;

}

#endif
