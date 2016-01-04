/*
 * SiEd - A text editor for PalmOS 3.5+ Copyright (C) 2003 Benjamin Roe
 * except for SiFileDialog::Beam and WriteProc functions, which are (C) 2003 Salomon Ramirez <yolasalo@yahoo.com>
 * Contact Email:ben@benroe.com
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
//PalmOS headers
#include <VFSMgr.h>
#include <FeatureMgr.h>
#include <ExpansionMgr.h>

//SiEd headers
#include "utility.h"
#include "error.h"
#include "text_resource.h"
#include "filedialog.h"
#include "root_dirnode.h"
#include "form_pos.h"
#include "encrypt_file_plugin.h"
#include "main.h"
#ifdef TEST_OBJECT_UNIT
#include "doc_dirnode.h"
#endif
//Static variable values for SiFileDialog
//class is entirely static
#define TEMP_FILE_NAME "x[]{},.654"

SiDirNode * SiFileDialog::curr_node=NULL;
Char * SiFileDialog::curr_path=NULL;
Int16 SiFileDialog::file_selected=noListSelection;
Boolean SiFileDialog::VFS_SUPPORT=false;
Boolean filelistloop(EventPtr e) __attribute__ ((section ("filefns")));;
Boolean SiFileDialog::update_dir_display=false;
TimeFormatType SiFileDialog::TIME_STYLE=(TimeFormatType)0;
DateFormatType SiFileDialog::DATE_STYLE=(DateFormatType)0;
UInt16 SiFileDialog::dialog_mode=SAVE_FILE;
UInt16 SiFileDialog::save_options=0;
Char * SiFileDialog::working_dir=NULL;
UInt16 SiFileDialog::working_vol=NON_VFS;

const Char * SiFileDialog::m_initial_filename=NULL;
//! Event handling loop for the file dialog
//!
Boolean filelistloop(EventPtr event)
{
	//event loop for file dialog (save/open)
	FormPtr form;
	Boolean handled = false;
	Int16 sel;
	ListPtr list;
	switch (event->eType)
	{
	case frmOpenEvent:
		break;
	case lstSelectEvent:
		form = FrmGetFormPtr(ResFileDialog);
		list =
			(ListPtr) FrmGetObjectPtr(form,
									  FrmGetObjectIndex(form, FileList));
		sel = LstGetSelection(list);
		handled=SiFileDialog::handle_list_select(sel);
		break;
	case ctlSelectEvent:
		handled=SiFileDialog::handle_button_press(event->data.ctlSelect.controlID);
		break;
	case menuEvent:
		SiFileDialog::Beam(event->data.menu.itemID);
		handled=true;
		break;
	case winEnterEvent:
		edit_interface->set_size_limits(ResFileDialog);
		if((FormType*)event->data.winEnter.enterWindow==FrmGetFormPtr(ResFileDialog)&&
				(FormType*)event->data.winEnter.exitWindow==FrmGetFormPtr(ResformID_text))
			SiFileDialog::initialise_dir_field();
		
		break;
	default:
		break;
	}
	return handled;

}
//! Callback for drawing text/ icon for file list
//!
void drawFileName(Int16 itemNum, RectangleType * bounds, Char ** itemsText)
{
#ifdef DEBUG_LOG
	log_entry_number("SiFileDialog::draw_file_name number ",itemNum);
#endif

	if(SiFileDialog::curr_node!=NULL&&itemNum>=0&&itemNum<(Int16)SiFileDialog::curr_node->number_children)
	{
		SiFileDialog::curr_node->child_nodes[itemNum]->draw_self(bounds->topLeft.x,bounds->topLeft.y,bounds->extent.x);
	}
}

//! Get preferences from the OS for date/time format on start up
//!

void SiFileDialog::initialise()
{
	TIME_STYLE= (TimeFormatType)    PrefGetPreference(prefTimeFormat);
	DATE_STYLE = (DateFormatType)    PrefGetPreference(prefDateFormat);
}

void SiFileDialog::initialise_dir_field()
{
	set_dir_field(curr_node);
	FormPtr frm = FrmGetFormPtr(ResFileDialog);
	FieldType* name_field=(FieldType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,OpenFileFileField));
	SiUtility::set_field_term(name_field,m_initial_filename,true);

}

void SiFileDialog::exit()
{
	if(working_dir!=NULL)
	{
		MemPtrFree(working_dir);
		working_dir=NULL;
	}
	tidy();
}
void SiFileDialog::tidy()
{
	SiDirNode * temp_node;
	while(curr_node!=NULL)
	{
		temp_node=curr_node->parent;
		delete curr_node;
		curr_node=temp_node;
	}
	if(curr_path!=NULL)
	{
		MemPtrFree(curr_path);
		curr_path=NULL;
	}
}

Boolean SiFileDialog::check_selected_file()
{
	Boolean handled=true;
	if(file_selected!=noListSelection)
	{
		/*		if(curr_node->child_nodes[file_selected]->get_file_size()>MAX_FILE_SIZE)
				{
					FrmAlert(FileTooLargeAlert);
				}
				else*/
		handled=false;
	}
	return handled;

}
//! Deal with the user pressing one of the RHS buttons on a file dialog
//!
Boolean SiFileDialog::handle_button_press(const UInt16 button)
{
	//respond to a user clicking on a button on the dialog
	Boolean handled=false;
	FormPtr frm;
	Char * dest;
	FieldPtr text_field;

	switch(button)
	{
	case RenameFileButton:
		if(dialog_mode==MANAGE_FILE)
			handle_rename();
		handled=true;
		break;
	case DeleteFileButton:
		handle_delete();
		handled=true;
		break;
	case OpenFileButton:
		if(dialog_mode==OPEN_FILE)
		{
			handled=check_selected_file();
		}
		else
		{
			frm = FrmGetFormPtr(ResFileDialog);
			text_field=(FieldPtr)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,OpenFileFileField));
			dest = FldGetTextPtr(text_field);
			if(dest==NULL)
				handled=true;
			else
			{
				if(!valid_file_name(dest))
				{
					DisplayError(INVALID_FILE_NAME,dest);
					handled=true;
				}
			}
		}
		break;
	case NewDirButton:
		handle_new_dir();
		handled=true;
		break;
	case CancelFileButton:
		break;
	case DecryptButton:
		handle_decrypt_file();
		handled=true;
		break;
	}
	return handled;
}


