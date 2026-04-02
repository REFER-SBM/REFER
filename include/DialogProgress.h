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

#if !defined(AFX_DIALOGPROGRESS_INCLUDED)
#define AFX_DIALOGPROGRESS_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DialogProgress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogProgress dialog

class CDialogProgress : public CDialog
{
// Construction
private:
	CTime m_HoraInicial;
	bool m_CancelAsked;
	CProgressCtrl *m_ControlProgress;

public:
	CDialogProgress(CWnd* pParent = NULL);   // standard constructor
	void Message( CString sMessage );
	// Call the following two functions ALWAYS IN THIS ORDER
	void RangeProgress(long lMin, long lMax);
	bool UpdateProgress(long lPosition);

// Dialog Data
	//{{AFX_DATA(CDialogProgress)
	enum { IDD = IDD_DIALOG_Progress };
	CString	m_sMessage;
	CString	m_sTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogProgress)
	virtual BOOL OnInitDialog();
	afx_msg void OnBUTTONCancelarProgreso();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGPROGRESS_INCLUDED)
