
#include "test.h"
#include "texthandler.h"
#include "sidocument.h"
#include "editor.h"
#include "filehandler.h"
#include "memhandler.h"
#include "textview.h"
#include "main.h"
#include "error.h"
#include "sections.h"
#include "vfsdir_dirnode.h"
#include "filedialog.h"
#include "siscrollbar.h"
#ifdef TEST_OBJECT_UNIT
void perform_object_tests()
{
SiEditor * edit=new SiEditor(160,0,1);

	edit->give_focus();
	edit->perform_tests();
	delete edit;
	block b;
	b.perform_tests();
	Position p;
	p.perform_tests();
	DrawPosition dp;
	dp.perform_tests();
	block_buffer bb;
	bb.perform_tests();

	SiDirNode * node=new vfsdir_SiDirNode("Test");
	node->perform_tests();
	delete node;
	SiFileDialog::perform_tests();
	SiMemHandler::perform_tests();
	


	DisplayError(DEBUG_MESSAGE,"Tests Complete");
}
#endif
#ifdef BENCH_OBJECTS
UInt32 memory_reallocated=0;
void display_statistics()
{
	Char buff[512];
	StrPrintF(buff,"Benchmark complete. Memory Reallocated = %lu",memory_reallocated);
	DisplayError(DEBUG_MESSAGE,buff);
}
void perform_benchmarks()
{
	reset_statistics();
	SiEditor * edit=new SiEditor(160,0,1);
	edit->set_scrollbar(ScrollBarMain);
	edit->give_focus();

	edit->perform_benchmark();
	display_statistics();
	delete edit;
}
void reset_statistics()
{
	memory_reallocated=0;
}
void add_reallocation(UInt32 size)
{
	memory_reallocated+=size;
}
#endif
#ifdef PERFORM_TEST
//Text for scripted editing test
//(from Penny-arcade.com, if you were wondering)
Char *test_data={"Did anybody else catch the first episode of Real World: Zebes? With fully seven distinct species and four different sexual orientations, it's the most diverse Real World yet. It makes the bridge of the Enterprise look like a KKK rally.\n\"Four different sexual orientations?\" you exclaim. \"Preposterous! There are only two thingies, and their interactions are well known.\" "};
WChar  edit_data[21]={chrBackspace,chrLeftArrow,chrLeftArrow,chrDelete,chrDelete,chrPageUp,chrDelete,chrLeftArrow,chrPageDown,chrLeftArrow,chrLeftArrow,chrDelete,chrDelete,chrBackspace,chrDelete,chrPageUp,chrDownArrow,chrDownArrow,chrDownArrow,chrDownArrow,'\0'};

void perform_test()
{
	UInt32 test_length=StrLen(test_data);

	for(Int16 t=0;t<240;++t)
	{
		for(UInt32 i=0;i<test_length;++i)
		{
			edit_interface->the_editor_active->handle_key(test_data[i],0);

		}
		for(UInt32 j=0;j<1;++j)
			for(UInt32 i=0;i<20;++i)
				edit_interface->the_editor_active->handle_key(edit_data[i],0);
	}
}
#endif