void SiFileDialog::handle_decrypt_file()
{
	//! Decrypt the selected file by reading the data out of it
	//! and writing it out to another file
#ifdef DEBUG_LOG
	log_entry("SiFileDialog::Decrypting file");
#endif

	if(file_selected==noListSelection)
		return;

	SiFile * file=curr_node->child_nodes[file_selected]->get_file(NULL);
	if(file==NULL)
		return;
	SiFilePlugin * plug=file->is_encrypted();
	if(plug==NULL)
	{
		DisplayError(NOT_ENCRYPTED,FILE_NOT_ENCRYPTED_TEXT);
		delete file;
		return;
	}

	//initialise the files
	file->add_plugin(plug,AT_END);

	file->initialise(FILE_READ);
	if(!file->is_open())
	{
		//	DisplayError(FILE_ACCESS_ERROR,"Temporary decryption file");
		delete file;
		return;
	}

	SiFile * dest_file=curr_node->create_file(TEMP_FILE_NAME,file->get_size());

	dest_file->open();

	dest_file->set_access_mode(FILE_WRITE);

	dest_file->initialise(FILE_WRITE);

	if(!dest_file->is_open())
	{
		DisplayError(FILE_ACCESS_ERROR,"creating temporary decryption file");
		dest_file->delete_file();
		delete dest_file;
		delete file;

		return;
	}


	UInt32 amount_done=0;


	SiMemChunk * chunk;
	BlockInt n_chars;

	//read the data out one chunk at a time and write it to the other file
	do
	{
		chunk=file->read_data(n_chars);

		if(chunk!=NULL)
		{
			dest_file->write_data(chunk);

			amount_done+=chunk->size;

			SiMemHandler::free_chunk(chunk);

		}
	}
	while(chunk!=NULL);

	//then rename the new one
	//the same as the old one+'-decrypt'
	Char * name=(Char *)MemPtrNew(StrLen(file->get_name())+StrLen("-decrypt")+1);
	StrCopy(name,file->get_name());
	StrCopy(name+StrLen(name),"-decrypt");


	file->close();

	delete file;
	dest_file->resize(amount_done);
	dest_file->close();
	SiFile * existing_file=curr_node->get_file(name);
	if(existing_file!=NULL)
	{
		existing_file->delete_file();
		delete existing_file;
	}

	dest_file->rename(name);
	MemPtrFree(name);
	//dest_file->open();


#ifdef DEBUG
	//ErrFatalDisplayIf(dest_file->get_size()!=amount_done,"Incorrect resize");
#endif

	//MemPtrFree(name);
	make_current_node(curr_node);
	delete dest_file;
}

