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
#ifdef HANDSPRING
#include <68K/Hs.h>
#include <Common/Libraries/FileBrowser/FileBrowserLibCommon.h>
#include <68K/System/HsExtKbdUtils.h>
#include <Common/System/HsKeyCodes.h>
#endif

#ifdef COLLAPSIBLE
#include <M68KHwr.h>
#endif
#include <FrmGlue.h>
#include "form_pos.h"
#include "sikeyboard.h"
#include "preferences.h"
#include "utility.h"
#include "editor.h"
#include "textview.h"
#include "filehandler.h"
#include "error.h"
#include "resources.h"
#include "main.h"
#include "file_interface.h"
#include "filedialog.h"
#include "siscrollbar.h"

#define ctrlKeyMask 32
#define TEST_CHARS(data_char,term_char) (find_option_match_case?matches=!(data_char==term_char):matches=TxtCaselessCompare((Char*)(&data_char)+(2-d_size),d_size,NULL,(Char*)(&term_char)+(2-t_size),t_size,NULL))
#define INC_SEARCH_TITLE_TEXT "S:"
#define INC_SEARCH_MAX_WIDTH 69
#define FIND_FIELD_X 9
#define FIND_FIELD_Y (3+REPLACE_FORM_TOP)
#define FIND_FIELD_X_EXTENT 154
#define FIND_FIELD_Y_EXTENT 9
#define NOT_FOUND_RED 240
#define NOT_FOUND_GREEN 10
#define NOT_FOUND_BLUE 10
#define keycodeHome 2
#define keycodeEnd 3
#define WORD_COUNT_LEFT 80
#define WORD_COUNT_TOP 28
#define CHAR_COUNT_TOP 43
SiEditor *g_editor = NULL;
Boolean keyboard_active_event=true;

Boolean stats_dialog_loop(EventPtr event)
{
  //Event loop used while the statistics dialog is active
  Boolean handled=false;
  switch (event->eType)
    {
    case winEnterEvent:
      g_editor->draw_stats_data();
      break;
    case ctlSelectEvent:
      if(event->data.ctlSelect.controlID==StatsCalc)
	{
	  g_editor->update_word_count();
	  handled=true;
	}
      break;
    default:
      break;
    }
  return handled;
}

Boolean replaceEventLoop(EventPtr event)
{
  //Event loop used while the replace/find dialog is active
  Boolean handled = false;

  switch (event->eType)
    {
    case ctlSelectEvent:
      switch (event->data.ctlSelect.controlID)
	{
	case ReplaceFindNext:
	  g_editor->do_replace_find_next();
	  handled = true;
	  break;
	case ReplaceReplace:
	  g_editor->do_replace_replace_next(true);
	  handled = true;
	  break;
	case ReplaceReplaceAll:
	  g_editor->do_replace_replace_all();
	  handled = false;
	  break;
	case ReplaceMatchCaseCheck:
	  g_editor->toggle_find_match_case();
	  handled = true;
	  break;
	case ReplaceWholeWordCheck:
	  g_editor->toggle_find_whole_word();
	  handled = true;
	  break;
	case ReplaceFromTopCheck:
	  g_editor->toggle_find_from_top();
	  handled=true;
	  break;
	case ReplaceCancel:
	  g_editor->finished_replace();
	  handled=true;
	  break;
	  
	}

      break;
    case fldEnterEvent:
      g_editor->erase_not_found();  
      break;
    case keyDownEvent:
      g_editor->erase_not_found();
      g_editor->reset_from_top_find();
      
      if(event->data.keyDown.chr==chrLineFeed)
	{
	  g_editor->do_replace_find_next();
	}
      break;
    case penDownEvent:
      if (event->screenY <= SiUtility::CurrentScreenHeight)
	{  
	  g_editor->handle_tap(event->screenX,event->screenY,event->tapCount);
	}
      break;
      
    case sclRepeatEvent:
      handled = false;
      
      g_editor->handle_scroll(event->data.sclRepeat.value,event->data.sclRepeat.newValue);
      break;
     
      break;
      
    default:
      break;
    }
  return handled;
}

SiEditor::SiEditor(const Int16 height, const Int16 top_y,
		   const UInt16 index)
{
  m_font_id=stdFont;
  HEIGHT = height;
  TOP_Y = top_y;
  m_index = index;
  //  prev_replace_term=prev_search_term=NULL;
  start_selected_position.line=end_selected_position.line=SIGNED_NONE;

  RGBColorType notFoundRGB;
  notFoundRGB.r=NOT_FOUND_RED;
  notFoundRGB.g=NOT_FOUND_GREEN;
  notFoundRGB.b=NOT_FOUND_BLUE;
  m_not_found_colour=WinRGBToIndex(&notFoundRGB);
  
  m_keyboard=new SiKeyboard(this);
	
  m_text_handler = new SiTextHandler();
  m_document=m_text_handler->get_document();
	
  m_file_handler = new SiFileHandler(this,m_document);
	
  m_text_view = new SiTextView(this, m_document);
	
  m_mini_buffer=NULL;


  set_sizes(SiPreferencesDatabase::get_font());
	
  initialise();
	
  m_incremental_search=false;
  find_option_match_case = false;
  find_option_whole_word = false;
  find_option_from_top=false;

}

Int16 SiEditor::get_cursor_line() const
{
	return cursor_pos.line;
}
void SiEditor::show()
{
  m_active=true;
  m_text_view->enable_screen_updates();
    redraw();
}

void SiEditor::hide()
{
  m_active=false;
  m_text_view->disable_screen_updates();
}

void SiEditor::initialise()
{
  m_active=true;
  m_text_view->enable_screen_updates();
  mfrm=FrmGetFormPtr(ResformID_text);
  Altfield=(FieldPtr)FrmGetObjectPtr(mfrm,FrmGetObjectIndex(mfrm,AltCharField));
  first_find=true;
  exiting=false;
  ignore_events = false;
  m_displaying_dialog=false;
  m_read_only=false;
  cursor_pos.line = 0;
  cursor_pos.x=0;
  preferred_cursor_x = WIDTH + 1;

  // clear the selection
  start_selected_position.line=SIGNED_NONE;
  end_selected_position.line=SIGNED_NONE;

  m_text_handler->initialise();
  m_text_view->initialise_display();
  SiMemHandler::compact_memory();
  m_not_found=false;	
  m_has_found_term=false;
  check_gsi_status();
}

Boolean SiEditor::is_focus_tap(const Int16 x,const Int16 y)
{
  
  if(y<=BOTTOM_Y&&y>=TOP_Y)
    return true;
  else if(y>=TOP_Y&&y<=BOTTOM_Y+STATUS_BAR_HEIGHT)
    return !m_text_view->point_in_drag_area(x,y);
  else
    return false;
}

Boolean SiEditor::point_in_drag_area(const Int16 x,const Int16 y)
{
  //return true if the point is in the area of the status line
  //which will (eventually) allow the user to move the split view divider
  if(m_displaying_dialog)
    return false;
  else
    return m_text_view->point_in_drag_area(x,y);
}

Int16 SiEditor::get_min_height() const
{
  return STATUS_BAR_HEIGHT+m_text_view->get_line_height()*3+2;
}

void SiEditor::do_replace_find_next()
{
  erase_not_found();
  m_text_view->erase_cursor();
  
  FormPtr frm = FrmGetActiveForm();//FrmGetFormPtr(ResReplaceDialog);
  FieldPtr rep = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceField));
  char *rep_text = FldGetTextPtr(rep);

  if (rep_text != NULL)
    {
      if (!g_editor->find_term(rep_text))
	{
	  
	  m_text_view->stop_waiting();
	 
	  show_not_found();
	}
      else
	{
	  m_text_view->stop_waiting();
	}
		
    }
}

void SiEditor::show_not_found()
{
  
  m_not_found=true;
  
  RectangleType rp;
  FormPtr frm=FrmGetActiveForm();
  FrmGetObjectBounds(frm,FrmGetObjectIndex(frm,ReplaceField),&rp);
  IndexedColorType prev_colour=0;

  if(SiUtility::has_colour_support())
    {
      prev_colour=WinSetForeColor(m_not_found_colour);
      
    }
  WinDrawRectangleFrame(simpleFrame,&rp);
  if(SiUtility::has_colour_support())
    WinSetForeColor(prev_colour);
}

void SiEditor::reset_from_top_find()
{
  first_find=true;
  
}

void SiEditor::erase_not_found()
{
    if(m_not_found)
    {
      
      
      m_not_found=false;
      RectangleType rp;

      FormPtr frm=FrmGetActiveForm();
      FrmGetObjectBounds(frm,FrmGetObjectIndex(frm,ReplaceField),&rp);
      WinEraseRectangleFrame(simpleFrame,&rp);
      
      //      FieldPtr find_text_field = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceField));
     

      //      FldDrawField(find_text_field);
      
      
    }
  
}

void SiEditor::notify_selection_changed()
{
  if(m_displaying_dialog)
    {
      FormPtr frm=FrmGetActiveForm();
      UInt16 index=FrmGetFocus(frm);
      if(index!=noFocus)
	{
	  UInt16 id=FrmGetObjectId(frm,index);

	  if(id==ReplaceField||id==ReplaceWithField)
	    {
	      const Char * text=m_document->get_stripped_text(get_min_selected_position(),get_max_selected_position(),MAX_REPLACE_FIELD_LENGTH);
	      if(text!=NULL)
		{
		  FieldPtr field=(FieldPtr)FrmGetObjectPtr(frm,index);
		  SiUtility::set_field_term(field,text,true);
		  MemPtrFree(const_cast<Char*>(text));
		}
	    }

	}
      m_has_found_term=false;
    }
  
}
void SiEditor::initialise_replace_fields()
{
  //copy the previously searched for and replaced contents of the two fields on the replace dialog


  Position min_sel=get_min_selected_position();
  if(min_sel.line!=SIGNED_NONE)
    {
      FormPtr frm=FrmGetActiveForm();//FrmGetFormPtr(ResReplaceDialog);
      FieldPtr find_text_field = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceField));

      SiUtility::clear_field(find_text_field);
      const Char * sel_text=m_document->get_stripped_text(min_sel,get_max_selected_position(),MAX_REPLACE_FIELD_LENGTH);
      if(sel_text!=NULL)
	{
	  SiUtility::set_field_term(find_text_field,sel_text,true);
	  MemPtrFree(const_cast<Char*>(sel_text));
	}

    }
}

void SiEditor::replace_all(Char * rep_text,Char * with_text)
{
  start_waiting();
  clear_selection();
  m_has_found_term=false;
  m_text_view->disable_screen_updates();

  while(replace_text(rep_text,with_text,false))
    {
      //intentionally empty
    }

  m_text_view->enable_screen_updates();
  stop_waiting();
}

