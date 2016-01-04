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

#include "preferences.h"
#include "edit_interface.h"
#include "main.h"
#include "error.h"
#include "form_pos.h"
#include "utility.h"

UInt32 * SiPreferencesDatabase::crIDs=(UInt32*)1;
Char * SiPreferencesDatabase::appNames=(Char*)1;
UInt32  SiPreferencesDatabase::appCnt=1;
MainPrefsUnSavedType * SiPreferencesDatabase::m_unsaved_prefs=NULL;
MainPrefsSavedType * SiPreferencesDatabase::m_saved_prefs=NULL;
Char * SiPreferencesDatabase::location_data[2]={NULL,NULL};
MemHandle SiPreferencesDatabase::m_name_hand=NULL;
UInt16 SiPreferencesDatabase::m_curr_loaded_font_id=0;
MemHandle SiPreferencesDatabase::m_curr_loaded_font_handle=0;
Boolean SiPreferencesDatabase::m_saved_prefs_changed=false;
Boolean SiPreferencesDatabase::m_unsaved_prefs_changed=false;

Boolean prefs_loop(EventPtr event)
{
	if(event->eType==winEnterEvent)
		if((FormType*)event->data.winEnter.enterWindow==FrmGetFormPtr(ResPrefsDialog)&&(FormType*)event->data.winEnter.exitWindow==FrmGetFormPtr(ResformID_text))
			SiPreferencesDatabase::setup_font_controls(FrmGetFormPtr(ResPrefsDialog));

	//don't actually handle any events
	return false;
}

void SiPreferencesDatabase::startup()
{
#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::startup");
#endif

	load_preferences();

}

void SiPreferencesDatabase::shutdown()
{
#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::shutdown");
#endif

	write_preferences();
	if(m_saved_prefs!=NULL)
	{
		delete m_saved_prefs;
	}
	if(m_unsaved_prefs!=NULL)
		delete m_unsaved_prefs;
	m_saved_prefs=NULL;
	m_unsaved_prefs=NULL;
	
	tidy();
	//release font
	load_font(0);
}
void SiPreferencesDatabase::tidy()
{
#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::tidy");
#endif

	for(Int16 i=0;i<2;++i)
	{
		if(location_data[i]!=NULL)
		{
			delete location_data[i];
			location_data[i]=NULL;
		}
	}
}
/*
typedef struct
{
  Boolean split_screen;
  UInt16 keyboard_open;
  Position cursor_pos[2];
  Int16 file_type[2];
  UInt16 vol_ref[2];
  UInt16 font;
  UInt8 selection_mode;
  UInt8 show_codes;
  UInt8 load_mode;
  Int16 top_line[2];
  UInt8 focused_editor;
  UInt8 read_only[2];
}*/
void SiPreferencesDatabase::generate_unsaved_defaults()
{
#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::generate_unsaved_defaults");
#endif

	m_unsaved_prefs=(MainPrefsUnSavedType*)MemPtrNew(sizeof(MainPrefsUnSavedType));
	
	m_unsaved_prefs->cursor_pos[0].line=m_unsaved_prefs->cursor_pos[1].line=0;
	m_unsaved_prefs->cursor_pos[0].x=m_unsaved_prefs->cursor_pos[1].x=0;
	m_unsaved_prefs->top_line[0]=m_unsaved_prefs->top_line[1]=0;
	m_unsaved_prefs->split_screen=false;
	m_unsaved_prefs->file_type[0]=m_unsaved_prefs->file_type[1]=SIGNED_NONE;
	m_unsaved_prefs->vol_ref[0]=m_unsaved_prefs->vol_ref[1]=NONE;
	m_unsaved_prefs->keyboard_open=0;
	m_unsaved_prefs->focused_editor=0;
	m_unsaved_prefs->read_only[0]=m_unsaved_prefs->read_only[1]=false;
	m_unsaved_prefs->top_height=SiUtility::CurrentScreenHeight/2;
	m_unsaved_prefs_changed=true;
}
void SiPreferencesDatabase::generate_saved_defaults()
{
#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::generate_saved_defaults");
#endif
	m_saved_prefs=(MainPrefsSavedType*)MemPtrNew(sizeof(MainPrefsSavedType));
	m_saved_prefs->font=stdFont;
	m_saved_prefs->show_codes=0;
	m_saved_prefs->selection_mode=STANDARD_STYLE;
	m_saved_prefs->load_mode=MEDIUM_CHUNKS;
	m_saved_prefs->word_wrap=true;
	location_data[0]=location_data[1]=NULL;
#ifdef NEW_LINE_OPTIONS
	m_saved_prefs->newline_mode=PALM_NEWLINE;
#endif
	m_saved_prefs_changed=true;
	
}