//! Create a new directory
//!

void SiFileDialog::handle_new_dir()
{
	FormPtr form=FrmInitForm(NewDirDialog);
	FrmSetFocus(form,FrmGetObjectIndex(form,NewDirField));
	Int16 ret=FrmDoDialog(form);
	if(ret==NewDirCreate)
	{
		FieldPtr field=(FieldPtr)FrmGetObjectPtr(form,FrmGetObjectIndex(form,NewDirField));
		Char * text=FldGetTextPtr(field);
		if(text!=NULL)
		{
			Char * old_path=curr_node->get_path(STANDARD);
			Int16 old_length=StrLen(old_path);
			Char * new_path=(Char*)MemPtrNew(StrLen(text)+old_length+1);
			MemMove(new_path,old_path,old_length);
			StrCopy(new_path+old_length,text);
			Err err=VFSDirCreate(curr_node->volRefNum,new_path);
			if(err!=errNone)
			{
				DisplayError(UNABLE_TO_CREATE_DIR,new_path);
			}
			MemPtrFree(old_path);
			MemPtrFree(new_path);
		}
	}
	FrmDeleteForm(form);
	//file_selected=noListSelection;
	clear_selection();
	make_current_node(curr_node);
}

void SiFileDialog::handle_rename()
{
#ifdef DEBUG_LOG
	log_entry("Renaming file");
#endif

	if(file_selected==noListSelection)
		return;
	SiFile * file=curr_node->child_nodes[file_selected]->get_file(NULL);
	if(file==NULL)
		return;
	FormPtr form=FrmInitForm(ResRenameFileDialog);

	FieldPtr filename_field=(FieldPtr)FrmGetObjectPtr(form,FrmGetObjectIndex(form,RenameFileOldFileNameLabel));
	FldSetTextPtr(filename_field,curr_node->child_nodes[file_selected]->name);

	FrmSetFocus(form,FrmGetObjectIndex(form,RenameFileField));

	Int16 ret=FrmDoDialog(form);
	if(ret==RenameFileRename)
	{

		filename_field=(FieldPtr)FrmGetObjectPtr(form,FrmGetObjectIndex(form,RenameFileField));
		Char * text=FldGetTextPtr(filename_field);
		if(text!=NULL&&file->is_open())
		{
			//rename the file
			if(valid_file_name(text))
				file->rename(text);
			else
				DisplayError(INVALID_FILE_NAME,text);

		}

	}
	delete file;
	FrmDeleteForm(form);

	//file_selected=noListSelection;
	if(ret==RenameFileRename)
	{
		clear_selection();
		make_current_node(curr_node);
	}


}

void SiFileDialog::handle_delete()
{
#ifdef DEBUG_LOG
	log_entry("SiFileDialog::deleting file");
#endif

	if(file_selected==noListSelection)
		return;
	SiFile * file=curr_node->child_nodes[file_selected]->get_file(NULL);

	if(file==NULL)
		return;

	Int16 ret=FrmCustomAlert(ConfirmDeleteAlert,curr_node->child_nodes[file_selected]->name,NULL,NULL);
	if(ret==DeleteYes)
	{
		//delete the file
		file->delete_file();
	}
	delete file;
	if(ret==DeleteYes)
	{

		clear_selection();
		make_current_node(curr_node);
	}

}

