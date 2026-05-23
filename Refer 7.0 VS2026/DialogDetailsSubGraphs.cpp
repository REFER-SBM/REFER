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

// DialogDetailsSubGraphs.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DataRefer.h"
#include "resource.h"
#include "DialogDetailsSubGraphs.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSubGraphs dialog


CDialogDetailsSubGraphs::CDialogDetailsSubGraphs(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsSubGraphs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsSubGraphs)
	//}}AFX_DATA_INIT
}


void CDialogDetailsSubGraphs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsSubGraphs)
	DDX_Radio(pDX, IDC_METODO, m_SubGraphsApproach);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsSubGraphs, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsSubGraphs)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSubGraphs message handlers

void CDialogDetailsSubGraphs::SetParams(CDataSubGraphs SG)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_SubGraphsApproach= SG.SubGraphsApproach;
}


void CDialogDetailsSubGraphs::GetParams(CDataSubGraphs &SG)
// Updates the general parameters that have been modified in the dialog
{
	SG.SubGraphsApproach= m_SubGraphsApproach;
}
