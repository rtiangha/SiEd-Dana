/* $Id: resize.c,v 1.50 2009/03/20 02:42:10 arpruss Exp $ */
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

#include "resize.h"
#include <LstGlue.h>
#include <TblGlue.h>
#include <FrmGlue.h>

#ifndef _Str
#define _Str(x) #x
#endif

#ifdef SUPPORT_DIA_HANDERA
#include <Vga.h>
#endif

#ifdef SUPPORT_DIA

#define DebugInfo(s)          /*   { char _tempstring[500];StrPrintF(_tempstring,"%s(%d) ",s,active.formID);\
                                    ClipboardAppendItem(clipboardText,_tempstring,StrLen(_tempstring)); } */

#ifndef BUILD_RESIZE_LIBRARY
static void UniqueUpdateForm( UInt16 formID, UInt16 code );
static void SetRedrawOnAllOpenForms(D_GLOBALS_VOID);

static DIAFormEntryType *formList = NULL;
static DIAStateType *bins;
static UInt16 numBins;
static UInt16 formCount = 0;
static ResizeTemporaryData active;
static Boolean haveWinDisplayChangedEvent = false;
#endif


RESIZE_SECTION static void* SafeMemPtrNew( UInt32 size )
{
    void* p;

    p = MemPtrNew( size );
    if ( p == NULL )
        SysFatalAlert( "Out of memory." );

    return p;
}


RESIZE_SECTION static void SafeMemPtrFree( void* p )
{
    if ( p != NULL )
        MemPtrFree( p );
}


RESIZE_SECTION static UInt16 StandardExtent(D_REFNUM_VOID)
{
#ifdef SUPPORT_DIA_HANDERA
    if (GetDIAHardware(REFNUM_VOID) == DIA_HARDWARE_HANDERA)
        return 240;
    else
#endif
        return 160;
}



/* Update form, but only once */
static void UniqueUpdateForm(UInt16 formID, UInt16 code)
{
    EventType e;

    MemSet(&e, sizeof(EventType), 0);
    e.eType = frmUpdateEvent;
    e.data.frmUpdate.formID = formID;
    e.data.frmUpdate.updateCode = code;
    EvtAddUniqueEventToQueue(&e, 0, true);
    DebugInfo("Queuing frmUpdateForm");
}



/* Same as FrmGetObjectIndex() but works with GSI_OBJECT_ID, too */
RESIZE_SECTION static UInt16 MyFrmGetObjectIndex(FormPtr formPtr, UInt16 id)
{
    UInt16 i;

    if (id != GSI_OBJECT_ID)
        return FrmGetObjectIndex(formPtr, id);
    for (i = FrmGetNumberOfObjects(formPtr) - 1; 0 < i; i--) {
        if (FrmGetObjectType(formPtr, i) == frmGraffitiStateObj) {
            return i;
        }
    }
    return 0;
}



RESIZE_SECTION static void WinGlueGetBounds(WinHandle winH,
    RectangleType * rect)
{
    Err err;
    UInt32 version;

    err = FtrGet(sysFtrCreator, sysFtrNumROMVersion, &version);
    if (err == errNone && 0x04003000 <= version) {
        WinGetBounds(winH, rect);
    } else {
        WinHandle oldH;

        oldH = WinSetDrawWindow(winH);
        WinGetDrawWindowBounds(rect);
        WinSetDrawWindow(oldH);
    }
}



RESIZE_SECTION static void AddForm(D_GLOBALS UInt16 indexInList, MemHandle handle,
    UInt16 formID)
{
    UInt16 numObjects;
    DIAConfigType *config;

    G(formList)[indexInList].configHandle = handle;
    if (handle == NULL) {
        return;
    }

    config = (DIAConfigType *) MemHandleLock(handle);
    numObjects = ((config->sizeInWords + 1) * sizeof(UInt16) -
        sizeof(DIAConfigType)) / sizeof(DIAConfigEntryType);

    G(formList)[indexInList].formID = formID;
    G(formList)[indexInList].config = config;
    G(formList)[indexInList].numObjects = numObjects;
    G(formList)[indexInList].open = false;

    if (config->flags & DIA_FORM_USE_BIN && G(numBins) < 1 + config->bin)
        G(numBins) = 1 + config->bin;
}



