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

// DialogMainAxesRules.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogMainAxesRules.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogMainAxesAutomatic dialog


CDialogMainAxesAutomatic::CDialogMainAxesAutomatic(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogMainAxesAutomatic::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogMainAxesAutomatic)
	//}}AFX_DATA_INIT
}


void CDialogMainAxesAutomatic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogMainAxesAutomatic)
	DDX_Check(pDX, IDC_MainAxes_ProjectiveRule, m_ProjectiveRule);
	DDX_Text(pDX, IDC_MainAxes_ToleranceCubicAngles, m_ToleranceCubicAngles);

	DDX_Check(pDX, IDC_MainAxes_MatchingCornersRule, m_MatchingCornersRule);
	DDX_Text(pDX, IDC_MainAxes_MeritMatchingCorners, m_MeritMatchingCorners);
	DDX_Check(pDX, IDC_MainAxes_IsometricRule, m_IsometricRule);
	DDX_Text(pDX, IDC_MainAxes_MeritIsometric, m_MeritIsometric);
	DDX_Check(pDX, IDC_MainAxes_VerticalRule, m_VerticalRule);
	DDX_Text(pDX, IDC_MainAxes_MeritVertical, m_MeritVertical);
	DDX_Check(pDX, IDC_MainAxes_HorizontalRule, m_HorizontalRule);
	DDX_Text(pDX, IDC_MainAxes_MeritHorizontal, m_MeritHorizontal);
	DDX_Check(pDX, IDC_MainAxes_FrequencyRule, m_FrequencyRule);
	DDX_Text(pDX, IDC_MainAxes_MeritFrequency, m_MeritFrequency);

	DDX_Text(pDX, IDC_MainAxes_ThresholdOverlap, m_ThresholdOverlap);
	DDX_Text(pDX, IDC_MainAxes_ThresholdAngle, m_ThresholdAngle);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CDialogMainAxesAutomatic, CDialog)
	//{{AFX_MSG_MAP(CDialogMainAxesAutomatic)
	ON_BN_CLICKED(IDC_MainAxesRulesDefault, OnMainAxesRulesDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogMainAxesAutomatic message handlers


void CDialogMainAxesAutomatic::SetParams(CDataMainAxes MA)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ProjectiveRule= MA.ProjectiveRule;
	m_ToleranceCubicAngles = RADIANS_TO_DEGREES(MA.ToleranceCubicAngles);
	m_MatchingCornersRule= MA.MatchingCornersRule;
	m_MeritMatchingCorners = MA.MeritMatchingCorners;
	m_IsometricRule= MA.IsometricRule;
	m_MeritIsometric = MA.MeritIsometric;
	m_VerticalRule= MA.VerticalRule;
	m_MeritVertical = MA.MeritVertical;
	m_HorizontalRule= MA.HorizontalRule;
	m_MeritHorizontal = MA.MeritHorizontal;
	m_FrequencyRule= MA.FrequencyRule;
	m_MeritFrequency = MA.MeritFrequency;
	m_ThresholdOverlap = RADIANS_TO_DEGREES(MA.ThresholdOverlap);
	m_ThresholdAngle = RADIANS_TO_DEGREES(MA.ThresholdAngle);
}


void CDialogMainAxesAutomatic::GetParams(CDataMainAxes &MA)
// Updates the general parameters that have been modified in the dialog
{
	MA.ProjectiveRule= m_ProjectiveRule !=0;
	MA.ToleranceCubicAngles = DEGREES_TO_RADIANS(m_ToleranceCubicAngles);
	MA.MatchingCornersRule= m_MatchingCornersRule !=0;
	MA.MeritMatchingCorners = m_MeritMatchingCorners;
	MA.IsometricRule= m_IsometricRule !=0;
	MA.MeritIsometric = m_MeritIsometric;
	MA.VerticalRule= m_VerticalRule !=0;
	MA.MeritVertical = m_MeritVertical;
	MA.HorizontalRule= m_HorizontalRule !=0;
	MA.MeritHorizontal = m_MeritHorizontal;
	MA.FrequencyRule= m_FrequencyRule !=0;
	MA.MeritFrequency = m_MeritFrequency;
	MA.ThresholdOverlap = DEGREES_TO_RADIANS(m_ThresholdOverlap);
	MA.ThresholdAngle = DEGREES_TO_RADIANS(m_ThresholdAngle);
}

void CDialogMainAxesAutomatic::OnMainAxesRulesDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataMainAxes Initial;
	SetParams(Initial);
	UpdateData(false);
}
