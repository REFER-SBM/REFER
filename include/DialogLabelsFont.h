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

#if !defined DIALOG_LABELSFONT_INCLUDED
#define DIALOG_LABELSFONT_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogLabelsFont.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogLabelsFont dialog

	class CDialogLabelsFont : public CDialog
	{
	// Construction
	public:
		CDialogLabelsFont(CWnd* pParent = NULL);   // standard constructor
		void SetParams(double FontHeighthScale);
		void GetParams(double &FontHeightScale);


	// Dialog Data
		//{{AFX_DATA(CDialogLabelsFont)
		enum { IDD = IDD_DIALOG_LabelsFont };
		double m_FontHeightScale;
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDialogLabelsFont)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CDialogLabelsFontD)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined DIALOG_LABELSFONT_INCLUDED