void SiPreferencesDatabase::load_preferences()
{
	
	load_saved_preferences();
	load_unsaved_preferences();
	
}

void * SiPreferencesDatabase::load_preferences_data(Boolean saved,UInt16 & size)
{
#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::load_preferences_data");
#endif
	size=0;
	Int16 version=PrefGetAppPreferences(AppID,PrefsID,NULL,&size,saved);
	void * prefs_buffer=NULL;
	if(version>=SIED_VER_NUMBER&&version!=noPreferenceFound)
	{
		prefs_buffer=MemPtrNew(size);
		MemSet(prefs_buffer,size,0);
		PrefGetAppPreferences(AppID,PrefsID,prefs_buffer,&size,saved);
		
	}
	else
		size=0;
	return prefs_buffer;
}

void SiPreferencesDatabase::load_saved_preferences()
{
	#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::load_saved_preferences()");
	#endif
	UInt16 size=0;
	m_saved_prefs=(MainPrefsSavedType*)load_preferences_data(true,size);
	if(!m_saved_prefs)
		generate_saved_defaults();
}

void SiPreferencesDatabase::load_unsaved_preferences()
{
	#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::load_unsaved_preferences()");
	#endif
	UInt16 size=0;
	m_unsaved_prefs=(MainPrefsUnSavedType*)load_preferences_data(false,size);
	if(!m_unsaved_prefs)
		generate_unsaved_defaults();
	else
		load_file_location();
}

void SiPreferencesDatabase::load_file_location()
{
	#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::load_file_location()");
	#endif
	UInt16 start_pos=m_unsaved_prefs->location_offset;
	UInt16 len;
	for(Int16 i=0;i<2;++i)
	{
		#ifdef DEBUG_LOG
	log_entry_number("SiPreferencesDatabase::load_file_location()",i);
	#endif
		if((len=m_unsaved_prefs->location_size[i])>0)
		{

			location_data[i]=(Char*)MemPtrNew(len+1);
			
			MemMove(location_data[i],((Char*)m_unsaved_prefs)+start_pos,len+1);
			
			start_pos+=(len+1);
		}
		else
			location_data[i]=NULL;
	}
	#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::load_file_location() done location");
	#endif
	if(start_pos>0)
	{
		MainPrefsUnSavedType * temp_prefs=(MainPrefsUnSavedType*)MemPtrNew(sizeof(MainPrefsUnSavedType));
		MemMove(temp_prefs,m_unsaved_prefs,sizeof(MainPrefsUnSavedType));
		MemPtrFree(m_unsaved_prefs);
		m_unsaved_prefs=temp_prefs;
	}
	#ifdef DEBUG_LOG
	log_entry("SiPreferencesDatabase::load_file_location() done");
	#endif
}

void SiPreferencesDatabase::write_saved_preferences()
{
	if(m_saved_prefs_changed)
		write_preferences_data((void*)m_saved_prefs,sizeof(MainPrefsSavedType),true);
}

