/* $Id: DIA.c,v 1.23 2007/12/23 07:18:42 arpruss Exp $ */
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

#ifdef SUPPORT_DIA_PALMONE
#include <palmOne_68K.h>
#include <68K/System/palmOneStatusBar.h>
#endif

#ifndef _Str
#define _Str(x) #x
#endif

#ifdef SUPPORT_DIA_HANDERA
#include <Vga.h>
#include <Silk.h>
#endif

#ifdef SUPPORT_DIA_SONY
#include <SonyCLIE.h>

#ifndef vskResizeVertically
#  define vskResizeVertically     1
#endif
#ifndef vskResizeHorizontally
#  define vskResizeHorizontally   2
#endif
#endif

#define COORDSYS_STACK_LEN 10

#ifdef SUPPORT_DIA

#ifndef BUILD_RESIZE_LIBRARY
static DIAHardwareType hardware = DIA_HARDWARE_NONE;
static UInt32 displayChangeNotification = 0;
static Boolean haveNotification = false;
static UInt16 card;
static LocalID db;
static Boolean palmHiRes;
static Boolean sony1AllowResize = false;
static UInt16 coordSysStack[10];
static UInt16 coordSysStackPtr = 0;

#ifdef SUPPORT_DIA_SONY
static UInt16 sonyRefNum;
#endif
#ifdef SUPPORT_DIA_HANDERA
static VgaRotateModeType savedHanderaRotation;
static VgaScreenModeType savedHanderaScreenMode;
#endif

#ifdef SUPPORT_DIA_PALMONE
static UInt16 palmOnePinRefNum;
static UInt16 palmOneStatRefNum;
static Boolean palmOneAllowResize = false;
#endif

#endif


#ifdef SUPPORT_DIA_HANDERA
static DIAHardwareType InitializeHandera(D_GLOBALS_VOID)
{
    UInt32 version;

    if (_TRGSilkFeaturePresent(&version)) {
        VgaRotateModeType rotation;
        VgaScreenModeType screenMode;

        VgaGetScreenMode(&screenMode, &rotation);

        G(savedHanderaScreenMode) = screenMode;
        G(savedHanderaRotation) = rotation;

        if (screenMode != screenMode1To1)
            VgaSetScreenMode(screenMode1To1, rotation);

        return DIA_HARDWARE_HANDERA;
    } else {
        return DIA_HARDWARE_NONE;
    }
}
#else
# define InitializeHandera(GLOBALS_VOID) DIA_HARDWARE_NONE
#endif



#ifdef SUPPORT_DIA_PALMONE
static DIAHardwareType InitializePalmOne(D_GLOBALS_VOID)
{
    Err err;

    err = SysLibFind(pinLibName, &G(palmOnePinRefNum));
    if (err == sysErrLibNotFound)
        err = SysLibLoad(pinLibType, pinLibCreator, &G(palmOnePinRefNum));
    if (err != errNone)
        return DIA_HARDWARE_NONE;
    if (errNone != PinLibOpen( G(palmOnePinRefNum ) ) ) {
        SysLibRemove(G(palmOnePinRefNum));
        return DIA_HARDWARE_NONE;
    }

    err = SysLibFind(palmOneStatusBarLibName, &G(palmOneStatRefNum));
    if (err == sysErrLibNotFound)
        err = SysLibLoad(palmOneStatusBarLibType,
                  palmOneStatusBarLibCreator, &G(palmOneStatRefNum));
    if ( err != errNone || errNone != PalmOneStatusBarLibOpen( G(palmOneStatRefNum) ) ) {
        if ( err == errNone )
            SysLibRemove(G(palmOneStatRefNum));
        SysLibRemove(G(palmOnePinRefNum));
        G(palmOnePinRefNum) = 0;
        return DIA_HARDWARE_NONE;
    }
    
    return DIA_HARDWARE_PALMONE;
}
#else
# define InitializePalmOne(GLOBALS_VOID)   DIA_HARDWARE_NONE
#endif