Boolean SiEditor::replace_text(Char * rep_text,Char * with_text,const Boolean show_end)
{
  if (!m_has_found_term)
    {
      if(show_end)
	{
	  clear_selection();
	 
	}
      Boolean found=find_term(rep_text);

      if (show_end)
	{

	  m_text_view->stop_waiting();

	  if(!found)
	    {
	      show_not_found();
	    }
	  else
	    return true;		       
	}
      if(!found)
	return false;	       		
    }

  Position start_pos=get_min_selected_position();
  delete_selection();
  //m_document->make_valid_pos(start);
  if(show_end)
    m_text_view->erase_cursor();
  Position temp_cursor_pos(SIGNED_NONE,NONE);

  if (with_text != NULL)
    {
      Int16 len = StrLen(with_text);
      Int16 bytes=SiUtility::calc_byte_length(with_text,len);

      if(!m_text_handler->insert_chars(with_text,len,bytes,cursor_pos,temp_cursor_pos))
	return false;
      //end=temp_cursor_pos;
    }
  if(temp_cursor_pos.line!=SIGNED_NONE)
    update_cursor_pos(temp_cursor_pos,false);
  else
    update_cursor_pos(start_pos,false);


  return true;
}

Boolean SiEditor::do_replace_replace_next(Boolean show_end)
{
  if(m_read_only)
    return false;
  erase_not_found();
  //replace just the next occurence of the search term
  //if it is already selected, otherwise just do a find 
  FormPtr frm = FrmGetActiveForm();//FrmGetFormPtr(ResReplaceDialog);

  // find the search term
  FieldPtr rep = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceField));

  Char *rep_text = FldGetTextPtr(rep);

  if (rep_text == NULL)
    return false;
		
  /* if(show_end)
    {
      m_text_view->erase_cursor();
      }*/

  FieldPtr with = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceWithField));
  char *with_text = FldGetTextPtr(with);
  return replace_text(rep_text,with_text,show_end);
  
  
}

void SiEditor::do_replace_replace_all()
{
  if(m_read_only)
    return;
  erase_not_found();
  //Replace all occurences of the search term with the replace term
  //Position start,end,middle;
  Position c_pos = cursor_pos;
  FormPtr frm = FrmGetActiveForm();
  FrmEraseForm(FrmGetActiveForm());
	
  FormPtr back_form = FrmGetFormPtr(ResformID_text);
  FrmSetActiveForm(back_form);
  FrmDrawForm(back_form);
  m_text_view->redraw_display(true);

  m_text_view->start_waiting();
  m_text_view->disable_screen_updates();

  // find the search term
  FieldPtr rep = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceField));

  Char *rep_text = FldGetTextPtr(rep);

  if (rep_text == NULL)
    return;
		
  FieldPtr with = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceWithField));
  char *with_text = FldGetTextPtr(with);

  //do the replacing
  replace_all(rep_text,with_text);
 
  //update screen
  m_text_view->enable_screen_updates();
  m_text_view->stop_waiting();
  m_document->make_valid_pos(c_pos);
  update_cursor_pos(c_pos,false);
  redraw();

}

void SiEditor::do_quick_slosh()
{
  //do a quick bit of tidying of long/short lines
  //called when the EvtGetEvent returns a nilEvent
#ifdef BENCHMARK_QUICK_SLOSH
  Int16 start_time=TimGetTicks();
#endif
	
  m_document->tidy_up_lines(QUICK_SLOSH_SIZE);
	
#ifdef BENCHMARK_QUICK_SLOSH
  start_time=TimGetTicks()-start_time;
  Char buff[100];
  StrPrintF(buff,"Ticks to slosh=%i",start_time);
  DisplayError(DEBUG_MESSAGE,buff);
#endif
}
Boolean SiEditor::finished_processing() const
{
  //returns true if the document is completely finished sloshing
  //or if the editor has run out of memory or is displaying a dialog
  if(m_document->has_had_memory_error()||ignore_events)
    {
      return true;
    }  
  else
    return (m_document->last_line_tidied>=m_document->get_number_blocks()-1);


  
}

void SiEditor::set_sizes(const fontID font_id)
{
  //set the text view up for the passed in font
  clear_selection();
  FntSetFont(stdFont);
  m_font_id=font_id;
  FntSetFont(font_id);
  set_viewport();
}

void SiEditor::set_viewport()
{
  //size the text view
  COMMAND_LINE = ((HEIGHT-STATUS_BAR_HEIGHT)-1) / FntLineHeight() ;
  BOTTOM_Y = TOP_Y + COMMAND_LINE*FntLineHeight();
}

void SiEditor::change_font(const fontID font)
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::changing font");
#endif

  if(font!=m_font_id||font==customFontID)
    {
      cursor_pos.x=0;
      cursor_pos.line=0;
      set_sizes(font);
      reopen();

      m_text_view->notify_font_changed();
    }

}
void SiEditor::select_current_word()
{
  clear_selection();
  set_start_selected_position(m_text_handler->find_prev_word_boundary(cursor_pos));
  set_end_selected_position(m_text_handler->find_next_word_boundary(cursor_pos));
}

void SiEditor::save_preferences()
{
  SiPreferencesDatabase::set_cursor_pos(m_index,get_c_pos());
  SiPreferencesDatabase::set_top_line(m_index,m_text_view->get_top_line());
  SiPreferencesDatabase::set_read_only(m_index,m_read_only);
  SiFile *file=m_file_handler->get_file();
  Boolean set_open=false;
  if(file!=NULL)
    {
      if(file->get_location()!=NULL)
	{
	  if(StrLen(file->get_location())<MAX_PATH_LENGTH)
	    {

	      SiPreferencesDatabase::set_file_open(m_index,file->get_location(),file->get_vol_ref(),file->get_type());
#ifdef DEBUG
	      if(file->get_location()!=NULL&&SiPreferencesDatabase::get_location_data(m_index)!=NULL)
		ErrFatalDisplayIf(StrCompare(file->get_location(),SiPreferencesDatabase::get_location_data(m_index))!=0,"Incorrect filename saved");
#endif

	      set_open=true;
	    }
	}
    }
  if(!set_open)
    {

      SiPreferencesDatabase::set_file_open(m_index,NULL,SIGNED_NONE,SIGNED_NONE);
    }

  SiPreferencesDatabase::set_keyboard_open(m_keyboard->get_current_keyboard());

}

//!
//! Take the preferences struct and setup the editor correctly
//!
void SiEditor::read_preferences(Char * file_to_open)
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::read_preferences()");
#endif
  m_text_view->enable_screen_updates();	
  if(NULL==file_to_open)
    {
      if(SiPreferencesDatabase::get_location_data(m_index)==NULL)
	{
	  m_file_handler->read_from_scratch(m_index);
	  
	}
      else
	{
	  if( m_file_handler->read_file(
				    SiPreferencesDatabase::get_location_data(m_index),
				    SiPreferencesDatabase::get_file_type(m_index),
				    SiPreferencesDatabase::get_vol_ref(m_index)) )
	    {
	      if(SiPreferencesDatabase::get_read_only(m_index))
		toggle_read_only_mode();
	    }
	  m_document->clear_dirty_flag();
	  
	}
    

      ensure_tidied(SiPreferencesDatabase::get_top_line(m_index));
     
      m_text_view->make_top_line(SiPreferencesDatabase::get_top_line(m_index),true);
      update_cursor_pos(SiPreferencesDatabase::get_cursor_pos(m_index),true);	
    }
  else
    {
      m_file_handler->read_file(file_to_open,DOC_FILE,THE_CARD);

    }



  m_text_view->redraw_chrome(true);
  if(SiPreferencesDatabase::get_keyboard_open()&&SiPreferencesDatabase::get_focused_editor()==m_index)
    m_keyboard->set_current_keyboard(SiPreferencesDatabase::get_keyboard_open());

}

void SiEditor::give_focus()
{
  FntSetFont(m_font_id);
  m_text_view->notify_got_focus();
}

void SiEditor::resize(Int16 height, const Int16 top_y)
{
 
#ifdef DEBUG_LOG
  log_entry_number("Resizing editor: ",height);
#endif
  m_text_view->clear_self();

  if(height<get_min_height())
    height=get_min_height();

  //horrible hack to make the bottom editor
  //stay flush with the bottom of the screen
  if(m_index==0)
    height=m_text_view->get_line_multiple_size(height);

  #ifdef DEBUG
  ErrFatalDisplayIf(height<get_min_height(),"Resizing too small");
  #endif
  HEIGHT = height;
  TOP_Y = top_y;

  set_viewport();

  FormPtr frm=FrmGetFormPtr(ResformID_text);
  m_text_view->notify_size_changed();

  if(HEIGHT>=SiUtility::CurrentScreenHeight)
    {
      FrmShowObject(frm,FrmGetObjectIndex(frm,PopupTriggerButton));
    }

  do_scroll(0,true,true);

	//save the current
  if(m_index==1)
    SiPreferencesDatabase::set_top_height(HEIGHT);

  move_controls();
}

void SiEditor::update_control(FormPtr frm,UInt16 id)
{
     Coord x,y;
  UInt16 index=FrmGetObjectIndex(frm,id);
   FrmGetObjectPosition(frm,index,&x,&y);
   ControlType * ctl=(ControlType*)FrmGetObjectPtr(frm,index);
   CtlEraseControl(ctl);
   FrmSetObjectPosition(frm,index,x,TOP_Y+HEIGHT-STATUS_BAR_HEIGHT);
}

void SiEditor::move_controls()
{
  if(SiUtility::has_dynamic_IA())
    {
 
      FormPtr frm=FrmGetFormPtr(ResformID_text);
      if(m_index==1||(m_index==0&&HEIGHT==SiUtility::CurrentScreenHeight))
	{
	  update_control(frm,PopupMacroList);
	  update_control(frm,PopupPositionListBottom);
	  update_control(frm,PopupTriggerButton);
	  update_control(frm,PasteButton);
	  update_control(frm,CopyButton);

	}
      else if(m_index==0&&HEIGHT<SiUtility::CurrentScreenHeight)
	{
	  update_control(frm,PopupPositionListTop);

	}
    }
}

void SiEditor::lost_focus()
{
  m_text_view->notify_lost_focus();
}

SiEditor::~SiEditor()
{
  exiting=true;
  m_text_view->start_waiting();
  save_preferences();

  if(m_file_handler->get_file()==NULL)
    {
      if(m_document->file_dirty())
	m_file_handler->write_to_scratch(m_index);
      else if(m_document->get_number_blocks()==0)
	m_file_handler->delete_scratch_file(m_index);
    }
  else
    {
      m_file_handler->delete_scratch_file(m_index);
      if(m_document->file_dirty())
	m_file_handler->save_file();
    }

  delete m_file_handler;
  m_file_handler=NULL;
  delete m_text_view;
  m_text_view=NULL;
  delete m_text_handler;
  m_text_handler=NULL;
  delete m_keyboard;
  m_keyboard=NULL;

  //  if (prev_search_term != NULL)
  //    MemHandleFree(prev_search_term);
  //  if(prev_replace_term!=NULL)
  //    MemHandleFree(prev_replace_term);
	
  stop_inc_search();

}

void SiEditor::set_ignore_events()
{
  ignore_events = true;
}
void SiEditor::start_waiting()
{
  m_text_view->start_waiting();
}
void SiEditor::stop_waiting()
{
  m_text_view->stop_waiting();
}
void SiEditor::clear_ignore_events()
{
  ignore_events = false;

  EvtFlushPenQueue();
}

void SiEditor::do_scroll(const Int16 amount, Boolean scroll,const Boolean quick)
{
  if(ensure_tidied(m_text_view->get_top_line()+amount))
    scroll=true;

  m_text_view->notify_scrolled(amount,scroll,quick);
}