void SiPreferencesDatabase::write_unsaved_preferences()
{
	if(m_unsaved_prefs_changed)
	{
		UInt16 total_size=sizeof(MainPrefsUnSavedType);
		for(UInt16 i=0;i<2;++i)
		{
			if(location_data[i])
			{
				total_size+=m_unsaved_prefs->location_size[i]=StrLen(location_data[i]);
				++total_size;
			}
			else
			{
				m_unsaved_prefs->location_size[i]=0;
			}
		}
		Char* prefs_buffer=(Char*)MemPtrNew(total_size+2);
		m_unsaved_prefs->location_offset=sizeof(MainPrefsUnSavedType);
		if(location_data[0])
		{
			StrCopy(prefs_buffer+m_unsaved_prefs->location_offset,location_data[0]);
			#ifdef DEBUG_LOG
			log_entry("SiPreferencesDatabase file location 0");
			log_entry(location_data[0]);
			#endif
		}
		if(location_data[1])
		{
			StrCopy(prefs_buffer+m_unsaved_prefs->location_offset+m_unsaved_prefs->location_size[0]+1,location_data[1]);
			#ifdef DEBUG_LOG
			log_entry("SiPreferencesDatabase file location 1");
			log_entry(location_data[1]);
			#endif
		}
		MemMove(prefs_buffer,m_unsaved_prefs,sizeof(MainPrefsUnSavedType));
		write_preferences_data((void*)prefs_buffer,total_size,false);
		MemPtrFree(prefs_buffer);
	}
}
void SiPreferencesDatabase::write_preferences_data(void* data,UInt16 size,Boolean saved)
{
	PrefSetAppPreferences(AppID,PrefsID,(Int16)SIED_VER_NUMBER,data,size,saved);
}

void SiPreferencesDatabase::write_preferences()
{
	write_saved_preferences();
	write_unsaved_preferences();
}

/*
	Char * prefs_buffer=(Char*)MemPtrNew(PREFERENCES_SIZE);
	MemMove(prefs_buffer,m_prefs,sizeof(MainPrefsType));
	for(Int16 i=0;i<2;++i)
	{
		if(location_data[i]!=NULL)
		{
#ifdef DEBUG_LOG
			log_entry("Writing file location:");
			log_entry(location_data[i]);
#endif

			StrCopy(prefs_buffer+sizeof(MainPrefsType)+i*MAX_PATH_LENGTH,location_data[i]);
		}
		else
			prefs_buffer[sizeof(MainPrefsType)+i*MAX_PATH_LENGTH]='\0';

	}
	PrefSetAppPreferences(AppID,PrefsID,(Int16)SIED_VER_NUMBER,prefs_buffer,PREFERENCES_SIZE,false);
	MemPtrFree(prefs_buffer);
}*/

fontID SiPreferencesDatabase::get_font()
{
	return lookup_font_id((UInt16)m_saved_prefs->font);
}

Int16 SiPreferencesDatabase::get_selection_mode()
{
	return m_saved_prefs->selection_mode;
}
Boolean SiPreferencesDatabase::get_split_screen()
{
	return m_unsaved_prefs->split_screen;
}
UInt8 SiPreferencesDatabase::get_show_codes()
{
	return m_saved_prefs->show_codes;
}
void SiPreferencesDatabase::set_cursor_pos(const Int16 index,const Position pos)
{
	m_unsaved_prefs->cursor_pos[index]=pos;
	m_unsaved_prefs_changed=true;
}
void SiPreferencesDatabase::set_font(const UInt16 font)
{
	m_saved_prefs->font=font;
	m_saved_prefs_changed=true;
}
void SiPreferencesDatabase::set_selection_mode(const Int16 mode)
{
	m_saved_prefs->selection_mode=mode;
	m_saved_prefs_changed=true;
}
void SiPreferencesDatabase::set_split_screen(const Boolean split)
{
	m_unsaved_prefs->split_screen=split;
m_unsaved_prefs_changed=true;
}

void SiPreferencesDatabase::set_file_open(const Int16 index,Char * location,const Int16 ref,const Int16 type)
{
	m_unsaved_prefs->file_type[index]=type;
	m_unsaved_prefs->vol_ref[index]=ref;
m_unsaved_prefs_changed=true;
	if(location_data[index]!=NULL)
	{
		MemPtrFree(location_data[index]);
		location_data[index]=NULL;
	}
	if(location==NULL)
		return;

	location_data[index]=(Char*)MemPtrNew(StrLen(location)+1);

	if(location_data==NULL)
		return;

	StrCopy(location_data[index],location);
}

void SiPreferencesDatabase::set_show_codes(const UInt8 code)
{
	m_saved_prefs->show_codes=code;
m_saved_prefs_changed=true;
}