void SiFileDialog::check_decrypt_button(const Int16 num)
{
	FormPtr frm=FrmGetActiveForm();

	if(num==noListSelection)
		FrmHideObject(frm,FrmGetObjectIndex(frm,DecryptButton));

	if(curr_node->child_nodes[num]->is_encrypted())
	{
		FrmShowObject(frm,FrmGetObjectIndex(frm,DecryptButton));
	}
	else
	{
		FrmHideObject(frm,FrmGetObjectIndex(frm,DecryptButton));
	}
}
Boolean SiFileDialog::handle_list_select(const Int16 num)
{
	if(curr_node==NULL)
		return true;
	file_selected = num;
	switch(curr_node->child_nodes[num]->select_action)
	{
	case ACTION_SELECT_FILE:
		draw_data_display(num);
		set_open_field(curr_node->child_nodes[num]);
		check_decrypt_button(num);
		break;
	case ACTION_CHANGE_DIR:
		switch_to_dir(num);
		//file_selected=noListSelection;
		clear_selection();
		break;
	case ACTION_MOVE_UP_DIR:
		make_current_node(curr_node->parent);
		break;
	default:
		//file_selected=noListSelection;
		clear_selection();
		break;
	}
	return true;
}

void SiFileDialog::make_current_node(SiDirNode* node)
{
	//switch the current dir to the argument node
	SiDirNode *temp_node=node;
	if(curr_node->parent==node)
	{
		delete curr_node;
	}
	else if(node!=curr_node)
	{
		temp_node=temp_node->get_copy();

	}
	clear_selection();
	curr_node=temp_node;
	curr_node->flush_children();
	curr_node->populate();

	curr_node->sort_children();

	set_dir_field(curr_node);
	//enable the new_dir button if appropriate
	show_buttons();
	redraw_list();
}

void SiFileDialog::show_buttons()
{
	FormPtr frm=FrmGetFormPtr(ResFileDialog);
	//ControlType* dir_button=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,NewDirButton));
	if((curr_node->options&CREATE_DIRECTORIES) && (dialog_mode!=OPEN_FILE) )
	{
		FrmShowObject(frm,FrmGetObjectIndex(frm,NewDirButton));

	}
	else
	{
		FrmHideObject(frm,FrmGetObjectIndex(frm,NewDirButton));
	}
	FrmHideObject(frm,FrmGetObjectIndex(frm,DecryptButton));

}

void SiFileDialog::switch_to_dir(UInt16 num)
{
	//ErrFatalDisplayIf(&(curr_node->child_nodes[num])==NULL,"Null child");
	make_current_node(curr_node->child_nodes[num]);

}

void SiFileDialog::set_dir_field(SiDirNode * node)
{
	FormPtr form=FrmGetFormPtr(ResFileDialog);
	FieldPtr field=(FieldPtr)FrmGetObjectPtr(form,FrmGetObjectIndex(form,OpenFileDirField));
	FldEraseField(field);


	if(curr_path!=NULL)
	{
		MemPtrFree(curr_path);
		curr_path=NULL;
	}

	Char * path_name=node->get_path(INCLUDE_CARD|EXCLUDE_LEAF_NODE);


	if(path_name!=NULL)
	{
		FldSetTextPtr(field,path_name);
		curr_path=path_name;
		FldDrawField(field);
	}




	set_open_field(node);

}
void SiFileDialog::set_open_field(SiDirNode* node)
{
	FormPtr form=FrmGetFormPtr(ResFileDialog);
	FieldType *field=(FieldPtr)FrmGetObjectPtr(form,FrmGetObjectIndex(form,OpenFileFileField));

	if((node->options&DISPLAY_FILENAME))
	{
		FldEraseField(field);
		UInt16 length=FldGetTextLength(field);
		FldDelete(field,0,length);
		FldSetInsertionPoint(field,0);
		if((node->name!=NULL))
		{
			FldInsert(field,node->name,StrLen(node->name));

		}
	}
	FldDrawField(field);
}

