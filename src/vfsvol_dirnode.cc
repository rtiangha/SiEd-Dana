#include "vfsvol_dirnode.h"
#include "error.h"

vfsvol_SiDirNode::vfsvol_SiDirNode(Char * filename):vfs_SiDirNode(filename)
{
	image_id=SDCARD_BITMAP;
	x_offset=ICON_WIDTH;
	select_action=ACTION_CHANGE_DIR;
	options=TOP_LEVEL|CREATE_DIRECTORIES;
	m_font=boldFont;
}
SiDirNode * vfsvol_SiDirNode::get_copy()
{
	SiDirNode * ret_node=new vfsvol_SiDirNode(name);
	ret_node->volRefNum=volRefNum;
	ret_node->parent=parent;
	return ret_node;

}

Int16 vfsvol_SiDirNode::get_position_index()
{
	return VFSVOL_POSITION;
}