Boolean SiEditor::ensure_tidied(const Int16 line)
{
  return m_document->ensure_tidied(line+COMMAND_LINE*2);
}

//!
//! Delete the currently selected text
//!
Boolean SiEditor::delete_selection()
{
  if(m_read_only)
    return false;
  if (start_selected_position.line == SIGNED_NONE || end_selected_position.line == SIGNED_NONE)
    return false;
  cursor_pos=get_min_selected_position();
  Position min=get_min_selected_position();
  Position max=get_max_selected_position();
  m_text_view->erase_cursor();
  clear_selection();

  cursor_pos=m_text_handler->delete_chars(min,max);

  //m_document->make_valid_pos(cursor_pos);
  //m_text_view->notify_block_changed(earliest_change, end_redraw_pos);

  if(m_document->get_number_blocks()>0)
    {
      m_document->make_valid_pos(cursor_pos);
      m_document->check_newline_position(cursor_pos,0);
		
    }
  else
    {
		
      cursor_pos.line=0;
      cursor_pos.x=0;
		
      //m_text_view->notify_scrolled(0,true,false);
    }
  check_scroll_position();
#ifdef LOG_ENTRY
  log_entry_number("Delete_selection::cursor line=",cursor_pos.line);
  log_entry_number("Delete_selection::cursor x=",cursor_pos.x);
#endif

  update_cursor_pos(cursor_pos,false);
  return true;
}

//! copy the current selection to the clipboard
//! Displays an error if it doesn't fit
Boolean SiEditor::copy_selection_to_clipboard()
{
  if (start_selected_position.line == SIGNED_NONE || end_selected_position.line == SIGNED_NONE)
    return true;

  BlockInt actual_nbytes;
  Position min_pos,max_pos;
  Int16 b_b_index1;

  //start and end_selected_position.line lines are locked here
  if(start_selected_position<end_selected_position)
    {

      min_pos = start_selected_position;
      max_pos = end_selected_position;

    }
  else if(end_selected_position<start_selected_position)
    {
      max_pos = start_selected_position;
      min_pos = end_selected_position;
    }
  else
    return true;

  BlockInt  start ;

  block *curr_block = m_document->get_line(min_pos.line,b_b_index1);

  m_document->get_char_bound(min_pos.line,curr_block->mem_chunk->mem_ptr,min_pos.x,&start);

  m_document->release_line(b_b_index1);

  Err err = errNone;
  Int16 curr_line=min_pos.line;
  Boolean first = true;
  Boolean success=true;
  while(curr_line<=max_pos.line)
    {
      curr_block=m_document->get_line(curr_line,b_b_index1);
      if((curr_block->mem_chunk==NULL)&&success)
	{
	  m_document->release_line(b_b_index1);
	  err=errNone;
	  break;
	}
      if(curr_line==max_pos.line)
	{
	  m_document->get_char_bound(max_pos.line,curr_block->mem_chunk->mem_ptr,max_pos.x,&actual_nbytes);
	  actual_nbytes-=start;

	}
      else
	{
	  actual_nbytes=curr_block->mem_chunk->size-start;
	}

      if (first)
	{
	  ClipboardAddItem(clipboardText,curr_block->mem_chunk->mem_ptr + start,(UInt16)actual_nbytes);
	}
      else
	{

	  err =ClipboardAppendItem(clipboardText,curr_block->mem_chunk->mem_ptr+start, (UInt16)actual_nbytes);
	}

      if (err != errNone)
	{
	  DisplayError(CLIPBOARD_TRUNCATION, "");
	  //	  curr_line=max_pos.line+1;
	  success=false;
	}

      m_document->release_line(b_b_index1);
      start=0;
      ++curr_line;
      first=false;
    }
  return success;
}

void SiEditor::paste_clipboard()
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::paste_clipboard()");
#endif
  UInt16 length=0;
  if(m_read_only)
    return;
  MemHandle clip = ClipboardGetItem(clipboardText, &length);

  if (NULL==clip||0==length)
    {
      return;
    }

  delete_selection();
  Char *text = (char *) MemHandleLock(clip);

  BlockInt n_chars=SiUtility::calc_n_chars(text,length);
  if(!m_text_handler->insert_chars(text,n_chars,length,cursor_pos,cursor_pos))
    {
      MemHandleUnlock(clip);
      return;
    }
  MemHandleUnlock(clip);
  update_cursor_pos(cursor_pos,true);
}

void SiEditor::delete_forward_cursor(const UInt16 modifiers)
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::delete_forward");
#endif
  if(m_read_only)
    return;
  Position next=m_text_handler->get_next_pos(cursor_pos);

  m_text_view->erase_cursor();
  if(modifiers&controlKeyMask)
    {
      next=m_text_handler->find_next_word_boundary(next);
      set_start_selected_position(cursor_pos);
      set_end_selected_position(next,true);
      delete_selection();
      
    }
  else if(!(next==cursor_pos))
    {
      m_text_view->erase_cursor();
      cursor_pos=m_text_handler->delete_chars(cursor_pos,next);
      
      update_cursor_pos(cursor_pos,false);
    }
}

void SiEditor::delete_backward_cursor(const UInt16 modifiers)
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::delete_backward");
#endif
  if(m_read_only)
    return;
  if (!(cursor_pos.line == 0&&cursor_pos.x==0))
    {

      Position prev=m_text_handler->get_prev_pos(cursor_pos);
      m_text_view->erase_cursor();
      if(modifiers&controlKeyMask)
	{
	  prev=m_text_handler->find_prev_word_boundary(prev);
	  set_start_selected_position(prev);
	  set_end_selected_position(cursor_pos,true);
	  delete_selection();

	}
      else
	{
	  cursor_pos=m_text_handler->delete_chars(prev,cursor_pos);
	  update_cursor_pos(cursor_pos,false);
	}
      //m_document->check_newline_position(cursor_pos,1);

    }
}

void SiEditor::delete_chars(const Position start,const Position end)
{
  m_text_view->erase_cursor();
  clear_selection();
  cursor_pos=m_text_handler->delete_chars(start,end);
  update_cursor_pos(cursor_pos,false);
}

void SiEditor::insert_chars(const Char * data, const BlockInt n_chars,const BlockInt n_bytes)
{
  m_text_view->erase_cursor();
  clear_selection();
  m_text_handler->insert_chars(data,n_chars,n_bytes,cursor_pos,cursor_pos);
  update_cursor_pos(cursor_pos,false);
}

void SiEditor::set_selection(const Position start,const Position end)
{
  set_start_selected_position(start);
  set_end_selected_position(end);
}

//!
//! Insert the character at the current cursor position
//!
void SiEditor::insert_ch_cursor(const WChar c)
{
#ifdef DEBUG_LOG
  log_entry("SiEditor::insert_ch_cursor");
#endif
  if(m_incremental_search)
    handle_inc_search_key(c);
  else
    {
      if(m_read_only)
	return;
      Boolean del=true;
      if(!delete_selection())
	{
	  m_text_view->erase_cursor();

	  del=false;	 
	}
      
      Int16 i=TxtCharSize(c);
#ifdef NEW_LINE_OPTIONS
      if(!do_new_line_insert(c))
#endif
	if(m_text_handler->insert_chars((Char*)&c+(2-i),1,i, cursor_pos,cursor_pos))
	  update_cursor_pos(cursor_pos,del);
    }
}
#ifdef NEW_LINE_OPTIONS
Boolean SiEditor::do_new_line_insert(const WChar c)
{
  if(chrLineFeed==c)
    {
      switch (SiPreferencesDatabase::new_line_mode())
	{
	case PALM_NEWLINE:
	  return false;
	case WINDOWS_NEWLINE:
	  break;
	case MAC_NEWLINE:
	  break;
	}

      return true;
    }
}
#endif
//!
//! Deselect the current selection
//!
void SiEditor::clear_selection()
{
  if (start_selected_position.line != SIGNED_NONE&&end_selected_position.line != SIGNED_NONE)
    {
      Position min_pos=get_min_selected_position();
      Position max_pos=get_max_selected_position();
      start_selected_position.line=end_selected_position.line=SIGNED_NONE;
      m_text_view->notify_selection_cleared();
      m_text_view->notify_text_changed(min_pos,max_pos);
      m_has_found_term=false;
    }
}

//!
//! Called to set the initial position in the selection when the user taps
//!
void SiEditor::set_start_selected_position(const Position p)
{
  if(m_document->get_number_blocks()==0)
    return;
  clear_selection();
  start_selected_position=p;
}
//! Called when the user lifts the stylus
//! sets the selection and redraws
void SiEditor::set_end_selected_position(Position p,Boolean check_newline)
{
  m_text_view->erase_cursor();
  if(m_document->get_number_blocks()==0)
    return;
  if(check_newline)
    m_document->check_newline_position(p,-1);
  Position prev_sel=end_selected_position;
  end_selected_position=p;
  if(prev_sel.line==SIGNED_NONE)
    m_text_view->redraw_selection(start_selected_position,p);
  else
    m_text_view->redraw_selection(prev_sel,p);
		
	
  update_cursor_pos(p,false);

}

Boolean SiEditor::valid_tap(const Int16 x, const Int16 y)
{
#ifdef LOG_ENTRY
  log_entry("sieditor::valid_tap");
#endif
  if (x < 0 || y < TOP_Y || x > ACTIVEWIDTH || y > BOTTOM_Y)
    return false;
  else
    return true;
}

//! Save the file to storage and reopen it
//! Used when changing font as it is quicker than
//! trying to reslosh entire document in memory
void SiEditor::reopen()
{
  Boolean read_only=m_read_only;
  m_file_handler->write_to_scratch(m_index);

  m_file_handler->read_from_scratch(m_index);

  set_open_file();
  if(read_only)
    toggle_read_only_mode();
  update_cursor_pos(cursor_pos,true);
}
//!
//! Show the main menu
//!
void SiEditor::popup_menu()
{
  return;
  MenuBarType * the_menu=MenuInit(MAIN_MENU);
  MenuSetActiveMenu(the_menu);
  MenuDrawMenu(the_menu);
}

Boolean SiEditor::ui_tap(Coord x,Coord y)
{
#ifdef LOG_ENTRY
  log_entry("sieditor::ui_tap");
#endif

  if(m_text_view->point_in_save_star(x,y)&&m_document->file_dirty())
    {
      save_current_file();
      return true;
    }
  else if(point_in_line_number(x,y))
    {
      return true;
    }
  else if(m_text_view->point_in_file_name(x,y))
    {
      do_open_file();
      return true;
    }
  else
    return false;
}
void SiEditor::select_word(Coord x,Coord y)
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::select_word");
#endif

  Position pos=m_text_view->find_position(x,y);
  set_start_selected_position(m_text_handler->find_prev_word_boundary(pos,true));
  set_end_selected_position(m_text_handler->find_next_word_boundary(pos,true));
}

