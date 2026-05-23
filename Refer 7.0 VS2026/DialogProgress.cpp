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

// DialogProgreso.cpp : implementation file
//

#include <stdafx.h>
#include "resource.h"
#include "DialogProgress.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogProgress dialog


CDialogProgress::CDialogProgress(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogProgress::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogProgress)
	m_sMessage = _T("");
	m_sTime = _T("");
	//}}AFX_DATA_INIT
}


void CDialogProgress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogProgress)
	DDX_Text(pDX, IDC_EDIT_Message, m_sMessage);
	DDX_Text(pDX, IDC_EDIT_Time, m_sTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogProgress, CDialog)
	//{{AFX_MSG_MAP(CDialogProgress)
	ON_BN_CLICKED(IDC_BUTTON_CancelarProgreso, OnBUTTONCancelarProgreso)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogProgress message handlers

BOOL CDialogProgress::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ControlProgress = (CProgressCtrl*) GetDlgItem( IDC_PROGRESS );
	m_CancelAsked = false;

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogProgress::OnBUTTONCancelarProgreso() 
{
	m_CancelAsked = true;
}


void CDialogProgress::Message(CString sMessage)
{
	m_sMessage = sMessage;
	UpdateData(false);
	MSG Message;
	if (::PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&Message);
		::DispatchMessage(&Message);
	};
}


void CDialogProgress::RangeProgress(long lMin, long lMax)
{
	m_ControlProgress->SetRange( (short)lMin, (short)lMax );
	m_HoraInicial = CTime::GetCurrentTime();
}


bool CDialogProgress::UpdateProgress(long lPosition)
{
	m_ControlProgress->SetPos( lPosition );

	CTime HoraFinal = CTime::GetCurrentTime();
	CTimeSpan Tiempo = HoraFinal - m_HoraInicial;
	m_sTime.Format(" %i' %i\"", Tiempo.GetMinutes(), Tiempo.GetSeconds() );
	UpdateData(false);
	UpdateWindow();

// --------------------------------------------------------------------------
	//This code opens the way for processing other Windows messages,
	//in parallel with the algorithn that called this Dialog
	MSG Message;
	if (::PeekMessage(&Message, NULL, 0, 0, PM_REMOVE)) {
		::TranslateMessage(&Message);
		::DispatchMessage(&Message);
	};
// --------------------------------------------------------------------------
	
	return !m_CancelAsked;
}
