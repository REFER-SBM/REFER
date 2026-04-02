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

// DialogDetailsPerimeter.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DialogDetailsPerimeter.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPerimeter dialog


CDialogDetailsPerimeter::CDialogDetailsPerimeter(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsPerimeter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsPerimeter)
	//}}AFX_DATA_INIT
}


void CDialogDetailsPerimeter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsPerimeter)
	DDX_Check(pDX, IDC_PerimeterWithDangling, m_IncludeDanglingEdges);
	DDX_Text(pDX, IDC_PerimeterIntThreshold, m_ThresholdIntersection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsPerimeter, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsPerimeter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPerimeter message handlers


void CDialogDetailsPerimeter::SetParams(CDataPerimeter Pe)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_IncludeDanglingEdges = Pe.IncludeDanglingEdges;
	m_ThresholdIntersection = 100*Pe.ThresholdIntersection;
}


void CDialogDetailsPerimeter::GetParams(CDataPerimeter &Pe)
// Updates the general parameters that have been modified in the dialog
{
	Pe.IncludeDanglingEdges= (m_IncludeDanglingEdges!=0);
	Pe.ThresholdIntersection = m_ThresholdIntersection/100;
}
