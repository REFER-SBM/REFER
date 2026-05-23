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

// DialogFilterStroke.cpp : implementation file
//

#include <stdafx.h>

#include "resource.h"
#include "DialogFilterStroke.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogFilterStroke dialog


CDialogFilterStroke::CDialogFilterStroke(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogFilterStroke::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogFilterStroke)
	//}}AFX_DATA_INIT
}


void CDialogFilterStroke::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogFilterStroke)
	DDX_Radio(pDX, IDC_FilterMode, m_FilterMode);
	DDX_Text(pDX, IDC_FilterFactor, m_FilterFactor);
	DDX_Text(pDX, IDC_ResampleFactor, m_ResampleFactor);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDialogFilterStroke message handlers


void CDialogFilterStroke::SetParams(long FilterMode, long FilterFactor, long ResampleFactor)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_FilterMode= FilterMode;
	m_FilterFactor= FilterFactor;
	m_ResampleFactor= ResampleFactor;
}

void CDialogFilterStroke::GetParams(long &FilterMode, long &FilterFactor, long &ResampleFactor)
// Updates the general parameters that have been modified in the dialog
{
	FilterMode= m_FilterMode;
	FilterFactor= m_FilterFactor;
	ResampleFactor= m_ResampleFactor;
}
