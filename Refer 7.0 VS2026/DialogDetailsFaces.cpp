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

// DialogDetailsFaces.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsFaces.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsFaces dialog


CDialogDetailsFaces::CDialogDetailsFaces(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsFaces::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsFaces)
	//}}AFX_DATA_INIT
}


void CDialogDetailsFaces::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsFaces)
	DDX_Text(pDX, IDC_EDIT_EdgeCollinearityAngleThreshold, m_EdgeCollinearityAngleThreshold);
	DDV_MinMaxDouble(pDX, m_EdgeCollinearityAngleThreshold, 0., 100.);
	DDX_Radio(pDX, IDC_METODO, m_DetectFacesApproach);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsFaces, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsFaces)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsFaces message handlers



void CDialogDetailsFaces::SetParams(CDataCues Cu)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_DetectFacesApproach = Cu.Fa.DetectFacesApproach;
	m_EdgeCollinearityAngleThreshold = RADIANS_TO_DEGREES(Cu.Fa.EdgeCollinearityAngleThreshold);
}


void CDialogDetailsFaces::GetParams(CDataCues &Cu)
// Updates the general parameters that have been modified in the dialog
{
	Cu.Fa.DetectFacesApproach = m_DetectFacesApproach;
	Cu.Fa.EdgeCollinearityAngleThreshold = DEGREES_TO_RADIANS(m_EdgeCollinearityAngleThreshold);
}


void CDialogDetailsFaces::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataCues initial;
	SetParams(initial);
	UpdateData(false);
}