/* This sets up the original bounds array as needed */
RESIZE_SECTION static void InitialSetupForm(D_GLOBALS UInt16 indexInList)
{
    RectangleType *originalBounds;
    FormType *formPtr;
    UInt16 numObjects;
    UInt16 i;

    numObjects = G(formList)[indexInList].numObjects;
    if (numObjects == 0)
        return;
    if (G(formList)[indexInList].originalBounds != NULL)
        return;
    formPtr = FrmGetFormPtr(G(formList)[indexInList].formID);
    if (formPtr == NULL)
        return;

    originalBounds = (RectangleType *) SafeMemPtrNew(numObjects *
        sizeof(RectangleType));
    G(formList)[indexInList].originalBounds = originalBounds;

    /*
     * First object is the form itself
     */
    WinGlueGetBounds(FrmGetWindowHandle(formPtr), &(originalBounds[0]));

    /*
     * Now on to the other objects
     */
    for (i = 1; i < numObjects; i++) {
        UInt16 objectIndex;

        objectIndex = MyFrmGetObjectIndex(formPtr,
            G(formList)[indexInList].config->objectList[i].objectID);
        FrmGetObjectBounds(formPtr, objectIndex, &(originalBounds[i]));
    }
}



/* Adjust one coordinate */
RESIZE_SECTION static Coord Adjust(Coord value, /* old value to adjust */
    Coord oldObjectExtent,      /* old extent if relevant */
    Coord newObjectExtent,      /* already adjusted extent if relevant */
    Coord oldExtent,            /* old form/screen extent */
    Coord newExtent,            /* new form/screen extent */
    UInt16 shiftedFlag          /* adjustment flag, shifted down as needed */
    ) {
    Coord delta;

    delta = newExtent - oldExtent;

    switch (shiftedFlag) {
        case DIA_X_FIX >> DIA_SHIFT_X:
            return value;
        case DIA_X_RATIO >> DIA_SHIFT_X:
            return value * (Int32) newExtent / oldExtent;
        case DIA_X_RIGHT >> DIA_SHIFT_X:
            return newExtent - (oldExtent - value - oldObjectExtent) -
                newObjectExtent;
        case DIA_X_PLUS_DW >> DIA_SHIFT_X:
            return value + delta;
        case DIA_X_PLUS_1_2_DW >> DIA_SHIFT_X:
            return value + delta / 2;
        case DIA_X_PLUS_1_3_DW >> DIA_SHIFT_X:
            return value + delta / 3;
        case DIA_X_PLUS_2_3_DW >> DIA_SHIFT_X:
            return value + 2 * delta / 3;
        case DIA_X_PLUS_1_4_DW >> DIA_SHIFT_X:
            return value + delta / 4;
        case DIA_X_PLUS_3_4_DW >> DIA_SHIFT_X:
            return value + 3 * delta / 4;
        default:               /* should not happen */
            return value;
    }
}



/* Get new bounds for an object, adjusted for DIA */
RESIZE_SECTION static void GetNewBounds(UInt16 displayExtentX,
    UInt16 displayExtentY, UInt16 stdFormExtentX, UInt16 stdFormExtentY,
    RectangleType * oldBounds, RectangleType * newBounds,
    DIAConfigEntryType * configEntry)
{
    UInt16 flags;

    flags = configEntry->flags;

    if (flags & DIA_MASK_W) {
        newBounds->extent.x = Adjust(oldBounds->extent.x, 0, 0,
            stdFormExtentX,
            displayExtentX, (flags & DIA_MASK_W) >> DIA_SHIFT_W);
    }
    if (flags & DIA_MASK_H) {
        newBounds->extent.y = Adjust(oldBounds->extent.y, 0, 0,
            stdFormExtentY,
            displayExtentY, (flags & DIA_MASK_H) >> DIA_SHIFT_H);
    }
    newBounds->topLeft.x = Adjust(oldBounds->topLeft.x, oldBounds->extent.x,
        newBounds->extent.x,
        stdFormExtentX, displayExtentX, (flags & DIA_MASK_X) >> DIA_SHIFT_X);
    newBounds->topLeft.y = Adjust(oldBounds->topLeft.y, oldBounds->extent.y,
        newBounds->extent.y,
        stdFormExtentY, displayExtentY, (flags & DIA_MASK_Y) >> DIA_SHIFT_Y);
}



