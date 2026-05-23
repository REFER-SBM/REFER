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

#if !defined(AFX_DIALOGMAINAXESAUTOMATIC_INCLUDED)
#define AFX_DIALOGMAINAXESAUTOMATIC_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogMainAxesRules.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogMainAxesAutomatic dialog

class CDialogMainAxesAutomatic : public CDialog
{
// Construction
public:
	CDialogMainAxesAutomatic(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataMainAxes MA);
	void GetParams(CDataMainAxes &MA);


// Dialog Data
	//{{AFX_DATA(CDialogMainAxesAutomatic)
	enum { IDD = IDD_DIALOG_MainAxesRules };
	BOOL	m_ProjectiveRule;
	double	m_ToleranceCubicAngles;
	BOOL	m_MatchingCornersRule;
	double	m_MeritMatchingCorners;
	BOOL	m_IsometricRule;
	double	m_MeritIsometric;
	BOOL	m_VerticalRule;
	double	m_MeritVertical;
	BOOL	m_HorizontalRule;
	double	m_MeritHorizontal;
	BOOL	m_FrequencyRule;
	double	m_MeritFrequency;
	double	m_ThresholdOverlap;
	double	m_ThresholdAngle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogMainAxesAutomatic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogMainAxesAutomatic)
	afx_msg void OnMainAxesRulesDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGMAINAXESAUTOMATIC_INCLUDED)
