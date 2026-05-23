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

// DialogScaleStroke.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogScaleStroke.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogScaleStroke dialog


CDialogScaleStroke::CDialogScaleStroke(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogScaleStroke::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogScaleStroke)
	//}}AFX_DATA_INIT
}


void CDialogScaleStroke::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogScaleStroke)
	DDX_Text(pDX, IDC_ScaleStrokeX, m_ScaleStrokeX);
	DDX_Text(pDX, IDC_ScaleStrokeY, m_ScaleStrokeY);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDialogScaleStroke message handlers


void CDialogScaleStroke::SetParams(POINT2D ScaleStroke)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ScaleStrokeX= ScaleStroke.x;
	m_ScaleStrokeY= ScaleStroke.y;
}

void CDialogScaleStroke::GetParams(POINT2D &ScaleStroke)
// Updates the general parameters that have been modified in the dialog
{
	ScaleStroke.x= m_ScaleStrokeX;
	ScaleStroke.y= m_ScaleStrokeY;
}
