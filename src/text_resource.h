/*
 * SiEd-Dana - A text editor optimized for the AlphaSmart Dana.
 * Copyright (C) 2016 Reg Tiangha
 * Contact:  reg@reginaldtiangha.com
 *
 * Portions of SiEd-Dana are based off the original SiEd project,
 * Copyright (C) 2003-2005 Benjamin Roe with original translations by:
 *
 * French Translation (C) 2003 Johnny Brasseur/David Bosman
 * Spanish Translation (C) 2004 Abraham Estrada
 * German Tranlation (C) 2003 Sebastian Meyer
 * Catalan Tranlation (C) 2003 PalmCAT.org
 * Italian Translation (C) 2004 Alessandro Bagnoli
*  Norgwegian Translation (C) 2004 John Gothard/Simen Graaten
 * Turkish Translation (C) 2004 Tankut Enric
 * Russian Translation (C) 2004 Pravdin, Ilchenko Denis
 * Dutch Translation (C)2004 Maurice Heck
 * Portuguese Translation (C) 2004 Matheus Lamberti
 * Czech Translation (C) 2004 Benda Radek
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

//this file contains all the text for SiEd-Dana dialogs, menus etc.
//the language for the build is selected at build time by defining the
//correct LANGUAGE_ symbol
//if you want add another language, just replace the english words and change the language symbol
//definition. Leave all the punctuation and numbers unchanged!

#define SIED_VERSION "SiEd-Dana 0.10.0-beta4"
#define READ_ONLY_MODE "Read Only"
#define MAIN_DIRNODE_TEXT "Main"

#define OUT_OF_MEMORY_WARNING "Editing. Save your work now!" //warning dialog displays "Out of memory while " + this message
#define DEFAULT_TEXT_EDITOR "Default Editor"
#define TAP_TO_OPEN "Tap to Open"
#define TEXT_DOCUMENT_ALPHA_CHARS_1 "Alpha. characters:"
#define TEXT_DOCUMENT_ALPHA_CHARS_2 "(a-Z,0-9)"
#define CHARS_MESSAGE "(uncounted)"
#define ESTIMATE_TEXT "(estimate)"

//Untranslated strings
#define SMALL_CHUNK_TEXT "Small"
#define MEDIUM_CHUNK_TEXT "Medium"
#define LARGE_CHUNK_TEXT "Large"
#define OFF_CHUNK_TEXT "Off"
#define CLOSE "Close"
#define CLOSE_TITLE "Abandon Changes?"
#define CLOSE_TITLE_SAVED "Close File?"
#define CLOSE_TEXT "Close current file without saving? All changes will be lost."
#define CLOSE_TEXT_SAVED "Close current file?"
#define INCREMENTAL_FILE_OPEN_MODE "Initial File Load"
#define FILE_TOO_LARGE "File Too Large"
#define FILE_TOO_LARGE_TEXT "File exceeds maximum size for SiEd-Dana."
#define FILE_TRUNCATION_TEXT "Truncated version of file loaded due to insufficient memory."
#define FILE_TRUNCATION "Memory Error"
#define FROM_TOP "From Top"
#define KEYBOARD "Keyboard"
#define STOP_RECORD_MENU "Stop Record"
#define INC_SEARCH_TEXT "Inc. Search"

#ifdef LANGUAGE_ENGLISH
#define REVERT_FILE "Reload File?"
#define MESSAGE_REVERT_FILE "Reload current file? All changes will be lost."
#define REVERT "Reload"
#define NONE_TEXT "None"
#define EOL_TEXT "Line Feeds"
#define SHOW_CODES_TEXT "Show Codes"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "If you record a"
#define MACRO_POPUP_TEXT_2 "macro you can"
#define MACRO_POPUP_TEXT_3 "play it from here"
#define GET_PASSWORD "Enter Password"
#define PASSWORD_TEXT "Password:"
#define TEXT_ENCRYPTED "Encrypt"
#define FILE_NOT_ENCRYPTED_TITLE "File Not Encrypt"
#define FILE_NOT_ENCRYPTED_TEXT "File ^1 is not encrypted"
#define DECRYPT "Decrypt"
#define QUICK_MACRO "Quick Mac."
#define FILE_LOCATION "Enter password for file "
#define UNABLE_TO_CREATE_DIR_TEXT "Unable to create directory ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Error"
#define NO_COMPRESSED_DOC_TITLE "Not Supported"
#define NO_COMPRESSED_DOC_TEXT "File ^1 is compressed. No support for compressed DOCs yet."
#define MUST_ENTER_NAME_TITLE "No Name"
#define MUST_ENTER_NAME_TEXT "Must enter a name for the macro"
#define CONFIRM_MACRO_DELETE "Confirm"
#define MESSAGE_CONFIRM_MACRO_DELETE "Delete Macro ^1?"
#define PLAY_MACRO "Play Macro"
#define START_RECORD "Record Macro"
#define RECORD "Record"
#define PLAY "Play"
//C++ file text
#define INVALID_FILE_NAME_TITLE "Invalid Filename"
#define INVALID_FILE_NAME_TEXT "^1 is not a valid filename."
#define SELECTION_MODE "Selection Mode"
#define TEXT_BEAM "Beam"
#define BEAM_FILE "Selected file"
#define BEAM_APP "SiEd-Dana App."
#define PALM_STYLE_TEXT "Palm Style"
#define STANDARD_STYLE_TEXT "Standard"
#define SELECT_ALL "Select All"
#define WAITING_TEXT "Please Wait..." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "New Directory Name"

//simple ones
#define OK "OK"
#define CLOSE "Close"
#define OVERWRITE "Overwrite"
#define CANCEL "Cancel"
#define SAVE "Save"
#define DISCARD "Discard"
#define YES "Yes"
#define NO "No"
#define TEXT_FONT "Font"
#define TEXT_NONE "None"
#define TEXT_SIZE "Size:"
#define TOTAL "Total:"
#define OPEN "Open"
#define TEXT_FIND "Find"
#define TEXT_FIND_COLON "Find:"
#define TEXT_REPLACE "Find & Replace"
#define DELETE "Delete"
#define TEXT_DATE "Date:"
#define TEXT_COPY_FULL "Copy"
#define RENAME "Rename"
#define TEXT_EXIT "Exit"
#define TEXT_FILE "File"
#define TEXT_NEW "New"
#define PREFERENCES_ABBR "Prefs" //abbreviation for preferences
#define TEXT_ABOUT "About"
#define TEXT_COPY "Copy"
#define TEXT_PASTE "Paste"
#define TEXT_CUT "Cut"
#define TEXT_UNDO "Undo"
#define TEXT_REDO "Redo"
#define TEXT_CURSOR "Cursor"
#define TEXT_END "End"
#define TEXT_HOME "Home" //as in beginning of line key
#define TEXT_EDIT "Edit" //as in the menu title
#define TEXT_CALCULATE "Count" //abbreviation for count
#define TEXT_DOCUMENT_STATISTICS "Document Statistics"
#define TEXT_DOCUMENT_CHARS "Characters:"
#define TEXT_DOCUMENT_WORDS "Words:"
#define MENU_TEXT_STATS "Stats"
#define MANAGE_FILES "Files"
//other text
#define NEW_FILENAME "New Filename"
#define OLD_FILENAME "Current Filename"
#define RENAME_FILE "Rename File"
#define FIND_AGAIN "Find Again"
#define SAVE_AS "Save As"
#define SELECT_FILE "Select File"
#define TERM_NOT_FOUND "Term Not Found"
#define END_OF_DOCUMENT_REACHED "End of document reached"
#define CONFIRM_FILE_DELETE "Confirm File Delete"
#define MESSAGE_CONFIRM_FILE_DELETE "Delete file ^1?"

#define CONFIRM_OVERWRITE "Confirm Overwrite"
#define MESSAGE_CONFIRM_OVERWRITE "Opening a new file will erase current changes. Save file first?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\SiEd-Dana is based on the original SiEd text editor, \251 2003-2005\nBenjamin Roe.\nArtwork is \251 Stefan Smith & Tankut Eric.\nThis application is distributed under the terms of the GPLv2."

#define NEW_FILE "New File"
#define MESSAGE_NEW_FILE "Open a new file?"
#define EDITOR_PREFERENCES "Editor Preferences"

#define SAVE_FILE_AS "Save File As"
#define FILENAME "Filename"

#define SEARCH_TERM "Search Term"
#define MATCH_CASE "Match Case"
#define MATCH_WHOLE_WORD "Whole Word"

#define REPLACE_ABBR "Repl:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Repl" //abbreviation for replace
#define TEXT_WITH "With:"
#define REPLACE_ALL_ABBR "R all" //abbreviation for replace all
#define END_OF_DOCUMENT "End of Doc"
#define START_OF_DOCUMENT "Start of Doc"
#define DELETE_LINE "Delete Line"
#define SPLIT_VIEW "Split View"
#define UNSPLIT_VIEW "Unsplit View"
#define TEXT_VIEW "View"
#define NEW_DIRECTORY "New Dir"

//error messages - less important to translate
#define UNABLE_TO_UNDO "No undo"
#define OVERWRITE_FILE "Overwrite File?"
#define OVERWRITE_FILE_TEXT "A file with that name exists. Overwrite?"
#define UNABLE_TO_UNDO_TEXT "This change can not be undone."
#define TEXT_SELECTION_COPY_TRUNCATED "Selection Truncated"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Insufficient space in clipboard for entire selection"

#define MESSAGE_NOT_SUPPORTED "^1 Not Supported."
#define TEXT_FILE_NOT_FOUND "File Not Found"
#define MESSAGE_FILE_NOT_FOUND "File ^1 not found."
#define TEXT_DATABASE_ACCESS_ERROR "Database Access Error"
#define MESSAGE_DATABASE_ACCESS_ERROR "Database Error while ^1."
#define TEXT_FILE_ACCESS_ERROR "File Access Error"
#define MESSAGE_FILE_ACCESS_ERROR "Error in accessing file ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Screen Access Error"
#define MESSAGE_SCREEN_ACCESS_ERROR "Error in creating offscreen window."
#define TEXT_UNABLE_TO_SAVE "Unable to Save"
#define MESSAGE_UNABLE_TO_SAVE "Could not save file ^1."
#define TEXT_DEBUG_MESSAGE "Debug Message"
#define TEXT_OUT_OF_MEMORY "Out of Memory"
#define MESSAGE_OUT_OF_MEMORY "Memory Allocation Error in ^1"
#define FEATURE_NOT_IMPLEMENTED "Feature Not Implemented"

#endif
#ifdef LANGUAGE_FRENCH
#define DEFAULT_TEXT_EDITOR "Editeur par défaut"
#define TAP_TO_OPEN "Tap pour ouvrir"
#define TEXT_DOCUMENT_ALPHA_CHARS_1 "Caract. alphab. :"
#define TEXT_DOCUMENT_ALPHA_CHARS_2 "(a-Z,0-9)"
#define CHARS_MESSAGE "(non conté)"

#define SMALL_CHUNK_TEXT "Petit"
#define MEDIUM_CHUNK_TEXT "Medium"
#define LARGE_CHUNK_TEXT "Large"
#define OFF_CHUNK_TEXT "Off" //as in opposite of on
#define CLOSE "Fermer"
#define CLOSE_TITLE "Perdre modif?"
#define CLOSE_TITLE_SAVED "Fermer le fichier ?"
#define CLOSE_TEXT "Fermer le document actif sans enregistrer ? Toutes les modifs seront perdues."
#define CLOSE_TEXT_SAVED "Fermer le document actif ?"
#define INCREMENTAL_FILE_OPEN_MODE "Chargement initial du fichier"
#define STOP_RECORD_MENU "Arrêter Enreg."
//Untranslated strings for the French version
#define FILE_TOO_LARGE "Fichier trop large"
#define FILE_TOO_LARGE_TEXT "Fichier dépasse taille maximale" // File exceeds maximum size for SiEd-Dana."
#define FILE_TRUNCATION_TEXT "par manque de mémoire une version tronquée du fichier est chargée"//"Truncated version of file loaded due to insufficient memory."
#define FILE_TRUNCATION "erreur mémoire"//"Memory Error"
#define FROM_TOP "Du Début"//"From Top"
#define KEYBOARD "Clavier"
// short for incremental search in the menu
#define INC_SEARCH_TEXT "Recherche incrém." //"Inc. Search"
#define REVERT_FILE "Recharger le fichier?"
#define REVERT "Recharger"
#define MESSAGE_REVERT_FILE "Recharger le fichier? Cela supprimera toutes les modifications non sauvegardées"
#define NONE_TEXT "Aucun"
#define EOL_TEXT "Saut de ligne"
#define SHOW_CODES_TEXT "Afficher code"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "Si vous créez une"
#define MACRO_POPUP_TEXT_2 "macro vous pouvez"
#define MACRO_POPUP_TEXT_3 "la lancer ici"

#define GET_PASSWORD "Entr. mot de passe"
#define PASSWORD_TEXT "Mot de passe:"
#define TEXT_ENCRYPTED "Crypter"
#define FILE_NOT_ENCRYPTED_TITLE "Fichier non crypté"
#define FILE_NOT_ENCRYPTED_TEXT "Le fichier ^1 n'est pas crypté"
#define DECRYPT "Décrypter"
#define QUICK_MACRO "Quick Mac."
#define FILE_LOCATION "Entrez le mot de passe"
#define UNABLE_TO_CREATE_DIR_TEXT "Impossible de créer le rép. ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Erreur"
#define NO_COMPRESSED_DOC_TITLE "Pas supporté"
#define NO_COMPRESSED_DOC_TEXT "Le fichier ^1 est compressé. Cela n'est pas encore supporté."
#define MUST_ENTER_NAME_TITLE "Pas de nom"
#define MUST_ENTER_NAME_TEXT "Vous devez donner un nom à la macro"
#define CONFIRM_MACRO_DELETE "Confirmez"
#define MESSAGE_CONFIRM_MACRO_DELETE "Effacer la macro ^1?"
#define PLAY_MACRO "Lire macro"
#define START_RECORD "Enreg. macro"
#define RECORD "Enreg."
#define PLAY "Lire"
//C++ file text
#define INVALID_FILE_NAME_TITLE "Nom de fichier incorrect"
#define INVALID_FILE_NAME_TEXT "^1 n'est pas un nom valide."
#define SELECTION_MODE "Selection Mode"
#define TEXT_BEAM "Transmettre"
#define BEAM_FILE "Fichier sélectionné"
#define BEAM_APP "SiEd-Dana App."
#define PALM_STYLE_TEXT "Palm Style"
#define STANDARD_STYLE_TEXT "Standard"
#define SELECT_ALL "Sélect. tout"
#define WAITING_TEXT "Patientez SVP..." //Text displayed while leaving SiEd-Dana

//Resource text

//simple ones
#define OK "OK"
#define CANCEL "Annuler"
#define OVERWRITE "Effacer"
#define SAVE "Enregistrer" // "Enregistrer" should be ok as it is used by other applications
#define DISCARD "Annuler"  // be replaced by "Annuler" for same reasons
#define CLOSE "Fermer"
#define YES "Oui"
#define NO "Non"
#define TEXT_FONT "Police:"
#define TEXT_NONE "Aucun"
#define TEXT_SIZE "Taille:"
#define TOTAL "Total:"
#define OPEN "Ouvrir"
#define TEXT_FIND "Chercher"
#define TEXT_FIND_COLON "Chercher:"
#define TEXT_REPLACE "Remplacer"
#define DELETE "Effacer"
#define TEXT_DATE "Date:"
#define TEXT_COPY_FULL "Copier"
#define RENAME "Renommer"
#define TEXT_EXIT "Sortir"
#define TEXT_FILE "Fichier"
#define TEXT_NEW "Nouveau"
#define PREFERENCES_ABBR "Préfs." //abbreviation for preferences
#define TEXT_ABOUT "A propos de"
#define TEXT_COPY "Copier"
#define TEXT_PASTE "Coller"
#define TEXT_CUT "Couper"
#define TEXT_UNDO "Annuler" // can be replaced by english Undo
#define TEXT_REDO "Répéter"
#define TEXT_CURSOR "Curseur"
#define TEXT_END "Fin"
#define TEXT_HOME "Début" //as in beginning of line key
#define TEXT_EDIT "Editer" //as in the menu title
#define NEW_DIRECTORY "Nv. répert."
#define TEXT_NEW_DIRECTORY "Nom du répertoire"

#define MENU_TEXT_STATS "Stats"
#define TEXT_CALCULATE "Calc" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Statistiques"
#define TEXT_DOCUMENT_CHARS "Caractères:"
#define TEXT_DOCUMENT_WORDS "Mots:"
#define MANAGE_FILES "Fichiers"
//other text
#define FIND_AGAIN "Suivant..."
#define OLD_FILENAME "Nom actuel"
#define SAVE_AS "Enreg. sous..." // "Sauver en.."
#define SELECT_FILE "Sélect. fichier" // just put "Select. Fichier"
#define TERM_NOT_FOUND "Mot non trouvé" // funny TNT why not C4 ?
#define END_OF_DOCUMENT_REACHED "Fin du document"
#define CONFIRM_FILE_DELETE "Confirmez la suppression" // you coul use shorter "Confirmez la suppression"
#define MESSAGE_CONFIRM_FILE_DELETE "Effacer le fichier ^1?"
#define CONFIRM_OVERWRITE "Remplacer?"
#define OVERWRITE_FILE "Remplacer le fichier existant ?" // let's say : remove existing file ? you may put "Confirmez" for confirm
#define OVERWRITE_FILE_TEXT "Effacer le fichier existant ?"
#define MESSAGE_CONFIRM_OVERWRITE "Ouvrir un nouveau fichier effacera les modifications en cours. Sauvegarder le fichier d'abord ?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Basé sur SiEd, \251 2003-2005 Benjamin Roe.\nArtwork \251 Stefan Smith & Tankut Eric.\nCette application est sous licence GPL. Traduction française Johnny Brasseur (2003), David Bosman (2004) "// Is it ok ?

#define NEW_FILE "Nouveau fichier"
#define MESSAGE_NEW_FILE "Créer un nouveau fichier?"
#define EDITOR_PREFERENCES "Préférences"
#define TEXT_DELETE_ABBR "Ef" //two letter abbreviation for ReMove (Ef) for Effacer
#define TEXT_MOVE_ABBR "De" //Move (rename) (De) for Deplacer
#define TEXT_COPY_ABBR "Cp" //copy

#define COPY_FILE "Copier le fichier"
#define NEW_FILENAME "Nouveau nom de fichier"

#define RENAME_FILE "Renommer le fichier"

#define SAVE_FILE_AS "Enregistrer sous..."
#define FILENAME "Nom de fichier"

#define SEARCH_TERM "Chercher un mot" // "mot" means word, a term is a technical feature in french
#define MATCH_CASE "Respecter la casse"
#define MATCH_WHOLE_WORD "Mot entier"

#define REPLACE_ABBR "Rempl:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Rempl." //abbreviation for replace
#define TEXT_WITH "Avec:"
#define REPLACE_ALL_ABBR "R.tout" //abbreviation for replace all
#define END_OF_DOCUMENT "Fin du doc"
#define START_OF_DOCUMENT "Début du doc"
#define DELETE_LINE "Effacer la ligne"
#define SPLIT_VIEW "Diviser la vue"
#define UNSPLIT_VIEW "Une seule vue" // means one view only : "Rassembler les vues" means Gather views
#define TEXT_VIEW "Vue" // what is the meaning of view here ?

//error messages - less important to translate
#define TEXT_SELECTION_COPY_TRUNCATED "Sélection tronquée"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Espace insuffisant dans le presse-papier pour la sélection"
#define TEXT_FEATURE_ERROR "Erreur de fonctionnalité"
#define MESSAGE_NOT_SUPPORTED "^1 Non fourni." // means not yet implemented ? not supplied ? not yet done ?
#define TEXT_FILE_NOT_FOUND "Fichier non trouvé"
#define MESSAGE_FILE_NOT_FOUND "Fichier ^1 non trouvé."
#define UNABLE_TO_UNDO "Pas d'annulation."
#define UNABLE_TO_UNDO_TEXT "Impossible d'annuler."
#define TEXT_DATABASE_ACCESS_ERROR "Erreur d'accès à la database" // we sometimes use Database instead of "base de données" which is longer
#define MESSAGE_DATABASE_ACCESS_ERROR "Erreur database : ^1."
#define TEXT_FILE_ACCESS_ERROR "Fichier inaccessible"
#define MESSAGE_FILE_ACCESS_ERROR "Erreur lors de l'accès au fichier ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Ecran inaccessible"
#define MESSAGE_SCREEN_ACCESS_ERROR "Erreur lors de la création de la fenêtre dédiée." // what means offscreen here ? means out of the screen drawing ? or dedicated ?
#define TEXT_UNABLE_TO_SAVE "Erreur sauvegarde"
#define MESSAGE_UNABLE_TO_SAVE "Fichier non enregistré ^1."
#define TEXT_DEBUG_MESSAGE "Message Debug"
#define TEXT_OUT_OF_MEMORY "Dépassement de capacité mémoire"
#define MESSAGE_OUT_OF_MEMORY "Erreur d'allocation de la mémoire : ^1"
#define FEATURE_NOT_IMPLEMENTED "Fonction non implémentée"
#define MESSAGE_FEATURE_NOT_IMPLEMENTED "Cette fonction sera implémentée dans une version future. Soyez patient SVP!"
#endif

#ifdef LANGUAGE_GERMAN
#define REVERT_FILE "Neu laden?"
#define REVERT "Neu laden"
#define MESSAGE_REVERT_FILE "Aktuelle Datei neu laden? Alle Änderungen gehen verloren."
#define NONE_TEXT "Keine"
#define EOL_TEXT "Line Feeds"
#define SHOW_CODES_TEXT "Zeige Codes"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "Aufgezeichnete"
#define MACRO_POPUP_TEXT_2 "Makros sind"
#define MACRO_POPUP_TEXT_3 "hier abspielbar"
#define GET_PASSWORD "Passwort eingeben"
#define PASSWORD_TEXT "Passwort:"
#define TEXT_ENCRYPTED "Verschlüsseln"
#define FILE_NOT_ENCRYPTED_TITLE "Datei nicht verschlüsselt"
#define FILE_NOT_ENCRYPTED_TEXT "File ^1 is not encrypted"
#define DECRYPT "Entschlüsseln"
#define QUICK_MACRO "Quick-Makro"
#define FILE_LOCATION "Gib ein Passwort für Datei ein  "
#define UNABLE_TO_CREATE_DIR_TEXT "Verzeichnis ^1 kann nicht angelegt werden."
#define UNABLE_TO_CREATE_DIR_TITLE "Fehler"
#define NO_COMPRESSED_DOC_TITLE "Nicht unterstützt"
#define NO_COMPRESSED_DOC_TEXT "Datei ^1 ist komprimiert. Komprimierte DOCs werden noch nicht unterstützt."
#define MUST_ENTER_NAME_TITLE "Namenlos"
#define MUST_ENTER_NAME_TEXT "Name für Makro ist erforderlich!"
#define CONFIRM_MACRO_DELETE "Bestätigung"
#define MESSAGE_CONFIRM_MACRO_DELETE "Makro ^1 löschen?"
#define PLAY_MACRO "Makro abspielen"
#define START_RECORD "Makro aufnehmen"
#define STOP_RECORD_MENU "Aufnahme beenden"
#define RECORD "Aufnahme"
#define PLAY "Abspielen"
//C++ file text
#define INVALID_FILE_NAME_TITLE "Ungültiger Dateiname"
#define INVALID_FILE_NAME_TEXT "^1 ist kein gültiger Dateiname."
#define SELECTION_MODE "Auswahlmodus"
#define TEXT_BEAM "Beamen"
#define BEAM_FILE "Gewählte Datei"
#define BEAM_APP "SiEd-Dana App."
#define PALM_STYLE_TEXT "Palm Style"
#define STANDARD_STYLE_TEXT "Standard"
#define SELECT_ALL "Alles markieren"
#define WAITING_TEXT "Bitte warten..." //Text displayed while leaving SiEd-Dana

//Resource text

//simple ones
#define OK "OK"
#define CANCEL "Abbrechen"
#define OVERWRITE "überschreiben"
#define SAVE "Speichern"
#define DISCARD "Verwerfen"
#define YES "Ja"
#define NO "Nein"
#define TEXT_FONT "Schrift"
#define TEXT_NONE "Keine"
#define TEXT_SIZE "Größe:"
#define TOTAL "Gesamt:"
#define OPEN "Öffnen"
#define TEXT_FIND "Suchen"
#define TEXT_FIND_COLON "Suchen:"
#define TEXT_REPLACE "Ersetzen"
#define DELETE "Löschen"
#define TEXT_DATE "Datum:"
#define TEXT_COPY_FULL "Kopieren"
#define RENAME "Umbenenn."
#define TEXT_EXIT "Beenden"
#define TEXT_FILE "Datei"
#define TEXT_NEW "Neu"
#define PREFERENCES_ABBR "Einstellungen.." //abbreviation for preferences
#define TEXT_ABOUT "Über"
#define TEXT_COPY "Kopieren"
#define TEXT_PASTE "Einfügen"
#define TEXT_CUT "Ausschneiden"
#define TEXT_UNDO "Rückgängig"
#define TEXT_REDO "Wiederherstellen"
#define TEXT_CURSOR "Cursor"
#define TEXT_END "Ende"
#define TEXT_HOME "Anfang" //as in beginning of line key
#define TEXT_EDIT "Bearb." //as in the menu title
#define MANAGE_FILES "Datei"
#define TEXT_NEW_DIRECTORY "Neuer Ordner"

#define OLD_FILENAME "Alter Dateiname"
#define MENU_TEXT_STATS "Statistik"
#define TEXT_CALCULATE "Zähle" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Dokumentstatistik"
#define TEXT_DOCUMENT_CHARS "Zeichen:"
#define TEXT_DOCUMENT_WORDS "Wörter:"
//other text
#define FIND_AGAIN "Weitersuchen"
#define SAVE_AS "Speichern unter.."
#define SELECT_FILE "Datei wählen"
#define TERM_NOT_FOUND "Ausdruck nicht gefunden"
#define END_OF_DOCUMENT_REACHED "Ende des Dokumentes erreicht"
#define CONFIRM_FILE_DELETE "Datei löschen bestätigen"
#define MESSAGE_CONFIRM_FILE_DELETE "Datei ^1 löschen?"
#define INC_SEARCH_TEXT "Inkrem. Suche"
#define CONFIRM_OVERWRITE "Überschreiben bestätigen"
#define MESSAGE_CONFIRM_OVERWRITE "Öffnen einer neuen Datei löscht aktuelle Änderungen. Datei zuerst speichern?"
//#define MESSAGE_CONFIRM_OVERWRITE "Wenn Sie eine neue Datei öffnen wird dies die gemachten Änderungen löschen. Änderungen speichern?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Beyogen auf SiEd, \251 2003-2005 Benjamin Roe.\nKunstwerk \251 Stefan Smith & Tankut Eric.\n\Deutsche Übersetzung von Sebastian Meyern\nDieses Programm wird unter den Auflagen der GPL verbreitet."

#define NEW_FILE "Neue Datei"
#define MESSAGE_NEW_FILE "Eine neue Datei öffnen?"
#define EDITOR_PREFERENCES "Einstellungen"
#define TEXT_DELETE_ABBR "Lö" //two letter abbreviation for ReMove
#define TEXT_MOVE_ABBR "Ve" //Move (rename)
#define TEXT_COPY_ABBR "Ko" //copy
#define CLOSE "Ende"
#define COPY_FILE "Datei kopieren"
#define NEW_FILENAME "Neuer Dateiname"
#define NEW_DIRECTORY "Neuer Ordner"
#define RENAME_FILE "Umbenennen"

#define SAVE_FILE_AS "Speichern unter"
#define FILENAME "Dateiname"

#define SEARCH_TERM "Suchen nach"
#define MATCH_CASE "Gross/Klein"
#define MATCH_WHOLE_WORD "Ganzes Wort"

#define REPLACE_ABBR "Ersetz:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Ersetz" //abbreviation for replace
#define TEXT_WITH "Mit:"
#define REPLACE_ALL_ABBR "Alle" //abbreviation for replace all
#define END_OF_DOCUMENT "Ende der Datei"
#define START_OF_DOCUMENT "Beginn der Datei"
#define DELETE_LINE "Zeile löschen"
#define SPLIT_VIEW "Geteilte Ansicht"
#define UNSPLIT_VIEW "Gesamtansicht"
#define TEXT_VIEW "Ansicht"
#define OVERWRITE_FILE_TEXT "Datei überschreiben?"
#define OVERWRITE_FILE "Datei überschreiben?"
//error messages - less important to translate
#define UNABLE_TO_UNDO "Rückgängig unmöglich."
#define UNABLE_TO_UNDO_TEXT "Änderung kann nicht rückgängig gemacht werden."
#define TEXT_SELECTION_COPY_TRUNCATED "Auswahl beschnitten"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Nicht genügend Platz in der Zwischenablage für die gesamte Auswahl"
#define TEXT_FEATURE_ERROR "Funktions-Fehler"
#define MESSAGE_NOT_SUPPORTED "^1 nicht unterstützt."
#define TEXT_FILE_NOT_FOUND "Datei nicht gefunden"
#define MESSAGE_FILE_NOT_FOUND "Datei ^1 nicht gefunden."
#define TEXT_DATABASE_ACCESS_ERROR "Fehler beim Zugriff auf die Datenbank"
#define MESSAGE_DATABASE_ACCESS_ERROR "Datenbankfehler während ^1."
#define TEXT_FILE_ACCESS_ERROR "Fehler beim Dateizugriff"
#define MESSAGE_FILE_ACCESS_ERROR "Fehler beim Zugriff auf Datei ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Fehler beim Zugriff auf den Bildschirm"
#define MESSAGE_SCREEN_ACCESS_ERROR "Fehler beim Erstellen des Offscreen Fensters"
#define TEXT_UNABLE_TO_SAVE "Speichern nicht möglich"
#define MESSAGE_UNABLE_TO_SAVE "Datei ^1 kann nicht gespeichert werden."
#define TEXT_DEBUG_MESSAGE "Debug-Meldung"
#define TEXT_OUT_OF_MEMORY "Speicher voll"
#define MESSAGE_OUT_OF_MEMORY "Speicheranforderungs-Fehler bei ^1"
#define FEATURE_NOT_IMPLEMENTED "Funktion nicht implementiert"
#define MESSAGE_FEATURE_NOT_IMPLEMENTED "Diese Funktion wird in einer späteren Version eingefügt. Bitte haben Sie Geduld!"
#endif

#ifdef LANGUAGE_SPANISH
#define FILE_TOO_LARGE "Archivo muy grande"
#define FILE_TOO_LARGE_TEXT "El archivo excede el maximo por SiEd-Dana."
#define FILE_TRUNCATION_TEXT "Version incompleta del archivo dada la capacidad de memoria disponible."
#define FILE_TRUNCATION "Error de memoria"
#define FROM_TOP "Desde arriba"
#define KEYBOARD "Teclado"
#define STOP_RECORD_MENU "Detener grabacion"
#define INC_SEARCH_TEXT "Busqueda Inc."
#define REVERT_FILE "¿Cargar archivo?"
#define REVERT "Cargar"
#define MESSAGE_REVERT_FILE "¿Cargar archivo actual? Todos los cambios seran perdidos."
#define NONE_TEXT "Ninguno"
#define EOL_TEXT "Fin de la linea"
#define SHOW_CODES_TEXT "Mostrar Códigos"
#define SHOW_MODE "Modo"
#define MACRO_POPUP_TEXT_1 "Si grabas un macro"
#define MACRO_POPUP_TEXT_2 "macro puedes"
#define MACRO_POPUP_TEXT_3 "reproducir desde aqui"
#define GET_PASSWORD "Introduce Contraseña"
#define PASSWORD_TEXT "Contraseña:"
#define TEXT_ENCRYPTED "Encriptar"
#define FILE_NOT_ENCRYPTED_TITLE "Archivo no encrtiptado"
#define FILE_NOT_ENCRYPTED_TEXT "El archivo ^1 no esta encriptado"
#define DECRYPT "Desencriptar"
#define QUICK_MACRO "Macro Rapido"
#define FILE_LOCATION "Introduce contraseña para el archivo "
#define UNABLE_TO_CREATE_DIR_TEXT "No se puede crear el directorio ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Error"
#define NO_COMPRESSED_DOC_TITLE "No hay soporte"
#define NO_COMPRESSED_DOC_TEXT "Archivo ^1 esta comprimido. No hay soporte para archivos comprimidos aun."
#define MUST_ENTER_NAME_TITLE "Sin nombre"
#define MUST_ENTER_NAME_TEXT "Debes introducir un nombre a la Macro"
#define CONFIRM_MACRO_DELETE "Confirmar"
#define MESSAGE_CONFIRM_MACRO_DELETE "¿Borrar Macro ^1?"
#define PLAY_MACRO "Reproducir Macro"
#define START_RECORD "Grabar Macro"
#define RECORD "Grabar"
#define PLAY "Reproducir"
//C++ file text
#define INVALID_FILE_NAME_TITLE "Nombre del archivo invalido"
#define INVALID_FILE_NAME_TEXT "^1 no es un nombre valido."
#define SELECTION_MODE "Selecciona modo"
#define TEXT_BEAM "Beam"
#define BEAM_FILE "Selecciona archivo"
#define BEAM_APP "SiEd-Dana App."
#define PALM_STYLE_TEXT "Estilo Palm"
#define STANDARD_STYLE_TEXT "Estandard"
#define SELECT_ALL "Seleccionar Todo"

//C++ file text

#define WAITING_TEXT "Por favor Espera..." //Text displayed while leaving SiEd-Dana

//Resource text

//simple ones
#define OK "Aceptar"
#define CANCEL "Cancelar"
#define OVERWRITE "Sobreescribir"
#define SAVE "Grabar"
#define DISCARD "Ignorar"
#define YES "Si"
#define NO "No"
#define TEXT_FONT "Fuente"
#define TEXT_NONE "None"
#define TEXT_SIZE "Tamaño:"
#define TOTAL "Total:"
#define OPEN "Abrir"
#define TEXT_FIND "Buscar"
#define TEXT_FIND_COLON "Buscar:"
#define TEXT_REPLACE "Reemplazar"
#define DELETE "Borrar"
#define TEXT_DATE "Fecha:"
#define TEXT_COPY_FULL "Copiar"
#define RENAME "Renombrar"
#define TEXT_EXIT "Salir"
#define TEXT_FILE "Archivo"
#define TEXT_NEW "Nuevo"
#define PREFERENCES_ABBR "Prefs" //abbreviation for preferences
#define TEXT_ABOUT "Acerca de"
#define TEXT_COPY "Copiar"
#define TEXT_PASTE "Pegar"
#define TEXT_CUT "Cortar"
#define TEXT_UNDO "Deshacer"
#define TEXT_REDO "Rehacer"
#define TEXT_CURSOR "Cursor"
#define TEXT_END "Fin"
#define TEXT_HOME "Inicio" //as in beginning of line key
#define TEXT_EDIT "Editar" //as in the menu title
#define CLOSE "Cerrar"
#define TEXT_NEW_DIRECTORY "Nuevo nombre del Dir"
#define MANAGE_FILES "Archivos"
#define MENU_TEXT_STATS "Stads"
#define OLD_FILENAME "Nombre de archivo"
#define NEW_DIRECTORY "Nuevo Dir"
#define RENAME_FILE "Renombrar"
#define TEXT_CALCULATE "Calc" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Estadisticas del Doc"
#define TEXT_DOCUMENT_CHARS "Caracteres:"
#define TEXT_DOCUMENT_WORDS "Palabras:"

//other text
#define FIND_AGAIN "Buscar de nuevo"
#define SAVE_AS "Grabar como"
#define SELECT_FILE "Seleccionar archivo"
#define TERM_NOT_FOUND "Termino no encontrado"
#define END_OF_DOCUMENT_REACHED "Fin de documento"
#define CONFIRM_FILE_DELETE "Confirmar Borrar Archivo"
#define MESSAGE_CONFIRM_FILE_DELETE "Borrar archivo ^1?"

#define CONFIRM_OVERWRITE "Confirmar Sobreescritura"
#define MESSAGE_CONFIRM_OVERWRITE "Abrir un nuevo archivo borrara los cambios actuales.\n\¿Grabar archivo primero?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Residencia en SiEd, \251 2003-2005 Benjamin Roe.\nObra de arte \251 Stefan Smith & Tankut Eric.\nTraducción al español por Abraham Estrada\n\nEsta aplicación es distribuida bajo los terminos de GPL."

#define NEW_FILE "Nuevo Archivo"
#define MESSAGE_NEW_FILE "¿Crear nuevo archivo?"
#define EDITOR_PREFERENCES "Preferencias"
#define TEXT_DELETE_ABBR "Rm" //two letter abbreviation for ReMove
#define TEXT_MOVE_ABBR "Mv" //Move (rename)
#define TEXT_COPY_ABBR "Cp" //copy

#define NEW_FILENAME "Nuevo nombre de archivo"

#define SAVE_FILE_AS "Grabar Archivo como"
#define FILENAME "Nombre de archivo"

#define SEARCH_TERM "Buscar Termino"
#define MATCH_CASE "Mayúsculas"
#define MATCH_WHOLE_WORD "Palabra completa"

#define REPLACE_ABBR "Reepl:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Reepl" //abbreviation for replace
#define TEXT_WITH "Con:"
#define REPLACE_ALL_ABBR "R all" //abbreviation for replace all
#define END_OF_DOCUMENT "Fin del Doc"
#define START_OF_DOCUMENT "Inicio del Doc"
#define DELETE_LINE "Borrar Linea"
#define SPLIT_VIEW "Vista Doble"
#define UNSPLIT_VIEW "Vista Simple"
#define TEXT_VIEW "Vista"
#define UNABLE_TO_UNDO "No se puede deshacer."
#define UNABLE_TO_UNDO_TEXT "El cambio al archivo no se puede deshacer."
#define OVERWRITE_FILE "¿Sobreescribir el archivo?"
#define OVERWRITE_FILE_TEXT "¿Sobreescribir el archivo?"

//error messages - less important to translate
#define TEXT_SELECTION_COPY_TRUNCATED "Selección Truncada"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Espacio insuficiente para copiar la selección"
#define TEXT_FEATURE_ERROR "Error en la característica"
#define MESSAGE_NOT_SUPPORTED "^1 No Soportado."
#define TEXT_FILE_NOT_FOUND "Archivo no encontrado"
#define MESSAGE_FILE_NOT_FOUND "El Archivo ^1 no fue encontrado."
#define TEXT_DATABASE_ACCESS_ERROR "Error en la base de datos"
#define MESSAGE_DATABASE_ACCESS_ERROR "Error en la base de datos cuando ^1."
#define TEXT_FILE_ACCESS_ERROR "Error al accesar al archivo"
#define MESSAGE_FILE_ACCESS_ERROR "Error al accesar al archivo ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Error de acceso a pantalla"
#define MESSAGE_SCREEN_ACCESS_ERROR "Error in creating offscreen window."
#define TEXT_UNABLE_TO_SAVE "Imposible de grabar"
#define MESSAGE_UNABLE_TO_SAVE "No se pudo grabar el archivo ^1."
#define TEXT_DEBUG_MESSAGE "Mensaje de depuración"
#define TEXT_OUT_OF_MEMORY "Sin memoria"
#define MESSAGE_OUT_OF_MEMORY "Error en la memoria de asiganación ^1"
#define FEATURE_NOT_IMPLEMENTED "Característica no implementada"
#define MESSAGE_FEATURE_NOT_IMPLEMENTED "Característica no implementada hasta versiones posteriores. Por favor se paciente!"

#endif//end spanish

#ifdef LANGUAGE_CATALAN
#define CHARS_MESSAGE "(no s'han comptat)"
#define ESTIMATE_TEXT "(estimat)"
#define SMALL_CHUNK_TEXT "Petit"
#define MEDIUM_CHUNK_TEXT "Mitg"
#define LARGE_CHUNK_TEXT "Mitg"
#define OFF_CHUNK_TEXT "Cap"
#define DEFAULT_TEXT_EDITOR "Editor per omissió"
#define INCREMENTAL_FILE_OPEN_MODE "Carrega Arxiu Inicial"

#define READ_ONLY_MODE "Només de lectura"
#define TAP_TO_OPEN "Clica per obrir"
#define MAIN_DIRNODE_TEXT "Principal"

#define INC_SEARCH_TEXT "Cerca progressiva"
#define MACRO_POPUP_FOUR_STRINGS
//remaining english strings I could find
#define FILE_TOO_LARGE "L'arxiu és massa gran"
#define FILE_TOO_LARGE_TEXT "L'arxiu supera la mida màxima que SiEd-Dana pot tractar."
#define FILE_TRUNCATION_TEXT "S'ha carregat una versió truncada de l'arxiu donat que ho hi ha prou memòria."
#define FILE_TRUNCATION "Error de memòria"
#define FROM_TOP "Des d'adalt"
#define KEYBOARD "Teclat"
#define REVERT_FILE "Tornar a carregar l'arxiu?"
#define REVERT "Torna a carregar"
#define MESSAGE_REVERT_FILE "Tornar a carregar? Es perdràn tots els canvis."

#define PLAY_MACRO_MENU "Executa Macro"
#define START_RECORD_MENU "Registra"
#define STOP_RECORD_MENU "Atura"

#define NONE_TEXT "Cap"
#define EOL_TEXT "Finals de lí­nia"
#define SHOW_CODES_TEXT "Mostra els Codis"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "Si heu registrat"
#define MACRO_POPUP_TEXT_2 "una macro, la"
#define MACRO_POPUP_TEXT_3 "podeu executar"
#define MACRO_POPUP_TEXT_4 "des d'aquí"
#define GET_PASSWORD "Trieu la contrassenya"
#define PASSWORD_TEXT "Contrassenya:"
#define TEXT_ENCRYPTED "Xifrat"
#define FILE_NOT_ENCRYPTED_TITLE "Arxiu no xifrat"
#define FILE_NOT_ENCRYPTED_TEXT "L'arxiu ^1 no està  xifrat"
#define DECRYPT "Desxifra"
#define QUICK_MACRO "Macro ràpida"
#define FILE_LOCATION "Trieu la contrassenya per l'arxiu "
#define UNABLE_TO_CREATE_DIR_TEXT "No es pot crear la carpeta ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Error"
#define NO_COMPRESSED_DOC_TITLE "No permés"
#define NO_COMPRESSED_DOC_TEXT "L'arxiu ^1 està  comprimit. Encara no es permeten arxius DOC comprimits."
#define MUST_ENTER_NAME_TITLE "Sense nom"
#define MUST_ENTER_NAME_TEXT "Heu de donar un nom a la Macro"
#define CONFIRM_MACRO_DELETE "Confirmació"
#define MESSAGE_CONFIRM_MACRO_DELETE "Voleu suprimir la Macro ^1?"
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
#define TEXT_CALCULATE "Calcula" //abbreviation for calculate
#define MENU_TEXT_STATS "Estadístiques" //abbreviation for statistics
#define TEXT_DOCUMENT_STATISTICS "Estadí­stiques" //abbreviation for statistics
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
#define MESSAGE_CONFIRM_OVERWRITE "Obrir un nou arxiu esborrarà  els canvis actuals. Voleu desar-los primer?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Basat en SiEd, \251 2003-2005 Benjamin Roe.\nObra d'art \251 Stefan Smith & Tankut Eric.\nAquesta aplicació es distribueix i tradueix sota els termes de la GPL.\251 de la traducció: PalmCAT 2003\nwww.palmcat.org\n"

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
#define DELETE_LINE "Esborra la línia"
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

#define MESSAGE_NOT_SUPPORTED "^1 No és permés."
#define TEXT_FILE_NOT_FOUND "No es troba l'arxiu"
#define MESSAGE_FILE_NOT_FOUND "L'arxiu ^1 no es troba."
#define TEXT_DATABASE_ACCESS_ERROR "Error de Base de Dades"
#define MESSAGE_DATABASE_ACCESS_ERROR "Hi ha hagut un error mentre ^1."
#define TEXT_FILE_ACCESS_ERROR "Error d'accés a arxiu"
#define MESSAGE_FILE_ACCESS_ERROR "Error d'accés a l'arxiu ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Error d'accés a la pantalla"
#define MESSAGE_SCREEN_ACCESS_ERROR "Error al crear una finestra fora de la pantalla."
#define TEXT_UNABLE_TO_SAVE "No es pot desar"
#define MESSAGE_UNABLE_TO_SAVE "No s'ha pogut desar l'arxiu ^1."
#define TEXT_OUT_OF_MEMORY "Sense Memòria"
#define MESSAGE_OUT_OF_MEMORY "Error d'assignació de memòria a ^1"
#define TEXT_DEBUG_MESSAGE "Error"
#endif

#ifdef LANGUAGE_PORTUGUESE
#define REVERT_FILE "Reload File?"
#define REVERT "Reload"
#define MESSAGE_REVERT_FILE "Reload current file? All changes will be lost."
#define NONE_TEXT "None"
#define EOL_TEXT "Line Feeds"
#define SHOW_CODES_TEXT "Show Codes"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "If you record a"
#define MACRO_POPUP_TEXT_2 "macro you can"
#define MACRO_POPUP_TEXT_3 "play it from here"
#define GET_PASSWORD "Enter Password"
#define PASSWORD_TEXT "Password:"
#define TEXT_ENCRYPTED "Encrypt"
#define FILE_NOT_ENCRYPTED_TITLE "File Not Encrypt"
#define FILE_NOT_ENCRYPTED_TEXT "File ^1 is not encrypted"
#define DECRYPT "Decrypt"
#define QUICK_MACRO "Quick Mac."
#define FILE_LOCATION "Enter password for file "
#define UNABLE_TO_CREATE_DIR_TEXT "Unable to create directory ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Error"
#define NO_COMPRESSED_DOC_TITLE "Not Supported"
#define NO_COMPRESSED_DOC_TEXT "File ^1 is compressed. No support for compressed DOCs yet."
#define MUST_ENTER_NAME_TITLE "No Name"
#define MUST_ENTER_NAME_TEXT "Must enter a name for the macro"
#define CONFIRM_MACRO_DELETE "Confirm"
#define MESSAGE_CONFIRM_MACRO_DELETE "Delete Macro ^1?"
#define PLAY_MACRO "Play Macro"
#define START_RECORD "Record Macro"
#define RECORD "Record"
#define PLAY "Play"
//C++ file text
#define INVALID_FILE_NAME_TITLE "Nome de arq. Inválido"
#define INVALID_FILE_NAME_TEXT "^1 não é um nome válido."
#define SELECTION_MODE "Modo de Seleção"
#define TEXT_BEAM "Enviar"
#define BEAM_FILE "Arquivo Selecionado"
#define BEAM_APP "Enviar SiEd-Dana"
#define PALM_STYLE_TEXT "Estilo Palm"
#define STANDARD_STYLE_TEXT "Padrão"
#define SELECT_ALL "Selecionar Tudo"
#define WAITING_TEXT "Aguarde um momento..." //Text displayed while leaving


//Resource text
#define TEXT_NEW_DIRECTORY "Novo Nome de Diretório"

//simple ones
#define OK "OK"
#define CLOSE "Fechar"
#define OVERWRITE "Sobrescrever"
#define CANCEL "Cancelar"
#define SAVE "Salvar"
#define DISCARD "Discartar"
#define YES "Sim"
#define NO "Não"
#define TEXT_FONT "Fonte"
#define TEXT_NONE "Nenhum"
#define TEXT_SIZE "Tamanho:"
#define TOTAL "Total:"
#define OPEN "Abrir"
#define TEXT_FIND "Procu."
#define TEXT_FIND_COLON "Procu:"
#define TEXT_REPLACE "Subst."
#define DELETE "Deletar"
#define TEXT_DATE "Data:"
#define TEXT_COPY_FULL "Copiar"
#define RENAME "Renomear"
#define TEXT_EXIT "Sair"
#define TEXT_FILE "Arquivo"
#define TEXT_NEW "Novo"
#define PREFERENCES_ABBR "Prefs" //abbreviation for preferences
#define TEXT_ABOUT "Sobre"
#define TEXT_COPY "Copiar"
#define TEXT_PASTE "Colar"
#define TEXT_CUT "Recortar"
#define TEXT_UNDO "Desfazer"
#define TEXT_REDO "Refazer"
#define TEXT_CURSOR "Cursor"
#define TEXT_END "Fim"
#define TEXT_HOME "Início" //as in beginning of line key
#define TEXT_EDIT "Editar" //as in the menu title
#define TEXT_CALCULATE "Calcular" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Estatísticas do Documento"
#define TEXT_DOCUMENT_CHARS "Caracteres:"
#define TEXT_DOCUMENT_WORDS "Palavras:"
#define MENU_TEXT_STATS "Estatísticas"
#define MANAGE_FILES "Arquivos"
//other text
#define NEW_FILENAME "Novo Nome"
#define OLD_FILENAME "Nome Atual"
#define RENAME_FILE "Renomear Arquivo"
#define FIND_AGAIN "Procurar Novamente"
#define SAVE_AS "Salvar Como"
#define SELECT_FILE "Selecionar Arquivo"
#define TERM_NOT_FOUND "Termo não encontrado"
#define END_OF_DOCUMENT_REACHED "Final do Documento"
#define CONFIRM_FILE_DELETE "Confirmar Deletação do Arquivo"
#define MESSAGE_CONFIRM_FILE_DELETE "Deletar arquivo ^1?"

#define CONFIRM_OVERWRITE "Confirmar Sobrescrição"
#define MESSAGE_CONFIRM_OVERWRITE "Abrir um novo arquivo irá apagar as mudanças. Deseja salvar primeiro?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Baseado em SiEd, \251 2003-2005 Benjamin Roe.\nObra de arte \251 Stefan Smith & Tankut Eric.\nEsta aplicação é distribuída sob os termos GPL."

#define NEW_FILE "Novo Arquivo"
#define MESSAGE_NEW_FILE "Abrir um novo arquivo?"
#define EDITOR_PREFERENCES "Preferências do Editor"

#define SAVE_FILE_AS "Salvar Arquivo Como"
#define FILENAME "Nome do Arquivo"

#define SEARCH_TERM "Termo a Procurar"
#define MATCH_CASE "Coincidir Maiusc./Minusc."
#define MATCH_WHOLE_WORD "Palavra Completa"

#define REPLACE_ABBR "Trocar:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Trocar" //abbreviation for replace
#define TEXT_WITH "Com:"
#define REPLACE_ALL_ABBR "Tr. Tu." //abbreviation for replace all
#define END_OF_DOCUMENT "Fim do Documento"
#define START_OF_DOCUMENT "Início do Documento"
#define DELETE_LINE "Deletar Linha"
#define SPLIT_VIEW "Visualização Dividida"
#define UNSPLIT_VIEW "Visualização Única"
#define TEXT_VIEW "Vis."
#define NEW_DIRECTORY "Novo Diretório"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Sem volta!"
#define OVERWRITE_FILE "Sobrescrever Arquivo?"
#define OVERWRITE_FILE_TEXT "Já existe um arquivo com esse nome. Sobrescrever?"
#define UNABLE_TO_UNDO_TEXT "Esta mudança não poderá ser desfeita."
#define TEXT_SELECTION_COPY_TRUNCATED "Seleção truncada."
#define MESSAGE_SELECTION_COPY_TRUNCATED "Espaço insuficiente na área de transferência para esta seleção."

#define MESSAGE_NOT_SUPPORTED "^1 Não Suportado."
#define TEXT_FILE_NOT_FOUND "Arquivo não encontrado"
#define MESSAGE_FILE_NOT_FOUND "Arquivo ^1 não encontrado."
#define TEXT_DATABASE_ACCESS_ERROR "Erro de acesso ao banco de dados"
#define MESSAGE_DATABASE_ACCESS_ERROR "Erro de banco de dados enquanto ^1."
#define TEXT_FILE_ACCESS_ERROR "Erro de acesso de arquivo"
#define MESSAGE_FILE_ACCESS_ERROR "Erro no acesso ao arquivo ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Erro de acesso a tela."
#define MESSAGE_SCREEN_ACCESS_ERROR "Erro na criação da janela."
#define TEXT_UNABLE_TO_SAVE "Não posso salvar"
#define MESSAGE_UNABLE_TO_SAVE "Não posso salvar o arquivo ^1."
#define TEXT_DEBUG_MESSAGE "Mensagem de Debug"
#define TEXT_OUT_OF_MEMORY "Falta de Memória"
#define MESSAGE_OUT_OF_MEMORY "Erro de alocação de memória em ^1"
#define FEATURE_NOT_IMPLEMENTED "Recurso não implementado"

#endif


#ifdef LANGUAGE_NORWEGIAN

//Remaining untranslated strings
#define INC_SEARCH_TEXT "Inkr. Søk" //abbreviation for incremental search
#define FILE_TOO_LARGE "For stor fil"
#define FILE_TOO_LARGE_TEXT "Fil for stor for SiEd-Dana."
#define FILE_TRUNCATION_TEXT "Filen avkuttet pga for lite minne."
#define FILE_TRUNCATION "Minne feil"
#define STOP_RECORD_MENU "Stans innspilling"

#define FROM_TOP "Fra topp"
#define KEYBOARD "Tastatur"
#define INVALID_FILE_NAME_TITLE "Ugyldig filnavn"
#define INVALID_FILE_NAME_TEXT "^1 er ikke et gyldig filnavn."
#define SELECTION_MODE "Utvalgsmodus"
#define TEXT_BEAM "Send"
#define BEAM_FILE "Valgt fil"
#define BEAM_APP "SiEd-Dana Prog."
#define PALM_STYLE_TEXT "Palm stil"
#define STANDARD_STYLE_TEXT "Standard"
#define SELECT_ALL "Velg alt"
#define REVERT_FILE "Gjenåpne fil?"
#define REVERT "Gjenåpne fil"
#define MESSAGE_REVERT_FILE "Gjenåpne aktuell fil? Alle endringer tapes."
#define NONE_TEXT "Ingen"
#define EOL_TEXT "Linjeskift"
#define SHOW_CODES_TEXT "Vis koder"
#define SHOW_MODE "Modus"
#define MACRO_POPUP_TEXT_1 "Innspilte makro"
#define MACRO_POPUP_TEXT_2 "kan spilles"
#define MACRO_POPUP_TEXT_3 "av herfra"
#define GET_PASSWORD "Skriv passord"
#define PASSWORD_TEXT "Passord:"
#define TEXT_ENCRYPTED "Krypter"
#define FILE_NOT_ENCRYPTED_TITLE "Fil er ikke kryptert"
#define FILE_NOT_ENCRYPTED_TEXT "Fil ^1 er ikke kryptert"
#define DECRYPT "Dekrypter"
#define QUICK_MACRO "Hurtig Mak."
#define FILE_LOCATION "Skriv passord for fil"
#define UNABLE_TO_CREATE_DIR_TEXT "Kan ikke lage mappe ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Feil"
#define NO_COMPRESSED_DOC_TITLE "Ikke støttet"
#define NO_COMPRESSED_DOC_TEXT "Fil ^1 er komprimert. Ingen støtte for komprimerte DOK ennå."
#define MUST_ENTER_NAME_TITLE "Ingen navn"
#define MUST_ENTER_NAME_TEXT "Trenger navn for makroen"
#define CONFIRM_MACRO_DELETE "Bekreft"
#define MESSAGE_CONFIRM_MACRO_DELETE "Slett makro ^1?"
#define PLAY_MACRO "Avspill makro"
#define START_RECORD "Innspill makro"
#define RECORD "Innspill"
#define PLAY "Avspill"


#define WAITING_TEXT "Vennligst vent..." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "Nytt mappenavn"

//simple ones
#define OK "OK"
#define CLOSE "Lukk"
#define OVERWRITE "Overskriv"
#define CANCEL "Avbryt"
#define SAVE "Lagre"
#define DISCARD "Forkast"
#define YES "Ja"
#define NO "Nei"
#define TEXT_FONT "Skrifttype"
#define TEXT_NONE "Ingen"
#define TEXT_SIZE "Størrelse:"
#define TOTAL "Total:"
#define OPEN "Åpne"
#define TEXT_FIND "Finn"
#define TEXT_FIND_COLON "Finn:"
#define TEXT_REPLACE "Finn & erstatt"
#define DELETE "Slett"
#define TEXT_DATE "Dato:"
#define TEXT_COPY_FULL "Kopier"
#define RENAME "Gi nytt navn"
#define TEXT_EXIT "Avslutt"
#define TEXT_FILE "Fil"
#define TEXT_NEW "Ny"
#define PREFERENCES_ABBR "Alt." //abbreviation for preferences
#define TEXT_ABOUT "Om"
#define TEXT_COPY "Kopier"
#define TEXT_PASTE "Lim inn"
#define TEXT_CUT "Klipp ut"
#define TEXT_UNDO "Angre"
#define TEXT_REDO "Gjenta"
#define TEXT_CURSOR "Markør"
#define TEXT_END "Linjeslutt"
#define TEXT_HOME "Hjem" //as in beginning of line key
#define TEXT_EDIT "Rediger" //as in the menu title
#define TEXT_CALCULATE "Beregn" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Dokument statistikk"
#define TEXT_DOCUMENT_CHARS "Tegn:"
#define TEXT_DOCUMENT_WORDS "Ord:"
#define MENU_TEXT_STATS "Stats"
#define MANAGE_FILES "Filer"
//other text
#define NEW_FILENAME "Nytt filnavn"
#define OLD_FILENAME "Gammelt filnavn"
#define RENAME_FILE "Endre filnavn"
#define FIND_AGAIN "Finn neste"
#define SAVE_AS "Lagre som"
#define SELECT_FILE "Velg fil"
#define TERM_NOT_FOUND "Fant ikke tekst"
#define END_OF_DOCUMENT_REACHED "Nådde slutten av dokumentet"
#define CONFIRM_FILE_DELETE "Bekreft sletting av fil"
#define MESSAGE_CONFIRM_FILE_DELETE "Slett filen ^1?"

#define CONFIRM_OVERWRITE "Bekreft overskriving"
#define MESSAGE_CONFIRM_OVERWRITE "Endringer vil gå tapt ved åpning av ny fil. Lagre først?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Basert pa SiEd, \251 2003-2005 Benjamin Roe.\nArtwork \251 Stefan Smith & Tankut Eric.\nDenne applikasjonen distribueres under GPL-lisensen. Oversatt til norsk av John Gothard" //Norwegian translation by jorigo@online.no

#define NEW_FILE "Ny fil"
#define MESSAGE_NEW_FILE "Åpne en ny fil?"
#define EDITOR_PREFERENCES "Editor alternativer"

#define SAVE_FILE_AS "Lagre filen som"
#define FILENAME "Filnavn"

#define SEARCH_TERM "Søkeord"
#define MATCH_CASE "Små/store bokst."
#define MATCH_WHOLE_WORD "Hele ordet"

#define REPLACE_ABBR "Erst:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Erst" //abbreviation for replace
#define TEXT_WITH "Med:"
#define REPLACE_ALL_ABBR "Erst. alle" //abbreviation for replace all
#define END_OF_DOCUMENT "Dok slutt"
#define START_OF_DOCUMENT "Dok start"
#define DELETE_LINE "Slett linje"
#define SPLIT_VIEW "Delt skjerm"
#define UNSPLIT_VIEW "Udelt skjerm"
#define TEXT_VIEW "Vindu"
#define NEW_DIRECTORY "Ny mappe"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Kan ikke angres"
#define OVERWRITE_FILE "Overskrive fil?"
#define OVERWRITE_FILE_TEXT "En fil med samme navn ekstisterer. Overskriv?"
#define UNABLE_TO_UNDO_TEXT "Denne forandringen kan ikke angres."
#define TEXT_SELECTION_COPY_TRUNCATED "Utvalg avkuttet"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Ikke plass i utklippstavlet for valget"

#define MESSAGE_NOT_SUPPORTED "^1 støttes ikke."
#define TEXT_FILE_NOT_FOUND "Fant ikke filen"
#define MESSAGE_FILE_NOT_FOUND "Filen ^1 ble ikke funnet."
#define TEXT_DATABASE_ACCESS_ERROR "Databasefeil"
#define MESSAGE_DATABASE_ACCESS_ERROR "Databasefeil ved ^1."
#define TEXT_FILE_ACCESS_ERROR "Filtilgangsfeil"
#define MESSAGE_FILE_ACCESS_ERROR "Feil ved Error in accessing file ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Skjermfeil"
#define MESSAGE_SCREEN_ACCESS_ERROR "Feil ved tilgang til dobbelbuffer."
#define TEXT_UNABLE_TO_SAVE "Kunne ikke lagre"
#define MESSAGE_UNABLE_TO_SAVE "Kunne ikke lagre til filen ^1."
#define TEXT_DEBUG_MESSAGE "Debugmelding"
#define TEXT_OUT_OF_MEMORY "Tom for minne"
#define MESSAGE_OUT_OF_MEMORY "Minnetildelingsfeil ved ^1"
#define FEATURE_NOT_IMPLEMENTED "Funksjonen er ikke implimentert"
#endif
#ifdef LANGUAGE_ITALIAN
#define REVERT_FILE "Reload File?"
#define REVERT "Reload"
#define MESSAGE_REVERT_FILE "Reload current file? All changes will be lost."
#define NONE_TEXT "None"
#define EOL_TEXT "Line Feeds"
#define SHOW_CODES_TEXT "Show Codes"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "If you record a"
#define MACRO_POPUP_TEXT_2 "macro you can"
#define MACRO_POPUP_TEXT_3 "play it from here"
#define GET_PASSWORD "Enter Password"
#define PASSWORD_TEXT "Password:"
#define TEXT_ENCRYPTED "Encrypt"
#define FILE_NOT_ENCRYPTED_TITLE "File Not Encrypt"
#define FILE_NOT_ENCRYPTED_TEXT "File ^1 is not encrypted"
#define DECRYPT "Decrypt"
#define QUICK_MACRO "Quick Mac."
#define FILE_LOCATION "Enter password for file "
#define UNABLE_TO_CREATE_DIR_TEXT "Unable to create directory ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Error"
#define NO_COMPRESSED_DOC_TITLE "Not Supported"
#define NO_COMPRESSED_DOC_TEXT "File ^1 is compressed. No support for compressed DOCs yet."
#define MUST_ENTER_NAME_TITLE "No Name"
#define MUST_ENTER_NAME_TEXT "Must enter a name for the macro"
#define CONFIRM_MACRO_DELETE "Confirm"
#define MESSAGE_CONFIRM_MACRO_DELETE "Delete Macro ^1?"
#define PLAY_MACRO "Play Macro"
#define START_RECORD "Record Macro"
#define RECORD "Record"
#define PLAY "Play"


#define INVALID_FILE_NAME_TITLE "Nome di file non valido"
#define INVALID_FILE_NAME_TEXT "^1 non è un nome di file valido."
#define SELECTION_MODE "Modalità selezione"
#define TEXT_BEAM "Invia IR"
#define BEAM_FILE "File selezionato"
#define BEAM_APP "SiEd-Dana App."
#define PALM_STYLE_TEXT "Stile Palm"
#define STANDARD_STYLE_TEXT "Standard"
#define SELECT_ALL "Seleziona tutto"


#define WAITING_TEXT "Prego attendi..." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "Nuovo nome della directory"

//simple ones
#define OK "OK"
#define CLOSE "Chiudi"
#define OVERWRITE "Sovrascrivi"
#define CANCEL "Cancella"
#define SAVE "Salva"
#define DISCARD "Abbandona"
#define YES "Si"
#define NO "No"
#define TEXT_FONT "Font"
#define TEXT_NONE "Nessuno"
#define TEXT_SIZE "Dimensione:"
#define TOTAL "Totale:"
#define OPEN "Apri"
#define TEXT_FIND "Trova"
#define TEXT_FIND_COLON "Trova:"
#define TEXT_REPLACE "Sostituisci"
#define DELETE "Cancella"
#define TEXT_DATE "Data:"
#define TEXT_COPY_FULL "Copia"
#define RENAME "Rinomina"
#define TEXT_EXIT "Esci"
#define TEXT_FILE "File"
#define TEXT_NEW "Nuovo"
#define PREFERENCES_ABBR "Prefs" //abbreviation for preferences
#define TEXT_ABOUT "Informazioni"
#define TEXT_COPY "Copia"
#define TEXT_PASTE "Incolla"
#define TEXT_CUT "Taglia"
#define TEXT_UNDO "Undo"
#define TEXT_REDO "Redo"
#define TEXT_CURSOR "Cursore"
#define TEXT_END "Fine"
#define TEXT_HOME "Inizio" //as in beginning of line key
#define TEXT_EDIT "Edita" //as in the menu title
#define TEXT_CALCULATE "Conta" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Statistiche Documento "
#define TEXT_DOCUMENT_CHARS "Caratteri:"
#define TEXT_DOCUMENT_WORDS "Parole:"
#define MENU_TEXT_STATS "Statistiche"
#define MANAGE_FILES "Files"
//other text
#define NEW_FILENAME "Nuovo Nome"
#define OLD_FILENAME "Nome Corrente"
#define RENAME_FILE "Rinomina File"
#define FIND_AGAIN "Trova ancora"
#define SAVE_AS "Salva come"
#define SELECT_FILE "Seleziona File"
#define TERM_NOT_FOUND "Termine Non Trovato"
#define END_OF_DOCUMENT_REACHED "Raggiunta Fine del documento"
#define CONFIRM_FILE_DELETE "Conferma Eliminazione File"
#define MESSAGE_CONFIRM_FILE_DELETE "Cancello il file ^1?"

#define CONFIRM_OVERWRITE "Confermi Sovrascrittura"
#define MESSAGE_CONFIRM_OVERWRITE "L'apertura di un nuovo file annullerà le ultime modifiche. Salvo prima il file?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\SiEd-Dana is based on the original SiEd text editor, \251 2003-2005\nBenjamin Roe.\nArtwork is \251 Stefan Smith & Tankut Eric.\nThis application is distributed under the terms of the GPLv2."

#define NEW_FILE "Nuovo File"
#define MESSAGE_NEW_FILE "Apro un nuovo file?"
#define EDITOR_PREFERENCES "Impostazioni Editor"

#define SAVE_FILE_AS "Salva File Come"
#define FILENAME "Nomefile"

#define SEARCH_TERM "Termine ricercato"
#define MATCH_CASE "Maiuscole"
#define MATCH_WHOLE_WORD "Parola intera"

#define REPLACE_ABBR "Sost:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Sost" //abbreviation for replace
#define TEXT_WITH "Con:"
#define REPLACE_ALL_ABBR "Sost t." //abbreviation for replace all
#define END_OF_DOCUMENT "Fine del Doc"
#define START_OF_DOCUMENT "Inizio del Doc"
#define DELETE_LINE "Cancella Linea"
#define SPLIT_VIEW "Dividi Vista"
#define UNSPLIT_VIEW "Riunisci Vista"
#define TEXT_VIEW "Vista"
#define NEW_DIRECTORY "Nuova Dir"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Impossibile Annullare"
#define OVERWRITE_FILE "Sovrascrivo il File?"
#define OVERWRITE_FILE_TEXT "Un file con questo nome esiste già. Sovrascrivo?"
#define UNABLE_TO_UNDO_TEXT "Questa modifica non potrà essere annullata."
#define TEXT_SELECTION_COPY_TRUNCATED "Selezione Troncata"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Spazio Insufficiente in memoria per l'intera selezione"

#define MESSAGE_NOT_SUPPORTED "^1 Non Supportato."
#define TEXT_FILE_NOT_FOUND "File Non trovato"
#define MESSAGE_FILE_NOT_FOUND "File ^1 non trovato."
#define TEXT_DATABASE_ACCESS_ERROR "Errore di accesso al Database"
#define MESSAGE_DATABASE_ACCESS_ERROR "Errore Database  con ^1."
#define TEXT_FILE_ACCESS_ERROR "Errore di Accesso al File"
#define MESSAGE_FILE_ACCESS_ERROR "Errore di  accesso al file ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Errore di Accesso allo schermo"
#define MESSAGE_SCREEN_ACCESS_ERROR "Errore nel creare la finestra fuori schermo."
#define TEXT_UNABLE_TO_SAVE "Impossibile Salvare"
#define MESSAGE_UNABLE_TO_SAVE "Impossibile Salvare il file ^1."
#define TEXT_DEBUG_MESSAGE "Messaggio di Debug"
#define TEXT_OUT_OF_MEMORY "Memoria Piena"
#define MESSAGE_OUT_OF_MEMORY "Errore di Allocazione Memoria in ^1"
#define FEATURE_NOT_IMPLEMENTED "Caratteristica Non Implementata"
#endif

#ifdef LANGUAGE_DUTCH
#define REVERT_FILE "Reload File?"
#define REVERT "Reload"
#define MESSAGE_REVERT_FILE "Reload current file? All changes will be lost."
#define NONE_TEXT "None"
#define EOL_TEXT "Line Feeds"
#define SHOW_CODES_TEXT "Show Codes"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "If you record a"
#define MACRO_POPUP_TEXT_2 "macro you can"
#define MACRO_POPUP_TEXT_3 "play it from here"
#define GET_PASSWORD "Enter Password"
#define PASSWORD_TEXT "Password:"
#define TEXT_ENCRYPTED "Encrypt"
#define FILE_NOT_ENCRYPTED_TITLE "File Not Encrypt"
#define FILE_NOT_ENCRYPTED_TEXT "File ^1 is not encrypted"
#define DECRYPT "Decrypt"
#define QUICK_MACRO "Quick Mac."
#define FILE_LOCATION "Enter password for file "
#define UNABLE_TO_CREATE_DIR_TEXT "Unable to create directory ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Error"
#define NO_COMPRESSED_DOC_TITLE "Not Supported"
#define NO_COMPRESSED_DOC_TEXT "File ^1 is compressed. No support for compressed DOCs yet."
#define MUST_ENTER_NAME_TITLE "No Name"
#define MUST_ENTER_NAME_TEXT "Must enter a name for the macro"
#define CONFIRM_MACRO_DELETE "Confirm"
#define MESSAGE_CONFIRM_MACRO_DELETE "Delete Macro ^1?"
#define PLAY_MACRO "Play Macro"
#define START_RECORD "Record Macro"
#define RECORD "Record"
#define PLAY "Play"
#define INVALID_FILE_NAME_TITLE "Ongeldige bestandsnaam"
#define INVALID_FILE_NAME_TEXT "^1 is geen geldige bestandsnaam."
#define SELECTION_MODE "Selection Mode"
#define TEXT_BEAM "Zend met IR"
#define BEAM_FILE "Geselecteerd bestand"
#define BEAM_APP  "SiEd-Dana App."
#define PALM_STYLE_TEXT "Palm Style"
#define STANDARD_STYLE_TEXT "Standaard"
#define SELECT_ALL "Selecteer Alles"

#define WAITING_TEXT "Moment a.u.b." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "Naam nieuwe map"

//simple ones
#define OK "OK"
#define CLOSE "Sluit"
#define OVERWRITE "Overschrijven"
#define CANCEL "Annuleer"
#define SAVE "Opslaan"
#define DISCARD "Negeer"
#define YES "Ja"
#define NO "Nee"
#define TEXT_FONT "Lettertype"
#define TEXT_NONE "Geen"
#define TEXT_SIZE "Grootte:"
#define TOTAL "Totaal:"
#define OPEN "Open"
#define TEXT_FIND "Zoek"
#define TEXT_FIND_COLON "Zoek:"
#define TEXT_REPLACE "Vervangen"
#define DELETE "Wissen"
#define TEXT_DATE "Datum:"
#define TEXT_COPY_FULL "Kopieer"
#define RENAME "Hernoem"
#define TEXT_EXIT "Exit"
#define TEXT_FILE "Bestand"
#define TEXT_NEW "Nieuw"
#define PREFERENCES_ABBR "Voork." //abbreviation for preferences
#define TEXT_ABOUT "Over"
#define TEXT_COPY "Kopieer"
#define TEXT_PASTE "Plak"
#define TEXT_CUT "Knip"
#define TEXT_UNDO "Ongedaan maken"
#define TEXT_REDO "Opnieuw"
#define TEXT_CURSOR "Cursor"
#define TEXT_END "Eind"
#define TEXT_HOME "Home" //as in beginning of line key
#define TEXT_EDIT "Bewerk" //as in the menu title
#define TEXT_CALCULATE "Calc." //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Document Statistieken"
#define TEXT_DOCUMENT_CHARS "Letters:"
#define TEXT_DOCUMENT_WORDS "Woorden:"
#define MENU_TEXT_STATS "Statistieken"
#define MANAGE_FILES "Bestanden"
//other text
#define NEW_FILENAME "Nieuwe Bestandsnaam"
#define OLD_FILENAME "Huidige Bestandsnaam"
#define RENAME_FILE "Bestand hernoemen"
#define FIND_AGAIN "Opnieuw zoeken"
#define SAVE_AS "Opslaan als"
#define SELECT_FILE "Selecteer Bestand"
#define TERM_NOT_FOUND "Gezochte woord niet gevonden"
#define END_OF_DOCUMENT_REACHED "Eind van document"
#define CONFIRM_FILE_DELETE "Bevestig bestandsverwijdering"
#define MESSAGE_CONFIRM_FILE_DELETE "Verwijder bestand ^1?"

#define CONFIRM_OVERWRITE "Bevestig overschrijven"
#define MESSAGE_CONFIRM_OVERWRITE "Openen van een nieuw bestand wist de aangebrachte veranderingen. Bestand eerst opslaan?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Gebaseerd op SiEd, \251 2003-2005 Benjamin Roe.\nArtwork \251 Stefan Smith & Tankut Eric.\nDit programma wordt verspreid volgens de bepalingen van de GPL."

#define NEW_FILE "Nieuw Bestand"
#define MESSAGE_NEW_FILE "Open een nieuw bestand?"
#define EDITOR_PREFERENCES "Editor voorkeuren"

#define SAVE_FILE_AS "Sla bestand op als"
#define FILENAME "Bestandsnaam"

#define SEARCH_TERM "Zoekterm"
#define MATCH_CASE "Identieke hoofdletters/kleine letters"
#define MATCH_WHOLE_WORD "Hele woord"

#define REPLACE_ABBR "Verv:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Verv" //abbreviation for replace
#define TEXT_WITH "Door:"
#define REPLACE_ALL_ABBR "All. verv." //abbreviation for replace all
#define END_OF_DOCUMENT "Eind van Document"
#define START_OF_DOCUMENT "Begin van Document"
#define DELETE_LINE "Verwijder regel"
#define SPLIT_VIEW "Splits Beeld"
#define UNSPLIT_VIEW "Voeg Beeld samen"
#define TEXT_VIEW "Bekijk"
#define NEW_DIRECTORY "Nieuwe map"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Geen undo mogelijk"
#define OVERWRITE_FILE "Bestand overschrijven?"
#define OVERWRITE_FILE_TEXT "Een bestand met die naam bestaat reeds. Overschrijven?"
#define UNABLE_TO_UNDO_TEXT "Deze verandering kan niet ongedaan gemaakt worden."
#define TEXT_SELECTION_COPY_TRUNCATED "Selectie ingekort"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Klembord is niet groot genoeg voor gehele selectie"

#define MESSAGE_NOT_SUPPORTED "^1 niet ondersteund."
#define TEXT_FILE_NOT_FOUND "bestand niet gevonden"
#define MESSAGE_FILE_NOT_FOUND "Bestand ^1 niet gevonden."
#define TEXT_DATABASE_ACCESS_ERROR "Fout bij openen database"
#define MESSAGE_DATABASE_ACCESS_ERROR "Fout in database tijdens ^1."
#define TEXT_FILE_ACCESS_ERROR "Fout bij openen bestand"
#define MESSAGE_FILE_ACCESS_ERROR "Fout bij openen bestand ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Scherm niet toegankelijk"
#define MESSAGE_SCREEN_ACCESS_ERROR "Fout bij maken van offscreen window."
#define TEXT_UNABLE_TO_SAVE "Fout bij opslaan"
#define MESSAGE_UNABLE_TO_SAVE "Bestand ^1 kon niet opgeslagen worden."
#define TEXT_DEBUG_MESSAGE "Debug Message"
#define TEXT_OUT_OF_MEMORY "Geen geheugen"
#define MESSAGE_OUT_OF_MEMORY "Memory Allocation Error in ^1"
#define FEATURE_NOT_IMPLEMENTED "Feature Not Implemented"

#endif
//Untranslated strings
#ifdef LANGUAGE_TURKISH
#define REVERT_FILE "Reload File?"
#define REVERT "Reload"
#define MESSAGE_REVERT_FILE "Reload current file? All changes will be lost."
#define NONE_TEXT "None"
#define EOL_TEXT "Line Feeds"
#define SHOW_CODES_TEXT "Show Codes"
#define SHOW_MODE "Mode"
#define MACRO_POPUP_TEXT_1 "If you record a"
#define MACRO_POPUP_TEXT_2 "macro you can"
#define MACRO_POPUP_TEXT_3 "play it from here"
#define GET_PASSWORD "Sifre giriniz"
#define PASSWORD_TEXT "Sifre:"
#define TEXT_ENCRYPTED "Sifrele"
#define FILE_NOT_ENCRYPTED_TITLE "Dosya sifreli degil"
#define FILE_NOT_ENCRYPTED_TEXT "^1 dosyasi sifreli degil"
#define DECRYPT "Desifre"
#define QUICK_MACRO "Hizli makro"
#define FILE_LOCATION "Dosya icin sifre giriniz "
#define UNABLE_TO_CREATE_DIR_TEXT "^1 klasoru yaratilamadi"
#define UNABLE_TO_CREATE_DIR_TITLE "Hata"
#define NO_COMPRESSED_DOC_TITLE "Desteklenmiyor"
#define NO_COMPRESSED_DOC_TEXT "^1 dosyasi sikistirilmis. Henuz bu tur DOC dosyalari acilamiyor."
#define MUST_ENTER_NAME_TITLE "Isim yok"
#define MUST_ENTER_NAME_TEXT "Makro icin isim girmelisiniz"
#define CONFIRM_MACRO_DELETE "Onaylayiniz"
#define MESSAGE_CONFIRM_MACRO_DELETE "Makro silinsin mi ^1?"
#define PLAY_MACRO "Makro calistir"
#define START_RECORD "Makro kaydet"
#define RECORD "Kaydet"
#define PLAY "Calistir"

//C++ file text
#define INVALID_FILE_NAME_TITLE "Gecersiz dosya adi"
#define INVALID_FILE_NAME_TEXT "^1 gecerli bir dosya degil."
#define SELECTION_MODE "Secme kipi"
#define TEXT_BEAM "Kizilotesi"
#define BEAM_FILE "Secili dosya"
#define BEAM_APP "SiEd-Dana Prog."
#define PALM_STYLE_TEXT "Palm Tarzi"
#define STANDARD_STYLE_TEXT "Standart"
#define SELECT_ALL "Tumunu Sec"
#define WAITING_TEXT "Bekleyiniz..." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "Yeni klasor adi"

//simple ones
#define OK "Tamam"
#define CLOSE "Kapat"
#define OVERWRITE "Ustune Yaz"
#define CANCEL "Iptal"
#define SAVE "Kaydet"
#define DISCARD "Kaybet"
#define YES "Evet"
#define NO "Hayir"
#define TEXT_FONT "Font:"
#define TEXT_NONE "Hicbiri"
#define TEXT_SIZE "Boy:"
#define TOTAL "Toplam:"
#define OPEN "Ac"
#define TEXT_FIND "Bul"
#define TEXT_FIND_COLON "Bul:"
#define TEXT_REPLACE "Degistir"
#define DELETE "Sil"
#define TEXT_DATE "Tarih:"
#define TEXT_COPY_FULL "Kopyala"
#define RENAME "Isim degistir"
#define TEXT_EXIT "Cikis"
#define TEXT_FILE "Dosya"
#define TEXT_NEW "Yeni"
#define PREFERENCES_ABBR "Secenek" //abbreviation for preferences
#define TEXT_ABOUT "Hakkinda"
#define TEXT_COPY "Kopyala"
#define TEXT_PASTE "Yapistir"
#define TEXT_CUT "Kes"
#define TEXT_UNDO "Geri Al"
#define TEXT_REDO "Tekrarla"
#define TEXT_CURSOR "Imlec"
#define TEXT_END "Son"
#define TEXT_HOME "Basa don" //as in beginning of line key
#define TEXT_EDIT "Duzenle" //as in the menu title
#define TEXT_CALCULATE "Hesapla" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Dokuman Istatistikleri:"
#define TEXT_DOCUMENT_CHARS "Karakter:"
#define TEXT_DOCUMENT_WORDS "Kelime:"
#define MENU_TEXT_STATS "Istatistik"
#define MANAGE_FILES "Dosyalar"
//other text
#define NEW_FILENAME "Yeni Dosya Adi"
#define OLD_FILENAME "Gecerli Dosya Adi"
#define RENAME_FILE "Isim Degistir"
#define FIND_AGAIN "Tekrar Bul"
#define SAVE_AS "Farkli Kaydet"
#define SELECT_FILE "Dosya Sec"
#define TERM_NOT_FOUND "Bulunamadi"
#define END_OF_DOCUMENT_REACHED "Dokuman sonuna ulasildi"
#define CONFIRM_FILE_DELETE "Dosya silmeyi onaylayiniz."
#define MESSAGE_CONFIRM_FILE_DELETE "^1 silinsin mi?"

#define CONFIRM_OVERWRITE "Uzerine yazmayi onaylayiniz"
#define MESSAGE_CONFIRM_OVERWRITE "Yeni dosya acarsaniz degisiklikleri kaybedersiniz.  Once kaydedelim mi?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Dayali SiEd, \251 2003-2005 Benjamin Roe.\nSanat eseri \251 Stefan Smith & Tankut Eric.\nBu yazilim GPL anlasmasi kurallari altinda dagitilmaktadir."

#define NEW_FILE "Yeni Dosya"
#define MESSAGE_NEW_FILE "Yeni dosya acilsin mi?"
#define EDITOR_PREFERENCES "Editor Secenekleri"

#define SAVE_FILE_AS "Farkli Kaydet"
#define FILENAME "Dosya Adi"

#define SEARCH_TERM "Aranacak terim"
#define MATCH_CASE "Kucuk/Buyuk eslestir"
#define MATCH_WHOLE_WORD "Tam kelime"

#define REPLACE_ABBR "Bunu:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Bunu" //abbreviation for replace
#define TEXT_WITH "Bununla:"
#define REPLACE_ALL_ABBR "Tum D" //abbreviation for replace all
#define END_OF_DOCUMENT "Son"
#define START_OF_DOCUMENT "Baslangic"
#define DELETE_LINE "Satir Sil"
#define SPLIT_VIEW "Ekrani Bol"
#define UNSPLIT_VIEW "Ekrani Birlestir"
#define TEXT_VIEW "Gorunum"
#define NEW_DIRECTORY "Yeni Klasor"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Geri Alinamaz"
#define OVERWRITE_FILE "Uzerine yazilsin mi?"
#define OVERWRITE_FILE_TEXT "Ayni isimle dosya var. Degistir?"
#define UNABLE_TO_UNDO_TEXT ".Bu degisiklik geri alinamaz."
#define TEXT_SELECTION_COPY_TRUNCATED "Secim kirpildi"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Tum secim icin yeterli alan yok"

#define MESSAGE_NOT_SUPPORTED "^1 Desteklenmiyor."
#define TEXT_FILE_NOT_FOUND "Dosya Bulunamadi"
#define MESSAGE_FILE_NOT_FOUND "^1 bulunamadi."
#define TEXT_DATABASE_ACCESS_ERROR "Veritabani erisim hatasi"
#define MESSAGE_DATABASE_ACCESS_ERROR "^1 yapilirken erisim hatasi."
#define TEXT_FILE_ACCESS_ERROR "Dosya Erisim Hatasi"
#define MESSAGE_FILE_ACCESS_ERROR "^1 dosyasina erisirken hata."
#define TEXT_SCREEN_ACCESS_ERROR "Ekran Erisim Hatasi"
#define MESSAGE_SCREEN_ACCESS_ERROR "Ekran disi pencere yaratilamadi."
#define TEXT_UNABLE_TO_SAVE "Kaydedilemiyor"
#define MESSAGE_UNABLE_TO_SAVE "^1 dosyasi kaydedilemedi."
#define TEXT_DEBUG_MESSAGE "Debug Mesaji"
#define TEXT_OUT_OF_MEMORY "Hafiza Kalmadi"
#define MESSAGE_OUT_OF_MEMORY "^1da hafiza alinamadi"
#define FEATURE_NOT_IMPLEMENTED "Ozellik henuz uygulanmadi"

#endif

#ifdef LANGUAGE_RUSSIAN
#define REVERT_FILE "Reload File?"
#define REVERT "Reload"
#define MESSAGE_REVERT_FILE "Ïåðåçàãðóçèòü ôàéë? Âñå èçìåíåíÿ áóäóò ïîòåðÿíû."
#define GET_PASSWORD "Ââåäèòå ïàðîëü"
#define PASSWORD_TEXT "Ïàðîëü:"
#define TEXT_ENCRYPTED "Êîäèð."
#define FILE_NOT_ENCRYPTED_TITLE "Ôàéë íå çàêîäèðîâàí"
#define FILE_NOT_ENCRYPTED_TEXT "Ôàéë ^1 íå çàêîäèðîâàí"
#define DECRYPT "Ðàñêîä."
#define QUICK_MACRO "Ìàêðîñ"
#define FILE_LOCATION "Ââåäèòå ïàðîëü äëÿ ôàéëà"
#define UNABLE_TO_CREATE_DIR_TEXT "Íåâîçìîæíî ñîçäàòü äèðåêòîðèé ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Îøèáêà"
#define NO_COMPRESSED_DOC_TITLE "Íå ïîääåðæèâàåòñÿ"
#define NO_COMPRESSED_DOC_TEXT "Ôàéë ^1 ñæàò. Ñæàòûå DOC ôàéëû íå ïîääåðæèâàþòñÿ."
#define MUST_ENTER_NAME_TITLE "Áåç èìåíè"
#define MUST_ENTER_NAME_TEXT "Ââåäèòå èìÿ ìàêðîñà"
#define CONFIRM_MACRO_DELETE "Ïîäòâåðäèòå"
#define MESSAGE_CONFIRM_MACRO_DELETE "Óäàëèòü ìàêðîñ ^1?"
#define PLAY_MACRO "Ñïèñîê ìàêðîñîâ"
#define START_RECORD "Çàïèñàòü ìàêðîñ"
#define RECORD "Çàïèñü"
#define PLAY "Çàïóñê"

//C++ file text
#define INVALID_FILE_NAME_TITLE "Íåïðàâèëüíîå èìÿ ôàéëà"
#define INVALID_FILE_NAME_TEXT "^1 íåïðàâèëüíîå èìÿ ôàéëà."
#define SELECTION_MODE "Âûáîð ðåæèìà"
#define TEXT_BEAM "Ïåðåäàòü ïî ÈÊ"
#define BEAM_FILE "Âûáðàííûé ôàéë"
#define BEAM_APP "SiEd-Dana"
#define PALM_STYLE_TEXT "Ñòèëü Palm"
#define STANDARD_STYLE_TEXT "Ñòàíäàðòíûé"
#define SELECT_ALL "Âûáðàòü âñå"
#define WAITING_TEXT "Æäèòå..." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "Èìÿ íîâîé äèðåêòîðèè"

//simple ones
#define OK "ÎÊ"
#define CLOSE "Çàêðûòü"
#define OVERWRITE "Äà"
#define CANCEL "Îòìåíà"
#define SAVE "Ñîõðàíèòü"
#define DISCARD "Íåò"
#define YES "Äà"
#define NO "Íåò"
#define TEXT_FONT "Øðèôò:"
#define TEXT_NONE "Íåò"
#define TEXT_SIZE "Ðàçìåð:"
#define TOTAL "Îáùèé:"
#define OPEN "Îòêðûòü"
#define TEXT_FIND "Íàéòè"
#define TEXT_FIND_COLON "Íàéòè:"
#define TEXT_REPLACE "Çàìåíèòü"
#define DELETE "Óäàëèòü"
#define TEXT_DATE "Äàòà:"
#define TEXT_COPY_FULL "Êîïèðîâàòü"
#define RENAME "Ïåðåèìåíîâàòü"
#define TEXT_EXIT "Âûõîä"
#define TEXT_FILE "Ôàéë"
#define TEXT_NEW "Íîâûé"
#define PREFERENCES_ABBR "Ñâîéñòâà" //abbreviation for preferences
#define TEXT_ABOUT "Î ïðîãðàììå"
#define TEXT_COPY "Êîïèðîâàòü"
#define TEXT_PASTE "Âñòàâèòü"
#define TEXT_CUT "Âûðåçàòü"
#define TEXT_UNDO "Îòìåíà"
#define TEXT_REDO "Âîññòàíîâèòü"
#define TEXT_CURSOR "Êóðñîð"
#define TEXT_END "Â êîíåö"
#define TEXT_HOME "Â íà÷àëî" //as in beginning of line key
#define TEXT_EDIT "Ðåä" //as in the menu title
#define TEXT_CALCULATE "Ñ÷åò." //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Ñòàòèñòèêà"
#define TEXT_DOCUMENT_CHARS "Ñèìâîëîâ:"
#define TEXT_DOCUMENT_WORDS "Ñëîâ:"
#define MENU_TEXT_STATS "Ñòàò"
#define MANAGE_FILES "Ôàéëû"
//other text
#define NEW_FILENAME "Íîâîå èìÿ"
#define OLD_FILENAME "Ñóùåñòâóþùåå èìÿ"
#define RENAME_FILE "Ïåðåèìåíîâàòü ôàéë"
#define FIND_AGAIN "Íàéòè åùå"
#define SAVE_AS "Ñîõðàíèòü êàê"
#define SELECT_FILE "Âûáðàòü ôàéë"
#define TERM_NOT_FOUND "Íå íàéäåíî"
#define END_OF_DOCUMENT_REACHED "Êîíåö äîêóìåíòà"
#define CONFIRM_FILE_DELETE "Ïîäòâåðäèòå!"
#define MESSAGE_CONFIRM_FILE_DELETE "Óäàëèòü ôàéë ^1?"
#define MACRO_POPUP_TEXT_1 "Çàïèøèòå ìàêðîñ"
#define MACRO_POPUP_TEXT_2 "è çàïóñêàéòå åãî"
#define MACRO_POPUP_TEXT_3 "â ýòîì îêíå."

#define CONFIRM_OVERWRITE "Ïîäòâåðäèòå!"
#define MESSAGE_CONFIRM_OVERWRITE "Îòêðûòèå íîâîãî ôàéëà óíè÷òîæåò èçìåíåíèÿ â ñòàðîì. Ñîõðàíèòü ôàéë?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\SiEd-Dana is based on the original SiEd text editor, \251 2003-2005\nBenjamin Roe.\nArtwork is \251 Stefan Smith & Tankut Eric.\nThis application is distributed under the terms of the GPLv2."

#define NEW_FILE "Íîâûé ôàéë!"
#define MESSAGE_NEW_FILE "Îòêðûòü íîâûé ôàéë?"
#define EDITOR_PREFERENCES "Íàñòðîéêè"

#define SAVE_FILE_AS "Ñîõðàíèòü êàê..."
#define FILENAME "Èìÿ ôàéëà"

#define SEARCH_TERM "Èñêàòü"
#define MATCH_CASE "Ñî÷åòàíèå"
#define MATCH_WHOLE_WORD "Ñëîâî öåëèêîì"

#define REPLACE_ABBR "Çàìåíà:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Çàìåíà" //abbreviation for replace
#define TEXT_WITH "Ñ:"
#define REPLACE_ALL_ABBR "Âñå" //abbreviation for replace all
#define END_OF_DOCUMENT "Êîíåö"
#define START_OF_DOCUMENT "Íà÷àëî"
#define DELETE_LINE "Óäàëèòü ñòðîêó"
#define SPLIT_VIEW "Äâà äèñïëåÿ"
#define UNSPLIT_VIEW "Îäèí äèñïëåé"
#define TEXT_VIEW "Ïðîñìîòð"
#define NEW_DIRECTORY "Íîâàÿ äèðåêòîðèÿ"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Íåò îòìåíû"
#define OVERWRITE_FILE "Ïåðåïèñàòü?"
#define OVERWRITE_FILE_TEXT "Ôàéë ñóùåñòâóåò. Ïåðåïèñàòü?"
#define UNABLE_TO_UNDO_TEXT "Èçìåíåíèÿ íå ìîãóò áûòü îòìåíåíû."
#define TEXT_SELECTION_COPY_TRUNCATED "Íå âûáðàíî"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Ìàëî ìåñòà â áóôåðå îáìåíà"

#define MESSAGE_NOT_SUPPORTED "^1 íå ïîääåðæèâàåòñÿ."
#define TEXT_FILE_NOT_FOUND "Ôàéë íå íàéäåí"
#define MESSAGE_FILE_NOT_FOUND "Ôàéë ^1 íå íàéäåí."
#define TEXT_DATABASE_ACCESS_ERROR "Îøèáêà äîñòóïà"
#define MESSAGE_DATABASE_ACCESS_ERROR "Îøèáêà â áàçå ^1."
#define TEXT_FILE_ACCESS_ERROR "Íåò äîñòóïà ê ôàéëó"
#define MESSAGE_FILE_ACCESS_ERROR "Íåò äîñòóïà ê ôàéëó ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Îøèáêà äèñïëåÿ"
#define MESSAGE_SCREEN_ACCESS_ERROR "Íå ìîãó ñîçäàòü îêíî"
#define TEXT_UNABLE_TO_SAVE "Íå ìîãó ñîõðàíèòü"
#define MESSAGE_UNABLE_TO_SAVE "Íå ìîãó ñîõðàíèòü ôàéë ^1."
#define TEXT_DEBUG_MESSAGE "Îøèáêà ñèñòåìû"
#define TEXT_OUT_OF_MEMORY "Íå äîñòàòî÷íî ïàìÿòè"
#define MESSAGE_OUT_OF_MEMORY "Îøèáêà ðàñïðåäåëåíèÿ ïàìÿòè ^1"
#define FEATURE_NOT_IMPLEMENTED "Íå ðàçðàáàòûâàëîñü"
#define NONE_TEXT "Íåò"
#define EOL_TEXT "Ðàçð.ñòðîê:"
#define SHOW_CODES_TEXT "Ïîêàçûâàòü:"
#define SHOW_MODE "Ðåæèì"

#endif

#ifdef LANGUAGE_POLISH
#define REVERT_FILE "Zaladowac ponownie plik?"
#define MESSAGE_REVERT_FILE "Czy zaladowac ponownie edytowany plik? Wszystkie zmiany zostana stracone!"
#define REVERT "Przeladowaæ plik?"
#define NONE_TEXT "Brak"
#define EOL_TEXT "Przejscie do nastepnej lini"
#define SHOW_CODES_TEXT "Pokaz kody"
#define SHOW_MODE "Tryb"
#define MACRO_POPUP_TEXT_1 "Jesli zapiszesz makro, "
#define MACRO_POPUP_TEXT_2 "to pozniej mozesz"
#define MACRO_POPUP_TEXT_3 "Uruchomic je tu"
#define GET_PASSWORD "Wprowadz haslo"
#define PASSWORD_TEXT "Haslo:"
#define TEXT_ENCRYPTED "Zaszyfrowany"
#define FILE_NOT_ENCRYPTED_TITLE "Plik niezaszyfrowany"
#define FILE_NOT_ENCRYPTED_TEXT "Plik ^1 jest niezaszyfrowany"
#define DECRYPT "Odszyfruj"
#define QUICK_MACRO "Szybkie Makro"
#define FILE_LOCATION "Podaj  haslo dla podanego pliku"
#define UNABLE_TO_CREATE_DIR_TEXT "Nie mozna utworzyc podanego katalogu ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Blad tworzenia katalogu"
#define NO_COMPRESSED_DOC_TITLE "Opcja niezaimplementowana"
#define NO_COMPRESSED_DOC_TEXT "Plik ^1 jest skompresowany. Opcja jeszcze niezaimplementowana."
#define MUST_ENTER_NAME_TITLE "Brak nazwy pliku"
#define MUST_ENTER_NAME_TEXT "Musisz podac nazwe makra"
#define CONFIRM_MACRO_DELETE "Zatwierdz"
#define MESSAGE_CONFIRM_MACRO_DELETE "Usunac makro?"
#define PLAY_MACRO "Odtworz makro"
#define START_RECORD "Zapisz makro"
#define RECORD "Zapisz"
#define PLAY "Odtworz"
//C++ file text
#define INVALID_FILE_NAME_TITLE "Bledna nazwa pliku"
#define INVALID_FILE_NAME_TEXT "^1 nie jest poprawna nazwa pliku."
#define SELECTION_MODE "Tryb zaznaczania"
#define TEXT_BEAM "Podczerwien"
#define BEAM_FILE "Wybierz plik"
#define BEAM_APP "SiEd-Dana App."
#define PALM_STYLE_TEXT "Palm Style"
#define STANDARD_STYLE_TEXT "Standard"
#define SELECT_ALL "Zaznacz wszystko"
#define WAITING_TEXT "Poczekaj..." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "Nowa nazwa katalogu"

//simple ones
#define OK "OK"
#define CLOSE "Zamknij"
#define OVERWRITE "Nadpisz"
#define CANCEL "Anuluj"
#define SAVE "Zapisz"
#define DISCARD "Odrzuc"
#define YES "Tak"
#define NO "Nie"
#define TEXT_FONT "Font"
#define TEXT_NONE "Brak"
#define TEXT_SIZE "Wielkosc:"
#define TOTAL "Calkowita:"
#define OPEN "Otworz"
#define TEXT_FIND "Znajdz"
#define TEXT_FIND_COLON "Znajdz:"
#define TEXT_REPLACE "Znajdz i zamien"
#define DELETE "Usun"
#define TEXT_DATE "Data:"
#define TEXT_COPY_FULL "Kopiuj"
#define RENAME "Zmien nazwe"
#define TEXT_EXIT "Wyjdz"
#define TEXT_FILE "Plik"
#define TEXT_NEW "Nowy"
#define PREFERENCES_ABBR "Pref." //abbreviation for preferences
#define TEXT_ABOUT "O programie"
#define TEXT_COPY "Kopiuj"
#define TEXT_PASTE "Wstaw"
#define TEXT_CUT "Wytnij"
#define TEXT_UNDO "Cofnij"
#define TEXT_REDO "Redo"
#define TEXT_CURSOR "Kursor"
#define TEXT_END "Koniec"
#define TEXT_HOME "Na poczatek" //as in beginning of line key
#define TEXT_EDIT "Edytuj" //as in the menu title
#define TEXT_CALCULATE "Przelicz" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Statystyka tekstu"
#define TEXT_DOCUMENT_CHARS "Liczba znakow:"
#define TEXT_DOCUMENT_WORDS "Slow:"
#define MENU_TEXT_STATS "Stat"
#define MANAGE_FILES "Pliki"
//other text
#define NEW_FILENAME "Nowa nazwa pliku"
#define OLD_FILENAME "Obecna nazwa pliku"
#define RENAME_FILE "Zmien nazwe pliku"
#define FIND_AGAIN "Znajdz ponownie"
#define SAVE_AS "Zapisz jako"
#define SELECT_FILE "Wybierz plik"
#define TERM_NOT_FOUND "Nieznaleziono tekstu"
#define END_OF_DOCUMENT_REACHED "Osiagnieto koniec pliku"
#define CONFIRM_FILE_DELETE "Zatwierdz  usuniecie pliku"
#define MESSAGE_CONFIRM_FILE_DELETE "Usunac plik ^1?"

#define CONFIRM_OVERWRITE "Zatwierdz nadpisanie"
#define MESSAGE_CONFIRM_OVERWRITE "Otwarcie nowego pliku spowoduje utrate zmian w pliku obecnym.  Zapiac zmiany?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\SiEd-Dana is based on the original SiEd text editor, \251 2003-2005\nBenjamin Roe.\nArtwork is \251 Stefan Smith & Tankut Eric.\nThis application is distributed under the terms of the GPLv2."

#define NEW_FILE "Nowy plik"
#define MESSAGE_NEW_FILE "Otworzyc nowy plik?"
#define EDITOR_PREFERENCES "Ustawienia edytora"

#define SAVE_FILE_AS "Zapisz plik jako.."
#define FILENAME "Nazwa pliku"

#define SEARCH_TERM "Szukaj tekstu"
#define MATCH_CASE "Wielkosc liter"
#define MATCH_WHOLE_WORD "Cale slowa"

#define REPLACE_ABBR "Zast:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Zast" //abbreviation for replace
#define TEXT_WITH "przez:"
#define REPLACE_ALL_ABBR "Z wszyst." //abbreviation for replace all
#define END_OF_DOCUMENT "Koniec pliku"
#define START_OF_DOCUMENT "Poczatek pliku"
#define DELETE_LINE "Usun linie"
#define SPLIT_VIEW "Podziel widok"
#define UNSPLIT_VIEW "Polacz widok"
#define TEXT_VIEW "Widok"
#define NEW_DIRECTORY "Nowy katalog"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Nie mozna cofnac"
#define OVERWRITE_FILE "Nadpisac plik?"
#define OVERWRITE_FILE_TEXT "Plik juz istnieje. Nadpisac?"
#define UNABLE_TO_UNDO_TEXT "Nie mozna cofnac tej zmiany."
#define TEXT_SELECTION_COPY_TRUNCATED "Zaznaczenie uciete"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Brak miejsca w schowku by zaznaczyc wszystko"

#define MESSAGE_NOT_SUPPORTED "^1 Nie wspierane."
#define TEXT_FILE_NOT_FOUND "Nieznaleziono pliku"
#define MESSAGE_FILE_NOT_FOUND "Plik ^1 nie zostal znaleziony."
#define TEXT_DATABASE_ACCESS_ERROR "Blad dostepu do bazy danych"
#define MESSAGE_DATABASE_ACCESS_ERROR "Blad bazy danych podczas ^1."
#define TEXT_FILE_ACCESS_ERROR "Blad dostepu do pliku"
#define MESSAGE_FILE_ACCESS_ERROR "Blad podczas dostepu do pliku ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Blad dostepu do ekranu"
#define MESSAGE_SCREEN_ACCESS_ERROR "Blad tworzenia niewidocznej czesci ekranu."
#define TEXT_UNABLE_TO_SAVE "Nie mozna zapisac"
#define MESSAGE_UNABLE_TO_SAVE "Nie mozna zapisac pliku ^1."
#define TEXT_DEBUG_MESSAGE "Debug Message"
#define TEXT_OUT_OF_MEMORY "Zabraklo pamieci"
#define MESSAGE_OUT_OF_MEMORY "Blad alokacji pamieci podczas ^1"
#define FEATURE_NOT_IMPLEMENTED "Funkcja nie zostala jeszcze zaimplementowana"
#endif
#ifdef LANGUAGE_SWEDISH
#define REVERT_FILE "Ladda om filen?"
#define MESSAGE_REVERT_FILE "Ladda om aktuell fil? Alla ändringar förloras."
#define REVERT "Återgå"
#define NONE_TEXT "Ingen"
#define EOL_TEXT "Radbrytningar"
#define SHOW_CODES_TEXT "Visa kod"
#define SHOW_MODE "Läge"
#define MACRO_POPUP_TEXT_1 "Om du spelar in ett"
#define MACRO_POPUP_TEXT_2 "makro kan du"
#define MACRO_POPUP_TEXT_3 "spela upp det härifrån"
#define GET_PASSWORD "Ange Lösenord"
#define PASSWORD_TEXT "Lösenord:"
#define TEXT_ENCRYPTED "Kryptera"
#define FILE_NOT_ENCRYPTED_TITLE "Filen ej krypterad"
#define FILE_NOT_ENCRYPTED_TEXT "Fil ^1 är ej krypterad"
#define DECRYPT "Dekryptera"
#define QUICK_MACRO "Quick Mac." // ???
#define FILE_LOCATION "Ange lösenord för filen "
#define UNABLE_TO_CREATE_DIR_TEXT "Kunde inte skapa katalog ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Fel"
#define NO_COMPRESSED_DOC_TITLE "Stöd saknas"
#define NO_COMPRESSED_DOC_TEXT "Filen ^1 är komprimerad. Stöd för komprimerade DOC-filer saknas."
#define MUST_ENTER_NAME_TITLE "Inget Namn"
#define MUST_ENTER_NAME_TEXT "Ett namn krävs för makrot"
#define CONFIRM_MACRO_DELETE "Bekräfta"
#define MESSAGE_CONFIRM_MACRO_DELETE "Radera makro?"
#define PLAY_MACRO "Spela makro"
#define START_RECORD "Spela in makro"
#define RECORD "Spela in"
#define PLAY "Spela upp"
//C++ file text
#define INVALID_FILE_NAME_TITLE "Ogiltigt filnamn"
#define INVALID_FILE_NAME_TEXT "^1 är inte ett giltigt filnamn."
#define SELECTION_MODE "Markerings läge"
#define TEXT_BEAM "Beama"
#define BEAM_FILE "Aktuell fil"
#define BEAM_APP "Programmet SiEd-Dana"
#define PALM_STYLE_TEXT "Palm"
#define STANDARD_STYLE_TEXT "Standard"
#define SELECT_ALL "Markera allt"
#define WAITING_TEXT "Vänligen vänta..." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "Ny katalog"

//simple ones
#define OK "OK"
#define CLOSE "Stäng"
#define OVERWRITE "Skriv över"
#define CANCEL "Avbryt"
#define SAVE "Spara"
#define DISCARD "Överge"
#define YES "Ja"
#define NO "Nej"
#define TEXT_FONT "Font"
#define TEXT_NONE "None"
#define TEXT_SIZE "Size:"
#define TOTAL "Total:"
#define OPEN "Öppna"
#define TEXT_FIND "Sök"
#define TEXT_FIND_COLON "Sök:"
#define TEXT_REPLACE "Sök och ersätt"
#define DELETE "Radera"
#define TEXT_DATE "Daum:"
#define TEXT_COPY_FULL "Kopiera"
#define RENAME "Döp om"
#define TEXT_EXIT "Avsluta"
#define TEXT_FILE "Arkiv"// As in file menu? Otherwise File = Fil
#define TEXT_NEW "Ny"
#define PREFERENCES_ABBR "Inställningar" // The shorter "Alternativ" would work too.
#define TEXT_ABOUT "Om"
#define TEXT_COPY "Kopiera"
#define TEXT_PASTE "Klistra in"
#define TEXT_CUT "Klipp ut"
#define TEXT_UNDO "Ångra"
#define TEXT_REDO "Gör om"
#define TEXT_CURSOR "Markör"
#define TEXT_END "Till slutet"
#define TEXT_HOME "Till början" // Literally "Too start". "Hem" ("Home") would work too.
#define TEXT_EDIT "Redigera"
#define TEXT_CALCULATE "Räkna"
#define TEXT_DOCUMENT_STATISTICS "Dokumentstatistik"
#define TEXT_DOCUMENT_CHARS "Tecken:"
#define TEXT_DOCUMENT_WORDS "Ord:"
#define MENU_TEXT_STATS "Stats"// Unless "Statistik" will fit.
#define MANAGE_FILES "Filer"
//other text
#define NEW_FILENAME "Nytt filnamn"
#define OLD_FILENAME "Nuvarande filnamn"
#define RENAME_FILE "Döm om fil"
#define FIND_AGAIN "Sök nästa"
#define SAVE_AS "Spara som"
#define SELECT_FILE "Välj fil"
#define TERM_NOT_FOUND "Ord hittades ej"
#define END_OF_DOCUMENT_REACHED "Slut på dokumentet"
#define CONFIRM_FILE_DELETE "Bekräfta radera fil"
#define MESSAGE_CONFIRM_FILE_DELETE "Radera fil ^1?"

#define CONFIRM_OVERWRITE "Bekräfta överskrivning"
#define MESSAGE_CONFIRM_OVERWRITE "Öppna ny fil raderar gjorda ändringar.Spara filen först?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\251 Baserat pa SiEd, \251 2003-2005 Benjamin Roe.\nKonstverk \251 Stefan Smith & Tankut Eric.\netta program distribueras enligt villkoren i GPL. "

#define NEW_FILE "Ny Fil"
#define MESSAGE_NEW_FILE "Öppna en ny fil?"
#define EDITOR_PREFERENCES "Inställningar"

#define SAVE_FILE_AS "Spara fil som"
#define FILENAME "Filnamn"

#define SEARCH_TERM "Sökord"
#define MATCH_CASE "Små/stora bokstäver"
#define MATCH_WHOLE_WORD "Hela ordet"

#define REPLACE_ABBR "Ersätt:"
#define REPLACE_ABBR_BUTTON "Ers." // If necessary. Will obviously work above as well.
#define TEXT_WITH "Med:"
#define REPLACE_ALL_ABBR "E alla" //abbreviation for replace all
#define END_OF_DOCUMENT "Slutet av dokumentet" // dok. would work
#define START_OF_DOCUMENT "Början av dokumentet" // here too
#define DELETE_LINE "Radera rad"
#define SPLIT_VIEW "Dela fönster"
#define UNSPLIT_VIEW "Ett fönster" // Literally "One window". Couldn't think of a better translation . . .
#define TEXT_VIEW "Fönster" // "View" would work too. Shorter.
#define NEW_DIRECTORY "Ny katalog"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Kan ej ångra"
#define OVERWRITE_FILE "Skriva över fil?"
#define OVERWRITE_FILE_TEXT "En fil med det namnet finns. Skriva över?"
#define UNABLE_TO_UNDO_TEXT "Denna förändring kan inte ångras."
#define TEXT_SELECTION_COPY_TRUNCATED "Markering trunkerad"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Hela markeringen får inte plats i urklipp."

#define MESSAGE_NOT_SUPPORTED "^1 stöds ej."
#define TEXT_FILE_NOT_FOUND "Hittar ej filen"
#define MESSAGE_FILE_NOT_FOUND "File ^1 hittas ej."
#define TEXT_DATABASE_ACCESS_ERROR "Databasfel"
#define MESSAGE_DATABASE_ACCESS_ERROR "Databasfel vid ^1."
#define TEXT_FILE_ACCESS_ERROR "Filåtkomstfel"
#define MESSAGE_FILE_ACCESS_ERROR "Fel vid åtkomst av fil ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Skärmfel"
#define MESSAGE_SCREEN_ACCESS_ERROR "Error in creating offscreen window."  // Not sure what this is
#define TEXT_UNABLE_TO_SAVE "Kunde inte spara"
#define MESSAGE_UNABLE_TO_SAVE "Kunde inte spara filen ^1."
#define TEXT_DEBUG_MESSAGE "Debug-meddelande"
#define TEXT_OUT_OF_MEMORY "Slut på minne"
#define MESSAGE_OUT_OF_MEMORY "Minnesallokeringsfel i ^1"
#define FEATURE_NOT_IMPLEMENTED "Funktionen inte Implementerad"
#endif
#ifdef LANGUAGE_CZECH
#define REVERT_FILE "Obnovit Soubor?"
#define MESSAGE_REVERT_FILE "Obnovit tento soubor? Vechny zmìny budou ztraceny."
#define REVERT "Obnovit"
#define NONE_TEXT "ádný"
#define EOL_TEXT "Øádkování"
#define SHOW_CODES_TEXT "Ukázat kód"
#define SHOW_MODE "Mód"
#define MACRO_POPUP_TEXT_1 "Jestli chcete nahrát"
#define MACRO_POPUP_TEXT_2 "makro, mùete"
#define MACRO_POPUP_TEXT_3 "zaèít odtud"
#define GET_PASSWORD "Zadej Heslo"
#define PASSWORD_TEXT "Heslo:"
#define TEXT_ENCRYPTED "Zaifrovat"
#define FILE_NOT_ENCRYPTED_TITLE "Soubor nezaifrován"
#define FILE_NOT_ENCRYPTED_TEXT "Soubor ^1 není zaifrován"
#define DECRYPT "Deifrovat"
#define QUICK_MACRO "Rychlé Mac."
#define FILE_LOCATION "Zadej heslo pro soubor "
#define UNABLE_TO_CREATE_DIR_TEXT "Nelze vytvoøit sloku ^1"
#define UNABLE_TO_CREATE_DIR_TITLE "Chyba"
#define NO_COMPRESSED_DOC_TITLE "Není podporováno"
#define NO_COMPRESSED_DOC_TEXT "Soubor ^1 je komprimován. Komprimované DOC soubory nejsou zatím podporovány."
#define MUST_ENTER_NAME_TITLE "Bez jména"
#define MUST_ENTER_NAME_TEXT "Musíte vloit jméno pro makro"
#define CONFIRM_MACRO_DELETE "Potvrdit"
#define MESSAGE_CONFIRM_MACRO_DELETE "Vymazat Makro?"
#define PLAY_MACRO "Spustit Makro"
#define START_RECORD "Nahrát Makro"
#define RECORD "Nahraj"
#define PLAY "Spus"
//C++ file text
#define INVALID_FILE_NAME_TITLE "Neplatný název souboru"
#define INVALID_FILE_NAME_TEXT "^1 není platný název souboru."
#define SELECTION_MODE "Oznaèení textu"
#define TEXT_BEAM "Beam"
#define BEAM_FILE "Oznaèený soubor"
#define BEAM_APP "SiEd-Dana Apl."
#define PALM_STYLE_TEXT "Palm Styl"
#define STANDARD_STYLE_TEXT "Standard"
#define SELECT_ALL "Oznaèit ve"
#define WAITING_TEXT "Èekejte Prosím..." //Text displayed while leaving SiEd-Dana

//Resource text
#define TEXT_NEW_DIRECTORY "Nový Název Sloky"

//simple ones
#define OK "OK"
#define CLOSE "Zavøít"
#define OVERWRITE "Pøepsat"
#define CANCEL "Zruit"
#define SAVE "Uloit"
#define DISCARD "Zahodit"
#define YES "Ano"
#define NO "Ne"
#define TEXT_FONT "Font"
#define TEXT_NONE "Nic"
#define TEXT_SIZE "Velikost:"
#define TOTAL "Celkem:"
#define OPEN "Otevøít"
#define TEXT_FIND "Hledat"
#define TEXT_FIND_COLON "Hledej:"
#define TEXT_REPLACE "Najít & Nahradit"
#define DELETE "Vymazat"
#define TEXT_DATE "Datum:"
#define TEXT_COPY_FULL "Kopírovat"
#define RENAME "Pøejmenovat"
#define TEXT_EXIT "Konec"
#define TEXT_FILE "Soubor"
#define TEXT_NEW "Nový"
#define PREFERENCES_ABBR "Pref." //abbreviation for preferences
#define TEXT_ABOUT "O SiEd-Dana"
#define TEXT_COPY "Kopírovat"
#define TEXT_PASTE "Vloit"
#define TEXT_CUT "Vyjmout"
#define TEXT_UNDO "Zpìt"
#define TEXT_REDO "Znovu"
#define TEXT_CURSOR "Kurzor"
#define TEXT_END "Konec"
#define TEXT_HOME "Zaèátek" //as in beginning of line key
#define TEXT_EDIT "Úpravy" //as in the menu title
#define TEXT_CALCULATE "Souhrn" //abbreviation for calculate
#define TEXT_DOCUMENT_STATISTICS "Statistika dokumentu"
#define TEXT_DOCUMENT_CHARS "Znakù:"
#define TEXT_DOCUMENT_WORDS "Slov:"
#define MENU_TEXT_STATS "Stats"
#define MANAGE_FILES "Soubory"
//other text
#define NEW_FILENAME "Nový název Souboru"
#define OLD_FILENAME "Tento název"
#define RENAME_FILE "Pøepsat soubor"
#define FIND_AGAIN "Hledat znovu"
#define SAVE_AS "Uloit jako"
#define SELECT_FILE "Oznaè soubor"
#define TERM_NOT_FOUND "Nenalezeno"
#define END_OF_DOCUMENT_REACHED "Konec dokumentu"
#define CONFIRM_FILE_DELETE "Smazání souboru"
#define MESSAGE_CONFIRM_FILE_DELETE "Smazat soubor ^1?"

#define CONFIRM_OVERWRITE "Pøepsání souboru"
#define MESSAGE_CONFIRM_OVERWRITE "Otevøením souboru zruíte provedené zmìny. Chcete uloit?"

#define COPYRIGHT_MESSAGE "\251 2016 Reg Tiangha.\n\SiEd-Dana is based on the original SiEd text editor, \251 2003-2005\nBenjamin Roe.\nArtwork is \251 Stefan Smith & Tankut Eric.\nThis application is distributed under the terms of the GPLv2."

#define NEW_FILE "Nový"
#define MESSAGE_NEW_FILE "Vytvoøit nový soubor?"
#define EDITOR_PREFERENCES "Vlastnosti"

#define SAVE_FILE_AS "Uloit jako"
#define FILENAME "Název"

#define SEARCH_TERM "Hledat"
#define MATCH_CASE "Case sens."
#define MATCH_WHOLE_WORD "Celé slovo"

#define REPLACE_ABBR "Nahr:" //abbreviation for replace
#define REPLACE_ABBR_BUTTON "Nahr" //abbreviation for replace
#define TEXT_WITH "Za:"
#define REPLACE_ALL_ABBR "N ve" //abbreviation for replace all
#define END_OF_DOCUMENT "Na konec"
#define START_OF_DOCUMENT "Na zaèátek"
#define DELETE_LINE "Smazat øádek"
#define SPLIT_VIEW "2 okna"
#define UNSPLIT_VIEW "1 okno"
#define TEXT_VIEW "Dalí"
#define NEW_DIRECTORY "Sloka"

//error messages - less important to translate
#define UNABLE_TO_UNDO "Nelze zpìt"
#define OVERWRITE_FILE "Pøepsat soubor?"
#define OVERWRITE_FILE_TEXT "Soubor s tímto názvem existuje. Pøepsat?"
#define UNABLE_TO_UNDO_TEXT "Tato zmìna nemùe být vzata zpìt."
#define TEXT_SELECTION_COPY_TRUNCATED "Oznaèení zkráceno"
#define MESSAGE_SELECTION_COPY_TRUNCATED "Nedostatek pamìti ve schránce pro ozn. text"

#define MESSAGE_NOT_SUPPORTED "^1 Není podporován."
#define TEXT_FILE_NOT_FOUND "Soubor nenalezen"
#define MESSAGE_FILE_NOT_FOUND "Soubor ^1 nenalezen."
#define TEXT_DATABASE_ACCESS_ERROR "Database Access Error"
#define MESSAGE_DATABASE_ACCESS_ERROR "Database Error while ^1."
#define TEXT_FILE_ACCESS_ERROR "File Access Error"
#define MESSAGE_FILE_ACCESS_ERROR "Error in accessing file ^1."
#define TEXT_SCREEN_ACCESS_ERROR "Screen Access Error"
#define MESSAGE_SCREEN_ACCESS_ERROR "Error in creating offscreen window."
#define TEXT_UNABLE_TO_SAVE "Nelze uloit"
#define MESSAGE_UNABLE_TO_SAVE "Nemohu uloit ^1."
#define TEXT_DEBUG_MESSAGE "Debug Message"
#define TEXT_OUT_OF_MEMORY "Nedostatek pamìti"
#define MESSAGE_OUT_OF_MEMORY "Memory Allocation Error in ^1"
#define FEATURE_NOT_IMPLEMENTED "Není implementováno"
#endif