/* This compares two rectangles.  If sonyDoubleSecond is set and the first
   bounds are exactly double the size of the first and DIA hardware is
   DIA_HARDWARE_SONY1, then they also match. */
RESIZE_SECTION static Boolean SameBounds(D_REFNUM RectangleType * a, RectangleType * b,
    Boolean sonyDoubleSecond)
{
    Boolean compare;

    compare = (a->extent.y == b->extent.y && a->extent.x == b->extent.x &&
        a->topLeft.y == b->topLeft.y && a->topLeft.x == b->topLeft.x);
#ifdef SUPPORT_DIA_SONY
    if (!compare && GetDIAHardware(REFNUM_VOID) == DIA_HARDWARE_SONY1) {
        return a->extent.y == 2 * b->extent.y &&
            a->extent.x == 2 * b->extent.x &&
            a->topLeft.y == 2 * b->topLeft.y &&
            a->topLeft.x == 2 * b->topLeft.x;
    }
#endif
    return compare;
}



/* Returns true if there is moving done */
RESIZE_SECTION static Boolean MoveObjectsInForm(D_REFNUM UInt16 indexInList)
{
    Coord extentX;
    Coord extentY;
    UInt16 i;
    UInt16 focus;
    FormType *formPtr;
    DIAFormEntryType *entry;
    GET_GLOBALS

    RectangleType newBounds;
    RectangleType curBounds;

    entry = &(G(formList)[indexInList]);

    formPtr = FrmGetFormPtr(entry->formID);
    if (formPtr == NULL)
        return false;
    InitialSetupForm(GLOBALS indexInList);
    WinGetDisplayExtent(&extentX, &extentY);

    /*
     * First object is the form itself
     */
    WinGlueGetBounds(FrmGetWindowHandle(formPtr), &curBounds);
    newBounds = curBounds;
    GetNewBounds(extentX, extentY, StandardExtent(REFNUM_VOID), StandardExtent(REFNUM_VOID),
        &(G(formList)[indexInList].originalBounds[0]),
        &newBounds, &(entry->config->objectList[0]));

    if ((SameBounds(REFNUM &curBounds, &newBounds, true) ||
            (G(formList)[indexInList].open &&
                SameBounds(REFNUM &newBounds, &(G(formList)[indexInList].lastBounds),
                    false))) && extentX == entry->lastExtentX
        && extentY == entry->lastExtentY) {
        DebugInfo("skipping resize");
        return false;
    }

    if (GetDIAHardware(REFNUM_VOID) == DIA_HARDWARE_SONY1) {
        /*
         * Sony OS4 works differently from everybody else
         */
        FrmEraseForm(formPtr);
    }

    entry->lastBounds = newBounds;
    entry->lastExtentX = extentX;
    entry->lastExtentY = extentY;
    WinSetBounds(FrmGetWindowHandle(formPtr), &newBounds);

    /*
     * The rest is resized relative to the form itself
     */
    extentX = newBounds.extent.x;
    extentY = newBounds.extent.y;
    focus = FrmGetFocus(formPtr);

    for (i = 1; i < entry->numObjects; i++) {
        UInt16 objectIndex;

        objectIndex = MyFrmGetObjectIndex(formPtr,
            entry->config->objectList[i].objectID);
        FrmGetObjectBounds(formPtr, objectIndex, &curBounds);
        newBounds = curBounds;
        GetNewBounds(extentX, extentY,
            entry->originalBounds[0].extent.x,
            entry->originalBounds[0].extent.y,
            &(entry->originalBounds[i]),
            &newBounds, &(entry->config->objectList[i]));

        if (!SameBounds(REFNUM &curBounds, &newBounds, false)) {
            UInt16  ringObjectID;
            Err     err;
            RectangleType ringBounds;
            Int16         ringExtraInfo;
            FrmNavFocusRingStyleEnum ringStyle;
            Boolean                  haveRing;
            UInt16                   insPt = 0;

            err = FrmGlueNavGetFocusRingInfo( formPtr,
                            &ringObjectID, &ringExtraInfo,
                            &ringBounds, &ringStyle );

            haveRing = ( err == errNone &&
                         ringObjectID ==
                             entry->config->objectList[i].objectID );

            if ( haveRing )
                FrmGlueNavRemoveFocusRing( formPtr );

            if ( FrmGetObjectType(formPtr, objectIndex) == frmFieldObj )
                insPt = FldGetInsPtPosition( FrmGetObjectPtr(formPtr, objectIndex ) );

            FrmSetObjectBounds(formPtr, objectIndex, &newBounds);

            if ( haveRing ) {
                ringBounds.topLeft.x +=
                       newBounds.topLeft.x - curBounds.topLeft.x;
                ringBounds.topLeft.y +=
                       newBounds.topLeft.y - curBounds.topLeft.y;
                ringBounds.extent.x +=
                       newBounds.extent.x - curBounds.extent.x;
                ringBounds.extent.y +=
                       newBounds.extent.y - curBounds.extent.y;
                FrmGlueNavDrawFocusRing( formPtr, ringObjectID,
                    ringExtraInfo, &ringBounds, ringStyle, false );
            }

            switch (FrmGetObjectType(formPtr, objectIndex)) {
                case frmTableObj: {
                    TableType* table;

                    table = (TableType *) FrmGetObjectPtr(formPtr, objectIndex );

                    if (TblGlueGetNumberOfColumns(table)==1) {
                        /* one column only, follow resize of main */
                        Coord width;

                        width = TblGetColumnWidth( table, 0 ) -
                                    curBounds.extent.x +
                                    newBounds.extent.x;

                        if ( 0 < width ) {
                            TblSetColumnWidth( table, 0, width );
                        }
                    }
                    break;
                }
                case frmFieldObj:
                    /*
                     * case frmScrollBarObj:
                     */
                    {
                        FieldType *field;

                        field =
                            (FieldType *) FrmGetObjectPtr(formPtr, objectIndex);

                        if (entry->config->objectList[i].flags & DIA_H_GRANULAR) {
                            FontID oldFont;
                            Coord newHeight;

                            oldFont = FntSetFont(FldGetFont(field));

                            newHeight = newBounds.extent.y / FntLineHeight() *
                                FntLineHeight();

                            FntSetFont(oldFont);

                            if (0 < newHeight) {
                                newBounds.extent.y = newHeight;
                                FrmSetObjectBounds(formPtr, objectIndex,
                                    &newBounds);
                            }
                        }

                        if ( newBounds.extent.x != curBounds.extent.x )
                            FldRecalculateField(field, true);
                        FldSetInsPtPosition(field, insPt);

                        if ( newBounds.extent.x != curBounds.extent.x ||
                             newBounds.extent.y != curBounds.extent.y )
                            FldSendChangeNotification(field);
                        break;
                    }
                case frmListObj: {
                    ListType *list;
                    FontID   oldFont;
                    Coord    newHeight;

                    list =
                        (ListType *) FrmGetObjectPtr(formPtr, objectIndex);

                    oldFont = FntSetFont(LstGlueGetFont(list));

                    newHeight = newBounds.extent.y / FntLineHeight() *
                        FntLineHeight();

                    if ( ( entry->config->objectList[i].flags & DIA_H_GRANULAR ) &&
                         0 < newHeight) {
                        newBounds.extent.y = newHeight;
                        FrmSetObjectBounds(formPtr, objectIndex,
                            &newBounds);
                    }

                    LstSetHeight( list, newHeight / FntLineHeight() );

                    FntSetFont( oldFont );

                    break;
                }

                default:
                    break;
            }
        }
    }

    if (FrmGetFocus(formPtr) != focus)
        FrmSetFocus(formPtr, focus);

    return true;
}



