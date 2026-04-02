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

#if !defined(AFX_DIALOGDETAILSLEVELSHIDDEN_INCLUDED)
#define AFX_DIALOGDETAILSLEVELSHIDDEN_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsLevelsHidden.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsLevelsHidden dialog

class CDialogDetailsLevelsHidden : public CDialog
{
// Construction
public:
	void SetParams(CDataLevels DIL);
	void GetParams(CDataLevels &DIL);
	CDialogDetailsLevelsHidden(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsLevelsHidden)
	enum { IDD = IDD_DIALOG_DetailsLevelsHidden };
	int		m_IncrementoNiveles;

	double	m_Nivel0H;
	double	m_Nivel1H;
	double	m_Nivel2H;
	double	m_Nivel3H;
	double	m_Nivel4H;
	double	m_Nivel5H;
	double	m_Nivel6H;
	double	m_Nivel7H;
	double	m_Nivel8H;
	double	m_Nivel9H;
	double	m_Nivel10H;
	double	m_Nivel11H;
	double	m_Nivel12H;
	double	m_Nivel13H;
	double	m_Nivel14H;
	double	m_Nivel15H;
	double	m_Nivel16H;
	double	m_Nivel17H;

	int		m_Expansion;
	double	m_CoeficienteAlargamiento;
	double	m_CoeficienteAlargamiento2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsLevelsHidden)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsLevelsHidden)
	afx_msg void OnBUTTONAutomatic();
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSLEVELSHIDDEN_INCLUDED)
