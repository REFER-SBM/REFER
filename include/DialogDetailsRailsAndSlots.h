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

#if !defined(DIALOGDETAILSRAILSANDSLOTS_INCLUDED)
	#define DIALOGDETAILSRAILSANDSLOTS_INCLUDED

	#if _MSC_VER > 1000
	#pragma once
	#endif // _MSC_VER > 1000
	// DialogDetailsRailsAndSlots.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogDetailsRailsAndSlots dialog

	class CDialogDetailsRailsAndSlots : public CDialog
	{
		// Construction
		public:
			void SetParams(CDataRailsAndSlots RS);
			void GetParams(CDataRailsAndSlots &RS);
			CDialogDetailsRailsAndSlots(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogDetailsRailsAndSlots)
			enum { IDD = IDD_DIALOG_Details_RailsAndSlots };
			double m_LongEdges;
			double m_ShortEdges;
			double m_SimilarFacesDistances;
			double m_SimilarFacesAngles;
			double m_CollinearEdges;
			//}}AFX_DATA


		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogDetailsRailsAndSlots)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:

			// Generated message map functions
			//{{AFX_MSG(CDialogDetailsRailsAndSlots)
				// NOTE: the ClassWizard will add member functions here
			afx_msg void OnRestoreDefault();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGDETAILSRAILSANDSLOTS_INCLUDED)
