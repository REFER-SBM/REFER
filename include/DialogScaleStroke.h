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

#if !defined(DIALOSCALESTROKE_INCLUDED)
#define DIALOSCALESTROKE_INCLUDED



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogScaleStroke.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogScaleStroke dialog

class CDialogScaleStroke : public CDialog
{
// Construction
public:
	void GetParams(POINT2D &scale);
	void SetParams(POINT2D scale);
	CDialogScaleStroke(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogScaleStroke)
	enum { IDD = IDD_DIALOG_ScaleStroke };
	double	m_ScaleStrokeX;
	double	m_ScaleStrokeY;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogScaleStroke)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOSCALESTROKE_INCLUDED)
