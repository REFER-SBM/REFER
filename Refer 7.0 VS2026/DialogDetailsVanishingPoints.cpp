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

// DialogDetailsVanishingPoints.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsVanishingPoints.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsVanishingPoints dialog


CDialogDetailsVanishingPoints::CDialogDetailsVanishingPoints(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsVanishingPoints::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsVanishingPoints)
	//}}AFX_DATA_INIT
}


void CDialogDetailsVanishingPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsVanishingPoints)
	DDX_Text(pDX, IDC_VPs_ClusteringAngle, m_ClusteringAngle);
	DDX_Text(pDX, IDC_VPs_InRing, m_InRing);
	DDX_Text(pDX, IDC_VPs_OutRing, m_OutRing);
	DDX_Text(pDX, IDC_VPs_ToleranceCoincident, m_ToleranceCoincident);
	DDX_Text(pDX, IDC_VPs_ToleranceParallel, m_ToleranceParallel);
	DDX_Text(pDX, IDC_VPs_ToleranceDispersion, m_ToleranceDispersion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsVanishingPoints, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsVanishingPoints)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_VPs_RestoreDefault, OnRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsVanishingPoints message handlers

void CDialogDetailsVanishingPoints::SetParams(CDataVanishingPoints VP)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ClusteringAngle= RADIANS_TO_DEGREES(VP.ClusteringAngle);
	m_InRing= VP.InRing;
	m_OutRing= VP.OutRing;
	m_ToleranceCoincident= VP.ToleranceCoincident;
	m_ToleranceParallel= RADIANS_TO_DEGREES(VP.ToleranceParallel);
	m_ToleranceDispersion= VP.ToleranceDispersion;
}


void CDialogDetailsVanishingPoints::GetParams(CDataVanishingPoints &VP)
// Updates the general parameters that have been modified in the dialog
{
	VP.ClusteringAngle= DEGREES_TO_RADIANS(m_ClusteringAngle);
	VP.InRing= m_InRing;
	VP.OutRing= m_OutRing;
	VP.ToleranceCoincident= m_ToleranceCoincident;
	VP.ToleranceParallel= DEGREES_TO_RADIANS(m_ToleranceParallel);
	VP.ToleranceDispersion= m_ToleranceDispersion;
}


void CDialogDetailsVanishingPoints::OnRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataVanishingPoints Initial;
	SetParams(Initial);
	UpdateData(false);
}

