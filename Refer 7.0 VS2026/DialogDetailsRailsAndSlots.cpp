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

// DialogDetailsRailsAndSlots.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsRailsAndSlots.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRailsAndSlots dialog


CDialogDetailsRailsAndSlots::CDialogDetailsRailsAndSlots(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsRailsAndSlots::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsRailsAndSlots)
	//}}AFX_DATA_INIT
}


void CDialogDetailsRailsAndSlots::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsRailsAndSlots)
	DDX_Text(pDX, IDC_Slots_ThresholdShort, m_ShortEdges);
	DDX_Text(pDX, IDC_Slots_ThresholdLong, m_LongEdges);
	DDX_Text(pDX, IDC_Slots_TolSimilarFacesDistances, m_SimilarFacesDistances);
	DDX_Text(pDX, IDC_Slots_TolSimilarFacesAngles, m_SimilarFacesAngles);
	DDX_Text(pDX, IDC_Slots_TolCollinear, m_CollinearEdges);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsRailsAndSlots, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsRailsAndSlots)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_VPs_RestoreDefault, OnRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRailsAndSlots message handlers

void CDialogDetailsRailsAndSlots::SetParams(CDataRailsAndSlots RS)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ShortEdges= RS.ShortEdges;
	m_LongEdges= RS.LongEdges;
	m_SimilarFacesDistances= RS.SimilarFacesDistances*100;
	m_SimilarFacesAngles= RADIANS_TO_DEGREES(RS.SimilarFacesAngles);
	m_CollinearEdges= RADIANS_TO_DEGREES(RS.CollinearEdges);
}


void CDialogDetailsRailsAndSlots::GetParams(CDataRailsAndSlots &RS)
// Updates the general parameters that have been modified in the dialog
{
	RS.ShortEdges= m_ShortEdges;
	RS.LongEdges= m_LongEdges;
	RS.SimilarFacesDistances= m_SimilarFacesDistances/100;
	RS.SimilarFacesAngles= DEGREES_TO_RADIANS(m_SimilarFacesAngles);
	RS.CollinearEdges= DEGREES_TO_RADIANS(m_CollinearEdges);
}


void CDialogDetailsRailsAndSlots::OnRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
		CDataRailsAndSlots Initial;
	SetParams(Initial);
	UpdateData(false);
}