void InitializeResizeSupport(D_REFNUM UInt16 formMapId)
{
    MemHandle indexHandle;
    DIAIndexType *index;
    UInt16 i;
    GET_GLOBALS

    if (G(formList) != NULL)
        return;
    indexHandle = DmGetResource(WORD_LIST_TYPE, formMapId);
    if (indexHandle == NULL)
        return;

    if (DIA_HARDWARE_NONE == InitializeDIA(REFNUM_VOID))
        return;

    index = (DIAIndexType *) MemHandleLock(indexHandle);
    G(numBins) = 0;
    G(formCount) = index->count * sizeof(UInt16) / sizeof(DIAIndexEntryType);

    if (G(formCount) == 0) {
        G(numBins) = 0;
        return;
    }

    G(formList) = (DIAFormEntryType *) SafeMemPtrNew(G(formCount) *
        sizeof(DIAFormEntryType));
    MemSet(G(formList), G(formCount) * sizeof(DIAFormEntryType), 0);

    for (i = 0; i < G(formCount); i++) {
        AddForm(GLOBALS i, DmGetResource(WORD_LIST_TYPE, index->mapList[i].to),
            index->mapList[i].from);
    }

    MemHandleUnlock(indexHandle);
    DmReleaseResource(indexHandle);

    if (0 < G(numBins)) {
        G(bins) = (DIAStateType *) SafeMemPtrNew(G(numBins) * sizeof(DIAStateType));
        for (i = 0; i < G(numBins); i++) {
            G(bins)[i] = DIA_STATE_UNDEFINED;
        }
    }
}