Int16 SiPreferencesDatabase::get_load_chunk_size()
{
	switch(m_saved_prefs->load_mode)
	{
	case SMALL_CHUNKS:
		return SMALL_LOAD_CHUNK_SIZE;
	case MEDIUM_CHUNKS:
		return MEDIUM_LOAD_CHUNK_SIZE;
	case LARGE_CHUNKS:
		return LARGE_LOAD_CHUNK_SIZE;
	default:
		return MAXIMUM_CHUNK_SIZE;
	}
}



void SiPreferencesDatabase::set_load_mode(UInt8 load_mode)
{
	m_saved_prefs->load_mode=load_mode;
}

Char * SiPreferencesDatabase::get_location_data(const Int16 index)
{
	return location_data[index];
}
Position SiPreferencesDatabase::get_cursor_pos(const Int16 index)
{
	return m_unsaved_prefs->cursor_pos[index];
}
Int16 SiPreferencesDatabase::get_vol_ref(const Int16 index)
{
	return m_unsaved_prefs->vol_ref[index];
}

Int16 SiPreferencesDatabase::get_file_type(const Int16 index)
{
	return m_unsaved_prefs->file_type[index];
}

void SiPreferencesDatabase::display_hires_controls()
{
	if(SiUtility::has_hires_support())
	{
		FormPtr frm=FrmGetFormPtr(ResPrefsDialog);
		FrmShowObject(frm,FrmGetObjectIndex(frm,SmallHiresFontButton));
		FrmShowObject(frm,FrmGetObjectIndex(frm,TinyHiresFontButton));
		FrmShowObject(frm,FrmGetObjectIndex(frm,StdFixedHiresFontButton));
		FrmShowObject(frm,FrmGetObjectIndex(frm,SmallBoldHiresFontButton));

	}
}