#ifdef SUPPORT_DIA_SONY
static DIAHardwareType InitializeSony(D_GLOBALS_VOID)
{
    Err err;
    UInt32 version;

    err = SysLibFind(sonySysLibNameSilk, &G(sonyRefNum));
    if (err == sysErrLibNotFound) {
        err = SysLibLoad('libr', sonySysFileCSilkLib, &G(sonyRefNum));
    }
    if (err != errNone)
        return DIA_HARDWARE_NONE;
    if (errNone == FtrGet(sonySysFtrCreator, sonySysFtrNumVskVersion, &version)) {
        /*
         * Version 2 and up 
         */
        err = VskOpen(G(sonyRefNum));
        if (errNone == err)
            return DIA_HARDWARE_SONY2;
    } else {
        /*
         * Version 1 and up 
         */
        err = SilkLibOpen(G(sonyRefNum));
        if (errNone == err) {
            /*
             * Make sure we are in a Hi-Res mode 
             */
            UInt32 width;
            UInt32 height;
            UInt16 sonyHiResRefNum;

            err = SysLibFind(sonySysLibNameHR, &sonyHiResRefNum);
            if (err == sysErrLibNotFound)
                err = SysLibLoad('libr', sonySysFileCHRLib, &sonyHiResRefNum);

            if (err == errNone) {

                err = HROpen(sonyHiResRefNum);
                HRWinScreenMode(sonyHiResRefNum, winScreenModeGet, &width,
                    &height, NULL, NULL);

                if (width < hrWidth) {

                    width = hrWidth;
                    height = hrHeight;
                    if (errNone != HRWinScreenMode(sonyHiResRefNum,
                            winScreenModeSet, &width, &height, NULL, NULL)) {
                        HRClose(sonyHiResRefNum);
                        return DIA_HARDWARE_NONE;
                    }
                }
                HRClose(sonyHiResRefNum);
            } else
                return DIA_HARDWARE_NONE;
        }

        SilkLibDisableResize(G(sonyRefNum));

        G(sony1AllowResize) = false;

        return DIA_HARDWARE_SONY1;
    }
    return DIA_HARDWARE_NONE;
}
#else
# define InitializeSony(GLOBALS_VOID) DIA_HARDWARE_NONE
#endif



#ifdef HAVE_PALM_DIA_SDK
static DIAHardwareType InitializePalm(D_GLOBALS_VOID)
{
    UInt32 version;
    Err err;

    err = FtrGet(pinCreator, pinFtrAPIVersion, &version);
    if (err != errNone)
        return DIA_HARDWARE_NONE;
    if (pinAPIVersion1_1 <= version)
        return DIA_HARDWARE_PALM11;
    else if (pinAPIVersion1_0 <= version)
        return DIA_HARDWARE_PALM10;
    else
        return DIA_HARDWARE_NONE;
}
#else
# define InitializePalm() DIA_HARDWARE_NONE
#endif



/* Check if this is an unknown device with non-standard screen size
   so we can make proper use of the screen size. */
static DIAHardwareType InitializeUnknown(D_GLOBALS_VOID)
{
    Coord extentX;
    Coord extentY;

    PushCoordinateSystemToStandard(GLOBALS_VOID);
    WinGetDisplayExtent(&extentX, &extentY);
    PopCoordinateSystem(GLOBALS_VOID);
    if (extentX != STD_EXTENT_X || extentY != STD_EXTENT_Y)
        return DIA_HARDWARE_UNKNOWN;
    else
        return DIA_HARDWARE_NONE;
}



