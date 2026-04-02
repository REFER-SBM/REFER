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

// DialogConfigTessellate3D.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "ReferDoc.h"

#include "DialogConfigTessellate3D.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigTessellate3D dialog


CDialogConfigTessellate3D::CDialogConfigTessellate3D(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigTessellate3D::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigTessellate3D)
	//}}AFX_DATA_INIT
}


void CDialogConfigTessellate3D::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigTessellate3D)
	DDX_Check(pDX, IDC_REFER_Tessellate_3D, m_Tessellate3D);
	DDX_Check(pDX, IDC_CHECK_Faces_Outward, m_FacesOutward);
	DDX_Check(pDX, IDC_CHECK_Triangulate_Faces, m_TriangulateFaces);
	DDX_Check(pDX, IDC_CHECK_Normals_To_Faces, m_NormalsToFaces);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigTessellate3D, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigTessellate3D)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigTessellate3D message handlers


void CDialogConfigTessellate3D::SetParams(CDataTessellate3D T3D)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Tessellate3D = ReferApp.GeneralParams.bTessellate3D;
	m_FacesOutward = T3D.FacesOutward;
	m_TriangulateFaces = T3D.TriangulateFaces;
	m_NormalsToFaces = T3D.NormalsToFaces;
}


void CDialogConfigTessellate3D::GetParams(CDataTessellate3D &T3D)
{
	ReferApp.GeneralParams.bTessellate3D= (m_Tessellate3D!=0);
	T3D.FacesOutward= (m_FacesOutward!=0);
	T3D.TriangulateFaces= (m_TriangulateFaces!=0);
	T3D.NormalsToFaces= (m_NormalsToFaces!=0);
}

void CDialogConfigTessellate3D::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataTessellate3D initial;
	SetParams(initial);

	UpdateData(false);
}


BOOL CDialogConfigTessellate3D::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
