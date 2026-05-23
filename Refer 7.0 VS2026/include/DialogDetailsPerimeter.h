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

#if !defined(AFX_DIALOGDETAILSPERIMETER_INCLUDED)
#define AFX_DIALOGDETAILSPERIMETER_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDetailsPerimeter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPerimeter dialog

class CDialogDetailsPerimeter : public CDialog
{
// Construction
public:
	CDialogDetailsPerimeter(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataPerimeter Pe);
	void GetParams(CDataPerimeter &Pe);


// Dialog Data
	//{{AFX_DATA(CDialogDetailsPerimeter)
	enum { IDD = IDD_DIALOG_DetailsPerimeter };
	BOOL m_IncludeDanglingEdges;
	double m_ThresholdIntersection;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsPerimeter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsPerimeter)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSPERIMETER_INCLUDED)