static void RegisterNotification(D_GLOBALS_VOID)
{
    Err err;

    G(haveNotification) = false;
    switch (G(hardware)) {
#ifdef SUPPORT_DIA_SONY
        case DIA_HARDWARE_SONY1:
        case DIA_HARDWARE_SONY2:
            G(displayChangeNotification) = sysNotifyDisplayChangeEvent;
            break;
#endif
#ifdef SUPPORT_DIA_HANDERA
        case DIA_HARDWARE_HANDERA:
            G(displayChangeNotification) = trgNotifySilkEvent;
            break;
#endif
#ifdef SUPPORT_DIA_PALMONE
        case DIA_HARDWARE_PALMONE:
            G(displayChangeNotification) = sysNotifyAiaEvent;
            break;
#endif
#ifdef HAVE_PALM_DIA_SDK
        case DIA_HARDWARE_PALM10:
        case DIA_HARDWARE_PALM11:
            G(displayChangeNotification) = sysNotifyDisplayResizedEvent;
            break;
#endif
        default:
            return;
    }
    err = SysCurAppDatabase(&G(card), &G(db));
    if (err != errNone)
        return;
    err = SysNotifyRegister(G(card), G(db), G(displayChangeNotification), NULL,
        sysNotifyNormalPriority, 0);
    G(haveNotification) = (err == errNone);
}



static void UnregisterNotification(D_GLOBALS_VOID)
{
    if (G(haveNotification)) {
        SysNotifyUnregister(G(card), G(db), G(displayChangeNotification),
            sysNotifyNormalPriority);
        G(haveNotification) = false;
    }
}



DIAHardwareType InitializeDIA(D_REFNUM_VOID)
{
    Err err;
    UInt32 value;
    UInt32 version;
    GET_GLOBALS

    err = FtrGet(sysFtrCreator, sysFtrNumWinVersion, &version);

    G(palmHiRes) = (err == errNone && 4 <= version);

    if ( errNone == FtrGet( palmResizeCreator, FTR_PALMRESIZE_DISABLE, &value ) ) {
        G(hardware) = DIA_HARDWARE_NONE;
    }
    else {
        G(hardware) = InitializeHandera(GLOBALS_VOID);
        if (G(hardware) == DIA_HARDWARE_NONE)
            G(hardware) = InitializePalmOne(GLOBALS_VOID);
        if (G(hardware) == DIA_HARDWARE_NONE)
            G(hardware) = InitializePalm(GLOBALS_VOID);
        if (G(hardware) == DIA_HARDWARE_NONE)
            G(hardware) = InitializeSony(GLOBALS_VOID);
        if (G(hardware) == DIA_HARDWARE_NONE)
            G(hardware) = InitializeUnknown(GLOBALS_VOID);
        RegisterNotification(GLOBALS_VOID);
    }

    return G(hardware);
}



void TerminateDIA(D_REFNUM_VOID)
{
    GET_GLOBALS

    UnregisterNotification(GLOBALS_VOID);
    if (GetDIAState(REFNUM_VOID) == DIA_STATE_NO_STATUS_BAR)
        SetDIAState(REFNUM DIA_STATE_MIN);
    switch (G(hardware)) {
#ifdef SUPPORT_DIA_SONY
        case DIA_HARDWARE_SONY1:
            SilkLibClose(G(sonyRefNum));
            break;
        case DIA_HARDWARE_SONY2:
            VskClose(G(sonyRefNum));
            break;
#endif
#ifdef SUPPORT_DIA_PALMONE
        case DIA_HARDWARE_PALMONE:
            PalmOneStatusBarLibClose(G(palmOneStatRefNum));
            SysLibRemove(G(palmOneStatRefNum));
            PinLibClose(G(palmOnePinRefNum));
            SysLibRemove(G(palmOneStatRefNum));
            break;
#endif

#ifdef SUPPORT_DIA_HANDERA
        case DIA_HARDWARE_HANDERA:{
                VgaRotateModeType rotation;
                VgaScreenModeType screenMode;

                VgaGetScreenMode(&screenMode, &rotation);

                if (screenMode != G(savedHanderaScreenMode) ||
                    G(savedHanderaRotation) != rotation) {
                    VgaSetScreenMode(G(savedHanderaScreenMode),
                        G(savedHanderaRotation));
                }
            }
#endif
        default:
            break;
    }
    G(hardware) = DIA_HARDWARE_NONE;
}



