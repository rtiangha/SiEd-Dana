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

#include <PalmOS.h>
#include <PalmTypes.h>
#include "utility.h"
#include "editor.h"
#include "error.h"
#include "memhandler.h"
#include "form_pos.h"
#include "main.h"
#include "ExgMgr.h"

#ifdef MACRO
#include "simacrorecorder.h"
#endif
#ifdef DEBUG

#endif
#include "test.h"
Char * g_file_to_launch=NULL;
SiEditInterface* edit_interface=NULL;
Boolean receiving_data_flag=false;
#define CHUNK_SIZE 1024
Int32 get_event_delay()
{

	//return how long the event loop should wait before giving a nilEvent
	//used to do the background sloshing

	if(NULL==edit_interface)
		return evtWaitForever;
	else
		return edit_interface->get_event_delay();
}

Boolean check_palmos_version()
{
	UInt32 romVersion;
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	if (romVersion < sysMakeROMVersion(3, 5, 0,sysROMStageRelease, 0))
	{
		FrmAlert(WrongPalmosVersion);
		return false;
	}
	else
		return true;

}

void register_for_file_types()
{
	//check the ROM version to call the correct function
	//to register for exchange data types

	UInt32 romVersion;
	FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

	if (romVersion >= sysMakeROMVersion(4, 0, 0,sysROMStageRelease, 0))
	{
		//version 4 or greater

		ExgRegisterDatatype(AppID,exgRegTypeID,MIME_TYPE,NULL,0);
		ExgRegisterDatatype(AppID,exgRegExtensionID,FILE_EXTENSIONS,"Plain Text",0);

	}
	else
	{
		ExgRegisterData(AppID,exgRegTypeID,"text/plain");
	}

}

#define MAX_HANDLE_SIZE 32000

void receive_data(ExgSocketPtr  exgSocketP)
{
	Err err;

	UInt32 len=0;
	UInt32 dataLen = 0;

	UInt32 size = exgSocketP->length;

	err = ExgAccept(exgSocketP);
	doc_non_virt_SiFile doc_file((exgSocketP->name),size);
	// accept will open a progress dialog and wait for your receive commands

	if (!err)
	{
		UInt32 handle_size;

		if(size>MAX_HANDLE_SIZE)
			handle_size=MAX_HANDLE_SIZE;
		else
			handle_size=size;

		MemHandle handle=MemHandleNew(handle_size);
		Char * mem_ptr=(Char*)MemHandleLock(handle);
		UInt32 total_done=0;
		UInt32 to_read=0;
		do
		{
			if((size-total_done)<handle_size)
				to_read=size-total_done;
			else
				to_read=handle_size;

			do
			{

				len = ExgReceive(exgSocketP,&(mem_ptr[dataLen]), to_read-dataLen,&err);

				if (len && !err)
				{
					dataLen+=len;
					total_done+=len;
				}
			}
			while (len && !err && total_done< size && to_read > dataLen);


			doc_file.write_data(mem_ptr,dataLen);
			dataLen=0;

		}
		while(total_done<size && !err);

		ExgDisconnect(exgSocketP,err); // closes transfer dialog
		MemHandleUnlock(handle);
		MemHandleFree(handle);
		if (!err)
		{
			exgSocketP->goToCreator = AppID;
			exgSocketP->goToParams.dbID=doc_file.m_id;

		}
	}

	// release memory if an error occured


}

