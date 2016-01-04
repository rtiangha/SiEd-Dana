#include "dirnode.h"
#include "error.h"
#include "file_interface.h"
#include "file_plugin.h"
//comparison functions for the quick sort of the dirnodes
Boolean compare_less(const SiDirNode * lhs,const SiDirNode * rhs)
{

	if(lhs->get_position_index()<rhs->get_position_index())
		return true;
	else if(lhs->get_position_index()>rhs->get_position_index())
		return false;
	else if(lhs->name==NULL)
	  return false;
	else if(rhs->name==NULL)
	  return true;
	else if(StrCompare(lhs->name,rhs->name)<0)
		return true;
	else
		return false;
}

Boolean compare_less_equal(const SiDirNode * lhs,const SiDirNode * rhs)
{
	if(lhs->get_position_index()<rhs->get_position_index())
	{
		return true;
	}
	else if(lhs->get_position_index()==rhs->get_position_index())
	{
	  if(lhs->name==NULL)
	    return false;
	  else if(rhs->name==NULL)
	    return true;
	  else
		return (StrCompare(lhs->name,rhs->name)<=0);
	}
	else
	{
		return false;
	}
}
/*SiDirNode & SiDirNode::operator =(SiDirNode & rhs)
{
	cardNo=rhs.cardNo;
	child_nodes=rhs.child_nodes;
	dbID=rhs.dbID;
	children_handle=rhs.children_handle;
	file_size=rhs.file_size;
	image_id=rhs.file_size;
	options=rhs.options;
	name=rhs.name;
	select_action=rhs.select_action;
	x_offset=rhs.x_offset;
	number_children=rhs.number_children;
	parent=rhs.parent;
	volRefNum=rhs.volRefNum;
 
}*/
SiDirNode::SiDirNode(Char * p_name,const UInt16 p_number_children)
{
	parent=NULL;
	m_font=stdFont;
	m_is_encrypted=NONE;
	if(p_name!=NULL)
	{
		name=(Char*)MemPtrNew(StrLen(p_name)+1);
		StrCopy(name,p_name);
	}
	else
		name=NULL;

	number_children=p_number_children;
	image_id=NULL;
	x_offset=0;
	if(number_children>0)
	{
		children_handle=MemHandle(number_children*sizeof(SiDirNode*));
		child_nodes=(SiDirNode**)MemHandleLock(children_handle);
	}
	else
	{
		children_handle=NULL;
		child_nodes=NULL;
	}

	options=0;
	file_size=0;
	file_date=0;
}

Boolean SiDirNode::is_encrypted()
{
	if(!(options&DISPLAY_FILENAME)||name==NULL)
		return false;

	if(m_is_encrypted==NONE)
	{
		SiFile::display_warnings=false;
		SiFile * file=get_file(NULL);
		if(file!=NULL)
		{
			SiFilePlugin * plug=file->is_encrypted();
			if(plug!=NULL)
			{
				m_is_encrypted=true;
				delete plug;
			}
			else
				m_is_encrypted=false;
			delete file;
			SiFile::display_warnings=true;
			return m_is_encrypted;
		}
		else
			return false;
	}
	else
	{
		return m_is_encrypted;
	}
}
SiFile * SiDirNode::get_file(Char * filename)
{
#ifdef DEBUG
	filename=NULL;
#endif

	return NULL;
}
SiFile * SiDirNode::create_file(Char * filename,UInt32 size)
{
#ifdef DEBUG
	filename=NULL;
	size=0;
#endif

	return NULL;
}
void SiDirNode::flush_children()
{
	if(number_children>0)
	{
		for(UInt16 i=0;i<number_children;++i)
		{
			delete child_nodes[i];
			//child_nodes[i]->tidy();
		}
		number_children=0;
		MemHandleUnlock(children_handle);
		MemHandleFree(children_handle);
		children_handle=NULL;
		child_nodes=NULL;
	}
}

/*SiDirNode::SiDirNode(SiDirNode* d)
{
	name=(Char*)MemPtrNew(StrLen(d->name)+1);
	StrCopy(name,d->name);
 
	number_children=d->number_children;
	if(number_children>0)
	{
		children_handle=MemHandle(number_children*sizeof(SiDirNode));
		child_nodes=(SiDirNode*)MemHandleLock(children_handle);
	}
	else
	{
		children_handle=NULL;
		child_nodes=NULL;
	}
	x_offset=0;
	image_id=0;
	options=d->options;
}*/

