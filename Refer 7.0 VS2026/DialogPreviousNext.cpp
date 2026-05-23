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

// DialogPreviousNext.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "resource.h"

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DB_CSG.h"
#include "ReferDoc.h"

#include "DialogPreviousNext.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogPreviousNext dialog


CDialogPreviousNext::CDialogPreviousNext(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPreviousNext::IDD, pParent)
{
	Create (IDD);

	//{{AFX_DATA_INIT(CDialogPreviousNext)
	//}}AFX_DATA_INIT
}


void CDialogPreviousNext::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPreviousNext)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPreviousNext, CDialog)
	//{{AFX_MSG_MAP(CDialogPreviousNext)
	ON_BN_CLICKED(IDC_BUTTON_PREVIOUS, OnButtonPrevious)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogPreviousNext message handlers

void CDialogPreviousNext::UpdateRange()
{
	CString str;
	str.Format("%d < %d < %d\n", m_First, m_Current, m_Last);
	GetDlgItem(IDC_RANGE_PREVIOUS_NEXT)->SetWindowText(str);
}


void CDialogPreviousNext::SetParams(long Current, long First, long Last,
									long MessagePrevious, long MessageNext, long move)
// Produces local versions of those general parameters that can be used by this dialog
{
	m_Current= Current;
	m_First= First;
	m_Last= Last;
	m_Previous= MessagePrevious;
	m_Next= MessageNext;

	//Moves dialog if requested (to prevent overlapped dialogs)
	if(move != 0){
		CRect myRect;
		GetWindowRect(&myRect);

		myRect.left   += move;
		myRect.top    += move;
		myRect.bottom += move;
		myRect.right  += move;

		MoveWindow(myRect.left, myRect.top,
				   myRect.Width(), myRect.Height());
	}

	UpdateRange();
}

void CDialogPreviousNext::OnButtonPrevious() 
{
	if(m_Current > m_First){
		m_Current--;
		CReferDoc::GetDoc()->m_pFrameSketch2D->GetActiveView()->SendMessage(WM_COMMAND, m_Previous, 0);
	}

	UpdateRange();
}


void CDialogPreviousNext::OnButtonNext() 
{
	if(m_Current < m_Last){
		m_Current++;
		CReferDoc::GetDoc()->m_pFrameSketch2D->GetActiveView()->SendMessage(WM_COMMAND, m_Next, 0);
	}

	UpdateRange();
}


void CDialogPreviousNext::OnCancel() 
{
	//Switch off the button that activated the Prev/Next dialog!!

	CDialog::OnCancel();
}
