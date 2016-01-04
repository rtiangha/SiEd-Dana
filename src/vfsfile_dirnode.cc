#include "vfsfile_dirnode.h"
#include "vfs_file_interface.h"
#include "error.h"
vfsfile_SiDirNode::vfsfile_SiDirNode(Char * file_name):vfs_SiDirNode(file_name)
{
	options=CREATE_DIRECTORIES|DISPLAY_FILENAME;
	select_action=ACTION_SELECT_FILE;

}
SiFile * vfsfile_SiDirNode::get_file(Char * filename)
{
	Char * path=get_path(STANDARD);
	vfs_SiFile * t_file=new vfs_SiFile(name,path,volRefNum);
	MemPtrFree(path);
	if(t_file->is_open())
	{
		return t_file;
	}
	else
	{
		delete t_file;
		return NULL;
	}
}
Int16 vfsfile_SiDirNode::get_position_index() const
{
	return VFSFILE_POSITION;
}
SiDirNode * vfsfile_SiDirNode::get_copy()
{
	SiDirNode * ret_node=new vfsfile_SiDirNode(name);
	ret_node->volRefNum=volRefNum;
	ret_node->parent=parent;
	return ret_node;

}
