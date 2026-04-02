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

// DialogConfigDetectCuesFeatures.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "DialogConfigCues.h"
#include "DialogConfigFeatures.h"

#include "DialogConfigDetectCuesFeatures.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDetectCuesFeatures dialog


CDialogConfigDetectCuesFeatures::CDialogConfigDetectCuesFeatures(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigDetectCuesFeatures::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigDetectCuesFeatures)
	//}}AFX_DATA_INIT
}


void CDialogConfigDetectCuesFeatures::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigDetectCuesFeatures)
	DDX_Check(pDX, IDC_Detect_Cues, m_DetectCues);
	DDX_Check(pDX, IDC_Detect_Features, m_DetectFeatures);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigDetectCuesFeatures, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigDetectCuesFeatures)
	ON_BN_CLICKED(IDC_Config_Cues, OnConfigCues)
	ON_BN_CLICKED(IDC_Config_Features, OnConfigFeatures)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDetectCuesFeatures message handlers

BOOL CDialogConfigDetectCuesFeatures::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(false);

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogConfigDetectCuesFeatures::SetParams(CDataRefer GeneralParams)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_DetectCues= GeneralParams.DetectCues;
	m_DetectFeatures= GeneralParams.DetectFeatures;
}


void CDialogConfigDetectCuesFeatures::GetParams(CDataRefer &GeneralParams)
// Updates the general parameters that have been modified in the dialog
{
	GeneralParams.DetectCues= m_DetectCues !=0;
	GeneralParams.DetectFeatures= m_DetectFeatures !=0;
}


void CDialogConfigDetectCuesFeatures::OnConfigCues() 
{
	CDialogConfigCues Dlg;
	Dlg.SetParams(ReferApp.GeneralParams);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.GeneralParams);
	}
}


void CDialogConfigDetectCuesFeatures::OnConfigFeatures() 
{
	CDialogConfigFeatures Dlg;
	Dlg.SetParams(ReferApp.GeneralParams, ReferApp.Fe);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.GeneralParams, ReferApp.Fe);
	}
}
