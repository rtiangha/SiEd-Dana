//
// C++ Implementation: simacrorecorder
//
// Description:
//
//
// Author: Benjamin Roe <sied@benroe.com>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "simacrorecorder.h"
#include <PalmOS.h>
#include <PalmTypes.h>
#include "resources.h"
#include "error.h"
#include "form_pos.h"
#include "main.h"
#include "utility.h"
#include "text_resource.h"
#define SIED_MACRO_DATABASE "sied_macro_store"
#define MACRO_DATA_TYPE 'MACR'
#define MAX_MACRO_LENGTH 1250

#define QUICK_MACRO_CHAR "Q"
#define RECORD_MACRO_CHAR "R"
enum {SHORT_MODE,LONG_MODE};
enum {PLAY_MACRO_MODE,START_RECORD_MODE};

SiMacroRecorder *g_record=NULL;
Boolean macroEventLoop(EventPtr e)
{
	Boolean handled=false;

	if(e->eType==ctlSelectEvent)
	{
		switch(e->data.ctlSelect.controlID)
		{
		case DeleteMacroButton:
			g_record->delete_selected_macro();
			handled=true;
			break;
		case MacroOK:
			handled=!g_record->check_valid();
			break;
		default:
			break;
		}
	}
	return handled;

}

void drawMacro(Int16 itemNum, RectangleType * bounds, Char ** itemsText)
{
	g_record->draw_macro_name(itemNum,bounds);
}

SiMacroRecorder::SiMacroRecorder()
{
	recording=false;
	playing=false;
	macro_handle=NULL;
	macro_name=NULL;
	open_ref=NULL;
	g_record=this;
	quick_macro_ready=false;
	number_macros=0;
}

Boolean SiMacroRecorder::check_valid()
{
	FormPtr frm=FrmGetFormPtr(MacroForm);
	ListPtr list = (ListPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, MacroList));
	FieldPtr field;
	Int16 selection=LstGetSelection(list);
	Boolean valid=false;
	;
	switch (dialog_mode)
	{
	case PLAY_MACRO_MODE:
		if(selection!=noListSelection)
			valid=true;
		break;
	case START_RECORD_MODE:
		field=(FieldPtr)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,MacroField));
		if(FldGetTextPtr(field)!=NULL)
			valid=true;
		break;
	}
	return valid;
}

void SiMacroRecorder::delete_selected_macro()
{
#ifdef LOG_ENTRY
	log_entry("SiMacro::delete macro");
#endif

	FormPtr frm=FrmGetFormPtr(MacroForm);
	ListPtr list = (ListPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, MacroList));
	Int16 selection=LstGetSelection(list);

	if(selection!=noListSelection)
	{
		MemHandle rec=DmQueryRecord(open_ref,selection);
		Char * record=(Char*)MemHandleLock(rec);

		UInt16 ret=FrmCustomAlert(DeleteMacro,record,NULL,NULL);
		MemHandleUnlock(rec);
		if(ret==DeleteMacroOK)
		{
			DmRemoveRecord(open_ref,selection);
			--number_macros;
			initialise_list(frm);
			LstDrawList(list);
		}
	}
}