void SiEditor::handle_commandbar_popup()
{
  UInt16 length;
  ClipboardGetItem(clipboardText,&length);
  if(length>0)
    {
#ifdef LOG_ENTRY
      log_entry("SiEditor::adding paste");
#endif

      MenuCmdBarAddButton(menuCmdBarOnLeft,BarPasteBitmap,menuCmdBarResultMenuItem,EditMenuPaste,NULL);
    }
  if(get_min_selected_position().line!=SIGNED_NONE)
    {
#ifdef LOG_ENTRY
      log_entry("SiEditor::adding cut+copy");
#endif

      MenuCmdBarAddButton(menuCmdBarOnLeft,BarCopyBitmap,menuCmdBarResultMenuItem,EditMenuCopy,NULL);
      MenuCmdBarAddButton(menuCmdBarOnLeft,BarCutBitmap,menuCmdBarResultMenuItem,EditMenuCut,NULL);
    }


}

void SiEditor::handle_control(EventPtr event)
{
  switch(event->data.ctlSelect.controlID)
    {
    case PasteButton:
      paste_clipboard();
      clear_control(PasteButton);
      break;
    case CopyButton:
      copy_selection_to_clipboard();
      clear_control(CopyButton);
      break;
    }
}
void SiEditor::clear_control(const UInt16 id)
{
  FormPtr frm=FrmGetFormPtr(ResformID_text);
  ControlType * button=(ControlType *)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,id));
  CtlSetValue(button,0);
}
void SiEditor::handle_tap(Coord x, Coord y,Int16 n_taps)
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::handle_tap");
#endif

  if(ignore_events)
    return;
  else if(m_text_view->get_scrollbar()->handle_tap(x,y))
    {
      return;
    }
  else if(ui_tap(x,y))
    {
      return;
    }
  else if (m_document->get_number_blocks()==0)
    {
      return;
    }
  else if(m_incremental_search)
    {
      stop_inc_search();
      return;
    }

  else if(y==0)
    {
      popup_menu();
      return;
    }
  else if (!valid_tap(x, y))
    {
      return;
    }

  m_text_view->erase_cursor();
  clear_selection();
  Boolean dragged=false;
  

  UInt16 ticksPause = SysTicksPerSecond()/10;
  Boolean isPenDown = true;
  Position temp_pos;

  //track the pen down event
  Position tap_pos=m_text_view->find_position(x,y);

  if(SiPreferencesDatabase::get_selection_mode()==PALM_STYLE)
    set_start_selected_position(m_text_handler->find_prev_word_boundary(tap_pos));
  else
    set_start_selected_position(tap_pos);

  Position prev_pos=tap_pos;


  Coord prev_x=x;
  Coord prev_y=y;

  while (isPenDown)
    {
      //save battery life by putting a delay
      //before checking the pen status again
      SysTaskDelay(ticksPause);
      EvtGetPen(&x, &y, &isPenDown);
		
      if(!((x==prev_x)&&(y==prev_y))&&isPenDown)
	{
	  //if the pen has moved and is still held down
	  //move something
	  //if at the top or bottom of the screen scroll
	  if (y > BOTTOM_Y)
	    {
	      if(m_document->get_number_blocks()>COMMAND_LINE)
		do_scroll(y-BOTTOM_Y,true);

	      y = BOTTOM_Y - 2;

	    }
	  else if (y < BORDER+TOP_Y)
	    {
	      if(m_document->get_number_blocks()>COMMAND_LINE)
		do_scroll(-2,true);

	      y = BORDER + 1+TOP_Y;

	    }
			
	  //otherwise find where the tap is
	  temp_pos=tap_pos=m_text_view->find_position(x,y);
	  prev_x=x;
	  prev_y=y;
	  if(!(tap_pos==prev_pos))
	    {
	      //and if it's different from the last loop
	      //update the selection
	      if(SiPreferencesDatabase::get_selection_mode()==PALM_STYLE)
		{
		  if(tap_pos<prev_pos)
		    tap_pos=m_text_handler->find_prev_word_boundary(tap_pos);
		  else
		    tap_pos=m_text_handler->find_next_word_boundary(tap_pos);

		}
	      dragged=true;
	      prev_pos=temp_pos;

	      //and set the selection correctly
	      //this redraws the screen correctly as well
	      set_end_selected_position(tap_pos);
	    }
	}
    }

  if(!dragged)
    {

      m_document->check_newline_position(tap_pos,-1);
      clear_selection();
      if(n_taps>=2)
	{
	  select_word(x,y);
	 
	}
      else
	update_cursor_pos(tap_pos,true);
		
    }
  else
    {
      notify_selection_changed();
    }
}

Boolean SiEditor::find_term(Char *term)
{
  //find the string passed in in the current document
  //starting at the current cursor pos
  //and select it
  //Char *dest = term;
	
  if(first_find&&find_option_from_top)
    {
      first_find=false;
      cursor_pos=Position(0,0);
    }
  clear_selection();
  block * b;
  Int16 len = StrLen(term);
  Int16 term_bytes=SiUtility::calc_byte_length(term,len);
  Int16 matched_byte_offset=0;
  Int16 start=cursor_pos.x;
  Position start_pos;
  WChar data_char,term_char;
  BlockInt i;
  Int16 matches,first_char_matched_size=0;
  Int16 b_b_index;
  BlockInt  curr_char=start;
  //  BlockInt mem_chunk_size;
  Int16 t_size,d_size;
  // Boolean found;

  for(Int16 line=cursor_pos.line;line<m_document->get_number_blocks();++line)
    {
      b=m_document->get_line(line,b_b_index);

      /*if(b->mem_chunk==NULL)
	{
	  mem_chunk_size=0;
	  i=0;
	}
      else
	{
	  mem_chunk_size=b->mem_chunk->size;
	  
	  }*/
      if(b->mem_chunk!=NULL)
	{
	  m_document->get_char_bound(line,b->mem_chunk->mem_ptr,start,&i);
	  while(i<b->mem_chunk->size)
	    {
	      
	      d_size=TxtGetNextChar(b->mem_chunk->mem_ptr,i,&data_char);
	      t_size=TxtGetNextChar(term,matched_byte_offset,&term_char);
	      TEST_CHARS(data_char,term_char);
	     
	      m_has_found_term=false;
	      if(matches==0)
		{
		  if(matched_byte_offset==0)
		    {
		      start_pos.line=line;
		      start_pos.x=curr_char;
		      first_char_matched_size=t_size;
		    }
		  matched_byte_offset+=t_size;
		  
		  if(matched_byte_offset==term_bytes)
		    {
		      if(find_option_whole_word)
			{
			  Position end_pos;
			  end_pos.line=line;
			  end_pos.x=curr_char;
			  if(!(m_has_found_term=check_whole_word_match(start_pos,end_pos)))
			    {
			      matches=1;
			    }
			  
			}
		      else
			m_has_found_term=true;
		    }

		  if(m_has_found_term)
		    {
		      //found the word so select it and move cursor to end
		      //of word for next search
		      Position end_pos;
		      end_pos.line=line;
		      end_pos.x=curr_char+1;
		      make_visible_selection(start_pos,end_pos);
		      //		  m_has_found_term=true;
		      //unlock line line before returning
		      m_document->release_line(b_b_index);
		      return true;
		    }

		  //move to next character
		  i+=d_size;
		  ++curr_char;
		}

	      //meant to not be an else
	      if(matches!=0)
		{
		  if(first_char_matched_size!=0)
		    {
		      i-=(matched_byte_offset-first_char_matched_size);
		      first_char_matched_size=0;
		      curr_char=start_pos.x+1;
		    }
		  else
		    {
		      ++curr_char;
		      i+=d_size;
		    }
		  
		  matched_byte_offset=0;
		}
	      
	    }
	  i=curr_char=start=0;
	  
	  //unlock line line
	  
	  m_document->release_line(b_b_index);
	}
    }

  return false;
}
//! Make sure that the match is correct for a whole word
//! i.e. previous and next characters are whitespace or punctuation
Boolean SiEditor::check_whole_word_match(Position start_pos,Position end_pos)
{

  BlockInt start=start_pos.x;
  m_document->move_position(start_pos,-1,false);
  WChar the_char;
  
  the_char=m_text_handler->get_charAtpos(start_pos);


  if(SiUtility::is_break_char(the_char,true)||start==0)
    {

      m_document->move_position(end_pos,1,false);
      //return true is end_pos is the end of the line
      Int16 b_b_index;
      block * b=m_document->get_line(end_pos.line,b_b_index);
      BlockInt n_chars=b->n_chars;
      m_document->release_line(b_b_index);

      if(n_chars==end_pos.x)
	return true;


      the_char=m_text_handler->get_charAtpos(end_pos);
      
      if(SiUtility::is_break_char(the_char,true))
	{
	  return true;
	}
    }
  return false;
}

//!
//! Scroll the display so that the given positions are visible
//!
void SiEditor::make_visible_selection(Position start_pos, Position end_pos)
{

  set_start_selected_position(start_pos);
  set_end_selected_position(end_pos);
  //update_cursor_pos(end_pos,true);
  m_text_view->ensure_visible(start_pos.line);

}
void SiEditor::make_cursor_visible()
{
  m_text_view->ensure_visible(cursor_pos.line);
  update_cursor_pos(cursor_pos,false);
}


void SiEditor::toggle_find_whole_word()
{
#ifdef LOG_ENTRY
  log_entry("sieditor::toggle_find_whole_word");
#endif

  find_option_whole_word=!find_option_whole_word;
}
void SiEditor::toggle_find_match_case()
{
#ifdef LOG_ENTRY
  log_entry("sieditor::toggle_find_match_case");
#endif

  find_option_match_case=!find_option_match_case;
}
void SiEditor::toggle_find_from_top()
{
  find_option_from_top=!find_option_from_top;
}

void SiEditor::show_replace_controls()
{

 
  FormPtr frm=FrmGetActiveForm();
  RectangleType rp;
  rp.topLeft.x=0;
  rp.topLeft.y=REPLACE_FORM_TOP+1;
  rp.extent.x=SiUtility::SCREEN_WIDTH;
  rp.extent.y=SiUtility::CurrentScreenHeight-REPLACE_FORM_TOP;
  WinEraseRectangle(&rp,0);

  FrmHideObject(frm,FrmGetObjectIndex(frm,PopupTriggerButton));  
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceField));
 
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceWithField));
 
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceMatchCaseCheck));
 
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceWholeWordCheck));
 
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceFromTopCheck));

  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceFindLabel));

  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceWithLabel));
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceReplace));
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceReplaceAll));
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceFindNext));
  FrmShowObject(frm,FrmGetObjectIndex(frm,ReplaceCancel));
  initialise_replace_fields();

}

void SiEditor::hide_replace_controls()
{

  FormPtr frm=FrmGetActiveForm();

  FrmShowObject(frm,FrmGetObjectIndex(frm,PopupTriggerButton));

  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceField));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceWithField));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceMatchCaseCheck));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceWholeWordCheck));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceFromTopCheck));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceFindLabel));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceWithLabel));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceReplace));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceReplaceAll));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceFindNext));
  FrmHideObject(frm,FrmGetObjectIndex(frm,ReplaceCancel));
}

void SiEditor::resize_for_dialog(const Int16 height)
{
  #ifdef DEBUG_LOG
  log_entry("resize for dialog");
  #endif
  #ifdef DEBUG
  ErrFatalDisplayIf(edit_interface==NULL,"NULL edit_interface on resize");
  #endif
 m_displaying_dialog=true;
 edit_interface->make_exclusive(this);
  m_initial_height=HEIGHT;
  m_initial_top=TOP_Y;
 
  resize(height,0);
}

