/*
 * SiEd-Dana - A text editor optimized for the AlphaSmart Dana.
 * Copyright (C) 2016 Reg Tiangha
 * Contact:  reg@reginaldtiangha.com
 *
 * Portions of SiEd-Dana are based off the original SiEd project,
 * Copyright (C) 2003-2005 Benjamin Roe
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

#ifdef LANGUAGE_CATALAN

//remaining english strings I could find
#define FILE_TOO_LARGE "File Too Large"
#define FILE_TOO_LARGE_TEXT "File exceeds maximum size for SiEd-Dana."
#define FILE_TRUNCATION_TEXT "Truncated version of file loaded due to insufficient memory."
#define FILE_TRUNCATION "Memory Error"
#define FROM_TOP "From Top"
#define KEYBOARD "Keyboard"
#define REVERT_FILE "Reload File?"
#define REVERT "Reload"
#define MESSAGE_REVERT_FILE "Reload current file? All changes will be lost."

#define PLAY_MACRO "Play Macro"
#define START_RECORD "Start Rec."
#define STOP_RECORD "Stop Rec."

#define NONE_TEXT "Cap"
#define EOL_TEXT "Finals de línia"
#define SHOW_CODES_TEXT "Mostra els Codis"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "Si heu registrat "
#define MACRO_POPUP_TEXT_2 "una macro, la podeu "
#define MACRO_POPUP_TEXT_3 "executar des d'aquí"
#define GET_PASSWORD "Trieu la contrassenya"
#define PASSWORD_TEXT "Contrassenya:"
#define TEXT_ENCRYPTED "Xifrat"
#define FILE_NOT_ENCRYPTED_TITLE "Arxiu no xifrat"
#define FILE_NOT_ENCRYPTED_TEXT "L'arxiu ^1 no està xifrat"
#define DECRYPT "Desxifra"
#define QUICK_MACRO "Macro ràpida"
#define FILE_LOCATION "Trieu la contrassenya per l'arxiu "
#define UNABLE_TO_CREATE_DIR_TEXT "No es pot crear la carpeta ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Error"
#define NO_COMPRESSED_DOC_TITLE "No permès"
#define NO_COMPRESSED_DOC_TEXT "L'arxiu ^1 està comprimit. Encara no es permeten arxius DOC comprimits."
#define MUST_ENTER_NAME_TITLE "Sense nom"
#define MUST_ENTER_NAME_TEXT "Heu de donar un nom a la Macro"
#define CONFIRM_MACRO_DELETE "Confirmació"
#define MESSAGE_CONFIRM_MACRO_DELETE "Voleu suprimir la Macro?"
#define PLAY_MACRO "Executa Macro"
#define START_RECORD "Registra Macro"
#define RECORD "Registra"
#define PLAY "Executa"
//C++ file text
#define INVALID_FILE_NAME_TITLE "El nom de l'arxiu no és vàlid"
#define INVALID_FILE_NAME_TEXT "^1 no és un nom d'arxiu vàlid."
#define SELECTION_MODE "Tipus de selecció"
#define TEXT_BEAM "Envia"
#define BEAM_FILE "L'arxiu seleccionat"
#define BEAM_APP "l'aplicació SiEd-Dana"
#define PALM_STYLE_TEXT "A l'estil Palm"
#define STANDARD_STYLE_TEXT "Estàndar"
#define SELECT_ALL "Selecciona-ho tot"
//Resource text
#define WAITING_TEXT "Un moment..." //Text displayed while a long action is happening

//simple ones
#define OK "D'acord"
#define OVERWRITE "Sobreesciu"
#define CANCEL "Cancel·la"
#define SAVE "Desa"
#define DISCARD "Descarta"
#define YES "Sí"
#define NO "No"
#define TEXT_FONT "Lletra:"
#define TEXT_NONE "Cap"
#define TEXT_SIZE "Mida:"
#define TOTAL "Total:"
#define OPEN "Obre"
#define TEXT_FIND "Cerca"
#define TEXT_FIND_COLON "Cerca:"
#define TEXT_REPLACE "Reemplaça"
#define DELETE "Suprimeix"
#define TEXT_DATE "Data:"
#define TEXT_COPY_FULL "Copia"
#define RENAME "Anomena"
#define TEXT_EXIT "Surt"
#define TEXT_FILE "Arxiu"
#define TEXT_NEW "Nou"
#define PREFERENCES_ABBR "Preferències" //abbreviation for preferences
#define TEXT_ABOUT "Quant a"
#define TEXT_COPY "Copia"
#define TEXT_PASTE "Enganxa"
#define TEXT_CUT "Talla"
#define TEXT_UNDO "Desfés"
#define TEXT_REDO "Restaura"
#define TEXT_CURSOR "Cursor"
#define TEXT_END "Fi"
#define TEXT_HOME "Inici" //as in beginning of line key
#define TEXT_EDIT "Edita" //as in the menu title
#define TEXT_CALCULATE "Calc" //abbreviation for calculate
#define MENU_TEXT_STATS "Estadístiques" //abbreviation for statistics
#define TEXT_DOCUMENT_STATISTICS "Estadístiques" //abbreviation for statistics
#define TEXT_DOCUMENT_CHARS "Caràcters:"
#define TEXT_DOCUMENT_WORDS "Mots:"

#define TEXT_NEW_DIRECTORY "Nom del nou direc."
#define MANAGE_FILES "Arxius"
#define CLOSE "Tanca"
#define OLD_FILENAME "Nom actual"
#define NEW_FILENAME "Nou nom"
#define RENAME_FILE "Canvia de nom"
#define NEW_DIRECTORY "Nou Dir"
//other text
#define FIND_AGAIN "Següent"
#define SAVE_AS "Anomena i desa"
#define SELECT_FILE "Obre"
#define TERM_NOT_FOUND "No es troba"
#define END_OF_DOCUMENT_REACHED "Final del document"
#define CONFIRM_FILE_DELETE "Confirmeu Suprimir Arxiu"
#define MESSAGE_CONFIRM_FILE_DELETE "Segur que voleu esborrar l'arxiu ^1?"

#define CONFIRM_OVERWRITE "Confirmeu Sobreescriptura"
#define MESSAGE_CONFIRM_OVERWRITE "Obrir un nou arxiu esborarrà els canvis actuals. Voleu desar-los primer?"

#define COPYRIGHT_MESSAGE "\251 Benjamin Roe 2003-2004\nwww.benroe.com/sied\n\nAquesta aplicació es distribueix i tradueix sota els termes de la GPL.\251 de la traducció: PalmCAT 2003\nwww.palmcat.org\n\n"
#define NEW_FILE "Nou Arxiu"
#define MESSAGE_NEW_FILE "Obrir un nou arxiu?"
#define EDITOR_PREFERENCES "Editor de Preferències"

#define SAVE_FILE_AS "Anomena i desa"
#define FILENAME "Nom"

#define SEARCH_TERM "Cerca"
#define MATCH_CASE "Coinc. exacta"
#define MATCH_WHOLE_WORD "Paraul. complet."

#define REPLACE_ABBR "Reemp:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Reemp" //abbreviation for replace
#define TEXT_WITH "amb:"
#define REPLACE_ALL_ABBR "R tot" //abbreviation for replace all
#define END_OF_DOCUMENT "Fi del Doc"
#define START_OF_DOCUMENT "Inici del Doc"
#define DELETE_LINE "Esborra línia"
#define SPLIT_VIEW "Vista dividida" //into two
#define UNSPLIT_VIEW "Una vista"
#define TEXT_VIEW "Vista" //as in the screen display

//error messages - less important to translate
#define UNABLE_TO_UNDO "No es pot desfer"
#define OVERWRITE_FILE "Segur que voleu sobreescriure arxiu?"
#define OVERWRITE_FILE_TEXT "Aquest arxiu ja existeix. Voleu substituir el fitxer existent?"
#define UNABLE_TO_UNDO_TEXT "Aquest canvi no es pot desfer."
#define TEXT_SELECTION_COPY_TRUNCATED "Selecció retallada"
#define MESSAGE_SELECTION_COPY_TRUNCATED "No hi ha prou memòria al portapapers per tota la selecció"

#define MESSAGE_NOT_SUPPORTED "^1 No és permès."
#define TEXT_FILE_NOT_FOUND "No es troba l'arxiu"
#define MESSAGE_FILE_NOT_FOUND "L'arxiu ^1 no es troba."
#define TEXT_DATABASE_ACCESS_ERROR "Error de Base de Dades"
#define MESSAGE_DATABASE_ACCESS_ERROR "Hi ha hagut un error mentre ^1."
#define TEXT_FILE_ACCESS_ERROR "Error d'accès a arxiu"
#define MESSAGE_FILE_ACCESS_ERROR "Error d'accès a l'arxiu ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Error d'accès a la pantalla"
#define MESSAGE_SCREEN_ACCESS_ERROR "Error al crear una finestra fora de la pantalla."
#define TEXT_UNABLE_TO_SAVE "No es pot desar"
#define MESSAGE_UNABLE_TO_SAVE "No s'ha pogut desar l'arxiu ^1."
#define TEXT_OUT_OF_MEMORY "Sense Memòria"
#define MESSAGE_OUT_OF_MEMORY "Error d'assignació de memòria a ^1"
#define TEXT_DEBUG_MESSAGE "Error"
#endif