void SetDIAState(D_REFNUM DIAStateType state)
{
    GET_GLOBALS

    if (DIA_HARDWARE_HANDERA == G(hardware) && GetDIAState(REFNUM_VOID) == state)
        return;

    switch (G(hardware)) {
#ifdef SUPPORT_DIA_SONY
        case DIA_HARDWARE_SONY1:{
                Boolean oldAllow;

                oldAllow = G(sony1AllowResize);

                if (!oldAllow)
                    SetDIAAllowResize(REFNUM true);

                switch (state) {
                    case DIA_STATE_MAX:
                        SilkLibResizeDispWin(G(sonyRefNum), silkResizeNormal);
                        break;
                    case DIA_STATE_MIN:
                        SilkLibResizeDispWin(G(sonyRefNum), silkResizeToStatus);
                        break;
                    case DIA_STATE_NO_STATUS_BAR:
                        SilkLibResizeDispWin(G(sonyRefNum), silkResizeMax);
                        break;
                    default:
                        break;
                }
                break;

                if (!oldAllow)
                    SetDIAAllowResize(REFNUM false);
            }

        case DIA_HARDWARE_SONY2:{
                Boolean oldAllow;
                UInt16 allowState;

                if (errNone == VskGetState(G(sonyRefNum), vskStateEnable,
                        &allowState))
                    oldAllow = (allowState != 0);
                else
                    oldAllow = false;

                if (!oldAllow) {
                    SetDIAAllowResize(REFNUM true);
                }

                switch (state) {
                    case DIA_STATE_MAX:
                        VskSetState(G(sonyRefNum), vskStateResize, vskResizeMax);
                        break;
                    case DIA_STATE_MIN:
                        VskSetState(G(sonyRefNum), vskStateResize, vskResizeMin);
                        break;
                    case DIA_STATE_NO_STATUS_BAR:
                        VskSetState(G(sonyRefNum), vskStateResize, vskResizeNone);
                        break;
                    default:
                        break;
                }
                break;

                if (!oldAllow)
                    SetDIAAllowResize(REFNUM false);
            }
#endif
#ifdef SUPPORT_DIA_HANDERA
        case DIA_HARDWARE_HANDERA:
            switch (state) {
                case DIA_STATE_MAX:
                    SilkMaximizeWindow();
                    break;
                case DIA_STATE_MIN:
                case DIA_STATE_NO_STATUS_BAR:
                    SilkMinimizeWindow();
                    break;
                default:
                    break;
            }
            break;
#endif
#ifdef SUPPORT_DIA_PALMONE
        case DIA_HARDWARE_PALMONE:
            switch (state) {
                case DIA_STATE_MAX:
                    PalmOneStatusBarShow( G(palmOneStatRefNum) );
                    if ( G(palmOneAllowResize) )
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaShow );
                    else
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaLegacyMode );
                    break;
                case DIA_STATE_MIN:
                    if ( G(palmOneAllowResize) )
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaHide );
                    else
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaFullScreen );
                    PalmOneStatusBarShow( G(palmOneStatRefNum) );

                    break;
                case DIA_STATE_NO_STATUS_BAR:
                    if ( G(palmOneAllowResize) )
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaHide );
                    else
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaFullScreen );
                    PalmOneStatusBarHide( G(palmOneStatRefNum) );
                    break;
                default:
                    break;
            }
            break;
#endif
#ifdef HAVE_PALM_DIA_SDK
        case DIA_HARDWARE_PALM10:
        case DIA_HARDWARE_PALM11:
            switch (state) {
                case DIA_STATE_MAX:
                    StatShow();
                    PINSetInputAreaState(pinInputAreaOpen);
                    break;
                case DIA_STATE_MIN:
                    PINSetInputAreaState(pinInputAreaClosed);
                    StatShow();
                    break;
                case DIA_STATE_NO_STATUS_BAR:
                    StatHide();
                    if (PINGetInputAreaState() == pinInputAreaOpen)
                        PINSetInputAreaState(pinInputAreaClosed);
                    /* extra hide for some devices */
                    if ( GetDIAState(REFNUM_VOID) != DIA_STATE_NO_STATUS_BAR )
                        StatHide();
                    break;
                default:
                    break;
            }
            break;