void go_to_file(GoToParamsType *goto_param)
{
	if(NULL==goto_param)
		return;
	if(NULL==goto_param->dbID)
		return;
	g_file_to_launch=(Char*)MemPtrNew(33);
	DmDatabaseInfo(THE_CARD,goto_param->dbID,g_file_to_launch,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

}

UInt16 do_normal_launch()
{
	if(!check_palmos_version())
		return 0;

	UInt16 error = StartApplication();	
	if (error)
		return error;

	EventLoop();		

	StopApplication();
	return 0;
}

UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{

	switch(cmd)
	{
	case sysAppLaunchCmdNormalLaunch:
		register_for_file_types();
		do_normal_launch();

		break;
	case sysAppLaunchCmdSyncNotify:
		register_for_file_types();
		break;
	case sysAppLaunchCmdExgReceiveData:
		receive_data((ExgSocketPtr)cmdPBP);
		break;
	case sysAppLaunchCmdGoTo:
		go_to_file((GoToParamsType*)cmdPBP);
		do_normal_launch();
		break;
	case sysNotifyDisplayResizedEvent:
		if(((SysNotifyParamType*)cmdPBP)->notifyType==sysNotifyDisplayResizedEvent)
		{
			EventType resizedEvent;
			MemSet(&resizedEvent,sizeof(EventType),0);
			resizedEvent.eType=winDisplayChangedEvent;
			EvtAddUniqueEventToQueue(&resizedEvent,0,true);
		}
		break;
	}
	return 0;
}

UInt16 StartApplication()
{
	FrmGotoForm(ResformID_text);
	SiUtility::check_system_capabilities();
	SiPreferencesDatabase::startup();
	SiMemHandler::initialise_memory();

	return 0;
}

Boolean AppHandleEvent(EventType * event)
{


	if (event->eType == frmLoadEvent)
	{
	UInt32 formID = event->data.frmLoad.formID;
		FormPtr form = FrmInitForm(formID);
		FrmSetActiveForm(form);
		edit_interface->set_size_limits(formID);
		switch (formID)
		{
		case ResformID_text:
			FrmSetEventHandler(form,(FormEventHandlerPtr) texteditloop);

			break;
		case AlphaKeyboardDialog:
		case NumKeyboardDialog:
		case IntlKeyboardDialog:
		case CapsAlphaKeyboardDialog:
		case CapsIntlKeyboardDialog:
			FrmSetEventHandler(form,(FormEventHandlerPtr) keyboardEventLoop);
			break;
		default:
			break;
		}

		return true;
	}
	return false;
}
Boolean intercept_event(EventPtr e)
{
	if(edit_interface==NULL)
		return false;
	else
		return edit_interface->intercept_event(e);
}

void EventLoop(void)
{
	UInt16 err;
	EventType event;

	do
	{
		EvtGetEvent(&event, get_event_delay());

		if(!intercept_event(&event))
			if (!SysHandleEvent(&event))
				if (!MenuHandleEvent(NULL, &event, &err))
					if (!AppHandleEvent(&event))
						FrmDispatchEvent(&event);
	}
	while (event.eType != appStopEvent);
}

void StopApplication()
{
#ifdef TEST_OBJECTS_LOG
	log_entry("StopApplication()");
	ErrFatalDisplayIf(edit_interface==NULL,"Null interface");
#endif

	delete edit_interface;
	edit_interface=NULL;
	FrmCloseAllForms();
	SiPreferencesDatabase::shutdown();
	SiMemHandler::exit_memory();
	if(g_file_to_launch!=NULL)
		MemPtrFree(g_file_to_launch);

}

Boolean texteditloop(EventPtr e)
{
	FormPtr form;
	//AlphaSmart Dana
	RectangleType rect;

	switch(e->eType)
	{
	case frmOpenEvent:
		UInt32 version;
		form = FrmGetActiveForm();

                //Check for Dana Screen Extension and if present, grab new Window bounds
                if (_ScreenFeaturePresent (&version) == true)
                {
                        rect.topLeft.x = 0;
                        rect.topLeft.y = 0;
                        WinGetDisplayExtent(&rect.extent.x, &rect.extent.y);
                        WinSetWindowBounds(FrmGetWindowHandle(form), &rect);
                }


		FrmDrawForm(form);
		edit_interface=new SiEditInterface();
		edit_interface->initialise(g_file_to_launch);
#ifdef PERFORM_TEST
		perform_test();
#endif

		break;
	default:
		if(edit_interface!=NULL)
			return edit_interface->handle_event(e);
	}
	return false;
}