void SiPreferencesDatabase::draw_font_text(FormPtr frm,UInt16 id,UInt16 font)
{
	UInt16 prev_font=m_saved_prefs->font;
	RectangleType rp;
	FrmGetObjectBounds(frm,FrmGetObjectIndex(frm,id),&rp);
	m_saved_prefs->font=load_font(font);
	FontID prev_draw_font=FntSetFont(get_font());
	UInt16 x=rp.topLeft.x+(rp.extent.x-TxtCharWidth('A'))/2;
	UInt16 y=rp.topLeft.y+(rp.extent.y-FntCharHeight())/2;
	WinDrawChars("A",StrLen("A"),x,y);
	if(prev_font>=customFontID)
		m_saved_prefs->font=load_font(prev_font);
	else
		m_saved_prefs->font=prev_font;
	FntSetFont(prev_draw_font);
}
void SiPreferencesDatabase::setup_font_controls(FormPtr frm)
{

	UInt16 button_id=StdFontButton;
	draw_font_text(frm,TinyFontButton,tinyFont);
	draw_font_text(frm,SmallFontButton,smallFont);
	draw_font_text(frm,StdFixedFontButton,stdFixedFont);
	draw_font_text(frm,NarrowFixedFontButton,narrowFixedFont);
	if(SiUtility::has_hires_support())
	{
		draw_font_text(frm,TinyHiresFontButton,tinyHiresFont);
		draw_font_text(frm,SmallHiresFontButton,smallHiresFont);
		draw_font_text(frm,StdFixedHiresFontButton,stdFixedHiresFont);
		draw_font_text(frm,SmallBoldHiresFontButton,smallBoldHiresFont);

	}
	switch(m_saved_prefs->font)
	{
	case stdFont:
		button_id=StdFontButton;

		break;
	case boldFont:
		button_id=BoldFontButton;

		break;
	case largeFont:
		button_id=LargeFontButton;

		break;
	case largeBoldFont:
		button_id=LargeBoldFontButton;

		break;
	case smallFont:
		button_id=SmallFontButton;
		break;
	case tinyFont:
		button_id=TinyFontButton;
		break;
	case stdFixedFont:
		button_id=StdFixedFontButton;
		break;
	case narrowFixedFont:
		button_id=NarrowFixedFontButton;
		break;
	case smallHiresFont:
		button_id=SmallHiresFontButton;
		break;
	case tinyHiresFont:
		button_id=TinyHiresFontButton;
		break;
	case stdFixedHiresFont:
		button_id=StdFixedHiresFontButton;
		break;
	case smallBoldHiresFont:
		button_id=SmallBoldHiresFontButton;
		break;
	default:
		//	  DisplayError(DEBUG_MESSAGE,"Unknown font");
		button_id=StdFontButton;
		break;
	}

	FrmSetControlGroupSelection(frm,1,button_id);
}
void SiPreferencesDatabase::setup_prefs_dialog(FormPtr form)
{
#ifdef DEBUG
	ErrFatalDisplayIf(m_saved_prefs==NULL,"NULL saved prefs struct on displaying dialog");
	ErrFatalDisplayIf(m_unsaved_prefs==NULL,"NULL unsaved prefs struct on displaying dialog");
#endif

	display_hires_controls();

	initialise_mime_list();

	ListType * list=(ListType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,SelectionList));
	ControlType * trigger=(ControlType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,SelectPopupTrigger));

	switch (SiPreferencesDatabase::get_selection_mode())
	{
	case PALM_STYLE:
		LstSetSelection(list,0);
		CtlSetLabel(trigger,PALM_STYLE_TEXT);
		break;
	case STANDARD_STYLE:
		LstSetSelection(list,1);
		CtlSetLabel(trigger,STANDARD_STYLE_TEXT);
		break;
	}

	list=(ListType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,CodesList));
	trigger=(ControlType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,CodesPopupTrigger));
	if(m_saved_prefs->show_codes&EOL_CODES)
	{
		LstSetSelection(list,1);
		CtlSetLabel(trigger,EOL_TEXT);
	}
	else
	{
		LstSetSelection(list,0);
		CtlSetLabel(trigger,NONE_TEXT);
	}

	list=(ListType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,LoadModeList));
	trigger=(ControlType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,LoadModePopupTrigger));
	switch(m_saved_prefs->load_mode)
	{
	case SMALL_CHUNKS:
		LstSetSelection(list,0);
		CtlSetLabel(trigger,SMALL_CHUNK_TEXT);
		break;
	case MEDIUM_CHUNKS:
		LstSetSelection(list,1);
		CtlSetLabel(trigger,MEDIUM_CHUNK_TEXT);
		break;
	case LARGE_CHUNKS:
		LstSetSelection(list,2);
		CtlSetLabel(trigger,LARGE_CHUNK_TEXT);
		break;
	case OFF_CHUNKS:
		LstSetSelection(list,3);
		CtlSetLabel(trigger,OFF_CHUNK_TEXT);
		break;
	}
}
void SiPreferencesDatabase::tidy_mime_list()
{

	if(crIDs)
	{
		MemPtrFree(crIDs);
		crIDs=(UInt32*)1;
	}
	if(appNames)
	{
		MemPtrFree(appNames);
		appNames=(Char*)1;
	}
	if(m_name_hand)
	{
		MemHandleUnlock(m_name_hand);
		MemHandleFree(m_name_hand);
	}
}
Boolean SiPreferencesDatabase::display_prefs_dialog()
{

	FormPtr form=FrmInitForm(ResPrefsDialog);
	FrmSetEventHandler(form,prefs_loop);
	setup_prefs_dialog(form);
	UInt16 prev_font=m_saved_prefs->font;
	UInt8 prev_code=SiPreferencesDatabase::get_show_codes();
	Int16 ret=FrmDoDialog(form);

	if(ret==PrefsOK)
	{

		ListType * list=(ListType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,SelectionList));
		SiPreferencesDatabase::set_selection_mode(LstGetSelection(list));

		switch (FrmGetObjectId(form,FrmGetControlGroupSelection(form,1)))
		{
		case StdFontButton:
			m_saved_prefs->font=stdFont;
			break;
		case BoldFontButton:
			m_saved_prefs->font=boldFont;
			break;
		case LargeFontButton:
			m_saved_prefs->font=largeFont;
			break;
		case LargeBoldFontButton:
			m_saved_prefs->font=largeBoldFont;
			break;
		case TinyFontButton:
			m_saved_prefs->font=load_font(tinyFont);
			break;
		case SmallFontButton:
			m_saved_prefs->font=load_font(smallFont);
			break;
		case StdFixedFontButton:
			m_saved_prefs->font=load_font(stdFixedFont);
			break;
		case NarrowFixedFontButton:
			m_saved_prefs->font=load_font(narrowFixedFont);
			break;
		case SmallHiresFontButton:
			m_saved_prefs->font=load_font(smallHiresFont);
			break;
		case TinyHiresFontButton:
			m_saved_prefs->font=load_font(tinyHiresFont);
			break;
		case StdFixedHiresFontButton:
			m_saved_prefs->font=load_font(stdFixedHiresFont);
			break;
		case SmallBoldHiresFontButton:
			m_saved_prefs->font=load_font(smallBoldHiresFont);
			break;
		default:
			//		  DisplayError(DEBUG_MESSAGE,"Unknown button");
			break;
		}
		list=(ListType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,CodesList));
		switch (LstGetSelection(list))
		{
		case 0:
			SiPreferencesDatabase::set_show_codes(NO_CODES);
			break;
		case 1:
			SiPreferencesDatabase::set_show_codes(EOL_CODES);
			break;
		}
		list=(ListType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,LoadModeList));
		switch(LstGetSelection(list))
		{
		case 0:
			SiPreferencesDatabase::set_load_mode(SMALL_CHUNKS);
			break;
		case 1:
			SiPreferencesDatabase::set_load_mode(MEDIUM_CHUNKS);
			break;
		case 2:

			SiPreferencesDatabase::set_load_mode(LARGE_CHUNKS);
			break;
		case 3:

			SiPreferencesDatabase::set_load_mode(OFF_CHUNKS);
			break;
		}



		if(prev_font!=m_saved_prefs->font)
			edit_interface->change_font();
		else if(SiPreferencesDatabase::get_show_codes()!=prev_code)
			edit_interface->emit_redraw_signal(NULL);

		list=(ListType*)FrmGetObjectPtr(form,FrmGetObjectIndex(form,MimeRegisterList));
		check_mime_register(form,list);
		tidy_mime_list();
		FrmDeleteForm(form);
		return true;
	}
	else
	{
		tidy_mime_list();
		FrmDeleteForm(form);
		return false;
	}
}
void SiPreferencesDatabase::check_mime_register(FormPtr frm,ListType * list)
{
	UInt32 romVersion;
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

	if (romVersion >= sysMakeROMVersion(4, 0, 0,sysROMStageRelease, 0))
	{
		Int16 id=LstGetSelection(list);
		if(id!=noListSelection)
		{
			UInt32 defaultID;
			ExgGetDefaultApplication(&defaultID, exgRegTypeID, "text/plain");

			if(defaultID!=(crIDs[id]))
			{
				ExgSetDefaultApplication(crIDs[id],exgRegTypeID,MIME_TYPE);
				ExgSetDefaultApplication(AppID,exgRegExtensionID,FILE_EXTENSIONS);
			}
		}
	}

}

