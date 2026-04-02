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

// DialogInflationDetalles.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogDetailsInflation.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsInflation dialog


CDialogDetailsInflation::CDialogDetailsInflation(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsInflation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsInflation)
	//}}AFX_DATA_INIT
}


void CDialogDetailsInflation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsInflation)
	DDX_Text(pDX, IDC_EDIT_NumIntermediateInflations, m_NumIntermediateInflations);
	DDV_MinMaxInt(pDX, m_NumIntermediateInflations, 1, 20);
	DDX_Check(pDX, IDC_EDIT_InflationDrawing, m_ViewInflationDrawing);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsInflation, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsInflation)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsInflation message handlers

void CDialogDetailsInflation::SetParams(CDataInflation In)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_ViewInflationDrawing= In.bViewInflationDrawing;
	m_NumIntermediateInflations = In.iNumIntermediateInflations;
}


void CDialogDetailsInflation::GetParams(CDataInflation &In)
// Updates the general parameters that have been modified in the dialog
{
	In.bViewInflationDrawing= (m_ViewInflationDrawing!=0);
	In.iNumIntermediateInflations =m_NumIntermediateInflations;
}
