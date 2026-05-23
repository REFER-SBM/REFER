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

#if !defined DIALOG_REGULARITIES2D_INCLUDED
#define DIALOG_REGULARITIES2D_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// Regularities2D.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CRegularities2D dialog

	class CDialogRegularities2D : public CDialog
	{
	// Construction
	public:
		CDialogRegularities2D(CWnd* pParent = NULL);   // standard constructor
		void SetParams(CDataRefine2D R2D);
		void GetParams(CDataRefine2D &R2D);


	// Dialog Data
		//{{AFX_DATA(CRegularidades2D)
		enum { IDD = IDD_DIALOG_Regularities2D };

		BOOL	m_CL2D;
		BOOL	m_PL2D;
		BOOL	m_VL2D;
		double	m_ErrorCollinearity;
		double	m_ErrorParallelism;
		double	m_ErrorVerticality;
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CRegularidades2D)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CRegularidades2D)
		afx_msg void OnRestoreDefaultRegularities2D();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined DIALOG_REGULARITIES2D_INCLUDED
