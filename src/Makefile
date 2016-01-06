#
# SiEd-Dana - A text editor optimized for the AlphaSmart Dana.
# Copyright (C) 2016 Reg Tiangha
# Contact:  reg@reginaldtiangha.com
#
# Portions of SiEd-Dana are based off the original SiEd project,
# Copyright (C) 2003-2005 Benjamin Roe
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

#Languages. English is default
VPATH=../build
all:LANG=LANGUAGE_ENGLISH
all: sied-dana.prc

french:LANG=LANGUAGE_FRENCH
french:sied-dana.prc
portuguese:LANG=LANGUAGE_PORTUGUESE
portuguese:sied-dana.prc

norwegian:LANG=LANGUAGE_NORWEGIAN
norwegian:sied-dana.prc

german:LANG=LANGUAGE_GERMAN
german:sied-dana.prc

spanish:LANG=LANGUAGE_SPANISH
spanish:sied-dana.prc
catalan:LANG=LANGUAGE_CATALAN
catalan:sied-dana.prc

italian:LANG=LANGUAGE_ITALIAN
italian:sied-dana.prc

dutch:LANG=LANGUAGE_DUTCH
dutch:sied-dana.prc
turkish:LANG=LANGUAGE_TURKISH
turkish:sied-dana.prc
russian:LANG=LANGUAGE_RUSSIAN
russian:sied-dana.prc
polish:LANG=LANGUAGE_POLISH
polish:sied-dana.prc
swedish:LANG=LANGUAGE_SWEDISH
swedish:sied-dana.prc
czech:LANG=LANGUAGE_CZECH
czech:sied-dana.prc

#Common compiler flags for all builds
COMMON=-fno-exceptions -O2 -fno-rtti  -Wall -W -D$(LANG)  -fmove-all-movables -frerun-loop-opt  -Wmissing-declarations  -DHANDSPRING -DMACRO -DV_KEYBOARD -DUSE_WIDTH_CACHE -DUSE_CHAR_CACHE -pedantic -Wshadow
ifeq ($(BUILD_TYPE),DEBUG)
	CXXFLAGS = $(COMMON) -DDEBUG
else
	ifeq ($(BUILD_TYPE),DEBUG_LOG)
		CXXFLAGS = $(COMMON) -DDEBUG -DDEBUG_LOG
	else
		ifeq ($(BUILD_TYPE),UNIT_TESTS)
			CXXFLAGS= $(COMMON) -DDEBUG -DTEST_OBJECT_UNIT
		else
			CXXFLAGS=$(COMMON)
		endif
	endif
endif


CC=m68k-palmos-g++
OBJ=texthandler.o main.o types.o textview.o resources.o editor.o utility.o filehandler.o  filedialog.o dirnode.o error.o memhandler.o file_interface.o root_dirnode.o vfsfile_dirnode.o updir_dirnode.o vfsdir_dirnode.o pdb_dirnode.o vfs_file_interface.o  file_plugin.o vfsvol_dirnode.o vfs_dirnode.o cache_file_plugin.o encrypt_file_plugin.o sha1.o doc_dirnode.o doc_file_interface.o  pdb_file_interface.o  simacrorecorder.o sikeyboard.o test.o sidocument.o edit_interface.o preferences.o minibuffer.o save_data.o siscrollbar.o div_dirnode.o

BUILD_OBJ=../build/texthandler.o ../build/main.o ../build/types.o ../build/textview.o ../build/resources.o ../build/editor.o ../build/utility.o ../build/filehandler.o  ../build/filedialog.o ../build/dirnode.o ../build/error.o ../build/memhandler.o ../build/file_interface.o ../build/root_dirnode.o ../build/vfsfile_dirnode.o ../build/updir_dirnode.o ../build/vfsdir_dirnode.o ../build/pdb_dirnode.o ../build/vfs_file_interface.o  ../build/file_plugin.o ../build/vfsvol_dirnode.o ../build/vfs_dirnode.o ../build/cache_file_plugin.o ../build/encrypt_file_plugin.o ../build/sha1.o ../build/doc_dirnode.o ../build/doc_file_interface.o  ../build/pdb_file_interface.o  ../build/simacrorecorder.o ../build/sikeyboard.o ../build/test.o ../build/sidocument.o ../build/edit_interface.o ../build/preferences.o ../build/minibuffer.o ../build/save_data.o ../build/siscrollbar.o ../build/div_dirnode.o

