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

#if !defined DIALOGHIDDENEDGES_DEFINED
#define DIALOGHIDDENEDGES_DEFINED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DialogHiddenEdges.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogHiddenEdges dialog

class CDialogHiddenEdges : public CDialog
{
	// Construction
	private:
		std::vector<std::vector<bool>> m_visible;
		long m_SelectedVariation;

		CDB_Model *DDBB;
		void GDI_DrawSketch2D();

	public:
		void SetParams(std::vector<std::vector<bool>> visible,
					   long SelectedVariation);
		void GetParams(long &SelectedVariation);
		CDialogHiddenEdges(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
		//{{AFX_DATA(CDialogHiddenEdges)
		enum { IDD = IDD_DIALOG_Display_HiddenEdges };
			// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDialogHiddenEdges)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(CDialogHiddenEdges)
		afx_msg void OnCHECKPrevious();
		afx_msg void OnCHECKNext();
		virtual BOOL OnInitDialog();
		afx_msg void OnPaint();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined DIALOGHIDDENEDGES_DEFINED