void SiFileDialog::redraw_list()
{
	FormPtr form = FrmGetFormPtr(ResFileDialog);
	ListPtr list =
		(ListPtr) FrmGetObjectPtr(form,FrmGetObjectIndex(form, FileList));
	LstEraseList(list);
	LstSetListChoices(list, NULL, curr_node->number_children);
	LstSetSelection(list, noListSelection);
	FormPtr frm2=FrmGetActiveForm();
	FrmSetActiveForm(frm2);
	LstDrawList(list);
}
void SiFileDialog::draw_data_display(UInt16 num)
{
	SiDirNode * node=curr_node->child_nodes[num];
	UInt32 size=node->get_file_size();
	UInt32 date=node->get_file_date();
	Char buff[60];
	FontID curr_font=FntSetFont(stdFont);
	RectangleType rp;
	rp.topLeft.x = SIZE_X;
	rp.topLeft.y = SIZE_Y;
	rp.extent.x = FILE_FORM_RIGHT - SIZE_X - 1;
	rp.extent.y = FntLineHeight();

	StrPrintF(buff, "%luKB", 1+size/1024);

	WinEraseRectangle(&rp, 0);
	WinDrawTruncChars(buff, StrLen(buff), SIZE_X, SIZE_Y, rp.extent.x);
	rp.topLeft.x = SIZE_X-2;
	rp.topLeft.y = SIZE_Y+FntLineHeight();
	rp.extent.x = FILE_FORM_RIGHT - SIZE_X - 4;
	rp.extent.y = FntLineHeight() * 2 + 1;

	WinEraseRectangle(&rp, 0);
	UInt16 date_y=FntLineHeight()+SIZE_Y;

	DateTimeType dt;

	TimSecondsToDateTime(date, &dt);

	TimeToAscii(dt.hour, dt.minute, TIME_STYLE, buff);

	WinDrawTruncChars(buff, StrLen(buff), SIZE_X, date_y, rp.extent.x);

	DateToAscii(dt.month, dt.day, dt.year, DATE_STYLE, buff);
	WinDrawTruncChars(buff, StrLen(buff), SIZE_X,
					  date_y +FntLineHeight() , rp.extent.x);

	rp.topLeft.x=SIZE_RECT_X;
	rp.topLeft.y=SIZE_RECT_Y;
	rp.extent.x= FILE_FORM_RIGHT - SIZE_X-2;
	rp.extent.y= FntLineHeight()*3+3;
	WinPaintRectangleFrame(boldRoundFrame,&rp);
	FntSetFont(curr_font);
}
void SiFileDialog::clear_selection()
{
	if(file_selected!=noListSelection)
	{
		file_selected=noListSelection;
		clear_data_display();
	}
}
void SiFileDialog::clear_data_display()
{
	FontID curr_font=FntSetFont(stdFont);
	RectangleType rp;
	rp.topLeft.x = SIZE_RECT_X-2;
	rp.topLeft.y = SIZE_RECT_Y-2;
	rp.extent.x = FILE_FORM_RIGHT - SIZE_RECT_X +4;
	rp.extent.y = FntLineHeight()*3+7;
	WinEraseRectangle(&rp,0);

	FntSetFont(curr_font);
}

void SiFileDialog::initialise_list(const char * initial_dir,UInt16 vol)
{
#ifdef DEBUG_LOG
	log_entry("SiFileDialog::initialise_list");
#endif
	edit_interface->set_size_limits(ResFileDialog);

	curr_path=NULL;
	curr_node=get_node(initial_dir,vol);
	file_selected=noListSelection;
	if(curr_node==NULL)
	{
		return;
	}

	FormPtr frm=FrmGetFormPtr(ResFileDialog);
	clear_selection();
	//file_selected = noListSelection;
	FrmSetEventHandler(frm, (FormEventHandlerPtr) filelistloop);
	ListPtr list = (ListPtr)
				   FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FileList));
	LstSetDrawFunction(list, drawFileName);
	LstSetListChoices(list, NULL, curr_node->number_children);
	LstSetSelection(list, noListSelection);

}

//enable another day
/*
#define PopupFileList 6840
#define FILE_POPUP_WIDTH 80
#define FILE_POPUP_HEIGHT 60
#define FILE_X_OFFSET 0
#define FILE_POPUP_ITEMS 6
SiFile * SiFileDialog::popup_file_list(const Int16 x,const Int16 y)
{
  curr_node=get_node(working_dir,working_vol);
 
  FormPtr frm=FrmGetFormPtr(ResformID_text);
  Err err=LstNewList ((void**)(&frm),PopupFileList,x+FILE_X_OFFSET,y-FILE_POPUP_HEIGHT,FILE_POPUP_WIDTH,FILE_POPUP_HEIGHT,stdFont,curr_node->number_children,0);
  if(err!=errNone)
    return NULL;
 
  ListPtr list = (ListPtr)FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, PopupFileList));
  LstSetDrawFunction(list, drawFileName);
  LstSetListChoices(list, NULL, curr_node->number_children);
  LstSetSelection(list, noListSelection);
  LstDrawList(list);
  DisplayError(DEBUG_MESSAGE,"here");
  return NULL;
}*/

