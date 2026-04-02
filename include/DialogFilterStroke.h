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

#if !defined(DIALOFILTERSTROKE_INCLUDED)
#define DIALOFILTERSTROKE_INCLUDED



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogFilterStroke.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogFilterStroke dialog

class CDialogFilterStroke : public CDialog
{
// Construction
public:
	void GetParams(long &FilterMode, long &FilterFactor, long &ResampleFactor);
	void SetParams(long FilterMode, long FilterFactor, long ResampleFactor);
	CDialogFilterStroke(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogFilterStroke)
	enum { IDD = IDD_DIALOG_FilterStroke };
	int	m_FilterMode;
	long	m_FilterFactor;
	long	m_ResampleFactor;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogFilterStroke)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOFILTERSTROKE_INCLUDED)
