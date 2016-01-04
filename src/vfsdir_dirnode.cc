#include "vfsdir_dirnode.h"
#include "VFSMgr.h"
#include "updir_dirnode.h"
#include "vfsfile_dirnode.h"
#include "vfs_file_interface.h"
#include "resources.h"
#include "error.h"

vfsdir_SiDirNode::vfsdir_SiDirNode(Char * filename): vfs_SiDirNode(filename)
{
	image_id=FOLDER_BITMAP;
	x_offset=ICON_WIDTH;
	select_action=ACTION_CHANGE_DIR;
	options=CREATE_DIRECTORIES;
}

SiDirNode * vfsdir_SiDirNode::get_copy()
{
	SiDirNode * ret_node=new vfsdir_SiDirNode(name);
	ret_node->volRefNum=volRefNum;
	ret_node->parent=parent;
	return ret_node;
}

Int16 vfsdir_SiDirNode::get_position_index() const
{
	return VFSDIR_POSITION;
}
