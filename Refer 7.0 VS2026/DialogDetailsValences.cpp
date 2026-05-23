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

// DialogDetailsValences.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsValences.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsValences dialog


CDialogDetailsValences::CDialogDetailsValences(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsValences::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsValences)
	//}}AFX_DATA_INIT
}


void CDialogDetailsValences::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsValences)
	DDX_Check(pDX, IDC_CollinearAsSingle, m_CollinearAsSingle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsValences, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsValences)
	ON_BN_CLICKED(ID_RestoreDefaultDetailsValences, OnRestoreDefaultDetailsValences)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsValences message handlers


void CDialogDetailsValences::SetParams(CDataValences VA)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_CollinearAsSingle= VA.CollinearAsSingle;

}

void CDialogDetailsValences::GetParams(CDataValences &VA)
// Updates the general parameters that have been modified in the dialog
{
	VA.CollinearAsSingle= (m_CollinearAsSingle!=0);
}

void CDialogDetailsValences::OnRestoreDefaultDetailsValences() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataValences VA_Default;
	m_CollinearAsSingle= VA_Default.CollinearAsSingle;
	UpdateData(false);
}