#endif
        default:
            break;
    }
}



DIAStateType GetDIAState(D_REFNUM_VOID)
{
    GET_GLOBALS

    switch (G(hardware)) {
#ifdef SUPPORT_DIA_PALMONE
        case DIA_HARDWARE_PALMONE:

            switch ( PinGetInputAreaState( G(palmOnePinRefNum) ) ) {
                case pinInputAreaHide:
                case pinInputAreaFullScreen: {
                    UInt32 statBar;

                    PalmOneStatusBarGetAttribute( G(palmOneStatRefNum),
                        palmOneStatusBarAttrBarVisible,
                        &statBar );

                    return statBar ? DIA_STATE_MIN : DIA_STATE_NO_STATUS_BAR;
                }
                default:
                    return DIA_STATE_MAX;
            }
            break;
#endif
#ifdef SUPPORT_DIA_SONY
        case DIA_HARDWARE_SONY1:{
                Coord extentY;

                PushCoordinateSystemToStandard(GLOBALS_VOID);
                WinGetDisplayExtent(NULL, &extentY);
                PopCoordinateSystem(GLOBALS_VOID);
                if (240 <= extentY)
                    return DIA_STATE_NO_STATUS_BAR;
                else if (225 <= extentY)
                    return DIA_STATE_MIN;
                else
                    return DIA_STATE_MAX;
                break;
            }
        case DIA_HARDWARE_SONY2:{
                UInt16 state;
                Err err;

                err = VskGetState(G(sonyRefNum), vskStateResize, &state);
                if (err != errNone)
                    return DIA_STATE_UNDEFINED;
                switch (state) {
                    case vskResizeMax:
                        return DIA_STATE_MAX;
                    case vskResizeMin:
                        return DIA_STATE_MIN;
                    case vskResizeNone:
                        return DIA_STATE_NO_STATUS_BAR;
                    default:
                        return DIA_STATE_UNDEFINED;
                }
            }
#endif
#ifdef SUPPORT_DIA_HANDERA
        case DIA_HARDWARE_HANDERA:
            if (SilkWindowMaximized())
                return DIA_STATE_MAX;
            else
                return DIA_STATE_MIN;
#endif
#ifdef HAVE_PALM_DIA_SDK
        case DIA_HARDWARE_PALM10:
        case DIA_HARDWARE_PALM11:
            switch (PINGetInputAreaState()) {
                case pinInputAreaOpen:
                    return DIA_STATE_MAX;
                case pinInputAreaClosed: {
                        Err err;
                        UInt32 value;

                        err = StatGetAttribute(statAttrBarVisible, &value);

                        if (err != errNone || value != 0)
                            return DIA_STATE_MIN;
                        else
                            return DIA_STATE_NO_STATUS_BAR;
                    }
                default:
                    return DIA_STATE_UNDEFINED;
            }
#endif
        default:
            return DIA_STATE_MAX;
    }
}



