/* $Id: resize.h,v 1.19 2007/12/23 07:18:42 arpruss Exp $ */
/*******************************************************************************
    Copyright (c) 2004, Alexander R. Pruss
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

        Redistributions of source code must retain the above copyright notice, this
        list of conditions and the following disclaimer.

        Redistributions in binary form must reproduce the above copyright notice, this
        list of conditions and the following disclaimer in the documentation and/or
        other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef _PALM_RESIZE_H
#define _PALM_RESIZE_H

#define resizeVersion 0x0128
#define palmResizeCreator 'pRes'
#define FTR_PALMRESIZE_DISABLE  10

#include <PalmOS.h>
#include "config.h"

typedef enum {
  DIA_HARDWARE_NONE = 0,
  DIA_HARDWARE_HANDERA,
  DIA_HARDWARE_SONY1,
  DIA_HARDWARE_SONY2,

  DIA_HARDWARE_PALM10,
  DIA_HARDWARE_PALM11,
  DIA_HARDWARE_UNKNOWN,
  DIA_HARDWARE_PALMONE
} DIAHardwareType;


typedef struct {
  UInt16 objectID;
  UInt16 flags;
  UInt16 reserved;
} DIAConfigEntryType;


typedef UInt8 DIAStateType;

typedef struct {
  UInt16 sizeInWords;
  UInt16 flags;
  UInt16 bin;
  UInt16 preferredState;
  DIAConfigEntryType objectList[0];
} DIAConfigType;


typedef struct {
    UInt16 from;
    UInt16 to;
} DIAIndexEntryType;


typedef struct {
    UInt16 count;
    DIAIndexEntryType mapList[0];
} DIAIndexType;


typedef struct {
    UInt16 formID;
    UInt16 numObjects;
    Boolean open;
    MemHandle configHandle;
    Coord lastExtentX;
    Coord lastExtentY;
    RectangleType lastBounds;
    Boolean forceRedraw;
    DIAConfigType * config;
    RectangleType * originalBounds;
} DIAFormEntryType;

typedef struct {
    UInt16 formID;
    FormType *formPtr;
    UInt16 index;
    UInt16 flags;
    DIAFormEntryType *entry;
} ResizeTemporaryData;



#ifdef BUILD_RESIZE_LIBRARY
#define D_REFNUM_VOID  UInt16 refNum
#define D_REFNUM       UInt16 refNum,
#define REFNUM         refNum,
#define REFNUM_VOID    refNum
#define D_GLOBALS      ResizeGlobals* g,
#define D_GLOBALS_VOID ResizeGlobals* g
#define GLOBALS        g,
#define GLOBALS_VOID   g
#define RESIZE_SECTION
#define DIA_SECTION
#define GET_GLOBALS    ResizeGlobals* g = SysLibTblEntry( refNum )->globalsP;
#define G( v )    ( g->v )

typedef struct {
    DIAHardwareType hardware;
    UInt32 displayChangeNotification;
    Boolean haveNotification;
    UInt16 card;
    LocalID db;
    Boolean palmHiRes;
    Boolean sony1AllowResize;
    UInt16 coordSysStack[10];
    UInt16 coordSysStackPtr;

#ifdef SUPPORT_DIA_SONY
    UInt16 sonyRefNum;
#endif

#ifdef SUPPORT_DIA_HANDERA
    UInt8 savedHanderaRotation;
    UInt8 savedHanderaScreenMode;
#endif

#ifdef SUPPORT_DIA_PALMONE
    UInt16 palmOnePinRefNum;
    UInt16 palmOneStatRefNum;
    Boolean palmOneAllowResize;
#endif

    DIAFormEntryType *formList;
    DIAStateType *bins;
    UInt16 numBins;
    UInt16 formCount;
    ResizeTemporaryData active;
    Boolean haveWinDisplayChangedEvent;
} ResizeGlobals;

#else
# define D_REFNUM_VOID  void
# define D_REFNUM
# define REFNUM_VOID
# define REFNUM
# define D_GLOBALS_VOID void
# define D_GLOBALS
# define GLOBALS
# define GLOBALS_VOID
# define STATIC_VAR     static
# define GET_GLOBALS
# define G( v )    ( v )
# include "sects.h"
#endif

#if ! defined( SUPPORT_DIA ) && defined( HAVE_SILKSCREEN )
# define SUPPORT_DIA
#endif

#if ! defined( SUPPORT_DIA_SONY ) && defined( HAVE_SONY_SDK )
# define SUPPORT_DIA_SONY
#endif

#if ! defined( SUPPORT_DIA_HANDERA ) && defined( HAVE_HANDERA_SDK )
# define SUPPORT_DIA_HANDERA
#endif

#ifndef winDisplayChangedEvent
# define winDisplayChangedEvent 0x4101
#else
# define HAVE_PALM_DIA_SDK
#endif

#include "resizeconsts.h"

#define WORD_LIST_TYPE 'wrdl'

#ifdef SUPPORT_DIA
/* resize.c */
extern void InitializeResizeSupport(D_REFNUM UInt16 formMapId) RESIZE_SECTION;
extern void TerminateResizeSupport(D_REFNUM_VOID) RESIZE_SECTION;
extern void SetResizePolicy(D_REFNUM UInt16 formID) RESIZE_SECTION;
extern Boolean ResizeHandleFrmOpenEvent(D_REFNUM_VOID) RESIZE_SECTION;
extern Boolean ResizeHandleFrmCloseEvent(D_REFNUM_VOID) RESIZE_SECTION;
extern Boolean ResizeHandleWinDisplayChangedEvent(D_REFNUM_VOID) RESIZE_SECTION;
extern Boolean ResizeHandleWinExitEvent(D_REFNUM_VOID) RESIZE_SECTION;
extern Boolean ResizeHandleWinEnterEvent(D_REFNUM_VOID) RESIZE_SECTION;
extern Boolean ResizeHandleFrmRedrawUpdateCode(D_REFNUM_VOID) RESIZE_SECTION;
extern void LoadResizePrefs(D_REFNUM UInt32 appID, UInt16 prefID) RESIZE_SECTION;
extern void SaveResizePrefs(D_REFNUM UInt32 appID, UInt16 prefID, Int16 version) RESIZE_SECTION;
extern void ResizeRefreshCurrentForm(D_REFNUM_VOID) RESIZE_SECTION;
extern void SetHaveWinDisplayChangedEvent(D_REFNUM Boolean value) RESIZE_SECTION;
extern Boolean ResizeHandleEvent(D_REFNUM EventType * event) RESIZE_SECTION;