void SiMacroRecorder::draw_macro_name(const UInt16 num,RectangleType * pos)
{
#ifdef LOG_ENTRY
	log_entry("Drawing macro name");
#endif

	MemHandle rec=DmQueryRecord(open_ref,num);
	if(rec!=NULL)
	{
		UInt16 width=0;

		Char * record=(Char*)MemHandleLock(rec);
		if(list_draw_mode==LONG_MODE)
		{
			Char buff[10];
			StrPrintF(buff,"%u. ",num+1);
			UInt16 len=StrLen(buff);
			width=FntCharsWidth(buff,len);
			WinDrawTruncChars(buff,len,pos->topLeft.x,pos->topLeft.y,MACRO_LIST_WIDTH);
		}
#ifdef LOG_ENTRY
		log_entry("Drawing macro name 1");
#endif

		WinDrawTruncChars(record,StrLen(record),pos->topLeft.x+width,pos->topLeft.y,pos->extent.x-width);

		MemHandleUnlock(rec);
#ifdef LOG_ENTRY

		log_entry("Drawing macro name 2");
#endif

	}
	else if(list_draw_mode==SHORT_MODE)
	{
#ifdef LOG_ENTRY
		log_entry("Drawing macro header");
#endif
		//draw the "play macro" text if there are no saved macros yet
		switch (num)
		{
		case 0:
			WinDrawTruncChars(MACRO_POPUP_TEXT_1,StrLen(MACRO_POPUP_TEXT_1),pos->topLeft.x,pos->topLeft.y,pos->extent.x);
			break;
		case 1:
			WinDrawTruncChars(MACRO_POPUP_TEXT_2,StrLen(MACRO_POPUP_TEXT_2),pos->topLeft.x,pos->topLeft.y,pos->extent.x);
			break;
		case 2:
			WinDrawTruncChars(MACRO_POPUP_TEXT_3,StrLen(MACRO_POPUP_TEXT_3),pos->topLeft.x,pos->topLeft.y,pos->extent.x);
			break;
#ifdef MACRO_POPUP_FOUR_STRINGS

		case 3:
			WinDrawTruncChars(MACRO_POPUP_TEXT_4,StrLen(MACRO_POPUP_TEXT_4),pos->topLeft.x,pos->topLeft.y,pos->extent.x);
			break;
#endif


		}
	}
}

SiMacroRecorder::~SiMacroRecorder()
{
	close_macro_database();
}

Boolean SiMacroRecorder::give_event(EventPtr e)
{
	Boolean handled=false;


	switch(e->eType)
	{
	case menuEvent:
		switch(e->data.menu.itemID)
		{
		case MacroStop:
			handled=true;
			stop_record();
			break;
		case MacroRecord:
			handled=true;
			start_record();
			break;
		case MacroPlay:
			handled=true;
			start_playback();
			break;
		case MacroMenuQuick:
			prepare_quick_macro();
			handled=true;
			break;
		case FileMenuSave:
		case FileMenuSaveAs:
		case FileMenuManage:
		case FileMenuNew:
		case FileMenuList:
		case FileMenuAbout:
		case FileMenuStats:
		case FileMenuPrefs:
		case SearchMenuFind:
		case SearchMenuFindAgain:
		case SearchMenuReplace:
		case ViewMenuSplit:
		case FileMenuRevert:
		case ViewMenuUnsplit:
			//Ignore these menu items during
			//macro recording
			//TODO make macro recorder work properly with all events
			//not just simple editor ones
			if(recording||playing)
				handled=true;
			break;
		default:
			if(recording)
				write_event_to_macro(e);
			break;
		}
		break;
	case ctlSelectEvent:
		if(e->data.ctlSelect.controlID==PopupTriggerButton&&recording)
			handled=true;
		else if (recording)
			write_event_to_macro(e);
		break;
	case keyDownEvent:
		if(quick_macro_ready)
		{
			Char * str=(Char*)(&(e->data.keyDown.chr));
			do_quick_macro(str+(2-TxtCharSize(e->data.keyDown.chr)));
			handled=true;
		}
		else if(recording)
		{
			if(e->data.keyDown.chr==vchrKeyboardAlpha||
					e->data.keyDown.chr==vchrKeyboardNumeric)
			{
				handled=true;
			}
			else
				write_event_to_macro(e);
		}
		break;


	case penDownEvent:
		handled=handle_tap(e);
		break;
	case penUpEvent:
	case penMoveEvent:
		break;
	default:
		break;
	}
	return handled;
}

Boolean SiMacroRecorder::handle_tap(EventPtr e)
{
	if(recording)
	{
		if(e->screenX>=MACRO_RECT_X
				&&e->screenX<=MACRO_RECT_X+MACRO_RECT_WIDTH)
		{
			if(e->screenY>=MACRO_RECT_Y
					&&e->screenY<=MACRO_RECT_Y+MACRO_RECT_WIDTH)
			{
				stop_record();
			}
		}
		return true;
	}
	return false;

}
ListPtr SiMacroRecorder::initialise_list(FormPtr frm)
{
	ListPtr list = (ListPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, MacroList));
	LstSetDrawFunction(list, drawMacro);
	LstSetListChoices(list, NULL, number_macros);
	LstSetSelection(list, noListSelection);
	return list;
}