void TerminateResizeSupport(D_REFNUM_VOID)
{
    UInt16 i;
    GET_GLOBALS

    if (G(formList) == NULL)
        return;
    TerminateDIA(REFNUM_VOID);
    for (i = 0; i < G(formCount); i++) {
        if (NULL != G(formList)[i].configHandle) {
            MemHandleUnlock(G(formList)[i].configHandle);
            DmReleaseResource(G(formList)[i].configHandle);
            if (G(formList)[i].originalBounds != NULL)
                SafeMemPtrFree(G(formList)[i].originalBounds);
        }
    }
    SafeMemPtrFree(G(formList));
    G(formCount) = 0;
    if (0 < G(numBins)) {
        SafeMemPtrFree(G(bins));
    }
}




void LoadResizePrefs(D_REFNUM UInt32 appID, UInt16 prefID)
{
    UInt16 size;
    GET_GLOBALS

    size = 0;

    if (G(formList) == NULL || G(numBins) == 0 ||
        noPreferenceFound ==
        PrefGetAppPreferences(appID, prefID, NULL, &size, true)) {
        return;
    }

    if (G(numBins) * sizeof(DIAStateType) < size) {
        size = G(numBins) * sizeof(DIAStateType);
    }

//    PrefGetAppPreferences(appID, prefID, G(bins), &size, true);
}



void SaveResizePrefs(D_REFNUM UInt32 appID, UInt16 prefID, Int16 version)
{
    GET_GLOBALS

    if (G(formList) == NULL || G(numBins) == 0)
        return;

    PrefSetAppPreferences(appID, prefID, version, G(bins),
        G(numBins) * sizeof(DIAStateType), true);
}



RESIZE_SECTION static UInt16 GetFormIndex(D_GLOBALS UInt16 formID)
{
    UInt16 i;

    if (G(formList) == NULL)
        return 0;

    for (i = 0; i < G(formCount); i++) {
        if (G(formList)[i].formID == formID)
            break;
    }

    return i;
}



void SetResizePolicy(D_REFNUM UInt16 formID)
{
    GET_GLOBALS

    if (GetFormIndex(GLOBALS formID) < G(formCount))
        SetCustomDIAPolicy(REFNUM formID);
}



RESIZE_SECTION static Boolean GetFormInfo(D_GLOBALS_VOID)
{
    G(active).formID = FrmGetActiveFormID();
    G(active).formPtr = FrmGetActiveForm();
    G(active).index = GetFormIndex(GLOBALS G(active).formID);

    if (G(active).index < G(formCount)) {
        G(active).entry = &(G(formList)[G(active).index]);
        G(active).flags = G(active).entry->config->flags;
        return true;
    } else {
        return false;
    }
}





RESIZE_SECTION static Boolean PrepareForm(D_REFNUM_VOID)
{
    GET_GLOBALS

    if (G(active).index == G(formCount)) {
        SetDIAState(REFNUM DIA_STATE_MAX);
        SetDIAAllowResize(REFNUM false);
        return false;
    }

    return MoveObjectsInForm(REFNUM G(active).index);
}



RESIZE_SECTION static void SaveDIAState(D_REFNUM_VOID)
{
    GET_GLOBALS

    if (G(active).index == G(formCount))
        return;

    if (G(active).entry->config->flags & DIA_FORM_USE_BIN) {
        G(bins)[G(active).entry->config->bin] = GetDIAState(REFNUM_VOID);
    }
}




