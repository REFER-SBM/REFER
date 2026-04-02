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

// Refer.h : main header file for the REFER application
//
#if !defined(REFER_H_INCLUDED)
	#define REFER_H_INCLUDED

	#include "DataRefer.h"

	#if _MSC_VER >= 1000
	#pragma once
	#endif // _MSC_VER >= 1000

	//#ifndef __AFXWIN_H__
	//	#error include 'stdafx.h' before including this file for PCH
	//#endif

	/////////////////////////////////////////////////////////////////////////////
	// CReferApp:
	// See Refer.cpp for the implementation of this class
	//

	// The main application class in MFC encapsulates the initialization,
	// running, and termination of an application for the Windows operating system.
	// An application built on the framework must have one and only one object
	// of a class derived from CWinApp.
	// This object is constructed before windows are created
	class CReferApp : public CWinApp
	{
		private:
			void LoadParamsFromINI();

		public:
			CReferApp();

			void SaveParamsToINI();

			CDocument* OpenDocumentFile(LPCTSTR lpszFileName);

			// The application uses different document types
			// ---------------------------------------------
			CMultiDocTemplate *m_pTemplateFrSketch2D;
			CMultiDocTemplate *m_pTemplateFrModelBrep;
			CMultiDocTemplate *m_pTemplateFrModelCSG;
			CMultiDocTemplate *m_pTemplateFrOptimization;

			// Operation modes
			// ---------------
			bool SketchMode;
			COLORREF m_BackgroundColor;

			bool BackgroundBitMap;
			CString BackgroundBitMapFile;

			CString m_sTimeCalculating;

			// Definition of sets of data
			// --------------------------
			CDataRefer GeneralParams;

			// Data for vectorization
			CDataVectorize Ve;

			// Data for 2D refinement
			CDataRefine2D R2D;

			// Data for detection of cues
			CDataCues Cu;

			// Data for detection of features
			CDataFeatures Fe;

			// Data for direct inflation
			CDataDirectInflation DI;
			CDataLevels DIL;
			CDataAxonometric DIA;

			// Data for iterative inflation
			CDataInflation In;

			// Data for CSG modeling
			CDataCSG CSG;

			// Data for 3D refinement
			CDataRefine3D R3D;

			// Data for 3D tessellation
			CDataTessellate3D T3D;

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CReferApp)
			public:
			virtual BOOL InitInstance();
			virtual int ExitInstance();
			//}}AFX_VIRTUAL

		// Implementation

			//{{AFX_MSG(CReferApp)
			afx_msg void OnAppAbout();
			afx_msg void OnFileOpen();
			afx_msg void OnSaveSettings();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};


	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


	/////////////////////////////////////////////////////////////////////////////
	// The one and only CReferApp object
	extern CReferApp ReferApp;

#endif // !defined(REFER_H_INCLUDED)
