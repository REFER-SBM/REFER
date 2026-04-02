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

// DialogToolsDisplayStrokes.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogToolsDisplayStrokes.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogToolsDisplayStrokes dialog


CDialogToolsDisplayStrokes::CDialogToolsDisplayStrokes(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogToolsDisplayStrokes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogToolsDisplayStrokes)
	//}}AFX_DATA_INIT
}


void CDialogToolsDisplayStrokes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogToolsDisplayStrokes)
	DDX_Radio(pDX, IDC_SELECTNODES, m_Nodes);
	DDX_Check(pDX, IDC_Display_Corners, m_Display_Corners);
	DDX_Check(pDX, IDC_Display_Line_Tolerance, m_Display_Line_Tolerance);
	DDX_Check(pDX, IDC_Display_Arc_Tolerance, m_Display_Arc_Tolerance);
	DDX_Check(pDX, IDC_Display_Convex_Hull, m_Display_Convex_Hull);
	DDX_Check(pDX, IDC_Display_Merits_Strokes, m_Display_Merits_Strokes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogToolsDisplayStrokes, CDialog)
	//{{AFX_MSG_MAP(CDialogToolsDisplayStrokes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogToolsDisplayStrokes message handlers


void CDialogToolsDisplayStrokes::SetParams(long SelectedNodes, bool Display_Corners,
										   bool Display_Line_Tolerance, bool Display_Arc_Tolerance,
										   bool Display_Convex_Hull, bool Display_Merits_Strokes)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Nodes = SelectedNodes;
	m_Display_Corners= Display_Corners;
	m_Display_Line_Tolerance= Display_Line_Tolerance;
	m_Display_Arc_Tolerance= Display_Arc_Tolerance;
	m_Display_Convex_Hull= Display_Convex_Hull;
	m_Display_Merits_Strokes= Display_Merits_Strokes;
}


void CDialogToolsDisplayStrokes::GetParams(long &SelectedNodes, bool &Display_Corners,
										   bool &Display_Line_Tolerance, bool &Display_Arc_Tolerance,
										   bool &Display_Convex_Hull, bool &Display_Merits_Strokes)
// Updates the general parameters that have been modified in the dialog
{
	SelectedNodes = m_Nodes;
	Display_Corners= (m_Display_Corners != 0);
	Display_Line_Tolerance= (m_Display_Line_Tolerance != 0);
	Display_Arc_Tolerance= (m_Display_Arc_Tolerance != 0);
	Display_Convex_Hull= (m_Display_Convex_Hull != 0);
	Display_Merits_Strokes= (m_Display_Merits_Strokes != 0);
}


BOOL CDialogToolsDisplayStrokes::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
