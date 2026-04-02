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

#if !defined(DIALOGDETAILSLABELS_INCLUDED)
	#define DIALOGDETAILSLABELS_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogDetailsLabels.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogDetailsLabels dialog

	class CDialogDetailsLabels : public CDialog
	{
		// Construction
		public:
			void SetParams(CDataLabels La);
			void GetParams(CDataLabels &La);
			CDialogDetailsLabels(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogDetailsLabels)
			enum { IDD = IDD_DIALOG_Details_Labels };
			int m_LabelsApproach;
			int m_VertexCatalogue;
			//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogDetailsLabels)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogDetailsLabels)
				// NOTE: the ClassWizard will add member functions here
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGDETAILSLABELS_INCLUDED)
