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

// DialogDetailsGroupNonSolidStrokes.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DataRefer.h"
#include "resource.h"

#include "DialogDetailsGroupNonSolidStrokes.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsGroupNonSolidStrokes dialog


CDialogDetailsGroupNonSolidStrokes::CDialogDetailsGroupNonSolidStrokes(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsGroupNonSolidStrokes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsGroupNonSolidStrokes)
	//}}AFX_DATA_INIT
}


void CDialogDetailsGroupNonSolidStrokes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsGroupNonSolidStrokes)
	DDX_Text(pDX, IDC_IsolatedTipThreshold, m_IsolatedTipThreshold);
	DDX_Text(pDX, IDC_DashSizeMax, m_DashSizeMax);
	DDX_Text(pDX, IDC_DashSizeShortRange, m_DashSizeShortRange);
	DDX_Text(pDX, IDC_DashSizeLongRange, m_DashSizeLongRange);
	DDX_Text(pDX, IDC_DashGapShortRange, m_DashGapShortRange);
	DDX_Text(pDX, IDC_DashGapLongRange, m_DashGapLongRange);
	DDX_Text(pDX, IDC_MaxDashAngle, m_MaxDashAngle);
	DDX_Text(pDX, IDC_MaxDashOffset, m_MaxDashOffset);
	DDX_Text(pDX, IDC_IncreaseDashTolerances, m_IncreaseDashTolerances);
	DDX_Text(pDX, IDC_DashBalanceStepAngle, m_DashBalanceStepAngle);
	DDX_Control(pDX, IDC_SLIDER_DashBalanceStepAngle, m_sliderDashBalanceStepAngle);

	DDX_Text(pDX, IDC_MaxNonStraightDashes, m_MaxNonStraightDashes);
	DDX_Text(pDX, IDC_DashLineTolMin, m_DashLineTolMin);
	DDX_Text(pDX, IDC_DashLineTolMax, m_DashLineTolMax);
	DDX_Text(pDX, IDC_DashLineSmoothPenalty, m_DashLineSmoothPenalty);
	DDX_Text(pDX, IDC_DashMinMeritLine, m_DashMinMeritLine);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsGroupNonSolidStrokes, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsGroupNonSolidStrokes)
		// NOTE: the ClassWizard will add message map macros here

	ON_WM_HSCROLL() // Called when Slider is scrolled
	ON_EN_CHANGE(IDC_DashBalanceStepAngle, OnEnChangeEdit) // Called when CEdit change value

	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsGroupNonSolidStrokes message handlers

void CDialogDetailsGroupNonSolidStrokes::SetParams(CDataVectorize Ve)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_IsolatedTipThreshold= Ve.IsolatedTipThreshold*100;
	m_DashSizeMax= Ve.DashSizeMax*100;
	m_DashSizeShortRange= Ve.DashSizeShortRange;
	m_DashSizeLongRange= Ve.DashSizeLongRange;
	m_DashGapShortRange= Ve.DashGapShortRange;
	m_DashGapLongRange= Ve.DashGapLongRange;
	m_MaxDashAngle= RADIANS_TO_DEGREES(Ve.MaxDashAngle);
	m_MaxDashOffset= Ve.MaxDashOffset*100;
	m_IncreaseDashTolerances= Ve.IncreaseDashTolerances*100;
	m_DashBalanceStepAngle= Ve.DashBalanceStepAngle*100;
	//m_iSliderDashBalanceStepAngle= (int)m_DashBalanceStepAngle;

	m_MaxNonStraightDashes= Ve.MaxNonStraightDashes*100;
	m_DashLineTolMin= Ve.DashLineTolMin*100;
	m_DashLineTolMax= Ve.DashLineTolMax*100;
	m_DashLineSmoothPenalty= Ve.DashLineSmoothPenalty;
	m_DashMinMeritLine= Ve.DashMinMeritLine;
}


void CDialogDetailsGroupNonSolidStrokes::GetParams(CDataVectorize &Ve)
// Updates the general parameters that have been modified in the dialog
{
	Ve.IsolatedTipThreshold= m_IsolatedTipThreshold/100;
	Ve.DashSizeMax= m_DashSizeMax/100;
	Ve.DashSizeShortRange= m_DashSizeShortRange;
	Ve.DashSizeLongRange= m_DashSizeLongRange;
	Ve.DashGapShortRange= m_DashGapShortRange;
	Ve.DashGapLongRange= m_DashGapLongRange;
	Ve.MaxDashAngle= DEGREES_TO_RADIANS(m_MaxDashAngle);
	Ve.MaxDashOffset= m_MaxDashOffset/100;
	Ve.IncreaseDashTolerances= m_IncreaseDashTolerances/100;
	Ve.DashBalanceStepAngle= m_DashBalanceStepAngle/100;
	//Ve.DashBalanceStepAngle= m_iSliderDashBalanceStepAngle/100;

	Ve.MaxNonStraightDashes= m_MaxNonStraightDashes/100;
	Ve.DashLineTolMin= m_DashLineTolMin/100;
	Ve.DashLineTolMax= m_DashLineTolMax/100;
	Ve.DashLineSmoothPenalty= m_DashLineSmoothPenalty;
	Ve.DashMinMeritLine= m_DashMinMeritLine;
}


BOOL CDialogDetailsGroupNonSolidStrokes::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_sliderDashBalanceStepAngle.SetRange(0,100);
	m_sliderDashBalanceStepAngle.SetPos((int)m_DashBalanceStepAngle);
	UpdateData(false);
	
	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogDetailsGroupNonSolidStrokes::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);

	if (pSlider == &m_sliderDashBalanceStepAngle){
		CString strSliderValue;
		int iValue = m_sliderDashBalanceStepAngle.GetPos(); // Get Slider value
		strSliderValue.Format("%d", iValue); 

		GetDlgItem(IDC_DashBalanceStepAngle)->SetWindowText(strSliderValue); // Change CEdit Value
	}
}

void CDialogDetailsGroupNonSolidStrokes::OnEnChangeEdit()
{
	CString strEditValue;
	GetDlgItem(IDC_DashBalanceStepAngle)->GetWindowText(strEditValue);
	int iCeditValue = atoi(strEditValue);

	m_sliderDashBalanceStepAngle.SetPos(iCeditValue); // set Slider value
}


void CDialogDetailsGroupNonSolidStrokes::OnBUTTONRestoreDefault()
{
	if(AfxMessageBox(IDSL_BY_DEFAULT, MB_YESNO) == IDNO)
		return;
	CDataVectorize Initial;
	SetParams(Initial);
	UpdateData(false);
}


void CDialogDetailsGroupNonSolidStrokes::OnOK() 
{
	UpdateData(true);
	m_DashBalanceStepAngle= (int)m_sliderDashBalanceStepAngle.GetPos(); 
	
	CDialog::OnOK();
}
