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
#include "DialogDetailsParallelEdges.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsParallelEdges dialog


CDialogDetailsParallelEdges::CDialogDetailsParallelEdges(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsParallelEdges::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsParallelEdges)
	//}}AFX_DATA_INIT
}


void CDialogDetailsParallelEdges::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsParallelEdges)
	DDX_Radio(pDX, IDC_PE_ParallelEdgesApproach, m_ParallelEdgesApproach);
	DDX_Text(pDX, IDC_PE_TolForParallel, m_TolForParallel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsParallelEdges, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsVanishingPoints)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_PE_DetailsVanishingPoints, OnDetailsVanishingPoints)
	ON_BN_CLICKED(IDC_VPs_RestoreDefault, OnRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsParallelEdges message handlers

void CDialogDetailsParallelEdges::SetParams(CDataParallelEdges PE)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ParallelEdgesApproach= PE.ParallelEdgesApproach;
	m_TolForParallel= RADIANS_TO_DEGREES(PE.TolForParallel);
}


void CDialogDetailsParallelEdges::GetParams(CDataParallelEdges &PE)
// Updates the general parameters that have been modified in the dialog
{
	PE.ParallelEdgesApproach= m_ParallelEdgesApproach;
	PE.TolForParallel= DEGREES_TO_RADIANS(m_TolForParallel);
}


void CDialogDetailsParallelEdges::OnRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
		CDataParallelEdges Initial;
	SetParams(Initial);
	UpdateData(false);
}


void CDialogDetailsParallelEdges::OnDetailsVanishingPoints() 
{
	CDialogDetailsVanishingPoints Dlg;
	Dlg.SetParams(ReferApp.Cu.VP);
	if (Dlg.DoModal() == IDOK)
		Dlg.GetParams(ReferApp.Cu.VP);
}
