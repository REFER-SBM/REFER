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

// DialogOptimizationConfig.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "WinRf_ViewOptimization_Graphics.h"
#include "DialogOptimizationConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogOptimizationConfig dialog


CDialogOptimizationConfig::CDialogOptimizationConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogOptimizationConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogOptimizationConfig)
	//}}AFX_DATA_INIT
}


void CDialogOptimizationConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogOptimizationConfig)
	DDX_Text(pDX, IDC_EDIT_InitialVariable, m_InitialVariable);
	DDV_MinMaxInt(pDX, m_InitialVariable, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_FinalVariable, m_FinalVariable);
	DDV_MinMaxInt(pDX, m_FinalVariable, 0, 1000);
	DDX_Radio(pDX, IDC_RADIO_GraphicStyle, m_GraphicStyle);
	DDX_Check(pDX, IDC_CHECK_ShowLegend, m_ShowLegend);
	DDX_Check(pDX, IDC_CHECK_ShowLabels, m_ShowLabels);
	DDX_Check(pDX, IDC_CHECK_CostComponentsEvolution, m_ViewCostComponentsEvolution);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogOptimizationConfig, CDialog)
	//{{AFX_MSG_MAP(CDialogOptimizationConfig)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogOptimizationConfig message handlers

BOOL CDialogOptimizationConfig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	UpdateData(false);
	
	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogOptimizationConfig::SetParams(CGraphicStyle m_GraphicStyleDlg)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_GraphicStyle = m_GraphicStyleDlg.iTypeOfGraphic;
	m_InitialVariable = m_GraphicStyleDlg.iInitialVariable;
	m_FinalVariable = m_GraphicStyleDlg.iFinalVariable;
	m_ShowLegend = m_GraphicStyleDlg.bShowLegend;
	m_ShowLabels = m_GraphicStyleDlg.bShowLabels;

	m_ViewCostComponentsEvolution = ReferApp.In.bViewPartialCostsEvolution;
}


void CDialogOptimizationConfig::GetParams(CGraphicStyle &m_GraphicStyleDlg)
{
	if ( m_FinalVariable < m_InitialVariable)
		m_FinalVariable = m_InitialVariable;
	m_GraphicStyleDlg.iTypeOfGraphic = (Graphic) m_GraphicStyle;
	m_GraphicStyleDlg.iInitialVariable = m_InitialVariable;
	m_GraphicStyleDlg.iFinalVariable = m_FinalVariable;
	m_GraphicStyleDlg.bShowLegend = m_ShowLegend !=0;
	m_GraphicStyleDlg.bShowLabels = m_ShowLabels !=0;
	ReferApp.In.bViewPartialCostsEvolution = m_ViewCostComponentsEvolution !=0;
}


void CDialogOptimizationConfig::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CGraphicStyle initial;
	SetParams(initial);

	UpdateData(false);
}