Char * SiMacroRecorder::display_macro_select(Int16 &index,Int16 mode)
{
#ifdef LOG_ENTRY
	log_entry("SiMacroRecorder::display_macro_select 1");
#endif

	FormPtr frm=FrmInitForm(MacroForm);
	list_draw_mode=LONG_MODE;
	dialog_mode=mode;
	ListPtr list=initialise_list(frm);
#ifdef LOG_ENTRY

	log_entry("SiMacroRecorder::display_macro_select 2 ");
#endif

	FrmSetEventHandler(frm, (FormEventHandlerPtr) macroEventLoop);
	ControlType * button=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,MacroOK));
#ifdef LOG_ENTRY

	log_entry("SiMacroRecorder::display_macro_select 3");
#endif

	switch(mode)
	{
	case PLAY_MACRO_MODE:
		FrmSetTitle(frm,PLAY_MACRO);
		CtlSetLabel(button,PLAY);
		FrmHideObject(frm,FrmGetObjectIndex(frm,MacroField));
		break;
	case START_RECORD_MODE:
		FrmSetFocus(frm,FrmGetObjectIndex(frm,MacroField));
		break;
	}
#ifdef LOG_ENTRY
	log_entry("SiMacroRecorder::display_macro_select 4");
#endif

	UInt16 ret=FrmDoDialog(frm);
	index=noListSelection;
	Char * ret_text=NULL;
	if(ret==MacroOK)
	{
		index=LstGetSelection(list);
		FieldPtr text_field=(FieldPtr)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,MacroField));
		Char *dest = FldGetTextPtr(text_field);
		if (dest != NULL)
		{
			ret_text=(Char*)MemPtrNew(StrLen(dest)+1);
			StrCopy(ret_text, dest);
		}
	}
	FrmDeleteForm(frm);
	return ret_text;
}

void SiMacroRecorder::start_playback()
{
	if(recording)
		return;
	Int16 index;
	open_macro_database();
	Char * ret=display_macro_select(index,PLAY_MACRO_MODE);
	if(ret!=NULL)
		MemPtrFree(ret);
	if(index!=noListSelection)
		play_macro(index);
	close_macro_database();

#ifdef LOG_ENTRY

	log_entry("Done playback");
#endif
}

Boolean SiMacroRecorder::start_record()
{
#ifdef LOG_ENTRY
	log_entry("SiMacroRecord start record");
#endif

	if(recording||playing)
		return false;
	Int16 index;
	open_macro_database();
#ifdef LOG_ENTRY

	log_entry("SiMacroRecord start record 2");
#endif

	Char * name=display_macro_select(index,START_RECORD_MODE);
	if(name==NULL)
	{
		close_macro_database();
	}
	else
	{
		recording=true;
		initialise_macro_record(name);
		draw_macro_active_rect();
		draw_macro_active_char(RECORD_MACRO_CHAR);
	}
	if(name!=NULL)
		MemPtrFree(name);

	return recording;
}

void SiMacroRecorder::stop_record()
{
	if(playing)
		return;
	recording=false;
	close_macro_database();
	erase_macro_active_rect();
}

Boolean SiMacroRecorder::open_macro_database()
{
#ifdef LOG_ENTRY
	log_entry("Opening Macro database");
#endif

	if(open_ref!=NULL||playing)
	{
#ifdef LOG_ENTRY
		log_entry("Opening Macro database already open");
#endif

		return true;
	}
	LocalID l_id=DmFindDatabase(THE_CARD,SIED_MACRO_DATABASE);
#ifdef LOG_ENTRY

	log_entry("Opening Macro database 1");
#endif

	if(l_id==0)
	{
#ifdef LOG_ENTRY
		log_entry("Not found - creating");
#endif
		//this is the first time,
		//so create the database
		DmCreateDatabase(THE_CARD,SIED_MACRO_DATABASE,AppID,MACRO_DATA_TYPE,false);
		l_id=DmFindDatabase(THE_CARD,SIED_MACRO_DATABASE);
	}

	open_ref=DmOpenDatabase(THE_CARD,l_id,dmModeReadWrite);

#ifdef DEBUG

	ErrFatalDisplayIf(open_ref==NULL,"NULL Macro database ref");
#endif

	number_macros=DmNumRecords(open_ref);
#ifdef LOG_ENTRY

	log_entry("Opening Macro database done");
#endif

	return true;
}

