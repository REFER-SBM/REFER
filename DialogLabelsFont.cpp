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

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"

#include "DialogLabelsFont.h"

CDialogLabelsFont::CDialogLabelsFont(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogLabelsFont::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogLabelsFont)
	//}}AFX_DATA_INIT
}


void CDialogLabelsFont::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogLabelsFont)
	DDX_Text(pDX, IDC_FontHeightScale, m_FontHeightScale);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogLabelsFont, CDialog)
	//{{AFX_MSG_MAP(CDialogLabelsFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogLabelsFont message handlers


void CDialogLabelsFont::SetParams(double FontHeightScale)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_FontHeightScale =  FontHeightScale * 100;
}


void CDialogLabelsFont::GetParams(double &FontHeightScale)
// Updates the general parameters that have been modified in the dialog
{
	FontHeightScale = m_FontHeightScale/100;
}