RESIZE_SECTION static Boolean FormStartup(D_REFNUM_VOID)
{
    DIAStateType newState;
    Boolean big;
    Boolean allowBig;
    Boolean changed;
    Boolean allowResize;
    GET_GLOBALS

    if (G(active).index == G(formCount)) {
        SetDIAState(REFNUM DIA_STATE_MAX);
        SetDIAAllowResize(REFNUM false);
        SetDIAConstraints(REFNUM FrmGetWindowHandle(G(active).formPtr), false, false);
        return false;
    }

    newState = GetDIAState(REFNUM_VOID);

    if ((G(active).flags & DIA_FORM_USE_BIN) &&
        (G(bins)[G(active).entry->config->bin] != DIA_STATE_UNDEFINED)) {
        newState = G(bins)[G(active).entry->config->bin];
/*        if ( newState == DIA_STATE_MIN ) DebugInfo("bin = min");
        if ( newState == DIA_STATE_MAX ) DebugInfo("bin = max"); */
    } else if (!(G(active).flags & DIA_FORM_KEEP_LAST)) {
        newState = G(active).entry->config->preferredState;
    }

    if (G(active).flags & DIA_FORM_NO_RESIZE) {
        allowResize = false;
        allowBig = false;
    } else {
        allowResize = true;
        allowBig = true;
    }

    big = (newState == DIA_STATE_MIN || newState == DIA_STATE_NO_STATUS_BAR);
    allowBig = allowBig || big;

    SetDIAConstraints(REFNUM FrmGetWindowHandle(G(active).formPtr), allowBig, allowBig);
    SetDIAState(REFNUM newState);

    SetDIAAllowResize(REFNUM allowResize);

    changed = PrepareForm(REFNUM_VOID);

/*    if ( changed ) DebugInfo("Changed in FormStartup"); */

    SaveDIAState(REFNUM_VOID);

    return changed;
}


RESIZE_SECTION static void ForceRedrawAll(D_GLOBALS_VOID)
{
    UInt16 i;

    for (i = 0; i < G(formCount); i++) {
        if (G(formList)[i].open)
            G(formList)[i].forceRedraw = true;
    }
}



Boolean ResizeHandleFrmOpenEvent(D_REFNUM_VOID)
{
    GET_GLOBALS

    GetFormInfo(GLOBALS_VOID);

    if (G(formList) == NULL)
        return false;
        
    SetCustomDIAPolicy(REFNUM FrmGetActiveFormID());

#ifdef SUPPORT_DIA_HANDERA
    if (GetDIAHardware(REFNUM_VOID) == DIA_HARDWARE_HANDERA &&
        (G(active).index == G(formCount) || !(G(active).flags & DIA_FORM_HANDERA))
        ) {
        VgaFormModify(G(active).formPtr, vgaFormModify160To240);
    }
#endif
    if (G(active).index < G(formCount)) {
        MemSet(&(G(active).entry->lastBounds), sizeof(RectangleType), 0);
        G(active).entry->lastExtentX = -1;
        G(active).entry->lastExtentY = -1;
        G(active).entry->forceRedraw = false;
    }
    DebugInfo("FrmOpen");
    PushCoordinateSystemToStandard(GLOBALS_VOID);
    FormStartup(REFNUM_VOID);
    if (G(active).index < G(formCount)) {
        G(active).entry->open = true;
    }
    PopCoordinateSystem(GLOBALS_VOID);
    G(haveWinDisplayChangedEvent) = false;
    return true;
}



Boolean ResizeHandleFrmCloseEvent(D_REFNUM_VOID)
{
    GET_GLOBALS

    if (GetFormInfo(GLOBALS_VOID)) {
        DebugInfo("FrmClose");
        G(active).entry->open = false;
        G(active).entry->forceRedraw = false;
        SaveDIAState(REFNUM_VOID);
    }
    return true;
}

static void SetRedrawOnAllOpenForms(D_GLOBALS_VOID)
{
    UInt16 i;

    DebugInfo( "SetRedrawOnAllOpenForms()" );

    for ( i = 0 ; i < G(formCount) ; i++ )
         if ( G(formList)[ i ].open )
             G(formList)[ i ].forceRedraw = true;
}





