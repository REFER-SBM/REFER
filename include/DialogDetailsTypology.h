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

#if !defined(AFX_DIALOGDETALLESNATURALEZAOBJETO_H__BD4440E1_A1B6_11D3_9C09_00508B0BD793__INCLUDED_)
#define AFX_DIALOGDETALLESNATURALEZAOBJETO_H__BD4440E1_A1B6_11D3_9C09_00508B0BD793__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsTypology.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsTypology dialog

class CDialogDetailsTypology : public CDialog
{
// Construction
public:
	void SetParams(CDataTypology Ty);
	void GetParams(CDataTypology &Ty);
	CDialogDetailsTypology(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsTypology)
	enum { IDD = IDD_DIALOG_DetailsTypology };
	double m_FrequencyPrismaticObjects;
	double m_FrequencyUndefinedObjects_D;
	double m_ThresholdForParallel;
	double m_ThresholdForArea;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsTypology)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsTypology)
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETALLESNATURALEZAOBJETO_H__BD4440E1_A1B6_11D3_9C09_00508B0BD793__INCLUDED_)
