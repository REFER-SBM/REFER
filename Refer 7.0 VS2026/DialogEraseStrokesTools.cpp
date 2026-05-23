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

// DialogHerramientasDistorsion.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogEraseStrokesTools.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogEraseStrokesTools dialog


CDialogEraseStrokesTools::CDialogEraseStrokesTools(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogEraseStrokesTools::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogEraseStrokesTools)
	//}}AFX_DATA_INIT
}


void CDialogEraseStrokesTools::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogEraseStrokesTools)
	DDX_Text(pDX, IDC_Eraser_Radius, m_Eraser_Radius);
	DDX_Radio(pDX, IDC_Eraser_Type, m_Eraser_Type);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogEraseStrokesTools, CDialog)
	//{{AFX_MSG_MAP(CDialogEraseStrokesTools)
	ON_BN_CLICKED(ID_RestoreDefaultEraserTools, OnRestoreDefaultEraserTools)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogEraseStrokesTools message handlers


void CDialogEraseStrokesTools::SetParams(long Eraser_Radius, long Eraser_Type)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Eraser_Radius= Eraser_Radius;
	m_Eraser_Type= Eraser_Type;
}

void CDialogEraseStrokesTools::GetParams(long &Eraser_Radius, long &Eraser_Type)
// Updates the general parameters that have been modified in the dialog
{
	Eraser_Radius= m_Eraser_Radius;
	Eraser_Type= m_Eraser_Type;
}

void CDialogEraseStrokesTools::OnRestoreDefaultEraserTools() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	m_Eraser_Radius= 10;
	m_Eraser_Type= 0;
	UpdateData(false);
}
