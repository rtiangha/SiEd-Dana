# SiEd-Dana - A Text Editor Optimized for the AlphaSmart Dana
A fork of the original SiEd Palm OS text editor project, optimized for the AlphaSmart Dana's widescreen display. SiEd-Dana is based off of the 0.10-beta3 release of SiEd from April 27, 2005 by Benjamin Roe.

## What Is It?
SiEd-Dana is a free and open-sourced simple text editor optimized for the AlphaSmart Dana based off of the original SiEd text editor for PalmOS devices by Benjamin Roe. Its purpose is to provide an easy way to create and edit large text documents without the limitations of the built-in Memopad program while taking advantage of the full 560x160 pixel display of the AlphaSmart Dana. The program is distributed under the GPL.

## Instructions
To install SiEd-Dana, just download the latest PRC file and use your favourite Palm desktop sofware to copy the file over to your AlphaSmart Dana. If you want to hack around with the source or build your own PRC you'll need the PRC-Tools package and a copy of Palm OS 5 SDK R3 or R4, Palm Desktop SDK 5.4, and a copy of the AlphaSmart Dana SDK.

Using SiEd-Dana is pretty self explanatory. All commands have a keyboard equivalent (listed in the menus). Documents auto-save on exiting the application.

## Features

* Support for plain-text files on VFS volumes
* DOC format support (uncompressed only at present)
* Edit ~600Kb files on most PalmOS 5 devices, ~100Kb files on earlier devices
* Search and replace
* Word count
* Large undo history
* Encrypted file support using 128-bit AES encryption
* Split view - view/edit two files at once Emacs style
* Macro Recording
* Support for text/plain files through Exchange Manager (beaming, inter-application transfer etc)
* 12 display fonts (4 Hires, 4 standard Palm, 4 normal-resolution)
* GPL licensed

## Requirements
* AlphaSmart Dana, either 8MB or 16MB versions

SiEd-Dana *may* run on other devices running PalmOS v.3.5 or greater, however the SiEd-Dana interfaces are optimized for the 560x160 resolution of the AlphaSmart Dana and as such, may not render/operate correctly on other Palm devces.

## Usage
There is a usage guide with screenshots and other useful information available from the original SiEd project site, which you can now access via [Archive.org's Wayback Machine](https://web.archive.org/web/20070130170506/http://benroe.com/sied/user.php). Even though the screen size in the User Guide is based on screens with a resolution of 160x160 pixels, functionality-wise, the interfaces are the same.

## Known Issues
* There is a bug where the program may crash when using the Inc. Search functionality.

## Development Environment
Compiles using PRC-Tools and PiLRC.

The original SiEd release from 2005 as published by Benjamin Roe was slightly modified in order to compile using more modern Palm SDKs released after 2005 (mainly, the addition of a few more library header includes in the editor.cc file) as well as to update the rendering engine to work with the AlphaSmart Dana's 560x160 pixel display. Has tested as successfully compiling using either Palm OS 5 SDK R3 or R4 combined with Palm Desktop SDK 5.4. It might work with other combinations of the Palm/Garnet SDKs as well, but for future development, it'd be wise to standardize on these versions of the SDKs. With the inclusion of the new library includes in `editor.cc`, SiEd-Dana may no longer compile successfully using older Palm SDK versions without reverting the changes first.

## How to Build?

Building a PalmOS executable from this source code requires a complete install of the PRC-Tools package, available from:

http://prc-tools.sourceforge.net

as well as Palm OS 5 SDK R3 or R4 along with Palm Desktop SDK 5.4 and a copy of the AlphaSmart Dana SDK.

When creating your Palm SDK directory by combining the AlphaSmart Dana SDK, Palm OS SDK and the Palm Desktop SDK, ensure that the `.h`. files from the AlphaSmart Dana SDK as well as the the `68K` and `Common` directories of the Palm Desktop SDK appear inside the root `include` folder of the Palm OS SDK. Depending on where/how you obtained your SDKs, the library files may have been formatted for Windows machines, which may give warnings when compiling SiEd-Dana under a Linux environment. If so, you'll want to run all of the text files through a utility such as `dos2unix` first to convert them into a Unix-friendly format before working with the files.

Also note that Palm changed some of the macro definitions in the later versions of their SDKs, which may require a modification to the `WideTallApp.h` file from the AlphaSmart SDK. If so, change all invocations of the _`Str()` macro in the `WideTallApp.h` file to invoke `_PalmTypes_OS_CALL_Str()` instead.

The targets (i.e. type make target) available for the make process are:

* `all`: builds the English language full version

* `portuguese`, `norwegian`, `dutch`, `italian`, `french`, `german`, `spanish`, `catalan`: builds appropriate language

Copyright Notice: This software is licensed under the terms of the GPL, included in the LICENSE file in the source code directory.

The icons are Copyright Stefan Smith and Tankut Enric, and the copyright for the translations is attributed in the `text_resource.h` file. Portions of SiEd-Dana are based off of the original SiEd project Copyright 2003-2005 Benjamin Roe. New additions are Copyright 2016 Reg Tiangha.

