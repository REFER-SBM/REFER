//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H_INCLUDED)
#define AFX_STDAFX_H_INCLUDED

	#if _MSC_VER >= 1000  //Executes if the compiler's version number is newer than 10.00.00000.00 
		#pragma once      //the file will be included (opened) only once by the compiler when compiling a source code file
	#endif // _MSC_VER >= 1000

	#define VC_EXTRALEAN			// Exclude rarely-used stuff from Windows headers
	//#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers (excludes less than VC_EXTRALEAN) 

	#define WINVER 0x0600
	#include <afxwin.h>			// MFC core and standard components
	#include <afxext.h>			// MFC extensions
	#include <afxcmn.h>			// MFC support for Windows Common Controls

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H_INCLUDED)