Char* SiFileDialog::show_save_form(const Char * initial_dir,UInt16 vol)
{
#ifdef TEST_OBJECTS_LOG
	log_entry("SiFileDialog::show_save_form");
#endif

	update_dir_display=false;
	FormPtr frm = FrmInitForm(ResFileDialog);

	initialise_list(initial_dir,vol);

	//turn the generic file dialog into a save dialog
	FrmSetTitle(frm,SAVE_AS);
	ControlType * open_button=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,OpenFileButton));
	CtlSetLabel(open_button,SAVE);
	show_buttons();
	if(SiUtility::encryption_support())
		FrmShowObject(frm,FrmGetObjectIndex(frm,EncryptionCheckbox));
	if(save_options&ENCRYPTED)
		FrmSetControlValue(frm,FrmGetObjectIndex(frm,EncryptionCheckbox),1);
	else
		FrmSetControlValue(frm,FrmGetObjectIndex(frm,EncryptionCheckbox),0);

	Char * name=NULL;

	FrmSetFocus(frm,FrmGetObjectIndex(frm,OpenFileFileField));

	//if the user clicked save, return the selected name

	if(FrmDoDialog(frm)==OpenFileButton)
	{
		FieldPtr text_field=(FieldPtr)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,OpenFileFileField));
		Char *dest = FldGetTextPtr(text_field);
		if (dest != NULL)
		{
			if(valid_file_name(dest))
			{
				name=(Char*)MemPtrNew(StrLen(dest)+1);
				StrCopy(name, dest);
			}
			else
			{

				dest=NULL;
			}
		}
	}
	if(FrmGetControlValue(frm,FrmGetObjectIndex(frm,EncryptionCheckbox))==1)
		save_options|=ENCRYPTED;
	else
		save_options&=(!ENCRYPTED);
	FrmDeleteForm(frm);

	return name;
}

UInt16 SiFileDialog::show_open_form(const Char * initial_dir,UInt16 vol)
{

	update_dir_display=true;
	FormPtr frm = FrmInitForm(ResFileDialog);

	initialise_list(initial_dir,vol);
	FrmSetFocus(frm, FrmGetObjectIndex(frm, OpenFileFileField));
	//turn the generic file dialog into an open dialog
	FrmSetTitle(frm,OPEN);

	//set_dir_field(curr_node);

	UInt16 ret=FrmDoDialog(frm);
	FrmDeleteForm(frm);
	return ret;

}
void SiFileDialog::show_manage_buttons()
{
	FormPtr frm=FrmGetFormPtr(ResFileDialog);
	FrmShowObject(frm,FrmGetObjectIndex(frm,RenameFileButton));
	FrmShowObject(frm,FrmGetObjectIndex(frm,DeleteFileButton));
}

SiFile * SiFileDialog::ShowFileManageDialog(Char * location,UInt16 vol)
{
	dialog_mode=MANAGE_FILE;
	FormPtr frm = FrmInitForm(ResFileDialog);

	FrmSetTitle(frm,MANAGE_FILES);
	initialise_list(location,vol);
	show_buttons();
	//turn the generic file dialog into a manage file dialog

	ControlType * open_button=(ControlType*)FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,CancelFileButton));
	CtlSetLabel(open_button,CLOSE);
	show_manage_buttons();


	//if AESLib is installed, show the decrypt button
	//	if(SiUtility::encryption_support())
	//FrmShowObject(frm,FrmGetObjectIndex(frm,DecryptButton));

	//set_dir_field(curr_node);

	UInt16 ret_button=FrmDoDialog(frm);
	FrmDeleteForm(frm);
	SiFile * t_file=NULL;
	if(ret_button==OpenFileButton&&file_selected!=noListSelection)
	{
		t_file=curr_node->child_nodes[file_selected]->get_file(NULL);
		set_working_dir(t_file);

	}
	tidy();

	return t_file;

}

