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

// DialogDetailsHiddenEdges.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DataRefer.h"
#include "resource.h"
#include "DialogDetailsHiddenEdges.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsHiddenEdges dialog


CDialogDetailsHiddenEdges::CDialogDetailsHiddenEdges(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsHiddenEdges::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsHiddenEdges)
	//}}AFX_DATA_INIT
}


void CDialogDetailsHiddenEdges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsHiddenEdges)
	DDX_Check(pDX, IDC_HiddenEdgesByDashedLines, m_HiddenEdgesByDashedLines);
	DDX_Check(pDX, IDC_HiddenEdgesRefineDashedLines, m_HiddenEdgesRefineDashedLines);
	DDX_Radio(pDX, IDC_METODO, m_HiddenEdgesApproach);
	DDX_Radio(pDX, IDC_METODOBIS, m_HiddenEdgesAutomatic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsHiddenEdges, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsHiddenEdges)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsHiddenEdges message handlers

void CDialogDetailsHiddenEdges::SetParams(CDataHiddenEdges Hi)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_HiddenEdgesByDashedLines= Hi.HiddenEdgesByDashedLines;
	m_HiddenEdgesRefineDashedLines= Hi.HiddenEdgesRefineDashedLines;
	m_HiddenEdgesApproach= Hi.HiddenEdgesApproach;
	if(Hi.HiddenEdgesAutomatic)
		m_HiddenEdgesAutomatic= 1;
	else
		m_HiddenEdgesAutomatic= 0;
}


void CDialogDetailsHiddenEdges::GetParams(CDataHiddenEdges &Hi)
// Updates the general parameters that have been modified in the dialog
{
	Hi.HiddenEdgesByDashedLines= m_HiddenEdgesByDashedLines !=0;
	Hi.HiddenEdgesRefineDashedLines= m_HiddenEdgesRefineDashedLines !=0;
	Hi.HiddenEdgesApproach= m_HiddenEdgesApproach;
	if(m_HiddenEdgesAutomatic == 1)
		Hi.HiddenEdgesAutomatic= true;
	else
		Hi.HiddenEdgesAutomatic= false;
}
