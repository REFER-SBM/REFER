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

// DialogDetailsRibsAndDividers.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsRibsAndDividers.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRibsAndDividers dialog


CDialogDetailsRibsAndDividers::CDialogDetailsRibsAndDividers(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsRibsAndDividers::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsRibsAndDividers)
	//}}AFX_DATA_INIT
}


void CDialogDetailsRibsAndDividers::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsRibsAndDividers)
	DDX_Text(pDX, IDC_Ribs_ThresholdShort, m_ShortEdges);
	DDX_Text(pDX, IDC_Ribs_ThresholdLong, m_LongEdges);
	DDX_Text(pDX, IDC_Ribs_TolSimilarFacesDistances, m_SimilarFacesDistances);
	DDX_Text(pDX, IDC_Ribs_TolSimilarFacesAngles, m_SimilarFacesAngles);
	DDX_Text(pDX, IDC_Ribs_TolCollinear, m_CollinearEdges);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsRibsAndDividers, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsRibsAndDividers)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_VPs_RestoreDefault, OnRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRibsAndDividers message handlers

void CDialogDetailsRibsAndDividers::SetParams(CDataRibsAndDividers RD)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ShortEdges= RD.ShortEdges;
	m_LongEdges= RD.LongEdges;
	m_SimilarFacesDistances= RD.SimilarFacesDistances*100;
	m_SimilarFacesAngles= RADIANS_TO_DEGREES(RD.SimilarFacesAngles);
	m_CollinearEdges= RADIANS_TO_DEGREES(RD.CollinearEdges);
}


void CDialogDetailsRibsAndDividers::GetParams(CDataRibsAndDividers &RD)
// Updates the general parameters that have been modified in the dialog
{
	RD.ShortEdges= m_ShortEdges;
	RD.LongEdges= m_LongEdges;
	RD.SimilarFacesDistances= m_SimilarFacesDistances/100;
	RD.SimilarFacesAngles= DEGREES_TO_RADIANS(m_SimilarFacesAngles);
	RD.CollinearEdges= DEGREES_TO_RADIANS(m_CollinearEdges);
}


void CDialogDetailsRibsAndDividers::OnRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
		CDataRibsAndDividers Initial;
	SetParams(Initial);
	UpdateData(false);
}

