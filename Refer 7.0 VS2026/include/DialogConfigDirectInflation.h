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

#if !defined(DIALOGCONFIGDIRECTINFLATION_INCLUDED)
	#define DIALOGCONFIGDIRECTINFLATION_INCLUDED

	#if _MSC_VER >= 1000
		#pragma once
	#endif // _MSC_VER >= 1000
	// DialogConfigDirectInflation.h : header file
	//

	class CDialogDetailsLevels;

	/////////////////////////////////////////////////////////////////////////////
	// CDialogConfigDirectInflation dialog

	class CDialogConfigDirectInflation : public CDialog
	{
		// Construction
		public:
			CDialogConfigDirectInflation(CWnd* pParent = NULL);   // standard constructor
			void SetParams();
			void GetParams();

		// Dialog Data
			//{{AFX_DATA(CDialogConfigDirectInflation)
			enum { IDD = IDD_DIALOG_ConfigDirectInflation };
			int m_DirectInflationApproach;

			//}}AFX_DATA

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogConfigDirectInflation)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogConfigDirectInflation)
			virtual BOOL OnInitDialog();
			afx_msg void OnDetailsAutomaticDirectInflation();
			afx_msg void OnDetailsDirectInflateAxonometric();
			afx_msg void OnDetailsDirectInflateLevels();
			afx_msg void OnBUTTONRestoreDefault();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGCONFIGDIRECTINFLATION_INCLUDED)
