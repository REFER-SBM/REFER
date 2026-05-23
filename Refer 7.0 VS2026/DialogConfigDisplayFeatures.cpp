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

// DialogConfigDisplayFeatures.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "resource.h"
#include "DialogConfigDisplayFeatures.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDisplayFeatures dialog


CDialogConfigDisplayFeatures::CDialogConfigDisplayFeatures(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigDisplayFeatures::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigDisplayFeatures)
	//}}AFX_DATA_INIT
}


void CDialogConfigDisplayFeatures::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigDisplayFeatures)
	DDX_Text(pDX, IDC_ScaleFeatureLabels, m_ScaleFeatureLabels);
	DDX_Check(pDX, IDC_DisplayNamesOfFeatures, m_DisplayNamesOfFeatures);
	DDX_Check(pDX, IDC_DisplayMeritsOfFeatures, m_DisplayMeritsOfFeatures);
	DDX_Check(pDX, IDC_DisplayContainerFaces, m_DisplayContainerFaces);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigDisplayFeatures, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigDisplayFeatures)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDisplayFeatures message handlers

void CDialogConfigDisplayFeatures::SetParams(double ScaleFeatureLabels,
											 bool DisplayNamesOfFeatures, 
											 bool DisplayMeritsOfFeatures, 
											 bool DisplayContainerFaces)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ScaleFeatureLabels = ScaleFeatureLabels;
	m_DisplayNamesOfFeatures = DisplayNamesOfFeatures;
	m_DisplayMeritsOfFeatures = DisplayMeritsOfFeatures;
	m_DisplayContainerFaces = DisplayContainerFaces;
}

void CDialogConfigDisplayFeatures::GetParams(double &ScaleFeatureLabels,
											 bool &DisplayNamesOfFeatures, 
											 bool &DisplayMeritsOfFeatures, 
											 bool &DisplayContainerFaces)
// Updates the general parameters that have been modified in the dialog
{
	ScaleFeatureLabels = m_ScaleFeatureLabels;
	DisplayNamesOfFeatures = (m_DisplayNamesOfFeatures != 0);
	DisplayMeritsOfFeatures = (m_DisplayMeritsOfFeatures != 0);
	DisplayContainerFaces = (m_DisplayContainerFaces != 0);
}


BOOL CDialogConfigDisplayFeatures::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
