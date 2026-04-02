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

#if !defined(AFX_DIALOGCONFIGCSG_INCLUDED_)
#define AFX_DIALOGCONFIGCSG_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigCSG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigCSG dialog

class CDialogConfigCSG : public CDialog
{
// Construction
public:
	CDialogConfigCSG(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataCSG CSG);
	void GetParams(CDataCSG &CSG);

// Dialog Data
	//{{AFX_DATA(CDialogConfigCSG)
	enum { IDD = IDD_DIALOG_ConfigCSG };
	BOOL	m_BuildCSG;
	BOOL	m_CSG_Inflation;
	BOOL	m_CSG_Inflation_Direct;
	BOOL	m_CSG_Inflation_Optimization;
	double	m_CSG_ThresholdFMerit;
	double	m_CSG_ThresholdDMerit;
	double	m_CSG_ThresholdAngle;
	double	m_CSG_ThresholdOffset;
	double	m_CSG_ThresholdLocation;
	BOOL	m_CSG_UseMultipleOrigins;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigCSG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigCSG)
	virtual BOOL OnInitDialog();
	afx_msg void OnBUTTONRestoreDefault();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGCSG_INCLUDED_)
