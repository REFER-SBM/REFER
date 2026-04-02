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

// DialogDetailsStepsAndPockets.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsStepsAndPockets.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsStepsAndPockets dialog


CDialogDetailsStepsAndPockets::CDialogDetailsStepsAndPockets(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsStepsAndPockets::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsStepsAndPockets)
	//}}AFX_DATA_INIT
}


void CDialogDetailsStepsAndPockets::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsStepsAndPockets)
	DDX_Text(pDX, IDC_Steps_ThresholdShort, m_ShortEdges);
	DDX_Text(pDX, IDC_Steps_ThresholdLong, m_LongEdges);
	DDX_Text(pDX, IDC_Steps_TolSimilarFacesDistances, m_SimilarFacesDistances);
	DDX_Text(pDX, IDC_Steps_TolSimilarFacesAngles, m_SimilarFacesAngles);
	DDX_Text(pDX, IDC_Steps_TolCollinear, m_CollinearEdges);
	DDX_Text(pDX, IDC_Steps_ThresholdAreaBases, m_ThresholdAreaBases);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsStepsAndPockets, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsStepsAndPockets)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_VPs_RestoreDefault, OnRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsStepsAndPockets message handlers

void CDialogDetailsStepsAndPockets::SetParams(CDataStepsAndPockets SP)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ShortEdges= SP.ShortEdges;
	m_LongEdges= SP.LongEdges;
	m_SimilarFacesDistances= SP.SimilarFacesDistances*100;
	m_SimilarFacesAngles= RADIANS_TO_DEGREES(SP.SimilarFacesAngles);
	m_CollinearEdges= RADIANS_TO_DEGREES(SP.CollinearEdges);
	m_ThresholdAreaBases= SP.ThresholdAreaBases;
}


void CDialogDetailsStepsAndPockets::GetParams(CDataStepsAndPockets &SP)
// Updates the general parameters that have been modified in the dialog
{
	SP.ShortEdges= m_ShortEdges;
	SP.LongEdges= m_LongEdges;
	SP.SimilarFacesDistances= m_SimilarFacesDistances/100;
	SP.SimilarFacesAngles= DEGREES_TO_RADIANS(m_SimilarFacesAngles);
	SP.CollinearEdges= DEGREES_TO_RADIANS(m_CollinearEdges);
	SP.ThresholdAreaBases= m_ThresholdAreaBases;
}


void CDialogDetailsStepsAndPockets::OnRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
		CDataStepsAndPockets Initial;
	SetParams(Initial);
	UpdateData(false);
}

