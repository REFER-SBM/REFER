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

#include "DialogRegularities2D.h"

CDialogRegularities2D::CDialogRegularities2D(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRegularities2D::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegularities2D)
	//}}AFX_DATA_INIT
}


void CDialogRegularities2D::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegularities2D)
	DDX_Check(pDX, IDC_CHECK_CL2D, m_CL2D);
	DDX_Check(pDX, IDC_CHECK_PL2D, m_PL2D);
	DDX_Check(pDX, IDC_CHECK_VL2D, m_VL2D);
	DDX_Text(pDX, IDC_ErrorCollinearity2D, m_ErrorCollinearity);
	DDX_Text(pDX, IDC_ErrorParallelism2D, m_ErrorParallelism);
	DDX_Text(pDX, IDC_ErrorVerticality2D, m_ErrorVerticality);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogRegularities2D, CDialog)
	//{{AFX_MSG_MAP(CRegularities2D)
	ON_BN_CLICKED(IDC_RestoreDefaultRegularities2D, OnRestoreDefaultRegularities2D)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegularities2D message handlers


void CDialogRegularities2D::SetParams(CDataRefine2D R2D)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_PL2D =  R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive;
	m_VL2D =  R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive;
	m_CL2D =  R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive;

	m_ErrorParallelism =  RADIANS_TO_DEGREES(R2D.ErrorParallelism2D);
	m_ErrorVerticality =  RADIANS_TO_DEGREES(R2D.ErrorVerticality2D);
	m_ErrorCollinearity =  RADIANS_TO_DEGREES(R2D.ErrorCollinearity2D);
}


void CDialogRegularities2D::GetParams(CDataRefine2D &R2D)
// Updates the general parameters that have been modified in the dialog
{
	R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive = m_PL2D !=0;
	R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive = m_VL2D !=0;
	R2D.Regularities2D.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive = m_CL2D !=0;
	long iActivadas= 0;
	for(long i=0; i<REGULARITY_LAST+1 ; i++ ){
		if (R2D.Regularities2D.ListOfRegularities[i].bActive)
			iActivadas++;
	}
	R2D.Regularities2D.NumActiveRegularities = iActivadas;

	R2D.ErrorParallelism2D = DEGREES_TO_RADIANS(m_ErrorParallelism);
	R2D.ErrorVerticality2D = DEGREES_TO_RADIANS(m_ErrorVerticality);
	R2D.ErrorCollinearity2D = DEGREES_TO_RADIANS(m_ErrorCollinearity);
}


void CDialogRegularities2D::OnRestoreDefaultRegularities2D() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataRefine2D initial;
	SetParams(initial);
	UpdateData( false );
}