Int16 SiPreferencesDatabase::get_top_line(const Int16 index)
{
	return m_unsaved_prefs->top_line[index];

}

void SiPreferencesDatabase::set_top_line(const Int16 index,const Int16 top_line)
{
	m_unsaved_prefs->top_line[index]=top_line;
}

void SiPreferencesDatabase::set_keyboard_open(const UInt16 o)
{
	m_unsaved_prefs->keyboard_open=o;
}

UInt16 SiPreferencesDatabase::get_keyboard_open()
{
	return m_unsaved_prefs->keyboard_open;
}
void SiPreferencesDatabase::set_focused_editor(const UInt8 e)
{
	m_unsaved_prefs->focused_editor=e;
}

UInt8 SiPreferencesDatabase::get_focused_editor()
{
	return m_unsaved_prefs->focused_editor;
}

void SiPreferencesDatabase::initialise_mime_list()
{
	UInt32 romVersion;
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	FormPtr frm=FrmGetFormPtr(ResPrefsDialog);
	if (romVersion >= sysMakeROMVersion(4, 0, 0,sysROMStageRelease, 0))
	{
		ControlPtr ctl;
		ListPtr lst;
		UInt32 defaultID;
		//pointers for app data
		//must be non-NULL for API call

		ctl = (ControlPtr)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,MimeRegisterTrigger));
		lst = (ListPtr)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,MimeRegisterList));
		Char ** appNamesArray=NULL;
		// crIDs, appCnt, appNames are all global variables.
		// Get the list of creator IDs if we don't have it already.
		appCnt=0;
		ExgGetRegisteredApplications(&crIDs, &appCnt, &appNames, NULL, exgRegTypeID, "text/plain");
		if(appCnt)
		{
			m_name_hand = SysFormPointerArrayToStrings(appNames, appCnt);
			if(m_name_hand)
				appNamesArray =(Char**) MemHandleLock(m_name_hand);
			else
				return;
		}
		else
			return;

		if(appNamesArray)
			LstSetListChoices(lst, appNamesArray, appCnt);

		LstSetHeight(lst, appCnt < 6 ? appCnt : 6);


		UInt16 i;
		ExgGetDefaultApplication(&defaultID, exgRegTypeID, "text/plain");

		for(i=0;i<appCnt;i++)
		{
			if(crIDs[i] == defaultID)
				LstSetSelection(lst, i);
		}
		if(LstGetSelection(lst)==noListSelection)
			LstSetSelection(lst,0);