Boolean ResizeHandleWinDisplayChangedEvent(D_REFNUM_VOID)
{
    Boolean didChange;
    GET_GLOBALS

    G(haveWinDisplayChangedEvent) = false;
    if (!GetFormInfo(GLOBALS_VOID))
        return true;

    PushCoordinateSystemToStandard(GLOBALS_VOID);

    DebugInfo("WinDisplayChanged");

    didChange = PrepareForm(REFNUM_VOID);
    if (didChange || G(active).entry->forceRedraw) {
        UniqueUpdateForm(G(active).formID, frmRedrawUpdateCode);
        if ( didChange )
            SetRedrawOnAllOpenForms(GLOBALS_VOID);
        G(active).entry->forceRedraw = false;
    }
    SaveDIAState(REFNUM_VOID);

    PopCoordinateSystem(GLOBALS_VOID);

    return true;
}



Boolean ResizeHandleWinExitEvent(D_REFNUM_VOID)
{
    DebugInfo("WinExit");

    return true;
}



Boolean ResizeHandleFrmRedrawUpdateCode(D_REFNUM_VOID)
{
    GET_GLOBALS

    GetFormInfo(GLOBALS_VOID);
    DebugInfo("FrmUpdate");
    if (G(active).index < G(formCount))
        G(active).entry->forceRedraw = false;
    return true;
}



Boolean ResizeHandleWinEnterEvent(D_REFNUM_VOID)
{
    FormType* f;
    GET_GLOBALS

    f = FrmGetActiveForm();

    if ( f != NULL && FrmGetWindowHandle( f ) != WinGetActiveWindow() ) {
        /* menu is probably up */
        return false;
    }

    GetFormInfo(GLOBALS_VOID);
    DebugInfo("WinEnter");
    if (G(active).index == G(formCount)) {
        return true;
    }

    PushCoordinateSystemToStandard(GLOBALS_VOID);

    /*
     * If the last form we have record of being in is this one, then
     * we may have missed what happened on a form.
     */

    if (G(active).entry->open && 
        (FormStartup(REFNUM_VOID) || G(active).entry->forceRedraw)) {
        EventType e;

        DebugInfo("WinEnter: Possibly missed winDisplayChangedEvent");
        MemSet(&e, sizeof(EventType), 0);
        e.eType = winDisplayChangedEvent;
        EvtAddUniqueEventToQueue(&e, 0, true);
        DebugInfo("Queuing display change");
        G(active).entry->forceRedraw = true;
    }

    PopCoordinateSystem(GLOBALS_VOID);

    return true;
}




void SetHaveWinDisplayChangedEvent(D_REFNUM Boolean value)
{
    GET_GLOBALS

    DebugInfo("SetHaveWinDisplayChangedEvent()");
    G(haveWinDisplayChangedEvent) = value;
    ForceRedrawAll(GLOBALS_VOID);
}




void ResizeRefreshCurrentForm(D_REFNUM_VOID)
{
    GET_GLOBALS

    GetFormInfo(GLOBALS_VOID);
    if (G(active).index == G(formCount))
        return;

    PushCoordinateSystemToStandard(GLOBALS_VOID);

    FormStartup(REFNUM_VOID);
    SaveDIAState(REFNUM_VOID);
    UniqueUpdateForm(G(active).formID, frmRedrawUpdateCode);

    PopCoordinateSystem(GLOBALS_VOID);
}




Boolean ResizeHandleEvent(D_REFNUM EventType * event)
{
    Boolean handled = false;
    ResizeTemporaryData old;
    GET_GLOBALS

    switch (event->eType) {
        case nilEvent:
            return false;
        case frmOpenEvent:{
                old = G(active);
                ResizeHandleFrmOpenEvent(REFNUM_VOID);
                G(active) = old;
                break;
            }
        case frmCloseEvent:{
                old = G(active);
                ResizeHandleFrmCloseEvent(REFNUM_VOID);
                G(active) = old;
                break;
            }
        case winEnterEvent:{
                old = G(active);
                ResizeHandleWinEnterEvent(REFNUM_VOID);
                G(active) = old;
                break;
            }
        case winExitEvent:{
                old = G(active);
                ResizeHandleWinExitEvent(REFNUM_VOID);
                G(active) = old;
                break;
            }
        case winDisplayChangedEvent:{
                old = G(active);
                ResizeHandleWinDisplayChangedEvent(REFNUM_VOID);
                G(active) = old;
                break;
            }
        case frmUpdateEvent:
            if (event->data.frmUpdate.updateCode == frmRedrawUpdateCode) {
                old = G(active);
                ResizeHandleFrmRedrawUpdateCode(REFNUM_VOID);
                G(active) = old;
            }
            break;
        default:
            break;
    }

    return handled;
}
#endif