Boolean SiEditor::is_displaying_dialog() const
{
  return m_displaying_dialog;
}

void SiEditor::return_to_original_size()
{
  m_displaying_dialog=false;
  edit_interface->release_exclusive();
  
  resize(m_initial_height,m_initial_top);

}

void SiEditor::do_replace_dialog()
{
  first_find=true;
  //  set_ignore_events();
  g_editor = this;
  resize_for_dialog(REPLACE_FORM_TOP);
 
  FormPtr frm = FrmGetActiveForm();// FrmInitForm(ResReplaceDialog);
  FrmSetControlValue(frm, FrmGetObjectIndex(frm, ReplaceMatchCaseCheck),
		     find_option_match_case);
  FrmSetControlValue(frm, FrmGetObjectIndex(frm, ReplaceWholeWordCheck),
		     find_option_whole_word);
  FrmSetControlValue(frm, FrmGetObjectIndex(frm, ReplaceFromTopCheck),
		     find_option_from_top);
  FrmSetEventHandler(frm, (FormEventHandlerPtr) replaceEventLoop);
	
  //  FieldPtr find_text_field = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceField));
  //FieldPtr replace_text_field = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceWithField));
	 

  FrmSetFocus(frm, FrmGetObjectIndex(frm, ReplaceField));
  //  FrmDoDialog(frm);
  show_replace_controls();
  clear_selection(); 
}

void SiEditor::finished_replace()
{
  FormPtr frm=FrmGetActiveForm();
  FrmSetEventHandler(frm, (FormEventHandlerPtr) texteditloop);  
  //  FieldPtr find_text_field = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceField));
  //  FieldPtr replace_text_field = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceWithField));

  //save the current search term for next time
  //options are saved by replace form event loop
  //  copy_field_term(find_text_field,prev_search_term);
  //  copy_field_term(replace_text_field,prev_replace_term);

  hide_replace_controls();
  return_to_original_size();
  
  m_text_view->redraw_display(true);
  edit_interface->emit_redraw_signal(this);
	
  update_cursor_pos(cursor_pos,false);

  clear_ignore_events();
  m_has_found_term=false;
}
Char* SiEditor::get_prev_search_term()
{
  FormPtr frm=FrmGetActiveForm();//FrmGetFormPtr(ResReplaceDialog);
  FieldPtr find_text_field = (FieldPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, ReplaceField));
  return FldGetTextPtr(find_text_field);
}



void SiEditor::revert_curr_file()
{
  if(FrmAlert(RevertFileAlert)==RevertFileYes)
    {
      SiFile * curr_file=m_file_handler->get_file();
      if(curr_file==NULL)
	{
	  m_file_handler->clear_curr_file();
	  initialise();
	}
      else
	{

	  Char * location=(Char *)MemPtrNew(StrLen(curr_file->get_location())+1);

	  StrCopy(location,curr_file->get_location());

	  Int16 type=curr_file->get_type();
	  Int16 vol_ref=curr_file->get_vol_ref();
	  m_file_handler->clear_curr_file();

	  initialise();

	  m_file_handler->read_file(location,type,vol_ref);
	  m_document->clear_dirty_flag();
	  redraw();
	  MemPtrFree(location);

	}

    }
}
void SiEditor::do_new_file()
{
  set_ignore_events();
  if (FrmAlert(NewFileAlert) == NewFileYes)
    {
      if(m_document->file_dirty())
	m_file_handler->save_file();
      close_file();
    }
  clear_ignore_events();
}
void SiEditor::close_file(Boolean ask)
{
 
  if(ask)
    {
      UInt16 alert;
      if(m_document->file_dirty())
	{
	  alert=CloseFileAlert;
	}
      else
	alert=CloseFileSavedAlert;
      
      if(FrmAlert(alert)!=ExitNoSave)
	return;
    }

  m_file_handler->clear_curr_file();
  initialise();
  m_document->clear_dirty_flag();
  m_text_view->redraw_chrome();
  update_cursor_pos(cursor_pos,false);
}
void SiEditor::redo()
{
   m_text_view->erase_cursor();
   clear_selection();
   
   if(m_text_handler->redo(cursor_pos))
     {
       m_document->make_valid_pos(cursor_pos);
       check_scroll_position();
       
       m_document->check_newline_position(cursor_pos,0);
     }
   update_cursor_pos(cursor_pos,false);
}
void SiEditor::undo()
{
  m_text_view->erase_cursor();
  clear_selection();
  
  if(m_text_handler->undo(cursor_pos))
    {
      m_document->make_valid_pos(cursor_pos);
      check_scroll_position();
      
      m_document->check_newline_position(cursor_pos,0);
    }
  
  update_cursor_pos(cursor_pos,false);
}
void SiEditor::check_gsi_status()
{
 if(TOP_Y+HEIGHT==SiUtility::CurrentScreenHeight)
   {
     //this editor is responsible for the GSI
    GsiEnable(!m_read_only);
   }
  
}
void SiEditor::toggle_read_only_mode()
{
  if(m_document->has_had_memory_error()||m_file_handler->file_is_read_only())
    m_read_only=true;
  else
    m_read_only=!m_read_only;

  //replace the shift indicator with the Read only indicator
  check_gsi_status();
   
  m_text_view->draw_status_read_only();
}

Boolean SiEditor::get_read_only()
{
  return m_read_only;
}

void SiEditor::handle_menu_command(const UInt16 id)
{
  MenuEraseStatus(NULL);
  	
  switch (id)
    {
    case ViewMenuReadOnly:
      toggle_read_only_mode();
      break;
      #ifdef COLLECT_STATISTICS
    case ViewMenuCacheStats:
      SiUtility::display_stats();
      break;
    case 2999:
      detailed_benchmarks();
      break;
      #endif
    case EditMenuCopy:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Copy");
#endif
      copy_selection_to_clipboard();
      break;
    case EditMenuPaste:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Paste");
#endif
      paste_clipboard();
      break;
    case EditMenuCut:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Cut");
#endif
      if(copy_selection_to_clipboard())
	delete_selection();
      break;
    case EditMenuUndo:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Undo");
#endif
      undo();
     
      break;
    case EditMenuRedo:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command redo");
#endif
      redo();
     
      break;
#ifdef V_KEYBOARD
    case EditMenuKeyboard:
      
      display_keyboard(kbdAlpha);
	
      break;
#endif
    case FileMenuSaveAs:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command save as");
#endif
      do_save_file_as();
      break;
    case FileMenuManage:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Manage");
#endif
      do_file_manage();

      break;
    case FileMenuRevert:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Revert");
#endif
      revert_curr_file();
      break;
    case FileMenuStats:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Stats");
#endif
      display_stats_dialog();
      break;
    case FileMenuPrefs:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Prefs");
#endif
      SiPreferencesDatabase::display_prefs_dialog();
      break;
    case FileMenuClose:
      close_file(true);
      break;
    case FileMenuSave:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Save");
#endif
      if (m_document->file_dirty())
	{
	  save_current_file();
	}
      break;
    case FileMenuList:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Open");
#endif
      do_open_file();
      break;
    case FileMenuAbout:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command About");
#endif
      set_ignore_events();
      FrmAlert(AboutAlert);
      
      clear_ignore_events();
      break;
    case FileMenuNew:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command New");
#endif
      do_new_file();
      break;
    case CursorMenuHome:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Home");
#endif
      cursor_pos.x=0;
      move_cursor_to(cursor_pos);
      break;
    case CursorMenuEnd:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command End");
#endif
      move_cursor_to(m_document->find_end_line(cursor_pos.line));
      break;
    case CursorMenuEndDoc:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command End Doc");
#endif
      move_cursor_to(m_document->find_end_document());
      break;
    case CursorMenuStartDoc:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Start Doc");
#endif
      cursor_pos.line = 0;
      cursor_pos.x=0;
		
      move_cursor_to(cursor_pos);
      break;
    case CursorMenuDelLine:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Delete Line");
#endif
      delete_line(cursor_pos.line);
      break;
    case CursorMenuSelectAll:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Select all");
#endif
      set_start_selected_position(Position(0,0));
      set_end_selected_position(m_document->find_end_document());
      break;
    case SearchMenuFindAgain:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Find Again");
#endif
      if (get_prev_search_term() != NULL)
	{
	  m_text_view->erase_cursor();
	  if(!find_term(get_prev_search_term()))
	    FrmAlert(UnableToFind);
	}
		
      break;
    case SearchMenuReplace:
#ifdef LOG_ENTRY
      log_entry("SiEditor::handle menu command Replace");
#endif
      do_replace_dialog();
      break;
    case EditMenuIncSearch:
      do_incremental_search();
      break;
#ifdef REPORT_MEM_USAGE
    case MemUsageMenu:
      show_mem_usage();
      break;
#endif
    default:
      break;
    }
}
void SiEditor::do_open_file()
{
  set_ignore_events();
  Char *location=m_file_handler->get_location();
  UInt16 vol=m_file_handler->get_vol_ref();
  if(!m_file_handler->do_open_file(location,vol))
    m_text_view->redraw_display(true);
  else
    {
      if(m_file_handler->file_is_read_only())
	toggle_read_only_mode();
    }
  set_open_file();
  update_cursor_pos(cursor_pos,false);
  edit_interface->emit_redraw_signal(this);
  clear_ignore_events();

}
void SiEditor::display_keyboard(const KeyboardType keyb)
{
  if(!ignore_events&&m_active)
    {
      make_cursor_visible();
      m_keyboard->display_virtual_keyboard(keyb);
    }
      
}

void SiEditor::do_file_manage()
{
  Char *location=m_file_handler->get_location();
  UInt16 vol=m_file_handler->get_vol_ref();
  if(!m_file_handler->do_file_manage(location,vol))
    m_text_view->redraw_display(true);
  edit_interface->emit_redraw_signal(this);
}
void SiEditor::do_save_file_as()
{
  Char *location=m_file_handler->get_location();
  UInt16 vol=m_file_handler->get_vol_ref();
  if(m_document->get_doc_length()>0)
    {
      set_ignore_events();

      if(m_file_handler->save_file_as(location,vol))
	{
	  m_document->clear_dirty_flag();
	}
      set_open_file();
      m_text_view->redraw_display(true);
      
      edit_interface->emit_redraw_signal(this);
      clear_ignore_events();

    }
}

void SiEditor::do_incremental_search()
{
  if(!m_incremental_search)
    {
      m_incremental_search=true;
	
      m_mini_buffer=new SiMiniBuffer(0,HEIGHT+TOP_Y-1,INC_SEARCH_MAX_WIDTH);
	
      m_mini_buffer->set_title(INC_SEARCH_TITLE_TEXT);
    }
	
}

void SiEditor::stop_inc_search()
{
  m_incremental_search=false;
  if(m_mini_buffer!=NULL)
    {
      delete m_mini_buffer;
      m_mini_buffer=NULL;
    }
	
}

