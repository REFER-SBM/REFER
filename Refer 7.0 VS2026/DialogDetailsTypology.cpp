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

// DialogDetailsTypology.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsTypology.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsTypology dialog


CDialogDetailsTypology::CDialogDetailsTypology(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsTypology::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsTypology)
	//}}AFX_DATA_INIT
}


void CDialogDetailsTypology::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsTypology)
	DDX_Text(pDX, IDC_EDIT_Min_Frequency_Prismatic_Objects, m_FrequencyPrismaticObjects);
	DDV_MinMaxDouble(pDX, m_FrequencyPrismaticObjects, 0., 100.);
	DDX_Text(pDX, IDC_EDIT_Min_Frequency_Undefined_Objects_D, m_FrequencyUndefinedObjects_D);
	DDV_MinMaxDouble(pDX, m_FrequencyUndefinedObjects_D, 0., 100.);
	DDX_Text(pDX, IDC_EDIT_ThresholdForParallel, m_ThresholdForParallel);
	DDV_MinMaxDouble(pDX, m_ThresholdForParallel, 0., 90.);
	DDX_Text(pDX, IDC_EDIT_ThresholdForArea, m_ThresholdForArea);
	DDV_MinMaxDouble(pDX, m_ThresholdForArea, 0., 100.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsTypology, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsTypology)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsTypology message handlers

void CDialogDetailsTypology::SetParams(CDataTypology Ty)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_FrequencyPrismaticObjects = Ty.dFrequencyPrismaticObjects * 100.;
	m_FrequencyUndefinedObjects_D = Ty.dFrequencyUndefinedObjects_D * 100;
	m_ThresholdForParallel = RADIANS_TO_DEGREES(Ty.ThresholdForParallel);
	m_ThresholdForArea = Ty.ThresholdForArea * 100.;
}

void CDialogDetailsTypology::GetParams(CDataTypology &Ty)
// Updates the general parameters that have been modified in the dialog
{
	Ty.dFrequencyPrismaticObjects = m_FrequencyPrismaticObjects / 100.;
	Ty.dFrequencyUndefinedObjects_D = m_FrequencyUndefinedObjects_D / 100.;
	Ty.ThresholdForParallel = DEGREES_TO_RADIANS(m_ThresholdForParallel);
	Ty.ThresholdForArea = m_ThresholdForArea / 100.;
}

void CDialogDetailsTypology::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataTypology initial;
	SetParams(initial);
	UpdateData(false);
}
