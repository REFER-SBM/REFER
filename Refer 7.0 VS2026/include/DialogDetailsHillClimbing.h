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

#if !defined(AFX_DIALOGDETAILSHILLCLIMBING_INCLUDED_)
#define AFX_DIALOGDETAILSHILLCLIMBING_INCLUDED_

class CDialogHillClimbingPreConfig;

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsHillClimbing.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsHillClimbing dialog

class CDialogDetailsHillClimbing : public CDialog
{
// Construction
public:
	void SetParams(CDataOptHC DataOptHC);
	bool GetParams(CDataOptHC &DataOptHC);
	CDialogDetailsHillClimbing(CWnd* pParent = NULL);   // standard constructor
	CDialogHillClimbingPreConfig* pAyuda;

// Dialog Data
	//{{AFX_DATA(CDialogDetailsHillClimbing)
	enum { IDD = IDD_DIALOG_DetailsHillClimbing };
	double	m_ThresholdRejectBestCost;
	CString	m_IncrementsOfVariables;
	double	m_LengthInitialStep;
	double	m_LengthStepReduction;
	int		m_EstiloIncremento;
	int		m_IncrementoPrefijado;
	double	m_LengthFinalStep;
	long	m_NumeroMaximoIteraciones;
	double	m_ThresholdRejectBestCostInLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsHillClimbing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsHillClimbing)
	afx_msg void OnBUTTONRestoreDefault();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioIncrementoPrefijado();
	afx_msg void OnRADIOEstiloIncremento();
	afx_msg void OnRadioEstiloVariablesMarill();
	afx_msg void OnRadioEstiloVariableLeclerc();
	afx_msg void OnRADIOIncrementoPrefijado();
	afx_msg void OnDetallesRestaurarValores();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSHILLCLIMBING_INCLUDED_)
