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

#if !defined(AFX_DIALOGCONFIGGRAPHICSOPTIMIZACION_INCLUDED)
#define AFX_DIALOGCONFIGGRAPHICSOPTIMIZACION_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// DialogOptimizationConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogOptimizationConfig dialog

class CDialogOptimizationConfig : public CDialog
{
// Construction
public:
	CDialogOptimizationConfig(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CGraphicStyle m_GraphicStyleDlg);
	void GetParams(CGraphicStyle &m_GraphicStyleDlg);

// Dialog Data
	//{{AFX_DATA(CDialogOptimizationConfig)
	enum { IDD = IDD_DIALOG_OptimizationGraphicsConfig };
	int		m_InitialVariable;
	int		m_FinalVariable;
	int		m_GraphicStyle;
	BOOL	m_ShowLegend;
	BOOL	m_ShowLabels;
	BOOL	m_ViewCostComponentsEvolution;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogOptimizationConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogOptimizationConfig)
	virtual BOOL OnInitDialog();
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGGRAPHICSOPTIMIZACION_INCLUDED)