Boolean SiEditor::handle_inc_search_key(WChar the_char)
{
  if(!m_incremental_search)
    return false;
	
  //	Int16 initial_size=0;
		

  Position prev_start=start_selected_position;
  Position prev_end=end_selected_position;
  switch(the_char)
    {
    case chrLineFeed:
      stop_inc_search();
      return true;
      break;
    case chrHorizontalTabulation:
      //move to the next search string
      if(start_selected_position.line!=SIGNED_NONE)
	{
	  m_text_view->erase_cursor();
	  set_start_selected_position(end_selected_position);
	  end_selected_position=start_selected_position;
				
	}
      else
	return true;
      break;
    default:
      m_mini_buffer->give_key(the_char);
      break;
      break;
    }
	
  //actually do the searching
  //and selecting based on the current string
  m_text_view->erase_cursor();
  Char * term=m_mini_buffer->get_text();
  if(start_selected_position.line!=SIGNED_NONE)
    {
      cursor_pos=start_selected_position;
      clear_selection();
    }
  if(term!=NULL)
    {
      if(!find_term(term))
	{
	  if(prev_start.line!=SIGNED_NONE)
	    {
	      make_visible_selection(prev_start,prev_end);
	    }
	}
      m_mini_buffer->release_text();
    }
  return true;
}

void SiEditor::check_scroll_position()
{
  Int16 scroll_to_line=m_document->get_number_blocks()-COMMAND_LINE;
  if(scroll_to_line<0)
    scroll_to_line=0;
  m_text_view->make_top_line(scroll_to_line,false);
}

void SiEditor::move_cursor_to(Position pos)
{
  m_text_view->erase_cursor();
  clear_selection();
  cursor_pos=pos;
  m_document->check_newline_position(cursor_pos,-1);
  update_cursor_pos(cursor_pos,false);
}
void SiEditor::delete_line(const Int16 line)
{
  //delete the line passed in by selecting it and
  //deleting the selection. This way the flash of the selection
  //shows the user what is happening

  Position start,end;
  start.x=0;
  start.line=line;
  end=m_document->find_end_line(line);
  //only delete line if there's a line to delete
  //ie not zero length line
	
  if(!(start==end))
    {
		
      set_start_selected_position(start);
		
      set_end_selected_position(end,false);
		
		
      delete_selection();
		
      //cursor_pos=start;
      //m_document->make_valid_pos(cursor_pos);
      //update_cursor_pos(cursor_pos,true);
		
    }
}
Boolean SiEditor::save_doc_file(Char * name)
{
  return m_file_handler->save_doc_file(name);
}
Boolean SiEditor::open_doc_file(Char * name)
{
  return m_file_handler->open_doc_file(name);
}

void SiEditor::save_current_file()
{
  if(m_file_handler->save_file())
    {
      set_open_file();
      //m_text_view->redraw_display();
    }
  else
    {
      set_ignore_events();
      if(m_file_handler->save_file_as(NULL,NON_VFS))
	{
	  set_open_file();
	  m_document->clear_dirty_flag();
	  edit_interface->emit_redraw_signal(NULL);
	}
      clear_ignore_events();
    }
}

fontID SiEditor::get_font() const
{
  return m_font_id;
}

void SiEditor::draw_stats_data()
{
  FormPtr form=FrmGetFormPtr(ResStatsDialog);
  FrmSetActiveForm(form);
  FontID curr_font=FntSetFont(stdFont);

  //Character count
  Char buff[50];
  StrPrintF(buff,"%lu",m_document->get_number_chars());
  WinDrawTruncChars(buff,StrLen(buff),WORD_COUNT_LEFT,13,(158-WORD_COUNT_LEFT));

  //Word Count - estimated
  //five characters + 1 space per word on average
  StrPrintF(buff,"%lu ESTIMATE_TEXT",m_document->get_number_chars()/6);
  WinDrawTruncChars(buff,StrLen(buff),WORD_COUNT_LEFT,WORD_COUNT_TOP,(158-WORD_COUNT_LEFT));

  WinDrawTruncChars(CHARS_MESSAGE,StrLen(CHARS_MESSAGE),WORD_COUNT_LEFT,CHAR_COUNT_TOP,(158-WORD_COUNT_LEFT));
  FntSetFont(curr_font);
}



void SiEditor::update_word_count()
{
  Char buff[50];
  FontID curr_font=FntSetFont(stdFont);

  //erase the existing text
  StrPrintF(buff,"%lu (estimate)",m_document->get_doc_length()/6);
  WinEraseChars(buff,StrLen(buff),WORD_COUNT_LEFT,WORD_COUNT_TOP);
  WinEraseChars(CHARS_MESSAGE,StrLen(CHARS_MESSAGE),WORD_COUNT_LEFT,CHAR_COUNT_TOP);
  WinDrawTruncChars(WAITING_TEXT,StrLen(WAITING_TEXT),WORD_COUNT_LEFT,WORD_COUNT_TOP,(158-WORD_COUNT_LEFT));
  WinDrawTruncChars(WAITING_TEXT,StrLen(WAITING_TEXT),WORD_COUNT_LEFT,CHAR_COUNT_TOP,(158-WORD_COUNT_LEFT));
  //do the word count
  //clearing the text then doing the count shows
  //the user something has happened when they click the calc button
  UInt32 n_alphanum_chars;
  UInt32 n_words=m_text_handler->get_number_words(n_alphanum_chars);
  WinEraseChars(WAITING_TEXT,StrLen(WAITING_TEXT),WORD_COUNT_LEFT,WORD_COUNT_TOP);
  WinEraseChars(WAITING_TEXT,StrLen(WAITING_TEXT),WORD_COUNT_LEFT,CHAR_COUNT_TOP);
  //draw the actual word count
  StrPrintF(buff,"%lu",n_words);
  WinDrawChars(buff,StrLen(buff),WORD_COUNT_LEFT,WORD_COUNT_TOP);
  StrPrintF(buff,"%lu",n_alphanum_chars);
  WinDrawChars(buff,StrLen(buff),WORD_COUNT_LEFT,CHAR_COUNT_TOP);

  FntSetFont(curr_font);
}

void SiEditor::display_stats_dialog()
{
  FormPtr form=FrmInitForm(ResStatsDialog);
  FrmSetEventHandler(form, (FormEventHandlerPtr) stats_dialog_loop);
  g_editor=this;
  FrmDoDialog(form);
  FrmDeleteForm(form);
}


void SiEditor::handle_cursor_movement(const UInt16 direction,const Int16 amount, const UInt16 mod)
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::handle_cursor_movement");
#endif

  if ((mod & shiftKeyMask)==0)
    clear_selection();

  Position start_pos=cursor_pos;
  

  switch (direction)
    {
    case HORIZONTAL:
	m_document->move_position(cursor_pos,amount,true,true);
      break;
    case VERTICAL:
      cursor_pos=m_text_view->find_vertical_cursor_position(cursor_pos.line+amount);
      m_document->check_newline_position(cursor_pos,0);
      break;
    }

  if(mod & controlKeyMask)
    {
      
      
      if(amount>0)
	cursor_pos=m_text_handler->find_next_word_boundary(cursor_pos);
      else
	cursor_pos=m_text_handler->find_prev_word_boundary(cursor_pos);
      m_document->check_newline_position(cursor_pos,amount);

      
    }
  if (mod&shiftKeyMask)
    {
      if (start_selected_position.line == SIGNED_NONE)
	{
	  set_start_selected_position(start_pos);
	}
      set_end_selected_position(cursor_pos);
    }
  else
    {
      update_cursor_pos(cursor_pos,true);
    }

}
void SiEditor::scroll_page(const Int16 dir,const UInt16 modifiers)
{
  Int16 c_offset=cursor_pos.line-m_text_view->get_top_line();
  Int16 orig_top_line=m_text_view->get_top_line();
  m_text_view->erase_cursor();
  m_text_view->disable_screen_updates();
  handle_cursor_movement(VERTICAL,dir*(COMMAND_LINE  ), modifiers);

  m_text_view->make_top_line(cursor_pos.line-c_offset,true);
  m_text_view->enable_screen_updates();	
  if(orig_top_line!=m_text_view->get_top_line())
    m_text_view->redraw_display();
  update_cursor_pos(cursor_pos,false);
}

Boolean SiEditor::handle_key(const WChar keycode, const UInt16 modifiers)
{

  if(modifiers==0)
    if(handle_inc_search_key(keycode))
      return true;

  Boolean handled=false;
  switch (keycode)
    {

    case keycodeHome:
      cursor_pos.x=0;
      move_cursor_to(cursor_pos);
      handled=true;
      break;
    case keycodeEnd:
      move_cursor_to(m_document->find_end_line(cursor_pos.line));
      handled=true;
      break;
#ifdef V_KEYBOARD
    case vchrKeyboardAlpha:
      display_keyboard(kbdAlpha);
      handled=true;
      break;
    case vchrKeyboardNumeric:
      display_keyboard(kbdNumbersAndPunc);
      handled=true;
      break;
#endif
    case chrBackspace:
      if (!delete_selection())
	delete_backward_cursor(modifiers);
      handled=true;
      break;
    case chrDelete:
      if (!delete_selection())
	delete_forward_cursor(modifiers);
      handled=true;
      break;
    case chrPageUp:
      scroll_page(-1,modifiers);
      handled=true;
      break;
    case chrPageDown:
      scroll_page(1,modifiers);
      handled=true;
      break;
    case chrLeftArrow:
#ifdef HANDSPRING
    case keyRockerLeft:
#endif

      handle_cursor_movement(HORIZONTAL, -1, modifiers);
      handled=true;
      break;
    case chrRightArrow:
#ifdef HANDSPRING
    case keyRockerRight:
#endif

      handle_cursor_movement(HORIZONTAL, 1, modifiers);
      handled=true;
      break;
    case chrUpArrow:
      handle_cursor_movement(VERTICAL, -1, modifiers);
      handled=true;
      break;
    case chrDownArrow:
      handle_cursor_movement(VERTICAL, 1, modifiers);
      handled=true;
      break;
#ifdef HANDSPRING

    case keyRockerUp:
      if (modifiers & optionKeyMask)
	{ // treat as page up
	  scroll_page(-1,modifiers);
	}
      else
	{ // treat as line up
	  handle_cursor_movement(VERTICAL, -1, modifiers);
	}
      break;
    case keyRockerDown:
      if (modifiers & optionKeyMask)
	{ // treat as page down
	  scroll_page(1,modifiers);
	}
      else
	{ // treat as line down
	  handle_cursor_movement(VERTICAL, 1, modifiers);
	}
      handled=true;
      break;
    case hsKeySymbol: // Handle Treo600 Alt key
      //code by Tim Brown
      if (start_selected_position.line == SIGNED_NONE && cursor_pos.x >0)
	{
	  // do nothing if there is selected text... or cursor at start of line

  char buff[4];
  WChar newchar;
  Int16 Altind;

	  buff[0]=(char)m_text_handler->get_charBeforepos(cursor_pos);
	  Altind = FrmGetObjectIndex(mfrm,AltCharField);
	  FrmShowObject(mfrm,Altind);
	  FldDelete(Altfield,0,2);
	  FldInsert(Altfield,buff,1);
	  FrmSetFocus(mfrm,Altind);
	  FldSetInsPtPosition(Altfield,2);
	  HsPostProcessPopupList (true, stdFont);
	  newchar=TxtGetChar(FldGetTextPtr(Altfield),0);
	  delete_backward_cursor(modifiers);
	  insert_ch_cursor(newchar);
	  FldFreeMemory(Altfield);
	  FrmHideObject(mfrm,Altind);
	  handled=true;
	}
      break;

#endif
    default:
      if(TxtCharIsValid(keycode))
	{
	  if (TxtCharIsPrint(keycode) || keycode == chrLineFeed
	      || keycode == chrHorizontalTabulation)
	    {

	      insert_ch_cursor(keycode);
	      handled=true;
	    }
	}
      break;
    }

  return handled;
}

