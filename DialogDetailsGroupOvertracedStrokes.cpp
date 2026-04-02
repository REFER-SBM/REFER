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

// DialogDetailsGroupOvertracedStrokes.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DataRefer.h"
#include "resource.h"

#include "DialogDetailsGroupOvertracedStrokes.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsGroupOvertracedStrokes dialog


CDialogDetailsGroupOvertracedStrokes::CDialogDetailsGroupOvertracedStrokes(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsGroupOvertracedStrokes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsGroupOvertracedStrokes)
	//}}AFX_DATA_INIT
}


void CDialogDetailsGroupOvertracedStrokes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsGroupOvertracedStrokes)
	DDX_Text(pDX, IDC_MinOvertraceOverlap, m_MinOvertraceOverlap);
	DDX_Text(pDX, IDC_MaxOvertraceAngle, m_MaxOvertraceAngle);
	DDX_Text(pDX, IDC_MaxOvertraceOffset, m_MaxOvertraceOffset);
	DDX_Text(pDX, IDC_IncreaseOvertraceTolerances, m_IncreaseOvertraceTolerances);
	DDX_Text(pDX, IDC_OvertraceLineTolMin, m_OvertraceLineTolMin);
	DDX_Text(pDX, IDC_OvertraceLineTolMax, m_OvertraceLineTolMax);
	DDX_Text(pDX, IDC_OvertraceLineSmoothPenalty, m_OvertraceLineSmoothPenalty);
	DDX_Text(pDX, IDC_OvertraceMinMeritLine, m_OvertraceMinMeritLine);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsGroupOvertracedStrokes, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsGroupOvertracedStrokes)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsGroupOvertracedStrokes message handlers

void CDialogDetailsGroupOvertracedStrokes::SetParams(CDataVectorize Ve)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_MinOvertraceOverlap= Ve.MinOvertraceOverlap*100;
	m_MaxOvertraceAngle= RADIANS_TO_DEGREES(Ve.MaxOvertraceAngle);
	m_MaxOvertraceOffset= Ve.MaxOvertraceOffset*100;
	m_IncreaseOvertraceTolerances= Ve.IncreaseOvertraceTolerances*100;

	m_OvertraceLineTolMin= Ve.OvertraceLineTolMin;
	m_OvertraceLineTolMax= Ve.OvertraceLineTolMax;
	m_OvertraceLineSmoothPenalty= Ve.OvertraceLineSmoothPenalty;
	m_OvertraceMinMeritLine= Ve.OvertraceMinMeritLine;
}


void CDialogDetailsGroupOvertracedStrokes::GetParams(CDataVectorize &Ve)
// Updates the general parameters that have been modified in the dialog
{
	Ve.MinOvertraceOverlap= m_MinOvertraceOverlap/100;
	Ve.MaxOvertraceAngle= DEGREES_TO_RADIANS(m_MaxOvertraceAngle);
	Ve.MaxOvertraceOffset= m_MaxOvertraceOffset/100;
	Ve.IncreaseOvertraceTolerances= m_IncreaseOvertraceTolerances/100;

	Ve.OvertraceLineTolMin= m_OvertraceLineTolMin;
	Ve.OvertraceLineTolMax= m_OvertraceLineTolMax;
	Ve.OvertraceLineSmoothPenalty= m_OvertraceLineSmoothPenalty;
	Ve.OvertraceMinMeritLine= m_OvertraceMinMeritLine;
}


void CDialogDetailsGroupOvertracedStrokes::OnBUTTONRestoreDefault()
{
	if(AfxMessageBox(IDSL_BY_DEFAULT, MB_YESNO) == IDNO)
		return;
	CDataVectorize Initial;
	SetParams(Initial);
	UpdateData(false);
}

