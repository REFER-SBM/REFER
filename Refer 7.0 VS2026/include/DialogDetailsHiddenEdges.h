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

#if !defined(DIALOGDETAILSHIDDENEDGES_INCLUDED)
	#define DIALOGDETAILSHIDDENEDGES_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogDetailsHiddenEdges.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogDetailsHiddenEdges dialog

	class CDialogDetailsHiddenEdges : public CDialog
	{
		// Construction
		public:
			void SetParams(CDataHiddenEdges Hi);
			void GetParams(CDataHiddenEdges &Hi);
			CDialogDetailsHiddenEdges(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogDetailsHiddenEdges)
			enum { IDD = IDD_DIALOG_Details_HiddenEdges };
			BOOL	m_HiddenEdgesByDashedLines;
			BOOL	m_HiddenEdgesRefineDashedLines;
			int		m_HiddenEdgesApproach;
			int		m_HiddenEdgesAutomatic;
			//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogDetailsHiddenEdges)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogDetailsHiddenEdges)
				// NOTE: the ClassWizard will add member functions here
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGDETAILSHIDDENEDGES_INCLUDED)