#ifdef DEBUG

		ErrFatalDisplayIf(appNamesArray[LstGetSelection(lst)]==NULL,"NULL name in MIME list");
#endif

		CtlSetLabel(ctl, appNamesArray[LstGetSelection(lst)]);

	}
	else
	{
		hide_mime_controls(frm);
		crIDs=NULL;
		m_name_hand=NULL;
		appNames=NULL;

	}
}

void SiPreferencesDatabase::hide_mime_controls(FormPtr frm)
{
	FrmHideObject(frm,FrmGetObjectIndex(frm,MimeRegisterList));
	FrmHideObject(frm,FrmGetObjectIndex(frm,MimeRegisterTrigger));
	FrmHideObject(frm,FrmGetObjectIndex(frm,MimeRegisterLabel));
}


UInt16 SiPreferencesDatabase::load_font(UInt16 font)
{
	if(font==m_curr_loaded_font_id)
		return font;
	else if(m_curr_loaded_font_id!=0)
	{
		MemHandleUnlock(m_curr_loaded_font_handle);
		DmReleaseResource(m_curr_loaded_font_handle);
		m_curr_loaded_font_id=0;
		m_curr_loaded_font_handle=NULL;
	}

	if(font!=0&&font>customFontID)
	{

		m_curr_loaded_font_handle=DmGetResource(fontRscType,(DmResID)font);
		if(m_curr_loaded_font_handle!=NULL)
		{

			m_curr_loaded_font_id=font;
			FntDefineFont(customFontID,(FontType*)MemHandleLock(m_curr_loaded_font_handle));

			return font;
		}
	}

	return (UInt16)0;
}

fontID SiPreferencesDatabase::lookup_font_id(UInt16 font)
{
	if(font<(UInt16)customFontID)
		return (fontID)font;
	else
		return customFontID;
}

Boolean SiPreferencesDatabase::get_read_only(const Int16 index)
{
	return m_unsaved_prefs->read_only[index];
}

void SiPreferencesDatabase::set_read_only(const Int16 index,const Boolean only)
{
	m_unsaved_prefs->read_only[index]=only;
	m_unsaved_prefs_changed=true;
}

void SiPreferencesDatabase::set_word_wrap(const Boolean b)
{
	m_saved_prefs->word_wrap=b;
	m_saved_prefs_changed=true;
}

Boolean SiPreferencesDatabase::get_word_wrap()
{
	return m_saved_prefs->word_wrap;
}

void SiPreferencesDatabase::set_top_height(const Int16 y)
{
	m_unsaved_prefs->top_height=y;
	m_unsaved_prefs_changed=true;
}

Int16 SiPreferencesDatabase::get_top_height()
{
	return m_unsaved_prefs->top_height;
}
