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

#if !defined(AFX_DIALOGDETAILSREGULARITIESNATURE_H_INCLUDED_)
#define AFX_DIALOGDETAILSREGULARITIESNATURE_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CDialogDetailsRegularitiesNature.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesNature dialog

class CDialogDetailsRegularitiesNature : public CDialog
{
// Construction
public:
	void SetParams(CDataInflation In);
	void GetParams(CDataInflation &In);
	CDialogDetailsRegularitiesNature(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DialogDetailsRegularitiesNature)
	enum { IDD = IDD_DIALOG_DetailsRegularitiesNature };
	BOOL	m_NDA;
	BOOL	m_NFP;
	BOOL	m_NPL;
	BOOL	m_NCL;
	BOOL	m_NVL;
	BOOL	m_NOL;
	BOOL	m_NI;
	BOOL	m_NCC;
	BOOL	m_NPEC;
	BOOL	m_NOFO;
	BOOL	m_NSFO;

	BOOL	m_CDA;
	BOOL	m_CFP;
	BOOL	m_CPL;
	BOOL	m_CCL;
	BOOL	m_CVL;
	BOOL	m_COL;
	BOOL	m_CI;
	BOOL	m_CCC;
	BOOL	m_CPEC;
	BOOL	m_COFO;
	BOOL	m_CSFO;
	
	BOOL	m_PDA;
	BOOL	m_PFP;
	BOOL	m_PPL;
	BOOL	m_PCL;
	BOOL	m_PVL;
	BOOL	m_POL;
	BOOL	m_PI;
	BOOL	m_PCC;
	BOOL	m_PPEC;
	BOOL	m_POFO;
	BOOL	m_PSFO;
	
	BOOL	m_PRDA;
	BOOL	m_PRFP;
	BOOL	m_PRPL;
	BOOL	m_PRCL;
	BOOL	m_PRVL;
	BOOL	m_PROL;
	BOOL	m_PRI;
	BOOL	m_PRCC;
	BOOL	m_PRPEC;
	BOOL	m_PROFO;
	BOOL	m_PRSFO;

	BOOL	m_IDDA;
	BOOL	m_IDFP;
	BOOL	m_IDPL;
	BOOL	m_IDCL;
	BOOL	m_IDOL;
	BOOL	m_IDI;
	BOOL	m_IDCC;
	BOOL	m_IDOFO;
	BOOL	m_IDPEC;
	BOOL	m_IDSFO;
	BOOL	m_IDVL;

	BOOL	m_INDDA;
	BOOL	m_INDFP;
	BOOL	m_INDPL;
	BOOL	m_INDCL;
	BOOL	m_INDVL;
	BOOL	m_INDOL;
	BOOL	m_INDI;
	BOOL	m_INDCC;
	BOOL	m_INDPEC;
	BOOL	m_INDOFO;
	BOOL	m_INDSFO;

//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DialogDetailsRegularitiesNature)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DialogDetailsRegularitiesNature)
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSREGULARITIESNATURE_H_INCLUDED_)