UInt32 SiDirNode::get_file_size()
{
	return 0;
}
UInt32 SiDirNode::get_file_date()
{
	return 0;
}
SiDirNode * SiDirNode::get_child(UInt16 vol)
{
	for(UInt16 i=0;i<number_children;++i)
	{
		if(child_nodes[i]->volRefNum==vol)
			return child_nodes[i];
	}
	return NULL;
}
SiDirNode * SiDirNode::get_child(Char * file_name)
{
	for(UInt16 i=0;i<number_children;++i)
	{
		if(StrCompare(file_name,child_nodes[i]->name)==0)
			return child_nodes[i];
	}
	return NULL;
}

SiDirNode::SiDirNode()
{
	children_handle=NULL;
	child_nodes=NULL;
	number_children=0;
	name=NULL;
	x_offset=0;
	image_id=0;

}
SiDirNode * SiDirNode::get_copy()
{
#ifdef EN_LOG
	DisplayError(UNABLE_TO_SAVE,"Called wrong copy");
#endif

	return NULL;
}
void SiDirNode::tidy()
{
	if(name!=NULL)
	{
		MemPtrFree(name);
		name=NULL;
	}

	if(child_nodes!=NULL&&number_children>0)
	{
		for(UInt16 i=0;i<number_children;++i)
		{
			delete child_nodes[i];
		}
		MemHandleUnlock(children_handle);
		child_nodes=NULL;
		number_children=0;
	}

	if(children_handle!=NULL)
	{
		MemHandleFree(children_handle);
		children_handle=NULL;
	}
}

SiDirNode::~SiDirNode()
{
	tidy();
}

//the position index is the position in the sort order
//ie directories go before files
Int16 SiDirNode::get_position_index() const
{
#ifdef EN_LOG
	log_entry("SiDirNode::get_position_index");
#endif

	return 0;
}

//add a child node to this node (eg a file in a directory node)
SiDirNode *SiDirNode::add_child(SiDirNode * child)
{

	child->parent=this;
	if(child_nodes!=NULL)
	{
		MemHandleUnlock(children_handle);
		MemHandleResize(children_handle,MemHandleSize(children_handle)+sizeof(SiDirNode*));
	}
	else
	{
		children_handle=MemHandleNew(sizeof(SiDirNode*));
	}

	child_nodes=(SiDirNode**)MemHandleLock(children_handle);
	child_nodes[number_children]=child;

	++number_children;

	return child_nodes[number_children-1];
}

void SiDirNode::populate()
{
#ifdef EN_LOG
	ErrFatalDisplay("Called the wrong thing");
#endif
}

#define PADLOCK_BITMAP_WIDTH 8
void SiDirNode::draw_self(UInt16 x,const UInt16 y,const UInt16 max_width)
{
	MemHandle hRsc=NULL;
	BitmapType* bitmapP=NULL;

	if(is_encrypted())
	{
		hRsc = DmGetResource(bitmapRsc,PADLOCK_IMAGE_ID);
		if(hRsc!=NULL)
		{
			bitmapP = (BitmapType*) MemHandleLock(hRsc);
			if(bitmapP!=NULL)
			{
				WinDrawBitmap(bitmapP,3,y+2);
				x+=PADLOCK_BITMAP_WIDTH+3;
				MemHandleUnlock(hRsc);
			}
			DmReleaseResource(hRsc);
		}
		hRsc=NULL;
		bitmapP=NULL;
	}

	if(image_id!=0)
	{
		hRsc = DmGetResource(bitmapRsc,image_id);
		if(hRsc!=NULL)
		{
			bitmapP = (BitmapType*) MemHandleLock(hRsc);
			if(bitmapP!=NULL)
			{
				WinDrawBitmap(bitmapP,x+2,y+1);
				MemHandleUnlock(hRsc);
			}
			DmReleaseResource(hRsc);
		}
	}

	if(name!=NULL)
	{
		FontID prev_font=FntSetFont(m_font);
		WinDrawTruncChars(name,StrLen(name), x+x_offset,y,max_width-x_offset-1-x);
		FntSetFont(prev_font);
	}


}

//get the filesystem path for this node
//type argument specifies which parts to include (volume name, filename etc)
Char * SiDirNode::get_path(UInt16 type)
{
	//move up the node tree and build up the path of the argument node
	//as a string and return it
	SiDirNode * this_node=this;
	SiDirNode** node_list=(SiDirNode**)MemPtrNew(sizeof(SiDirNode*)*128);
	Int16 node_count=0;
	Int16 path_length=1;
	if(type&EXCLUDE_LEAF_NODE&&this_node->select_action==ACTION_SELECT_FILE)
		this_node=this_node->parent;
	while(this_node!=NULL&&node_count<128)
	{
		if(this_node->options&TOP_LEVEL&&(!(type&INCLUDE_CARD)))
			break;
		if(this_node->name!=NULL)
		{
			node_list[node_count]=this_node;
			path_length+=StrLen(this_node->name)+1;
			++node_count;
		}
		else
			break;

		this_node=this_node->parent;
	}

	Char * ret_buff=(Char*)MemPtrNew(path_length+1);

	ret_buff[0]='/';
	Int16 pos=1;
	for(Int16 i=node_count-1;i>=0;--i)
	{
		if(node_list[i]->name!=NULL)
		{
			StrCopy(ret_buff+pos,node_list[i]->name);
			pos+=StrLen(node_list[i]->name);
			if(node_list[i]->select_action==ACTION_CHANGE_DIR)
			{
				ret_buff[pos]='/';
				++pos;
			}
		}
	}
	ret_buff[pos]='\0';
	MemPtrResize(ret_buff,StrLen(ret_buff)+1);
	MemPtrFree(node_list);
	return ret_buff;
}

