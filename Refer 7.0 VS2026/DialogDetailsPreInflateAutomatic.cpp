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

// DialogDetallesPREINFLADOAutomatico.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsPreInflateAutomatic.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPREINFLATEAutomatic dialog


CDialogDetailsPREINFLATEAutomatic::CDialogDetailsPREINFLATEAutomatic(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsPREINFLATEAutomatic::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsPREINFLATEAutomatic)
	//}}AFX_DATA_INIT
}


void CDialogDetailsPREINFLATEAutomatic::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsPREINFLATEAutomatic)
	DDX_Radio(pDX, IDC_RADIO_PreInfationIfNormalon, m_PreInfationIfNormalon);
	DDX_Radio(pDX, IDC_RADIO_PreInfationIfQuasi, m_PreInfationIfQuasi);
	DDX_Radio(pDX, IDC_RADIO_PreInfationIfUndefinedD, m_PreInfationIfUndefinedD);
	DDX_Radio(pDX, IDC_RADIO_PreInfationIfUndefinedND, m_PreInfationIfUndefinedND);
	DDX_Radio(pDX, IDC_RADIO_PreInfationIfPrism, m_PreInfationIfPrism);
	DDX_Radio(pDX, IDC_RADIO_PreInfationIfPyramid, m_PreInfationIfPyramid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsPREINFLATEAutomatic, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsPREINFLATEAutomatic)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPREINFLATEAutomatic message handlers


void CDialogDetailsPREINFLATEAutomatic::SetParams(CDataDirectInflation DI)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_PreInfationIfNormalon = (int)DI.iPreInfationIfNormalon;
	m_PreInfationIfQuasi = (int)DI.iPreInfationIfQuasi;
	m_PreInfationIfPyramid = (int)DI.iPreInfationIfPyramid;
	m_PreInfationIfPrism = (int)DI.iPreInfationIfPrism;
	m_PreInfationIfUndefinedD = (int)DI.iPreInfationIfUndefinedD;
	m_PreInfationIfUndefinedND = (int)DI.iPreInfationIfUndefinedND;
}


void CDialogDetailsPREINFLATEAutomatic::GetParams(CDataDirectInflation &DI)
// Updates the general parameters that have been modified in the dialog
{
	//if(m_PreInfationIfNormalon == 0)
	//	DI.iPreInfationIfNormalon = AUTOMATIC;
	//else if(m_PreInfationIfNormalon == 1)
	//	DI.iPreInfationIfNormalon = AXONOMETRIC;
	//else if(m_PreInfationIfNormalon == 2)
	//	DI.iPreInfationIfNormalon = LEVELS;
	//else if(m_PreInfationIfNormalon == 3)
	//	DI.iPreInfationIfNormalon = RANDOM;
	DI.iPreInfationIfNormalon = (DIRINFLATION)(m_PreInfationIfNormalon);

	DI.iPreInfationIfQuasi = (DIRINFLATION)(m_PreInfationIfQuasi);
	DI.iPreInfationIfPyramid = (DIRINFLATION)(m_PreInfationIfPyramid);
	DI.iPreInfationIfPrism = (DIRINFLATION)(m_PreInfationIfPrism);
	DI.iPreInfationIfUndefinedD = (DIRINFLATION)(m_PreInfationIfUndefinedD);
	DI.iPreInfationIfUndefinedND = (DIRINFLATION)(m_PreInfationIfUndefinedND);
}

void CDialogDetailsPREINFLATEAutomatic::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;

	CDataDirectInflation Initial;
	SetParams(Initial);
	UpdateData(false);
}