void SetDIAAllowResize(D_REFNUM Boolean allow)
{
    GET_GLOBALS

    switch (G(hardware)) {
#ifdef SUPPORT_DIA_SONY
        case DIA_HARDWARE_SONY2:
            if (allow) {
                /*
                 * If available, enable horizontal resize
                 */
                if (0x03 <= VskGetAPIVersion(G(sonyRefNum)))
                    VskSetState(G(sonyRefNum), vskStateEnable,
                        vskResizeHorizontally);
                /*
                 * Enable vertical resize 
                 */
                VskSetState(G(sonyRefNum), vskStateEnable, vskResizeVertically);
            } else {
                VskSetState(G(sonyRefNum), vskStateEnable, 0);
            }
            break;
        case DIA_HARDWARE_SONY1:
            if (allow)
                SilkLibEnableResize(G(sonyRefNum));
            else
                SilkLibDisableResize(G(sonyRefNum));

            G(sony1AllowResize) = allow;

            break;
#endif
#ifdef HAVE_PALM_DIA_SDK
        case DIA_HARDWARE_PALM11:
        case DIA_HARDWARE_PALM10:
            PINSetInputTriggerState(allow ? pinInputTriggerEnabled :
                pinInputTriggerDisabled);
            SysSetOrientationTriggerState(allow ?
                sysOrientationTriggerEnabled : sysOrientationTriggerDisabled);
            break;
#endif
#ifdef SUPPORT_DIA_PALMONE
        case DIA_HARDWARE_PALMONE:
            if ( allow ) {
                switch( PinGetInputAreaState( G(palmOnePinRefNum) ) ) {
                    case pinInputAreaLegacyMode:
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaShow );
                        break;
                    case pinInputAreaFullScreen:
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaHide );
                        break;
                }
            }
            else {
                switch( PinGetInputAreaState( G(palmOnePinRefNum) ) ) {
                    case pinInputAreaShow:
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaLegacyMode );
                        break;
                    case pinInputAreaHide:
                        PinSetInputAreaState( G(palmOnePinRefNum), pinInputAreaFullScreen );
                        break;
                }
            }

            G(palmOneAllowResize) = allow;
#endif
            /*
             * Note: On Handera, resizing is always enabled
             */
        default:
            break;
    }
}




static Boolean MatchLastExtents(D_GLOBALS_VOID)
{
    static Coord lastX;
    static Coord lastY;
    Coord extentX;
    Coord extentY;

    extentX = lastX;
    extentY = lastY;

    PushCoordinateSystemToStandard(GLOBALS_VOID);

    WinGetDisplayExtent(&lastX, &lastY);

    PopCoordinateSystem(GLOBALS_VOID);

    return extentX == lastX && extentY == lastY;
}




Boolean HandleResizeNotification(D_REFNUM UInt32 notificationType)
{
    GET_GLOBALS

    switch (notificationType) {
#ifdef SUPPORT_DIA_PALMONE
        case AiaExtentChangedEvent:
#endif
#ifdef SUPPORT_DIA_SONY
        case sysNotifyDisplayChangeEvent:
#endif
#ifdef HAVE_PALM_DIA_SDK
        case sysNotifyDisplayResizedEvent:
#endif
#ifdef SUPPORT_DIA_HANDERA
        case trgNotifySilkEvent:
#endif
            {
                EventType e;

                if (!MatchLastExtents(GLOBALS_VOID)) {
                    SetHaveWinDisplayChangedEvent(REFNUM true);
                    if (G(hardware) != DIA_HARDWARE_PALM11) {
                        MemSet(&e, sizeof(EventType), 0);
                        e.eType = winDisplayChangedEvent;
                        EvtAddUniqueEventToQueue(&e, 0, true);
                    }
                }
                return true;
            }
        default:
            return false;
    }
}



void SetCustomDIAPolicy(D_REFNUM UInt16 formID)
{
#ifdef HAVE_PALM_DIA_SDK
    GET_GLOBALS

    if (G(hardware) == DIA_HARDWARE_PALM10 || G(hardware) == DIA_HARDWARE_PALM11) {
        FormType *formPtr;

        formPtr = FrmGetFormPtr(formID);
        if (formPtr != NULL) {
            FrmSetDIAPolicyAttr(formPtr, frmDIAPolicyCustom);
        }
    }
#endif
}



DIAHardwareType GetDIAHardware(D_REFNUM_VOID)
{
    GET_GLOBALS

    return G(hardware);
}



void SetDIAConstraints(D_REFNUM WinHandle winH, Boolean big, Boolean allowBig)
{
#ifdef HAVE_PALM_DIA_SDK
    GET_GLOBALS

    if (G(hardware) == DIA_HARDWARE_PALM10 || G(hardware) == DIA_HARDWARE_PALM11) {
        PushCoordinateSystemToStandard(GLOBALS_VOID);

        WinSetConstraintsSize(winH,
            STD_EXTENT_Y,
            big ? MAX_EXTENT_Y : STD_EXTENT_Y,
            allowBig ? MAX_EXTENT_Y : STD_EXTENT_Y,
            STD_EXTENT_X,
            big ? MAX_EXTENT_X : STD_EXTENT_X,
            allowBig ? MAX_EXTENT_X : STD_EXTENT_X);

        PopCoordinateSystem(GLOBALS_VOID);
    }
#endif
}



