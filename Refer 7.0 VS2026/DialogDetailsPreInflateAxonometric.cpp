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
#include "DialogDetailsPreInflateAxonometric.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPREINFLATEAxonometric dialog


CDialogDetailsPREINFLATEAxonometric::CDialogDetailsPREINFLATEAxonometric(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsPREINFLATEAxonometric::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsPREINFLATEAutomatic)
	//}}AFX_DATA_INIT
}


void CDialogDetailsPREINFLATEAxonometric::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsPREINFLATEAxonometric)
	DDX_Radio(pDX, IDC_AXONOMETRIC_APPROACH, m_Approach);
	DDX_Check(pDX, IDC_AMEND_CABINET, m_AmendCabinet);
	DDX_Text(pDX, IDC_90DEGREES, m_90degrees);
	DDX_Text(pDX, IDC_135DEGREES, m_135degrees);

	DDX_Radio(pDX, IDC_EXPANSION_CRITERION, m_Expansion);
	DDX_Text(pDX, IDC_CABINET_THRESHOLD, m_CabinetThreshold);
	DDX_Text(pDX, IDC_CABINET_REDUCTION, m_CabinetReduction);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsPREINFLATEAxonometric, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsPREINFLATEAxonometric)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsPREINFLATEAxonometric message handlers


void CDialogDetailsPREINFLATEAxonometric::SetParams(CDataAxonometric DIA)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Approach = DIA.AxonometricApproach;
	m_AmendCabinet = DIA.bAmendCabinet;
	m_90degrees = RADIANS_TO_DEGREES(DIA.d90degrees);
	m_135degrees = RADIANS_TO_DEGREES(DIA.d135degrees);

	m_Expansion = DIA.SpanningTreeAxonometric;
	m_CabinetThreshold = RADIANS_TO_DEGREES(DIA.CabinetThreshold);
	m_CabinetReduction = DIA.CabinetReduction;
}


void CDialogDetailsPREINFLATEAxonometric::GetParams(CDataAxonometric &DIA)
// Updates the general parameters that have been modified in the dialog
{
	DIA.AxonometricApproach= m_Approach;
	DIA.bAmendCabinet = m_AmendCabinet !=0;
	DIA.d90degrees = DEGREES_TO_RADIANS(m_90degrees);
	DIA.d135degrees = DEGREES_TO_RADIANS(m_135degrees);

	DIA.SpanningTreeAxonometric= m_Expansion;
	DIA.CabinetThreshold= DEGREES_TO_RADIANS(m_CabinetThreshold);
	DIA.CabinetReduction= m_CabinetReduction;
}

void CDialogDetailsPREINFLATEAxonometric::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataAxonometric Initial;
	SetParams(Initial);
	UpdateData( false );
}
