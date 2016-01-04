#include "doc_dirnode.h"

#include "doc_file_interface.h"
#include "error.h"
doc_SiDirNode::doc_SiDirNode(Char * filename):pdb_SiDirNode(filename)
{
#ifdef EN_LOG
	log_entry("doc_SiDirNode ctor");
#endif

	select_action=ACTION_SELECT_FILE;
	options=DISPLAY_FILENAME|CAN_BEAM_FILE;

}

SiFile * doc_SiDirNode::get_file(Char * filename)
{
	return new doc_SiFile(name,name,cardNo);
}
Int16 doc_SiDirNode::get_position_index() const
{
#ifdef EN_LOG
	log_entry("doc_SiDirNode::get_position_index");
#endif

	return DOC_POSITION;
}
SiDirNode * doc_SiDirNode::get_copy()
{
	SiDirNode * ret_node=new doc_SiDirNode(name);
	ret_node->volRefNum=volRefNum;
	ret_node->parent=parent;
	return ret_node;

}
