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

// DialogSelectTree.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"
#include "DialogSelectTree.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectTree dialog


CDialogSelectTree::CDialogSelectTree(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSelectTree::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogSelectTree)
	//}}AFX_DATA_INIT
}


void CDialogSelectTree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogSelectTree)
	DDX_Radio(pDX, IDC_AXONOMETRICTREE, m_Tree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogSelectTree, CDialog)
	//{{AFX_MSG_MAP(CDialogSelectTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectTree message handlers


void CDialogSelectTree::SetParams(long SelectedTree)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Tree = SelectedTree;
}


void CDialogSelectTree::GetParams(long &SelectedTree)
// Updates the general parameters that have been modified in the dialog
{
	SelectedTree = m_Tree;
}

BOOL CDialogSelectTree::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
