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

// DialogConfigCSG.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "ReferDoc.h"

#include "DialogConfigCSG.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigCSG dialog


CDialogConfigCSG::CDialogConfigCSG(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigCSG::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigCSG)
	//}}AFX_DATA_INIT
}


void CDialogConfigCSG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigCSG)
	DDX_Check(pDX, IDC_CSG_Build_CSG, m_BuildCSG);
	DDX_Check(pDX, IDC_CSG_Inflation, m_CSG_Inflation);
	DDX_Check(pDX, IDC_CSG_Inflation_Direct, m_CSG_Inflation_Direct);
	DDX_Check(pDX, IDC_CSG_Inflation_Optimization, m_CSG_Inflation_Optimization);
	DDX_Text(pDX, IDC_CSG_ThresholdFMerit, m_CSG_ThresholdFMerit);
	DDX_Text(pDX, IDC_CSG_ThresholdDMerit, m_CSG_ThresholdDMerit);
	DDX_Text(pDX, IDC_CSG_ThresholdAngle, m_CSG_ThresholdAngle);
	DDX_Text(pDX, IDC_CSG_ThresholdOffset, m_CSG_ThresholdOffset);
	DDX_Text(pDX, IDC_CSG_ThresholdLocation, m_CSG_ThresholdLocation);
	DDX_Radio(pDX, IDC_CSG_UseMultipleOrigins, m_CSG_UseMultipleOrigins);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigCSG, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigCSG)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigCSG message handlers

void CDialogConfigCSG::SetParams(CDataCSG CSG)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_BuildCSG = ReferApp.GeneralParams.bBuildModelCSG;
	m_CSG_Inflation= CSG.CSG_Inflation;
	m_CSG_Inflation_Direct= CSG.CSG_Inflation_Direct;
	m_CSG_Inflation_Optimization= CSG.CSG_Inflation_Optimization;
	m_CSG_ThresholdFMerit= CSG.CSG_ThresholdFMerit;
	m_CSG_ThresholdDMerit= CSG.CSG_ThresholdDMerit;
	m_CSG_ThresholdAngle= RADIANS_TO_DEGREES(CSG.CSG_ThresholdAngle);
	m_CSG_ThresholdOffset= CSG.CSG_ThresholdOffset * 100.;
	m_CSG_ThresholdLocation= CSG.CSG_ThresholdLocation;
	m_CSG_UseMultipleOrigins= CSG.CSG_UseMultipleOrigins;
}


void CDialogConfigCSG::GetParams(CDataCSG &CSG)
{
	ReferApp.GeneralParams.bBuildModelCSG= (m_BuildCSG!=0);
	CSG.CSG_Inflation= (m_CSG_Inflation!=0);
	CSG.CSG_Inflation_Direct= (m_CSG_Inflation_Direct!=0);
	CSG.CSG_Inflation_Optimization= (m_CSG_Inflation_Optimization!=0);
	CSG.CSG_ThresholdFMerit= m_CSG_ThresholdFMerit;
	CSG.CSG_ThresholdDMerit= m_CSG_ThresholdDMerit;
	CSG.CSG_ThresholdAngle= DEGREES_TO_RADIANS(m_CSG_ThresholdAngle);
	CSG.CSG_ThresholdOffset= m_CSG_ThresholdOffset / 100.;
	CSG.CSG_ThresholdLocation= m_CSG_ThresholdLocation;
	CSG.CSG_UseMultipleOrigins= (m_CSG_UseMultipleOrigins!=0);
}


void CDialogConfigCSG::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataCSG initial;
	SetParams(initial);

	UpdateData(false);
}


BOOL CDialogConfigCSG::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
