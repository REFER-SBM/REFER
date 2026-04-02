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

#if !defined(AFX_DIALOGDETAILSSIMULATEDANNEALING_H_INCLUDED_)
#define AFX_DIALOGDETAILSSIMULATEDANNEALING_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsSimulatedAnnealing.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSimulatedAnnealing dialog

class CDialogDetailsSimulatedAnnealing : public CDialog
{
// Construction
public:
	void SetParams(CDataOptSA DataOptSA);
	bool GetParams(CDataOptSA &DataOptSA);
	CDialogDetailsSimulatedAnnealing(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsSimulatedAnnealing)
	enum { IDD = IDD_DIALOG_DetailsSimulatedAnnealing };
	double	m_CriterioCongelacion;
	double	m_DecrementoTemperaturaPotencial;
	int		m_NumeroMaximoIteracionesEnEscalon;
	double	m_CriterioEquilibrio;
	double	m_CoefAciertosTemperaturaInicial;
	int		m_StyleIncrement;
	double	m_LengthInitialStep;
	double	m_LengthStepReduction;
	CString	m_IncrementsOfVariables;
	double	m_LengthFinalStep;
	int		m_MethodInitialTemperature;
	int		m_Repetitions;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsSimulatedAnnealing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsSimulatedAnnealing)
	afx_msg void OnBUTTONRestoreDefault();
	virtual BOOL OnInitDialog();
	afx_msg void OnRADIOParametrico();
	afx_msg void OnRadioPrefijado();
	//afx_msg void OnDetallesRestaurarValores();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSSIMULATEDANNEALING_H_INCLUDED_)