Boolean SiMacroRecorder::add_macro_record(Char * name)
{
#ifdef LOG_ENTRY
	log_entry("Adding macro record");
	log_entry(name);
#endif

	if(open_ref==NULL)
		return false;
	Int16 len=StrLen(name)+1;

	//make sure the events are stored on a 4-byte boundary
	Int16 even_addition=len-(len/4)*4;
	name_offset=len+even_addition;

	curr_macro_index=dmMaxRecordIndex;
	macro_handle=DmNewRecord(open_ref,&curr_macro_index,sizeof(EventType)+name_offset);

	Char * array=(Char*)MemHandleLock(macro_handle);

	DmWrite(array,0,name,name_offset-even_addition);

	if(even_addition>0)
	{
		DmSet(array,name_offset,even_addition,0);
	}
#ifdef LOG_ENTRY
	log_entry_number("even_addition",even_addition);
	log_entry_number("name_offset ",name_offset);
#endif

	MemHandleUnlock(macro_handle);
	return true;
}

void SiMacroRecorder::initialise_macro_record(Char * name)
{
	add_macro_record(name);
	number_events=0;
}

Boolean SiMacroRecorder::write_event_to_macro(EventPtr e)
{
	if(playing)
		return false;
	if(number_events>=MAX_MACRO_LENGTH)
	{
		stop_record();
		return false;
	}
#ifdef LOG_ENTRY
	log_entry("Writing event to macro");
	log_entry_number("Event ",number_events);
	log_entry_number("Writing bytes ",sizeof(EventType));
	log_entry_number("record size ",MemHandleSize(macro_handle));
#endif

	EventPtr macro_array=(EventPtr)MemHandleLock(macro_handle);

	DmWrite(macro_array,name_offset+sizeof(EventType)*number_events,e,sizeof(EventType));
	MemHandleUnlock(macro_handle);

	++number_events;

	macro_handle=DmResizeRecord(open_ref,curr_macro_index,sizeof(EventType)*(1+number_events)+name_offset);

#ifdef LOG_ENTRY

	ErrFatalDisplayIf(macro_handle==NULL,"Null macro handle");
	ErrFatalDisplayIf(MemHandleSize(macro_handle)!=sizeof(EventType)*(1+number_events)+name_offset,"Incorrect macro resize");
#endif

	return true;
}

void SiMacroRecorder::close_macro_database()
{
	if(macro_handle!=NULL)
	{
		DmReleaseRecord(open_ref,curr_macro_index,true);
		macro_handle=NULL;
	}
	if(open_ref!=NULL)
	{
		DmCloseDatabase(open_ref);
		open_ref=NULL;
	}

}
Boolean SiMacroRecorder::open_macro(Int16 index)
{
#ifdef LOG_ENTRY
	log_entry_number("opening macro index",index);
#endif

	if(open_ref==NULL)
		open_macro_database();
	macro_handle=DmQueryRecord(open_ref,index);
	if(macro_handle==NULL)
	{
		close_macro_database();
		return false;
	}
	else
	{
		return true;
	}
}
Boolean SiMacroRecorder::play_macro(Int16 index)
{
#ifdef LOG_ENTRY
	log_entry_number("Playing macro",index);
#endif

	if(recording)
		return false;
	if(!open_macro(index))
		return false;
	playing=true;
	Char* macro_char_array=(Char*)MemHandleLock(macro_handle);
	Int16 len=StrLen(macro_char_array)+1;

	//the events are stored on a 4-byte boundary
	Int16 even_addition=len-((len/4)*4);
	macro_char_array+=len+even_addition;
	EventPtr macro_array=(EventPtr)macro_char_array;
	Int16 l_number_events=(MemHandleSize(macro_handle)-len)/sizeof(EventType);
#ifdef LOG_ENTRY

	log_entry_number("Number of events ",l_number_events);
#endif

	Err err;
	EventPtr event;
	for(Int16 i=0;i<l_number_events-1;++i)
	{
#ifdef LOG_ENTRY
		log_entry_number("Playing event ",i);
#endif

		event=&(macro_array[i]);
		if (!SysHandleEvent(event))
			if (!MenuHandleEvent(NULL, event, &err))
				FrmDispatchEvent(event);

		//remove remaining events from the queue
		//to avoid an overflow of the event queue
		EventType e;
		while(EvtEventAvail())
		{
			EvtGetEvent(&e, 1);

		}
	}
	//	break;
	//EvtAddEventToQueue(&(macro_array[i]));
	MemHandleUnlock(macro_handle);
	macro_handle=NULL;
	//emit_redraw_signal(NULL);
	playing=false;
	return true;
}

