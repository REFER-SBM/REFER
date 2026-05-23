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

#if !defined(DIALOGDETAILSSUBGRAPHS_INCLUDED)
	#define DIALOGDETAILSSUBGRAPHS_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogDetailsSubGraphs.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogDetailsSubGraphs dialog

	class CDialogDetailsSubGraphs : public CDialog
	{
		// Construction
		public:
			void SetParams(CDataSubGraphs SG);
			void GetParams(CDataSubGraphs &SG);
			CDialogDetailsSubGraphs(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogDetailsSubGraphs)
			enum { IDD = IDD_DIALOG_Details_SubGraphs };
			int		m_SubGraphsApproach;
			//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogDetailsSubGraphs)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogDetailsSubGraphs)
				// NOTE: the ClassWizard will add member functions here
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGDETAILSSUBGRAPHS_INCLUDED)
