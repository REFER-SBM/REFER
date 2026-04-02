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

#if !defined(DIALOGDETAILSSGROUPOVERTRACEDSTROKES_INCLUDED)
	#define DIALOGDETAILSSGROUPOVERTRACEDSTROKES_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogDetailsGroupOvertracedStrokes.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogDetailsGroupOvertracedStrokes dialog

	class CDialogDetailsGroupOvertracedStrokes : public CDialog
	{
		// Construction
		public:
			void SetParams(CDataVectorize Ve);
			void GetParams(CDataVectorize &Ve);
			CDialogDetailsGroupOvertracedStrokes(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogDetailsGroupOvertracedStrokes)
			enum { IDD = IDD_DIALOG_Details_GroupOvertracedStrokes };
			double m_MinOvertraceOverlap;
			double m_MaxOvertraceAngle;
			double m_MaxOvertraceOffset;
			double m_IncreaseOvertraceTolerances;
			double m_OvertraceLineTolMin;
			double m_OvertraceLineTolMax;
			double m_OvertraceLineSmoothPenalty;
			double m_OvertraceMinMeritLine;
			//}}AFX_DATA

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogDetailsGroupOvertracedStrokes)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogDetailsGroupOvertracedStrokes)
				// NOTE: the ClassWizard will add member functions here
			afx_msg void OnBUTTONRestoreDefault();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGDETAILSSGROUPOVERTRACEDSTROKES_INCLUDED)