void SiMacroRecorder::do_quick_macro(Char * the_char)
{
	//! If in the quick macro mode, take the passed char
	//! convert it to a number and play that macro if it exists
	//! counting 0 as 10
	if(playing||recording)
		return;
	erase_macro_active_rect();
	quick_macro_ready=false;
	WChar p_char=TxtGetChar(the_char,0);
	if(TxtCharIsDigit(p_char))
	{
		draw_macro_active_rect();
		draw_macro_active_char(the_char);


		UInt16 index=StrAToI(the_char);
		if(index==0)
			index=9;
		else
			--index;
		open_macro_database();
		if(index<number_macros)
			play_macro(index);
		close_macro_database();
		erase_macro_active_rect();
	}


}

void SiMacroRecorder::prepare_quick_macro()
{
	//! Set the recorder to take the next key press as a
	//! quick macro number

	if(recording)
		return;
	quick_macro_ready=true;
	draw_macro_active_rect();
	draw_macro_active_char(QUICK_MACRO_CHAR);
}

void SiMacroRecorder::draw_macro_active_rect()
{
	RectangleType rp;
	rp.topLeft.x=MACRO_RECT_X;
	rp.topLeft.y=MACRO_RECT_Y;
	rp.extent.x=MACRO_RECT_WIDTH;
	rp.extent.y=MACRO_RECT_HEIGHT;
	WinDrawRectangle(&rp,0);
}

void SiMacroRecorder::erase_macro_active_rect()
{
	RectangleType rp;
	rp.topLeft.x=MACRO_RECT_X;
	rp.topLeft.y=MACRO_RECT_Y;
	rp.extent.x=MACRO_RECT_WIDTH;
	rp.extent.y=MACRO_RECT_HEIGHT;
	WinEraseRectangle(&rp,0);
	FormPtr frm=FrmGetFormPtr(ResformID_text);
	ControlType* ctl=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,PopupTriggerButton));
	CtlDrawControl(ctl);
}

void SiMacroRecorder::draw_macro_active_char(Char * text)
{
	fontID font=FntSetFont(stdFont);
	Int16 x_offset=(MACRO_RECT_WIDTH-FntCharsWidth(text,StrLen(text)))/2;
	WinDrawInvertedChars(text,StrLen(text),MACRO_RECT_X+x_offset,MACRO_RECT_Y);
	FntSetFont(font);
}

void SiMacroRecorder::popup_macro_list()
{
#ifdef LOG_ENTRY
	log_entry("SiMacroRecorder::popup_macro_list()");
	log_entry_number("number_macros=",number_macros);
#endif

	if(recording)
		return;
	if(!open_macro_database())
		return;

	list_draw_mode=SHORT_MODE;
	FormPtr frm=FrmGetFormPtr(ResformID_text);

	ListType * lst=(ListType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,PopupMacroList));

	LstSetDrawFunction(lst, drawMacro);

	if(number_macros==0)
	{
		//some languages require four lines to fit the intro text in
		//some only require three
#ifdef MACRO_POPUP_FOUR_STRINGS
		LstSetListChoices(lst, NULL, 4);
#else

		LstSetListChoices(lst, NULL, 3);
#endif

	}
	else
		LstSetListChoices(lst, NULL, number_macros);

	LstSetSelection(lst, noListSelection);

	Int16 selected=LstPopupList(lst);

	ControlType* ctl=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,PopupTriggerButton));
	CtlSetValue(ctl,0);

	if(selected>=(Int16)0&&selected<(Int16)number_macros)
		play_macro(selected);

	close_macro_database();
}
