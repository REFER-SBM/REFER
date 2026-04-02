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

#if !defined(AFX_DIALOGPREVIOUSNEXT_INCLUDED)
#define AFX_DIALOGPREVIOUSNEXT_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogPreviousNext.h : header file
//
class CReferDoc;

/////////////////////////////////////////////////////////////////////////////
// CDialogPreviousNext dialog

class CDialogPreviousNext : public CDialog
{
// Construction
private:
	long m_Current, m_First, m_Last;
	long m_Previous, m_Next;

	void UpdateRange();

public:
	void SetParams(long Current, long First, long Last,
				   long MessagePrevious, long MessageNext, long move);
	CDialogPreviousNext(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogPreviousNext)
	enum { IDD = IDD_DIALOG_PreviousNext };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPreviousNext)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogPreviousNext)
	afx_msg void OnButtonPrevious();
	afx_msg void OnButtonNext();
	afx_msg void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DialogPreviousNext_INCLUDED)
