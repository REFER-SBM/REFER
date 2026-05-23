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

// DialogDetailsChamfersAndRounds.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsChamfersAndRounds.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsChamfersAndRounds dialog


CDialogDetailsChamfersAndRounds::CDialogDetailsChamfersAndRounds(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsChamfersAndRounds::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsChamfersAndRounds)
	//}}AFX_DATA_INIT
}


void CDialogDetailsChamfersAndRounds::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsChamfersAndRounds)
	DDX_Text(pDX, IDC_Rounds_ThresholdShort, m_ShortEdges);
	DDX_Text(pDX, IDC_Rounds_ThresholdLong, m_LongEdges);
	DDX_Text(pDX, IDC_Rounds_TolSimilarFacesDistances, m_SimilarFacesDistances);
	DDX_Text(pDX, IDC_Rounds_TolSimilarFacesAngles, m_SimilarFacesAngles);
	DDX_Text(pDX, IDC_Rounds_TolCollinear, m_CollinearEdges);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsChamfersAndRounds, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsChamfersAndRounds)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_VPs_RestoreDefault, OnRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsChamfersAndRounds message handlers

void CDialogDetailsChamfersAndRounds::SetParams(CDataChamfersAndRounds ChR)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ShortEdges= ChR.ShortEdges;
	m_LongEdges= ChR.LongEdges;
	m_SimilarFacesDistances= ChR.SimilarFacesDistances*100;
	m_SimilarFacesAngles= RADIANS_TO_DEGREES(ChR.SimilarFacesAngles);
	m_CollinearEdges= RADIANS_TO_DEGREES(ChR.CollinearEdges);
}


void CDialogDetailsChamfersAndRounds::GetParams(CDataChamfersAndRounds &ChR)
// Updates the general parameters that have been modified in the dialog
{
	ChR.ShortEdges= m_ShortEdges;
	ChR.LongEdges= m_LongEdges;
	ChR.SimilarFacesDistances= m_SimilarFacesDistances/100;
	ChR.SimilarFacesAngles= DEGREES_TO_RADIANS(m_SimilarFacesAngles);
	ChR.CollinearEdges= DEGREES_TO_RADIANS(m_CollinearEdges);
}


void CDialogDetailsChamfersAndRounds::OnRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
		CDataChamfersAndRounds Initial;
	SetParams(Initial);
	UpdateData(false);
}

