#include <PalmOS.h>
#include <PalmTypes.h>
#include "sections.h"
#include "edit_interface.h"

// Include AlphaSmart libraries
#include <WideTallApp.h>
#include <WideTallAppChars.h>
#include <Screen.h>


extern SiEditInterface* edit_interface;
#ifndef MAIN_HPP
#define MAIN_HPP


UInt16 StartApplication() __attribute__ ((section ("filefns")));
void EventLoop();
void StopApplication() ;

Boolean texteditloop(EventPtr event);


UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags);

Boolean texteditloop(EventPtr event);
class test
{
public:
	test();
private:
	Int16 m_test;
};

class doc_non_virt_SiFile
{
public:
	doc_non_virt_SiFile(Char * name,const UInt32 size) ;
	~doc_non_virt_SiFile() ;
	void write_data(Char * data,const UInt32 size) ;
	Char * m_name;
	LocalID m_id;
private:

	Boolean create_database() ;
	void unlock() ;
	void close() ;
	Boolean resize(const UInt32 size) ;
	void open() ;

	Boolean create_database(const UInt32 size) ;
	Boolean add_record(const UInt16 size) ;
	Boolean add_records(const UInt32 size) ;
	void startup() ;
	void lock(UInt16 rec)
		;
	void lock_next_record() ;
	Boolean write_header() ;
	void ensure_unique_name();
	DmOpenRef m_open_ref;
	MemHandle m_curr_handle;
	Char * m_curr_locked;
	Boolean m_changed;

	UInt16 curr_record_index;
	UInt16 num_records;
	UInt32 m_file_size;
	UInt32 m_cursor;
	UInt16 curr_record_cursor;
	UInt16 curr_record_size;


	Boolean m_file_open;
};

#endif