SiFile * SiFileDialog::ShowOpenFileDialog(const char* initial_dir,UInt16 vol)
{

	dialog_mode=OPEN_FILE;
	UInt16 ret_button;

	SiFile * t_file=NULL;

	//display the dialog using the passed in dir or the previous files's dir if it exists
	if(initial_dir!=NULL)
		ret_button = show_open_form(initial_dir,vol);
	else
		ret_button=show_open_form(working_dir,working_vol);

	if(ret_button==OpenFileButton&&file_selected!=noListSelection)
	{
		t_file=curr_node->child_nodes[file_selected]->get_file(NULL);
		set_working_dir(t_file);

	}
	tidy();

	return t_file;
}

void SiFileDialog::set_working_dir(SiFile * t_file)
{
	if(working_dir!=NULL)
	{
		MemPtrFree(working_dir);
		working_dir=NULL;
	}
	if(t_file!=NULL)
	{
		working_dir=(Char*)MemPtrNew(StrLen(t_file->get_location())+1);
		StrCopy(working_dir,t_file->get_location());
		working_vol=t_file->get_vol_ref();
	}
}

SiFile * SiFileDialog::ShowSaveFileDialog(const char * initial_filename,const char* initial_dir,const UInt32 size,UInt16 vol)
{
#ifdef TEST_OBJECTS_LOG
	log_entry("SiFileDialog::ShowSaveFileDialog");
#endif

	dialog_mode=SAVE_FILE;
	//actually display the save dialog
	Char * filename;
	m_initial_filename=initial_filename;
	if(initial_dir!=NULL)
		filename=show_save_form(initial_dir,vol);
	else
		filename=show_save_form(working_dir,working_vol);
	m_initial_filename=NULL;
	SiFile * t_file=NULL;
	if(filename!=NULL)
	{
		t_file=curr_node->get_file(filename);
		if(t_file!=NULL)
		{
			UInt16 ret_button=FrmAlert(OverWriteFileAlert);
			if(ret_button==OverWriteNo)
			{
				MemPtrFree(filename);
				tidy();
				delete t_file;

				return NULL;
			}
			t_file->delete_file();
			delete t_file;
		}
		t_file=curr_node->create_file(filename,size);
		if((save_options&ENCRYPTED)&&(t_file!=NULL))
		{
			SiFilePlugin * plug=new encrypt_SiFilePlugin(t_file);
			t_file->add_plugin(plug,AT_END);
		}
		set_working_dir(t_file);
		MemPtrFree(filename);

	}
	tidy();
	return t_file;
}

SiDirNode* SiFileDialog::get_node(const Char * initial_dir,UInt16 vol)
{

	SiDirNode *ret_node=parse_path(initial_dir,vol);

	if(ret_node==NULL)
	{
		ret_node=new root_SiDirNode();
		ret_node->populate();
	}

	ret_node->sort_children();
	return ret_node;
}

Char * SiFileDialog::get_next_name(const Char * location,Int16 & start_pos,const Int16 len)
{
	Int16 last_pos=start_pos;
	Char * ret_name=NULL;
	for(Int16 pos=start_pos;pos<len;++pos)
	{
		if(location[pos]=='/')
		{
			last_pos=pos-1;
			break;
		}
	}

	if(last_pos>start_pos)
	{
		ret_name=(Char *)MemPtrNew(last_pos-start_pos+2);
		MemMove(ret_name,location+start_pos,last_pos-start_pos+1);
		ret_name[last_pos-start_pos+1]='\0';
	}
	start_pos=last_pos+2;
	return ret_name;
}

SiDirNode * SiFileDialog::parse_path(const Char * path,UInt16 vol)
{
#ifdef TEST_OBJECTS_LOG
	log_entry("SiFileDialog::parse_path");
#endif

	if(path==NULL||vol==NON_VFS)
	{

		return NULL;
	}

	SiDirNode * temp_node;
	SiDirNode * ret_node=NULL;


	Int16 len=StrLen(path);


	temp_node=ret_node=new root_SiDirNode();

	ret_node->populate();
	ret_node=ret_node->get_child(vol)->get_copy();
	temp_node->flush_children();
	Char * curr_name;
	Int16 pos=1;
	while(ret_node!=NULL)
	{
		ret_node->populate();
		curr_name=get_next_name(path,pos,len);
		if(curr_name==NULL)
			break;
		temp_node=ret_node->get_child(curr_name)->get_copy();
		ret_node->flush_children();
		ret_node=temp_node;
		MemPtrFree(curr_name);
	}

	return ret_node;
}