sied-dana.prc: textedit frm.bin ../resources/sied-dana.def
	build-prc ../resources/sied-dana.def  ../build/textedit ../build/*.bin
	ls -l *.prc

frm.bin: ../resources/form.rcp
	cpp -D$(LANG) -DMACRO -P ../resources/form.rcp final_form.rcp
	pilrc final_form.rcp ../build/

textedit:  $(OBJ) sied-sections.o sied-sections.ld
	$(CC) $(CXXFLAGS) $(BUILD_OBJ) -Wl,-O1 -Wl,-relax -Wl,-T ../scripts/text_64k_palmos3  sied-sections.o sied-sections.ld -L ./presize/libResizeStatic.a  -lPalmOSGlue -o ../build/textedit

%.o:%.cc
	$(CC) $(CXXFLAGS) -c $< -o ../build/$@


sied-sections.o: sied-sections.s
	m68k-palmos-gcc -c sied-sections.s

sied-sections.s sied-sections.ld: ../resources/sied-dana.def
	m68k-palmos-multigen ../resources/sied-dana.def

clean:
	rm -f ../build/*.o
	rm -f ../build/*.bin
	rm -f ../build/textedit
	rm -f ../build/*.ld
	rm -f ../build/*.s
	rm -f *.orig
	rm -f *~
	rm -f sied-dana.prc
	rm -f final_form.rcp 
	rm -f ../build/final_form.rcp
tidy:
	astyle --style=ansi -T *.cc
	astyle --style=ansi -T *.h

# DO NOT DELETE
cache_file_plugin.o: cache_file_plugin.h file_plugin.h file_interface.h
cache_file_plugin.o: resources.h sections.h dirnode.h memhandler.h error.h
dirnode.o: dirnode.h sections.h resources.h error.h file_interface.h
dirnode.o: memhandler.h
doc_dirnode.o: doc_dirnode.h pdb_dirnode.h dirnode.h sections.h resources.h
doc_dirnode.o: doc_file_interface.h pdb_file_interface.h file_interface.h
doc_dirnode.o: memhandler.h error.h
doc_file_interface.o: doc_file_interface.h pdb_file_interface.h
doc_file_interface.o: file_interface.h resources.h sections.h dirnode.h
doc_file_interface.o: memhandler.h error.h file_plugin.h
edit_interface.o: edit_interface.h editor.h textview.h types.h
edit_interface.o: text_resource.h memhandler.h sections.h resources.h
edit_interface.o: sidocument.h texthandler.h filehandler.h file_interface.h
edit_interface.o: dirnode.h preferences.h minibuffer.h simacrorecorder.h
edit_interface.o: utility.h error.h
editor.o: form_pos.h sikeyboard.h preferences.h types.h text_resource.h
editor.o: memhandler.h sections.h resources.h utility.h editor.h textview.h
editor.o: sidocument.h texthandler.h filehandler.h file_interface.h dirnode.h
editor.o: minibuffer.h error.h main.h edit_interface.h simacrorecorder.h
encrypt_file_plugin.o: encrypt_file_plugin.h file_plugin.h
encrypt_file_plugin.o: file_interface.h resources.h sections.h dirnode.h
encrypt_file_plugin.o: memhandler.h error.h sha1.h AESLib/AESLib.h
error.o: error.h resources.h sections.h
file_interface.o: file_interface.h resources.h sections.h dirnode.h
file_interface.o: memhandler.h error.h file_plugin.h utility.h types.h
file_interface.o: text_resource.h
file_plugin.o: file_interface.h resources.h sections.h dirnode.h
file_plugin.o: memhandler.h file_plugin.h error.h
file_plugin.o: encrypt_file_plugin.h sha1.h AESLib/AESLib.h
filedialog.o: utility.h types.h text_resource.h memhandler.h sections.h
filedialog.o: resources.h error.h filedialog.h file_interface.h dirnode.h
filedialog.o: root_dirnode.h form_pos.h encrypt_file_plugin.h
filedialog.o: file_plugin.h sha1.h AESLib/AESLib.h
filehandler.o: filehandler.h sections.h resources.h sidocument.h types.h
filehandler.o: text_resource.h memhandler.h editor.h textview.h texthandler.h
filehandler.o: file_interface.h dirnode.h preferences.h minibuffer.h error.h
filehandler.o: utility.h doc_file_interface.h pdb_file_interface.h
filehandler.o: vfs_file_interface.h filedialog.h
filehandler.o: file_plugin.h
main.o: utility.h types.h text_resource.h memhandler.h sections.h resources.h
main.o: editor.h textview.h sidocument.h texthandler.h filehandler.h
main.o: file_interface.h dirnode.h preferences.h minibuffer.h error.h
main.o: form_pos.h main.h edit_interface.h simacrorecorder.h test.h
memhandler.o: memhandler.h sections.h resources.h error.h
minibuffer.o: minibuffer.h types.h text_resource.h memhandler.h sections.h
minibuffer.o: resources.h error.h utility.h
pdb_dirnode.o: pdb_dirnode.h dirnode.h sections.h resources.h
pdb_dirnode.o: pdb_file_interface.h file_interface.h
pdb_dirnode.o: memhandler.h error.h
pdb_file_interface.o: pdb_file_interface.h file_interface.h resources.h
pdb_file_interface.o: sections.h dirnode.h memhandler.h error.h
pdb_file_interface.o: file_plugin.h
preferences.o: preferences.h types.h text_resource.h memhandler.h sections.h
preferences.o: resources.h edit_interface.h editor.h textview.h sidocument.h
preferences.o: texthandler.h filehandler.h file_interface.h dirnode.h
preferences.o: minibuffer.h simacrorecorder.h main.h error.h
resources.o: resources.h sections.h
root_dirnode.o: root_dirnode.h dirnode.h sections.h resources.h
root_dirnode.o:  pdb_file_interface.h file_interface.h
root_dirnode.o: memhandler.h doc_file_interface.h utility.h types.h
root_dirnode.o: text_resource.h vfsdir_dirnode.h vfs_dirnode.h pdb_dirnode.h
root_dirnode.o: doc_dirnode.h vfsvol_dirnode.h error.h
sha1.o: /usr/include/string.h /usr/include/features.h
sha1.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h sha1.h sections.h
sidocument.o: sidocument.h types.h text_resource.h memhandler.h sections.h
sidocument.o: resources.h error.h textview.h
sikeyboard.o: sikeyboard.h editor.h textview.h types.h text_resource.h
sikeyboard.o: memhandler.h sections.h resources.h sidocument.h texthandler.h
sikeyboard.o: filehandler.h file_interface.h dirnode.h preferences.h
sikeyboard.o: minibuffer.h form_pos.h main.h edit_interface.h
sikeyboard.o: simacrorecorder.h error.h
simacrorecorder.o: simacrorecorder.h sections.h resources.h error.h
simacrorecorder.o: form_pos.h main.h edit_interface.h editor.h textview.h
simacrorecorder.o: types.h text_resource.h memhandler.h sidocument.h
simacrorecorder.o: texthandler.h filehandler.h file_interface.h dirnode.h
simacrorecorder.o: preferences.h minibuffer.h utility.h
test.o: test.h texthandler.h types.h text_resource.h memhandler.h sections.h
test.o: resources.h sidocument.h editor.h textview.h filehandler.h
test.o: file_interface.h dirnode.h preferences.h minibuffer.h main.h
test.o: edit_interface.h simacrorecorder.h error.h
texthandler.o: texthandler.h types.h text_resource.h memhandler.h sections.h
texthandler.o: resources.h sidocument.h error.h editor.h textview.h
texthandler.o: filehandler.h file_interface.h dirnode.h preferences.h
texthandler.o: minibuffer.h utility.h
textview.o: textview.h types.h text_resource.h memhandler.h sections.h
textview.o: resources.h sidocument.h form_pos.h error.h texthandler.h
textview.o: editor.h filehandler.h file_interface.h dirnode.h preferences.h
textview.o: minibuffer.h utility.h
types.o: types.h text_resource.h memhandler.h sections.h resources.h error.h
types.o: utility.h
updir_dirnode.o: updir_dirnode.h dirnode.h sections.h resources.h error.h
utility.o: utility.h types.h text_resource.h memhandler.h sections.h
utility.o: resources.h error.h main.h edit_interface.h editor.h textview.h
utility.o: sidocument.h texthandler.h filehandler.h file_interface.h
utility.o: dirnode.h preferences.h minibuffer.h simacrorecorder.h form_pos.h
utility.o: AESLib/AESLib.h
vfs_dirnode.o: vfs_dirnode.h dirnode.h sections.h resources.h
vfs_dirnode.o: vfsdir_dirnode.h updir_dirnode.h vfsfile_dirnode.h
vfs_dirnode.o: vfs_file_interface.h file_interface.h memhandler.h error.h
vfs_file_interface.o: vfs_file_interface.h file_interface.h resources.h
vfs_file_interface.o: sections.h dirnode.h memhandler.h error.h
vfs_file_interface.o: cache_file_plugin.h file_plugin.h
vfsdir_dirnode.o: vfsdir_dirnode.h vfs_dirnode.h dirnode.h sections.h
vfsdir_dirnode.o: resources.h updir_dirnode.h vfsfile_dirnode.h
vfsdir_dirnode.o: vfs_file_interface.h file_interface.h memhandler.h error.h
vfsfile_dirnode.o: vfsfile_dirnode.h vfs_dirnode.h dirnode.h sections.h
vfsfile_dirnode.o: resources.h vfs_file_interface.h file_interface.h
vfsfile_dirnode.o: memhandler.h error.h
vfsvol_dirnode.o: vfsvol_dirnode.h vfs_dirnode.h dirnode.h sections.h
vfsvol_dirnode.o: resources.h error.h

