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

// DialogRotateAngle.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogRotateAngle.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogRotateAngle dialog


CDialogRotateAngle::CDialogRotateAngle(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRotateAngle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogRotateAngle)
	//}}AFX_DATA_INIT
}


void CDialogRotateAngle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogRotateAngle)
	DDX_Text(pDX, IDC_Rotate, m_Angle);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDialogRotateAngle message handlers


void CDialogRotateAngle::SetParams(double angle)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Angle= angle * 180./3.14159265359;
}

void CDialogRotateAngle::GetParams(double &angle)
// Updates the general parameters that have been modified in the dialog
{
	angle=m_Angle *3.14159265359/180.;
}
