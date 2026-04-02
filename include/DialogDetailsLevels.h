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

#if !defined(AFX_DIALOGDETAILSLEVELS_INCLUDED)
#define AFX_DIALOGDETAILSLEVELS_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsLevels.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsLevels dialog

class CDialogDetailsLevels : public CDialog
{
// Construction
public:
	void SetParams(CDataLevels DIL);
	void GetParams(CDataLevels &DIL);
	CDialogDetailsLevels(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsLevels)
	enum { IDD = IDD_DIALOG_DetailsLevels };
	int		m_IncrementoNiveles;

	double	m_Nivel0;
	double	m_Nivel1;
	double	m_Nivel2;
	double	m_Nivel3;
	double	m_Nivel4;
	double	m_Nivel5;

	int		m_Expansion;
	double	m_CoeficienteAlargamiento;
	double	m_CoeficienteAlargamiento2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsLevels)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsLevels)
	afx_msg void OnBUTTONAutomatic();
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSLEVELS_INCLUDED)
