#include "vfs_dirnode.h"
#include "vfsdir_dirnode.h"
#include "VFSMgr.h"
#include "updir_dirnode.h"
#include "vfsfile_dirnode.h"
#include "vfs_file_interface.h"
#include "resources.h"
#include "error.h"
vfs_SiDirNode::vfs_SiDirNode(Char * filename):SiDirNode(filename,0)
{
	file_size=file_date=0;
}

SiFile * vfs_SiDirNode::get_file(Char * filename)
{
	Char * path=get_path(STANDARD);
	Char * full_path=(Char*)MemPtrNew(StrLen(path)+StrLen(filename)+1);
	StrCopy(full_path,path);
	StrCopy(full_path+StrLen(path),filename);
	SiFile *t_file=new vfs_SiFile(filename,full_path,volRefNum);
	if(!t_file->is_open())
	{
		delete t_file;
		t_file=NULL;
	}
	MemPtrFree(full_path);
	MemPtrFree(path);

	return t_file;

}

SiFile *vfs_SiDirNode::create_file(Char * filename,UInt32 size)
{
	Char * path=get_path(STANDARD);
	Char * full_path=(Char*)MemPtrNew(StrLen(path)+StrLen(filename)+1);
	StrCopy(full_path,path);
	StrCopy(full_path+StrLen(path),filename);
	SiFile *t_file=new vfs_SiFile(filename,full_path,volRefNum,size);
	MemPtrFree(full_path);
	MemPtrFree(path);
	return t_file;
}
UInt32 vfs_SiDirNode::get_file_size()
{
	if(file_size==0)
	{
		Char * path=get_path(STANDARD);
		FileRef ref;
		VFSFileOpen(volRefNum,path,vfsModeRead,&ref);
		if(ref!=NULL)
		{
			VFSFileSize(ref,&file_size);
			VFSFileClose(ref);
		}
		MemPtrFree(path);
	}
	return file_size;
}
UInt32 vfs_SiDirNode::get_file_date()
{
	if(file_date==0)
	{
		Char * path=get_path(STANDARD);
		FileRef ref;
		VFSFileOpen(volRefNum,path,vfsModeRead,&ref);

		if(ref!=NULL)
		{
			Err err=VFSFileGetDate(ref,vfsFileDateModified,&file_date);

			if(err!=errNone)
			{
				err=VFSFileGetDate(ref,vfsFileDateCreated,&file_date);
			}
			VFSFileClose(ref);
		}

		MemPtrFree(path);
	}
	return file_date;
}

void vfs_SiDirNode::populate()
{
	//clear the current list
	flush_children();

	Err err;

	FileRef dirRef=0;

	//prepare the new parent node
	//and the child nodes
	SiDirNode * child =new updir_SiDirNode();
	child->volRefNum=volRefNum;

	//add up directory
	add_child(child);
	//MemPtrFree(child);

	//open the directory for reading
	Char * path_name=get_path(STANDARD);
	err = VFSFileOpen(volRefNum, path_name, vfsModeRead, &dirRef);
	MemPtrFree(path_name);
	Char * temp_name=NULL;
	if(err == errNone)
	{
		// Iterate through all the files in the open directory
		UInt32 fileIterator=expIteratorStart;
		FileInfoType fileInfo;
		fileInfo.nameBufLen = 256;
		temp_name = (Char*)MemPtrNew(256);   // should check for err
		fileInfo.nameP = temp_name;    // point to local buffer
		while (fileIterator != expIteratorStop)
		{
			// Get the next file
			err = VFSDirEntryEnumerate(dirRef, &fileIterator,&fileInfo);

			if(err == errNone)
			{
				// Process the file here.
				if(fileInfo.attributes&vfsFileAttrDirectory)
				{
					child=new vfsdir_SiDirNode(temp_name);
				}
				else
				{
					child=new vfsfile_SiDirNode(temp_name);
				}
				child->volRefNum=volRefNum;
				add_child(child);
				//MemPtrFree(child);
			}
			else
				break;

		}
		MemPtrFree(temp_name);
	}

	VFSFileClose(dirRef);
}