/* DIA.c */
extern DIAHardwareType InitializeDIA(D_REFNUM_VOID) DIA_SECTION;
extern void TerminateDIA(D_REFNUM_VOID) DIA_SECTION;
extern void SetDIAState(D_REFNUM DIAStateType state) DIA_SECTION;
extern void SetDIAAllowResize(D_REFNUM Boolean allow) DIA_SECTION;
extern Boolean HandleResizeNotification(D_REFNUM UInt32 notificationType) DIA_SECTION;
extern DIAStateType GetDIAState(D_REFNUM_VOID) DIA_SECTION;
extern void SetCustomDIAPolicy(D_REFNUM UInt16 formID) DIA_SECTION;
extern DIAHardwareType GetDIAHardware(D_REFNUM_VOID) DIA_SECTION;
extern void SetDIAConstraints(D_REFNUM WinHandle winH, Boolean defaultBig, Boolean allowBig) DIA_SECTION;
extern Int16 CompareDIAState(D_REFNUM DIAStateType x, DIAStateType y) DIA_SECTION;
extern void GetHiddenStatusBarArea(D_REFNUM RectangleType * area) DIA_SECTION;
extern FontID HanderaAdjustFont(D_REFNUM FontID font) DIA_SECTION;
extern Coord HanderaCoord(D_REFNUM Coord x) DIA_SECTION;

/* These two functions should NOT be used by code other than that in
   resize.c and DIA.c. */
extern void PushCoordinateSystemToStandard(D_GLOBALS_VOID) DIA_SECTION;
extern void PopCoordinateSystem(D_GLOBALS_VOID) DIA_SECTION;


#else

#define InitializeResizeSupport( x )
#define TerminateResizeSupport()
#define SetResizePolicy( x )
#define ResizeHandleFrmOpenEvent()  true
#define ResizeHandleFrmCloseEvent() true
#define ResizeHandleWinDisplayChangedEvent() true
#define ResizeHandleWinExitEvent()  true
#define ResizeHandleWinEnterEvent() true
#define LoadResizePrefs( a, b )
#define SaveResizePrefs( a, b, c )
#define ResizeRefreshCurrentForm()
#define SetHaveWinDisplayChangedEvent( x )
#define ResizeHandleFrmRedrawUpdateCode() true
#define ResizeHandleEvent( e ) false

#define InitializeDIA()              DIA_HARDWARE_NONE
#define TerminateDIA()
#define SetDIAState( s )
#define SetDIAAllowResize( a )
#define HandleResizeNotification( n ) false
#define GetDIAState()                DIA_STATE_MAX
#define SetCustomDIAPolicy( f )
#define GetDIAHardware()             DIA_HARDWARE_NONE
#define SetDIAConstraints( w, d, a )
#define CompareDIAState( x, y )      0
#define GetHiddenStatusBarArea( a )  MemSet( a, sizeof( RectangleType ), 0 )
#define HanderaAdjustFont( font )    ( font )
#define HanderaCoord( x )            ( x )

#endif

#endif