/* Check which DIA state covers more screen space */
Int16 CompareDIAState(D_REFNUM DIAStateType x, DIAStateType y)
{
    if (y == DIA_STATE_UNDEFINED)
        y = DIA_STATE_MAX;
    if (x == DIA_STATE_UNDEFINED)
        x = DIA_STATE_MAX;
    if (x == y)
        return 0;
    switch (x) {
        case DIA_STATE_MIN:
            return y == DIA_STATE_NO_STATUS_BAR ? 1 : -1;
        case DIA_STATE_NO_STATUS_BAR:
            return -1;
        case DIA_STATE_MAX:
        default:
            return 1;
    }
}



static UInt16 SafeWinSetCoordinateSystem(D_GLOBALS UInt16 coordSys)
{
    if (!G(palmHiRes) || NULL == WinGetDrawWindow())
        return kCoordinatesStandard;
    else
        return WinSetCoordinateSystem(coordSys);
}



void PushCoordinateSystemToStandard(D_GLOBALS_VOID)
{
    G(coordSysStack)[G(coordSysStackPtr)++] =
        SafeWinSetCoordinateSystem(GLOBALS kCoordinatesStandard);
}




void PopCoordinateSystem(D_GLOBALS_VOID)
{
    SafeWinSetCoordinateSystem(GLOBALS G(coordSysStack)[--G(coordSysStackPtr)]);
}



void GetHiddenStatusBarArea(D_REFNUM RectangleType * area)
{
    GET_GLOBALS

    MemSet(area, sizeof(RectangleType), 0);

    if (DIA_STATE_NO_STATUS_BAR != GetDIAState(REFNUM_VOID)) {
        return;
    }

    switch (G(hardware)) {
        case DIA_HARDWARE_PALMONE:
        case DIA_HARDWARE_SONY2:
        case DIA_HARDWARE_SONY1:
        case DIA_HARDWARE_PALM11:
        case DIA_HARDWARE_PALM10:{
                Coord extentX;
                Coord extentY;

                WinGetDisplayExtent(&extentX, &extentY);
                switch (extentX) {
                    case 160:  /* 160 x 240 */
                        area->topLeft.x = 0;
                        area->topLeft.y = 225;
                        area->extent.x = 160;
                        area->extent.y = 240 - 225;
                        break;
                    case 240:  /* 240 x 160 */
                        area->topLeft.x = 225;
                        area->topLeft.y = 0;
                        area->extent.x = 240 - 225;
                        area->extent.y = 160;
                        break;
                    case 320:  /* 320 x 480 */
                        area->topLeft.x = 0;
                        area->topLeft.y = 450;
                        area->extent.x = 320;
                        area->extent.y = 480 - 450;
                        break;
                    case 480:  /* 480 x 320 */
                        area->topLeft.x = 450;
                        area->topLeft.y = 0;
                        area->extent.x = 480 - 450;
                        area->extent.y = 320;
                        break;
                    default:
                        break;
                }
                break;
            }
        default:
            break;
    }
}



FontID HanderaAdjustFont(D_REFNUM FontID font)
{
#ifdef SUPPORT_DIA_HANDERA
    GET_GLOBALS
    if (G(hardware) == DIA_HARDWARE_HANDERA && 0 == (font & fntAppFontCustomBase))
        return VgaBaseToVgaFont(font);
    else
#endif
        return font;
}



Coord HanderaCoord(D_REFNUM Coord x)
{
#ifdef SUPPORT_DIA_HANDERA
    GET_GLOBALS
    return G(hardware) == DIA_HARDWARE_HANDERA ? x * 3 / 2 : x;
#else
    return x;
#endif
}

#endif
