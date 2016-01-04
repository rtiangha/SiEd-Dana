/*
 * SiEd - A text editor for PalmOS 3.5+ Copyright (C) 2003 Benjamin Roe
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
#include "error.h"
void DisplayError(const UInt16 type, const char *data)
{
	switch (type)
	{
	case FILE_NOT_FOUND:
		FrmCustomAlert(FileNotFoundAlert, data, NULL, NULL);
		break;
	case UNABLE_TO_SAVE:
		FrmCustomAlert(UnableToSaveAlert, data, NULL, NULL);
		break;
	case FILE_ACCESS_ERROR:
		FrmCustomAlert(FileAccessAlert, data, NULL, NULL);
		break;
	case OUT_OF_MEMORY:
		FrmCustomAlert(MemoryAlert, data, NULL, NULL);
		break;
	case NOT_SUPPORTED:
		FrmCustomAlert(NotSupportedAlert, data, NULL, NULL);
		break;
	case DATABASE_ERROR:
		FrmCustomAlert(DatabaseAlert, data, NULL, NULL);
		break;
	case SCREEN_ERROR:
		FrmAlert(ScreenAlert);
		break;
	case NOT_IMPLEMENTED:
		FrmAlert(NotImplemented);
		break;
	case CLIPBOARD_TRUNCATION:
		FrmAlert(ClipTrunc);
		break;
	case UNABLE_TO_LOAD_LIB:
		FrmCustomAlert(DatabaseAlert,data,NULL,NULL);
		break;
	case NOT_ENCRYPTED:
		FrmCustomAlert(FileNotEncAlert,data,NULL,NULL);
		break;
	case UNABLE_TO_CREATE_DIR:
		FrmCustomAlert(UnableCreateDirAlert,data,NULL,NULL);
		break;
	case NO_COMPRESSED_SUPPORT:
		FrmCustomAlert(NoCompressedDocAlert,data,NULL,NULL);
		break;
	case FILE_TRUNCATION_ERROR:
		FrmAlert(FileTruncationAlert);
		break;
	case DEBUG_MESSAGE:
		FrmCustomAlert(DebugAlert,data,NULL,NULL);
		break;
	case INVALID_FILE_NAME:
		FrmCustomAlert(InvalidFileName,data,NULL,NULL);
		break;
	default:
		break;
	}
}


#ifdef DEBUG_LOG
void log_entry(const char * buff)
{
	if(HostGetHostID()!=hostIDPalmOSEmulator)
		return;
	HostFILE* hf=HostFOpen("log_file","a");
	ErrFatalDisplayIf(hf==NULL,"Unable to open log");
	HostFPrintF(hf,buff);
	HostFPrintF(hf,"\n");
	HostFClose(hf);
}

void log_entry_number(const char * buff,const Int32 num)
{
	Char* new_buff=(Char*)MemPtrNew(StrLen(buff)+20);
	StrCopy(new_buff,buff);
	StrPrintF(new_buff+StrLen(buff),"%li",num);
	log_entry(new_buff);
	MemPtrFree(new_buff);
}
#endif