//quick sort functions
void SiDirNode::swap_elements(SiDirNode ** partition,Int32 el_1,Int32 el_2)
{
#ifdef EN_LOG
	log_entry_number("Swapping elements ",el_1);
	log_entry_number("and ",el_2);
#endif

	SiDirNode  *temp_node=partition[el_1];
	partition[el_1]=partition[el_2];
	partition[el_2]=temp_node;

}

void SiDirNode::sort_partition(SiDirNode** partition,const UInt16 begin,const UInt16 end)
{
#ifdef EN_LOG
	log_entry_number("Sorting partition size ",size);
#endif

	if(end>begin)
	{
		UInt16 l=begin+1;
		UInt16 r=end;

		SiDirNode *pivot_node=partition[begin]->get_copy();
		while(l<=r)
		{
			if(compare_less_equal(partition[l],pivot_node))
			{
				++l;
			}
			else
			{
				swap_elements(partition,l,r);
				--r;
			}
		}

		//delete the node copy as soon as possible
		//to save memory
		delete pivot_node;
		--l;
		++r;
		swap_elements(partition,begin,l);

		//recursively sort left+right partitions
		sort_partition(partition,begin,l);
		sort_partition(partition,r,end);

	}

}

void SiDirNode::sort_children()
{
	if(child_nodes!=NULL)
		sort_partition(child_nodes,0,number_children-1);
}

#ifdef TEST_OBJECT_UNIT
Char * test_names[8]={"One","Two","Three","Four","one","two","three","four"};
#include "vfsfile_dirnode.h"
#include "doc_dirnode.h"
#define NUMBER_TEST_CHILDREN 300
void SiDirNode::perform_tests()
{

	SiDirNode * child=NULL;
	UInt16 rem;

	Char buff[50];
	for(UInt16 i=0;i<NUMBER_TEST_CHILDREN;++i)
	{
		rem=(i%8);

		StrCopy(buff,test_names[rem]);
		StrPrintF(buff+StrLen(test_names[rem]),"_%i",i);

		child=new vfsfile_SiDirNode(buff);

		add_child(child);
		ErrFatalDisplayIf(number_children!=i+1,"Incorrect child count, SiDirNode");
	}

	sort_children();
	ErrFatalDisplayIf(number_children!=NUMBER_TEST_CHILDREN,"Incorrect number of children after sort");
	SiDirNode* next_child;

	for(UInt16 i=0;i<NUMBER_TEST_CHILDREN-1;++i)
	{
		child=child_nodes[i];
		next_child=child_nodes[i+1];

		if(StrCompare(child->name,next_child->name)>0)
		{
			DisplayError(DEBUG_MESSAGE,child->name);
			DisplayError(DEBUG_MESSAGE,next_child->name);
			ErrFatalDisplay("Incorrect Sort Order");
		}
	}

	flush_children();

	ErrFatalDisplayIf(number_children!=0,"Incorrect flush_children");
	for(UInt16 i=0;i<NUMBER_TEST_CHILDREN;++i)
	{
		rem=(i%8);

		StrCopy(buff,test_names[rem]);
		StrPrintF(buff+StrLen(test_names[rem]),"_%i",i);

		child=new doc_SiDirNode(buff);

		add_child(child);
		ErrFatalDisplayIf(number_children!=i+1,"Incorrect child count, SiDirNode");
	}

	sort_children();
	ErrFatalDisplayIf(number_children!=NUMBER_TEST_CHILDREN,"Incorrect number of children after sort");


	for(Int16 i=0;i<NUMBER_TEST_CHILDREN-1;++i)
	{
		child=child_nodes[i];
		next_child=child_nodes[i+1];
		ErrFatalDisplayIf(StrCompare(child->name,next_child->name)>0,"Incorrect Sort Order");
	}
	flush_children();
	ErrFatalDisplayIf(number_children!=0,"Incorrect flush");
}
#endif