void SiEditor::set_open_file()
{
  if(m_file_handler->get_file()!=NULL)
    {
      m_text_view->set_filename_dirty();
      m_text_view->notify_saved();

    }
  m_read_only=m_file_handler->file_is_read_only();
  //m_text_view->notify_document_changed();
  update_cursor_pos(cursor_pos,true);
}



void SiEditor::handle_scroll(const Int16 old_v, const Int16 new_v)
{
#ifdef LOG_ENTRY
  log_entry("siEditor::handle_scroll");
#endif
  do_scroll(new_v-old_v,true);

}

Position SiEditor::get_c_pos() const
{
  return cursor_pos;
}

void SiEditor::update_cursor_pos(const Position& p,const Boolean erase)
{
#ifdef LOG_ENTRY
  log_entry("SiEditor::update_cursor_pos");
#endif

  cursor_pos=p;

  ensure_tidied(cursor_pos.line);
  m_document->make_valid_pos(cursor_pos);
  m_text_view->notify_cursor_moved(cursor_pos,erase);
}

void SiEditor::notify_block_deleted(const Int16 line)
{
  m_text_view->notify_block_deleted(line);
}

Position SiEditor::get_min_selected_position() const
{
  //returns the start of the current selection that is nearest the start
  //of the document
  if(start_selected_position.line==SIGNED_NONE)
    return start_selected_position;
  else if(end_selected_position.line==SIGNED_NONE)
    return end_selected_position;

  if(start_selected_position<end_selected_position)
    return start_selected_position;
  else
    return end_selected_position;
}

Position SiEditor::get_max_selected_position() const
{
  //returns the end of the current selection that is furthest forwards in the
  //document
  if(start_selected_position.line==SIGNED_NONE)
    return start_selected_position;
  else if(end_selected_position.line==SIGNED_NONE)
    return end_selected_position;

  if(start_selected_position<end_selected_position)
    return end_selected_position;
  else
    return start_selected_position;
}

void SiEditor::redraw()
{
  FrmSetActiveForm(FrmGetFormPtr(ResformID_text));
  m_text_view->redraw_display();
  update_cursor_pos(cursor_pos,true);
}

void SiEditor::redraw_chrome(Boolean force)
{
  if(force)
    m_text_view->redraw_chrome();
  else
    m_text_view->draw_status_line_number();
}

Char * SiEditor::get_filename()
{
  if(m_file_handler==NULL)
    return NULL;
  SiFile * file=m_file_handler->get_file();
  if(file!=NULL)
    return file->get_name();
  else
    return NULL;
}

Boolean SiEditor::point_in_line_number(Coord x,Coord y)
{
  if(m_document->get_number_blocks()==0)
    return false;

  Int16 selection=m_text_view->point_in_line_number(x,y);
	
  if(selection>=(Int16)0)
    {
      Int16 target;
      Int16 orig_number_lines;
      do
	{
	  target=(m_document->get_apparent_number_blocks()/PERCENTAGE_LIST_LENGTH);
	  orig_number_lines=m_document->get_apparent_number_blocks();
	  ensure_tidied((target*selection));
	}while(m_document->get_apparent_number_blocks()>orig_number_lines);
		
      m_text_view->make_top_line((m_document->get_apparent_number_blocks()/PERCENTAGE_LIST_LENGTH)*(selection),true);
      return true;
    }
  return false;
}

#ifdef REPORT_MEM_USAGE
void SiEditor::show_mem_usage()
{

  SiMemHandler::mem_show_mem_usage();
}
#endif
#ifdef BENCH_OBJECTS
Boolean SiEditor::perform_benchmark()
{
	
  

}
#endif
#ifdef TEST_OBJECT_UNIT
#define CORRECT_LENGTH 10
#define TEST_DOCUMENT_LENGTH 700
#define TEST_REPLACE_TEXT "voodoo"
#define TEST_INSERT_TEXT "wxyzw."
#define TEST_REPLACE_ALL_TEXT "womble"

#define TEST_TEXT  "wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. wxyz. "

#define WRAP_WORD "abcdefg "
void SiEditor::test_word_wrap()
{
	block * b;
	Int16 b_b_index;
	initialise();
	UInt16 len=StrLen(WRAP_WORD);
	for(Int16 i=0;i<8;++i)
	  {
	    insert_chars(WRAP_WORD,len,len);
	  }

	//check_new_broken_word
	Position c(0,1);
	update_cursor_pos(c,true);
	handle_key(chrBackspace,0);
	ErrFatalDisplayIf(cursor_pos.line!=1&&cursor_pos.x!=7,"Incorrect cursor_pos on check_new_broken_word");
	b=m_document->get_line(0,b_b_index);
	ErrFatalDisplayIf(b->n_chars!=24,"Incorrect check_new_broken_word line length,0");
	m_document->release_line(b_b_index);
	b=m_document->get_line(1,b_b_index);
	ErrFatalDisplayIf(b->n_chars!=31,"Incorrect check_new_broken_word line length,1");
	m_document->release_line(b_b_index);
	for(Int16 i=0;i<5;++i)
	  {
	    handle_key(chrBackspace,0);
	  }
	ErrFatalDisplayIf(cursor_pos.line!=0||cursor_pos.x!=26,"incorrect cursor_pos, tidy_line_too_short");
	b=m_document->get_line(0,b_b_index);
	ErrFatalDisplayIf(b->n_chars!=34,"Incorrect tidy_line_too_short line length,0");
	m_document->release_line(b_b_index);
	b=m_document->get_line(1,b_b_index);
	ErrFatalDisplayIf(b->n_chars!=24,"Incorrect tidy_line_too_short line length,1");
	m_document->release_line(b_b_index);

	insert_chars(WRAP_WORD,len,len);
	ErrFatalDisplayIf(cursor_pos.line!=0||cursor_pos.x!=34,"incorrect cursor_pos, tidy_line_too_long");
	b=m_document->get_line(0,b_b_index);
	ErrFatalDisplayIf(b->n_chars!=34,"Incorrect tidy_line_too_long line length,0");
	m_document->release_line(b_b_index);
	b=m_document->get_line(1,b_b_index);
	ErrFatalDisplayIf(b->n_chars!=32,"Incorrect tidy_line_too_long line length,1");
	m_document->release_line(b_b_index);
	initialise();
	do_typing();
	cursor_pos.x=cursor_pos.line=0;
	update_cursor_pos(cursor_pos,false);
	Int16 n_blocks=m_document->get_number_blocks();
	do_typing();
	ErrFatalDisplayIf(n_blocks*2!=m_document->get_number_blocks(),"Incorrect tidy line too long, long");
	
}


void SiEditor::test_undo()
{
  initialise();
  Int16 b_b_index;
  block * b;
  Position start_u(0,0);
  Position end_u(0,0);
  UInt16 orig_length;
  for (Int16 i=0;i<60;++i)
    {
      //Twenty two actions
     
      for(Int16 j=0;j<5;++j)
	{
	  handle_key('W',0);
	  handle_key('o',0);
	  handle_key('r',0);
	  handle_key(' ',0);
	}
      handle_key('\n',0);
      
      end_u.line=start_u.line=m_document->get_number_blocks()-2;
      end_u.x=15;
      set_start_selected_position(start_u);
      set_end_selected_position(end_u);
      delete_selection();
      b=m_document->get_line(start_u.line,b_b_index);
      orig_length=b->n_chars;
      m_document->release_line(b_b_index);
      //then undo them
      for(Int16 k=0;k<22;++k)
	{
	  handle_menu_command(EditMenuUndo);
	}

     
      //and redo them
      for(Int16 k=0;k<22;++k)
	{
	  handle_menu_command(EditMenuRedo);
	}
      
      b=m_document->get_line(start_u.line,b_b_index);
      ErrFatalDisplayIf(b->n_chars!=orig_length,"Incorrect redo char count");
      m_document->release_line(b_b_index);
      for(Int16 k=0;k<10;++k)
	{
	  handle_menu_command(EditMenuUndo);
	}
    }
 close_file(); 
for (Int16 i=0;i<200;++i)
    {
      
      //Twenty two actions
     
      for(Int16 j=0;j<5;++j)
	{
	  handle_key('W',0);
	  handle_key('o',0);
	  handle_key('r',0);
	  handle_key(' ',0);
	}
      handle_key('\n',0);
      
      end_u.line=start_u.line=m_document->get_number_blocks()-2;
      end_u.x=15;
      set_start_selected_position(start_u);
      set_end_selected_position(end_u);
      delete_selection();
      
      
    }
 close_file();
}
void SiEditor::test_large_insert()
{
  block * b;
  Int16 b_b_index;
  initialise();
  BlockInt large_len=StrLen(LARGE_INSERT_TEXT);
  insert_chars(LARGE_INSERT_TEXT,large_len,large_len);
  ErrFatalDisplayIf(m_document->get_number_chars()!=large_len,"Incorrect large text");

  reopen();
  ErrFatalDisplayIf(m_document->get_number_chars()!=large_len,"Incorrect large text");
  BlockInt n_patterns=large_len/7;
  Position beg(0,0);
  move_cursor_to(beg);

  for(BlockInt i=0;i<n_patterns;++i)
    {
      ErrFatalDisplayIf(!find_term("abcdefg"),"Incorrect text on large text find");

    }

  reopen();


  for(BlockInt i=0;i<n_patterns;++i)
    {
      ErrFatalDisplayIf(!find_term("abcdefg"),"Incorrect text on large text find after reopen");
      
    }

}
void SiEditor::test_char_insert()
{
  initialise();
  Int16 b_b_index;
  block * b;
  //test character insert
  do_typing();
  ErrFatalDisplayIf(m_document->get_number_chars()!=TEST_DOCUMENT_LENGTH,"Incorrect character insertion");
  ErrFatalDisplayIf(m_document->get_number_blocks()!=25,"Incorrect number blocks");
  const char * correct_text="wxyzw. wxyzw. wxyzw. wxyzw. ";

  UInt16 len=StrLen(correct_text);
  for(Int16 i=0;i<m_document->get_number_blocks();++i)
    {
      b=m_document->get_line(i,b_b_index);

      ErrFatalDisplayIf(len!=b->mem_chunk->size,"Incorrect line length");
      for(UInt16 j=0;j<len;++j)
	{
	  ErrFatalDisplayIf(correct_text[j]!=b->mem_chunk->mem_ptr[j],"Incorrect text");
	}
      
      m_document->release_line(b_b_index);
			
    }
  //test character delete

  for(UInt16 i=0;i<len;++i)
    {
      handle_key(chrLeftArrow,0);
    }

  for(UInt16 i=0;i<len;++i)
    {
      handle_key(chrBackspace,0);
    }

  b=m_document->get_line(23,b_b_index);
  ErrFatalDisplayIf(b->n_chars!=28,"Incorrect word wrap");
  
  for(UInt16 i=0;i<28;++i)
    {
      ErrFatalDisplayIf(b->mem_chunk->mem_ptr[i]!=correct_text[i],"Incorrect char in word wrap");
    }

  ErrFatalDisplayIf(m_document->get_number_blocks()!=24,"Incorrect number blocks on deleting");

  m_document->release_line(b_b_index);
  UInt32 length=m_document->get_doc_length();
  //test word wrapping
  for(UInt16 i=0;i<14;++i)
    {
      handle_key(chrDelete,0);
    }
  b=m_document->get_line(23,b_b_index);
  ErrFatalDisplayIf(b->n_chars!=14,"Incorrect word wrap");
  
  for(UInt16 i=0;i<14;++i)
    {
      ErrFatalDisplayIf(b->mem_chunk->mem_ptr[i]!=correct_text[i],"Incorrect char in word wrap");
    }
}
void SiEditor::test_selection()
{
  Position start(0,0);
  Position end(0,0);
  initialise();
  insert_test_text();
  
  for(Int16 i=0;i<20;++i)
    {
      start.line=i;
      start.x=10;
      update_cursor_pos(start,false);
      select_current_word();
      start=get_min_selected_position();
      end=get_max_selected_position();
      ErrFatalDisplayIf(start.line!=i||end.line!=i||start.x!=6||end.x!=10,"Incorrect select word");
    }
  Int16 b_b_index;
  block * b;

  BlockInt curr_total=m_document->get_number_chars();
  Int16 orig_blocks=m_document->get_number_blocks();
  for(Int16 t=0;t<2;++t)
    {
      for (Int16 l=0;l<5;++l)
	{
	  start.line=l;
	  start.x=l*6;
	  end.line=l;
	  end.x=start.x+6;
	  delete_chars(start,end);
	  curr_total-=6;
	  ErrFatalDisplayIf(cursor_pos.x!=start.x&&cursor_pos.line!=start.line,"Incorrect cursor position on delete selection");
	  ErrFatalDisplayIf(m_document->get_number_chars()!=curr_total,"Incorrect char count on delete selection");
	}
    }
  ErrFatalDisplayIf(m_document->get_number_blocks()!=orig_blocks-2,"Incorrect block count on delete selection");
}