Boolean SiFileDialog::valid_file_name(const char *name)
{
	if (name == NULL)
		return false;
	UInt16 length = StrLen(name);
	UInt16 offset=0;
	UInt16 change=0;
	WChar the_char;
	while(offset<length)
	{
		change=TxtGetNextChar(name,offset,&the_char);
		if(change>1)
			return false;
		else if(the_char=='\\'||the_char=='/'||the_char=='*'||the_char=='?')
			return false;
		else
			offset+=change;
	}
	return true;
}


static Err WriteProc(const void *dataP, UInt32 *sizeP, void *userDataP)
{
	Err err;

	*sizeP = ExgSend((ExgSocketPtr) userDataP, dataP, *sizeP, &err);
	return err;
}

/*
 * Beam SiEd files, and SiEd application
 * Author: Salomon Ramirez <yolasalo@yahoo.com>
 */
void SiFileDialog::Beam(const UInt16 menuItemId)
{
	ExgSocketType exgSocket;
	UInt16 cardNo;
	LocalID dbID;
	DmSearchStateType searchState;
	Err err;
	Char fileToBeam[80];
	Boolean send = false;

	/* -----------------28/11/2003 01:46p.---------------
	 * Important to init structure to zeros...
	 * --------------------------------------------------*/

	MemSet(&exgSocket, sizeof(exgSocket), 0);

	/* -----------------28/11/2003 01:41p.---------------
	 * Send SiEd application
	 * --------------------------------------------------*/

	if (menuItemId == BeamMenuSiEd)
	{
		err = DmGetNextDatabaseByTypeCreator(true, &searchState, sysFileTApplication,
											 AppID, true, &cardNo, &dbID);

		if (!err)
		{
			exgSocket.description = "SiEd Editor";
			exgSocket.name = "SiEd.prc";
			send = true;
		}
	}
	else if (file_selected != noListSelection && menuItemId == BeamMenuFile)
	{
		if(curr_node->child_nodes[file_selected]->options&CAN_BEAM_FILE)
		{
			StrPrintF(fileToBeam, "%s.pdb", curr_node->child_nodes[file_selected]->name);
			exgSocket.name = fileToBeam;
			exgSocket.description = curr_node->child_nodes[file_selected]->name;
			dbID = curr_node->child_nodes[file_selected]->dbID;
			cardNo = curr_node->child_nodes[file_selected]->cardNo;
			send = true;
		}
	}

	if (send)
	{
		err = ExgPut(&exgSocket);
		if (err == 0)
		{
			ExgDBWrite(WriteProc, &exgSocket, exgSocket.description, dbID, cardNo);
			ExgDisconnect(&exgSocket,err);
		}
	}
}


#ifdef TEST_OBJECT_UNIT
void SiFileDialog::perform_tests()
{
	return;

	SiDirNode * root=new root_SiDirNode();
	SiDirNode * child;
	Char buff[10];

	for(Int16 i=0;i<10;++i)
	{
		StrPrintF(buff,"n:%i",9-i);

		child=new doc_SiDirNode(buff);
		root->add_child(child);
	}
	ErrFatalDisplayIf(root->number_children!=10,"Incorrect number of children, root_SiDirNode");

	root->sort_children();
	for(Int16 i=0;i<10;++i)
	{
		StrPrintF(buff,"n:%i",i);
		child=root->get_child(i);
#ifdef DEBUG_LOG

		log_entry("SiFileDialog::Expected:");

		log_entry(buff);
		log_entry("SiFileDialog::Actual:");
#endif

		if(child->name!=NULL)
		{

#ifdef DEBUG_LOG
			log_entry(child->name);
#endif

			ErrFatalDisplayIf(StrCompare(child->name,buff)!=0,"Incorrect sorted order");
		}

	}

}

#endif
