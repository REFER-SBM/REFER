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

#if !defined(AFX_DIALOGCONFIGOPTIMIZATIONINFLATION_INCLUDED_)
#define AFX_DIALOGCONFIGOPTIMIZATIONINFLATION_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigOptimizationInflation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigOptimizationInflation dialog

class CDialogConfigOptimizationInflation : public CDialog
{
// Construction
public:
	CDataOptHC m_DataOptHC;
	CDataOptSA m_DataOptSA;
	CDataOptSAM m_DataOptSAM;

	CDialogConfigOptimizationInflation(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataInflation In);
	void GetParams(CDataInflation &In);

// Dialog Data
	//{{AFX_DATA(CDialogConfigOptimizationInflation)
	enum { IDD = IDD_DIALOG_ConfigOptimizationInflation };
	int		m_OptAlgorithm;
	int		m_SelectRegularities;
	double	m_InflationAngleThreshold;
	BOOL	m_NormalizeCosts;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigOptimizationInflation)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigOptimizationInflation)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioAlgoritmoOptimizacion();
	afx_msg void OnDetailsHillClimbing();
	afx_msg void OnDetailsSimulatedAnnealing();
	afx_msg void OnDetailsSimulatedAnnealingMulti();

	afx_msg void OnDetailsRegularitiesNature();
	afx_msg void OnDetailsRegularitiesUser();
	afx_msg void OnDetailsRegularitiesCost();

	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGOPTIMIZATIONINFLATION_INCLUDED_)
