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

#if !defined(AFX_DIALOGDETAILSPREINFLATEAXONOMETRIC_H_INCLUDED_)
#define AFX_DIALOGDETAILSPREINFLATEAXONOMETRIC_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsPREINFLATEAxonometric.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPREINFLATEAxonometric dialog

class CDialogDetailsPREINFLATEAxonometric : public CDialog
{
// Construction
public:
	void SetParams(CDataAxonometric DIA);
	void GetParams(CDataAxonometric &DIA);
	CDialogDetailsPREINFLATEAxonometric(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsPREINFLATEAxonometric)
	enum { IDD = IDD_DIALOG_DetailsPreInflateAxonometric };
	int		m_Approach;
	BOOL	m_AmendCabinet;
	double	m_135degrees;
	double	m_90degrees;

	int		m_Expansion;
	double	m_CabinetThreshold;
	double	m_CabinetReduction;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsPREINFLATEAxonometric)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsPREINFLATEAxonometric)
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSPREINFLATEAXONOMETRIC_H_INCLUDED_)
