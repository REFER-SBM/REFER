//+-----------------------------------------------------------------------------+
//| Copyright (C) 2009 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#if !defined(AFX_DIALOGDETALLESREGULARIDADESAUTOMATICA_H__3FBFD2C2_A70E_11D3_9C0C_00508B0BD793__INCLUDED_)
#define AFX_DIALOGDETALLESREGULARIDADESAUTOMATICA_H__3FBFD2C2_A70E_11D3_9C0C_00508B0BD793__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetallesRegularidadesAutomatica.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetallesRegularidadesAutomatica dialog

class CDialogDetallesRegularidadesAutomatica : public CDialog
{
// Construction
public:
	CDialogDetallesRegularidadesAutomatica(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetallesRegularidadesAutomatica)
	enum { IDD = IDD_DIALOG_DetallesSeleccionAutomatica };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetallesRegularidadesAutomatica)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetallesRegularidadesAutomatica)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETALLESREGULARIDADESAUTOMATICA_H__3FBFD2C2_A70E_11D3_9C0C_00508B0BD793__INCLUDED_)
