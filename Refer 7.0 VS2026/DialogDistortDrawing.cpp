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

// DialogDistortDrawing.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDistortDrawing.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDistortDrawing dialog


CDialogDistortDrawing::CDialogDistortDrawing(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDistortDrawing::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDistortDrawing)
	//}}AFX_DATA_INIT
}


void CDialogDistortDrawing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDistortDrawing)
	DDX_Text(pDX, IDC_PDistorsion, m_PDistorsion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDistortDrawing, CDialog)
	//{{AFX_MSG_MAP(CDialogDistortDrawing)
	ON_BN_CLICKED(ID_RestoreDefaultDistort, OnRestoreDefaultDistort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDistortDrawing message handlers


void CDialogDistortDrawing::SetParams(CDataRefine2D R2D)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_PDistorsion= R2D.CoeficienteDistorsion*100;

}

void CDialogDistortDrawing::GetParams(CDataRefine2D &R2D)
// Updates the general parameters that have been modified in the dialog
{
	R2D.CoeficienteDistorsion=m_PDistorsion/100;
}

void CDialogDistortDrawing::OnRestoreDefaultDistort() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataRefine2D R2D_Default;
	m_PDistorsion= R2D_Default.CoeficienteDistorsion*100;
	UpdateData(false);
}
