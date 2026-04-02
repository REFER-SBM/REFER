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

// DialogDetailsLabels.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DataRefer.h"
#include "resource.h"
#include "DialogDetailsLabels.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsLabels dialog


CDialogDetailsLabels::CDialogDetailsLabels(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsLabels::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsLabels)
	//}}AFX_DATA_INIT
}


void CDialogDetailsLabels::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsLabels)
	DDX_Radio(pDX, IDC_LabelsApproach, m_LabelsApproach);
	DDX_Radio(pDX, IDC_VertexCatalogue, m_VertexCatalogue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsLabels, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsLabels)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsLabels message handlers

void CDialogDetailsLabels::SetParams(CDataLabels La)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_LabelsApproach= La.LabelsApproach;
	m_VertexCatalogue= La.VertexCatalogue;
}


void CDialogDetailsLabels::GetParams(CDataLabels &La)
// Updates the general parameters that have been modified in the dialog
{
	La.LabelsApproach= m_LabelsApproach;
	La.VertexCatalogue= m_VertexCatalogue;
}
