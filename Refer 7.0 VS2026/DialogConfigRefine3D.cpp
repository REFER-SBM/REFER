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

// DialogConfigRefine3D.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "ReferDoc.h"

#include "DialogConfigRefine3D.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigRefine3D dialog


CDialogConfigRefine3D::CDialogConfigRefine3D(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigRefine3D::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigRefine3D)
	//}}AFX_DATA_INIT
}


void CDialogConfigRefine3D::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigRefine3D)
	DDX_Check(pDX, IDC_REFER_Refine_3D, m_Refine3D);
	DDX_Check(pDX, IDC_CHECK_Refine_Faces, m_RefineFaces);
	DDX_Check(pDX, IDC_CHECK_Align_Faces, m_AlignFaces);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigRefine3D, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigRefine3D)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigRefine3D message handlers

void CDialogConfigRefine3D::SetParams(CDataRefine3D R3D)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Refine3D = ReferApp.GeneralParams.bRefine3D;
	m_RefineFaces = R3D.RefineFaces;
	m_AlignFaces = R3D.AlignFaces;
}


void CDialogConfigRefine3D::GetParams(CDataRefine3D &R3D)
{
	ReferApp.GeneralParams.bRefine3D= (m_Refine3D!=0);
	R3D.RefineFaces= (m_RefineFaces!=0);
	R3D.AlignFaces= (m_AlignFaces!=0);
}


void CDialogConfigRefine3D::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataRefine3D initial;
	SetParams(initial);

	UpdateData(false);
}


BOOL CDialogConfigRefine3D::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