void SiEditor::test_reopen()
{
  initialise();
  insert_test_text();
  Int16 n_lines=m_document->get_number_blocks();
  for(UInt16 i=0;i<15;++i)
    {
      reopen();
      ErrFatalDisplayIf(m_document->get_number_blocks()!=n_lines,"Incorrect block count on reopen");
    }
}
void SiEditor::test_find()
{
  Position start,end;
  Int16 b_b_index;
  block * b;
  initialise();
  insert_test_text();
 //test find
  for(Int16 l=0;l<20;++l)
    {
      start.line=l;
      start.x=0;
      update_cursor_pos(start,false);
      for(UInt16 i=0;i<4;++i)
	{
	  ErrFatalDisplayIf(!find_term("wxyzw."),"Unable to Find term");
	  start=get_min_selected_position();
	  end=get_max_selected_position();
	  ErrFatalDisplayIf(start.line!=l||end.line!=l||start.x!=i*7||end.x!=start.x+6,"Incorrect search");
	  ErrFatalDisplayIf(!m_has_found_term,"Incorrectly set found flag");
	}
    }

  //test text replacement
  clear_selection();
  for(Int16 l=0;l<20;++l)
    {
      start.line=l;
      start.x=0;
      update_cursor_pos(start,false);
      ErrFatalDisplayIf(!replace_text(TEST_INSERT_TEXT,TEST_REPLACE_TEXT,true),"Unable to find text to replace");

      ErrFatalDisplayIf(!replace_text(TEST_INSERT_TEXT,TEST_REPLACE_TEXT,true),"Unable to Replace term");
      clear_selection();
      update_cursor_pos(start,false);
      ErrFatalDisplayIf(!find_term(TEST_REPLACE_TEXT),"Unable to find replaced text");
      start=get_min_selected_position();
      end=get_max_selected_position();
      ErrFatalDisplayIf(start.line!=l||end.line!=l||start.x!=0||end.x!=StrLen(TEST_REPLACE_TEXT),"Incorrect search");
      clear_selection();
    }

  for(UInt16 l=0;l<20;++l)
    {
      start.line=0;
      start.x=0;
      update_cursor_pos(start,false);
      replace_all(TEST_REPLACE_TEXT,TEST_REPLACE_ALL_TEXT);
      for(Int16 i=0;i<20;++i)
	{
	  start.line=i;
	  start.x=0;
	  update_cursor_pos(start,false);
	  ErrFatalDisplayIf(!find_term(TEST_REPLACE_ALL_TEXT),"Unable to find search term, replace_all");
	  start=get_min_selected_position();
	  end=get_max_selected_position();
	  ErrFatalDisplayIf(start.line!=i||start.x!=0||end.line!=i||end.x!=StrLen(TEST_REPLACE_ALL_TEXT),"Incorrect find term on replace_all");
	}
      start.line=0;
      start.x=0;
      update_cursor_pos(start,false);
      replace_all(TEST_REPLACE_ALL_TEXT,TEST_REPLACE_TEXT);
      for(Int16 i=0;i<20;++i)
	{
	  start.line=i;
	  start.x=0;
	  update_cursor_pos(start,false);
	  ErrFatalDisplayIf(!find_term(TEST_REPLACE_TEXT),"Unable to find search term, replace_all");
	  start=get_min_selected_position();
	  end=get_max_selected_position();
	  ErrFatalDisplayIf(start.line!=i||start.x!=0||end.line!=i||end.x!=StrLen(TEST_REPLACE_TEXT),"Incorrect find term on replace_all");
	}
    }

}
void SiEditor::test_file_save()
{
  initialise();
  for(Int16 i=0;i<5;++i)
    {
      insert_test_text();
    }
  UInt32 length=m_document->get_number_chars();
  save_doc_file("Test");

  for(Int16 i=0;i<5;++i)
    {
      initialise();
      open_doc_file("Test");
      ErrFatalDisplayIf(m_document->get_number_chars()!=length,"Incorrect file length on reopen doc initially");
      save_current_file();
    }

  for(Int16 i=0;i<5;++i)
    {
      initialise();
      open_doc_file("Test");
      update_cursor_pos(m_document->find_end_document(),false);
      ErrFatalDisplayIf(m_document->get_number_chars()!=length,"Incorrect file length on reopen doc while deleting text");
      for(Int16 j=0;j<100;++j)
	{
	  handle_key(chrBackspace,0);
	  --length;
	}
      save_current_file();
    }
  UInt32 test_length=StrLen(TEST_TEXT);
 for(Int16 i=0;i<5;++i)
    {
      initialise();
      open_doc_file("Test");
      update_cursor_pos(m_document->find_end_document(),false);
      handle_key(chrLineFeed,0);
      ++length;
      ErrFatalDisplayIf(m_document->get_number_chars()!=length,"Incorrect file length on reopen doc while adding text");
      for(Int16 j=0;j<5;++j)
	{
	  insert_test_text();
	  length+=test_length;
	}
      save_current_file();
    }
	
}

void SiEditor::insert_test_text()
{

  BlockInt len=StrLen(TEST_TEXT);
  insert_chars(TEST_TEXT,len,len);
}
void SiEditor::test_redraw()
{
  initialise();
  insert_test_text();
  //test redrawing
  for(UInt16 i=0;i<20;++i)
    redraw();

  //test changing fonts
  for(UInt16 i=0;i<20;++i)
    {
      Int16 n_blocks_orig=m_document->get_number_blocks();
      change_font(largeFont);
      ErrFatalDisplayIf(m_font_id!=largeFont,"Incorrect font database set");
      change_font(largeBoldFont);
      ErrFatalDisplayIf(m_font_id!=largeBoldFont,"Incorrect font database set");
      change_font(boldFont);
      ErrFatalDisplayIf(m_font_id!=boldFont,"Incorrect font database set");

      

      change_font(stdFont);
      ErrFatalDisplayIf(m_font_id!=stdFont,"Incorrect font database set");
      ErrFatalDisplayIf(m_document->get_number_blocks()!=n_blocks_orig,"Incorrect number of blocks after font change");

    }

  //test resizing
  for(Int16 i=get_min_height()+1;i<160;i+=5)
    {
      resize_for_dialog(i);
      ErrFatalDisplayIf(HEIGHT!=i,"Incorrect resize for dialog");
      return_to_original_size();
      ErrFatalDisplayIf(HEIGHT!=SiUtility::CurrentScreenHeight,"Incorrect return to size");
    }
  for(Int16 i=0;i<80;i+=5)
    {
      resize(80,(UInt16)i);
      //      ErrFatalDisplayIf(HEIGHT!=80,"Incorrect height on resize");
      ErrFatalDisplayIf(TOP_Y!=i,"Incorrect TOP_Y on resize");
    }
}
void SiEditor::test_controls()
{
 for(UInt16 i=0;i<20;++i)
    {
      show_replace_controls();
      hide_replace_controls();
    }
}
Boolean SiEditor::perform_tests()
{
  test_undo();
  test_word_wrap();
  m_text_view->perform_tests();
  test_file_save();


  test_large_insert();

  test_char_insert();

  test_selection();
  //test selection delete
  
  test_reopen();
  test_redraw();
  test_controls();

  initialise();
#ifdef DEBUG_LOG
  log_entry("SiEditor tests complete");
#endif
  m_document->perform_tests();
  m_text_handler->perform_tests();
  return true;

}

#endif


void SiEditor::do_typing()
{
  for(Int16 i=0;i<100;++i)
    {
      handle_key('w',0);
      handle_key('x',0);
      handle_key('y',0);
      handle_key('z',0);
      handle_key('w',0);
      handle_key('.',0);      
      handle_key(' ',0);
      
    }
}

#ifdef COLLECT_STATISTICS
void SiEditor::detailed_benchmarks()
{
  Char buff[512];
  initialise();
  Int16 start_time=TimGetTicks();
  do_typing();
  Int16 type_time=TimGetTicks()-start_time;
  start_time=TimGetTicks();
  handle_cursor_movement(VERTICAL,-30,0);

  do_typing();

  Int16 slosh_time=TimGetTicks()-start_time;
  start_time=TimGetTicks();
  Int16 i;
  for(i=0;i<28;++i)
    {
      handle_menu_command(EditMenuUndo);
      
    }
  
  
  for(i=0;i<28;++i)
    {
      handle_menu_command(EditMenuRedo);
      
    }
  
  Int16 undo_time=TimGetTicks()-start_time;
  start_time=TimGetTicks();

  for(i=0;i<5;++i)
    reopen();

  Int16 file_time=TimGetTicks()-start_time;
  start_time=TimGetTicks();

  StrPrintF(buff,"Typing time: %i , Sloshing time: %i , Undo Time: %i , File access time: %i",type_time,slosh_time,undo_time,file_time);
  DisplayError(DEBUG_MESSAGE,buff);

  initialise();


}
#endif
