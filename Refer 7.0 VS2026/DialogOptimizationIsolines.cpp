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

// DialogOptimizationIsolines.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"

#include "DialogOptimizationIsolines.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogOptimizationIsolines dialog


CDialogOptimizationIsolines::CDialogOptimizationIsolines(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogOptimizationIsolines::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogOptimizationIsolines)
	//}}AFX_DATA_INIT
}


void CDialogOptimizationIsolines::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogOptimizationIsolines)
	DDX_Control(pDX, IDC_SLIDER_INTERPOLATION, m_InterpolationDegree);
	DDX_Text(pDX, IDC_X, m_X);
	DDX_Text(pDX, IDC_Y, m_Y);
	DDX_Text(pDX, IDC_INCX, m_INCX);
	DDX_Text(pDX, IDC_INCY, m_INCY);
	DDX_Text(pDX, IDC_X0, m_X0);
	DDX_Text(pDX, IDC_XF, m_XF);
	DDX_Text(pDX, IDC_Y0, m_Y0);
	DDX_Text(pDX, IDC_YF, m_YF);
	DDX_Radio(pDX, IDC_TG, m_TG);
	DDX_Text(pDX, IDC_INCZ, m_INCZ);
	DDX_Text(pDX, IDC_Z, m_Z);
	DDX_Text(pDX, IDC_V, m_V);
	DDX_Radio(pDX, IDC_OTHER_VARIABLES, m_OtherVariables);
	DDX_Radio(pDX, IDC_COLOR, m_Color);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogOptimizationIsolines, CDialog)
	//{{AFX_MSG_MAP(CDialogOptimizationIsolines)
	ON_BN_CLICKED(IDC_TG, OnRadioUnaVariable)
	ON_BN_CLICKED(IDC_RADIO1, OnRadioDosVariables)
	ON_BN_CLICKED(IDC_OTHER_VARIABLES, OnZCero)
	ON_BN_CLICKED(IDC_RADIO2, OnZActuales)
	ON_BN_CLICKED(IDC_RADIO3, OnModificarVariable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogOptimizationIsolines message handlers

void CDialogOptimizationIsolines::SetParams(CParamsIsolines* pParamsIsolines)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_TG = pParamsIsolines->TG;
	m_OtherVariables = pParamsIsolines->OtherVariables;
	m_iInterpolationDegree = pParamsIsolines->InterpolationDegree;
	m_V = pParamsIsolines->V;
	m_Z = pParamsIsolines->Z;

	m_X = pParamsIsolines->X;
	m_X0=pParamsIsolines->X0;
	m_INCX=pParamsIsolines->INCX;
	m_XF=pParamsIsolines->XF;

	m_Y = pParamsIsolines->Y;
	m_Y0=pParamsIsolines->Y0;
	m_INCY=pParamsIsolines->INCY;
	m_YF=pParamsIsolines->YF;

	m_INCZ = 100*pParamsIsolines->INCZ;
	m_Color = pParamsIsolines->Color;
}

void CDialogOptimizationIsolines::GetParams(CParamsIsolines* pParamsIsolines)
// Updates the general parameters that have been modified in the dialog
{
	pParamsIsolines->TG= m_TG;
	pParamsIsolines->OtherVariables= m_OtherVariables;
	pParamsIsolines->InterpolationDegree = m_iInterpolationDegree;
	pParamsIsolines->V= m_V;
	pParamsIsolines->Z= m_Z;

	pParamsIsolines->X= m_X;
	pParamsIsolines->X0= m_X0;
	pParamsIsolines->INCX= m_INCX;
	pParamsIsolines->XF= m_XF;

	pParamsIsolines->Y= m_Y;
	pParamsIsolines->Y0= m_Y0;
	pParamsIsolines->INCY= m_INCY;
	pParamsIsolines->YF= m_YF;

	pParamsIsolines->INCZ= m_INCZ/100;
	pParamsIsolines->Color= m_Color;
}


BOOL CDialogOptimizationIsolines::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CButton *pBoton;
	
	if (m_TG==0){
		pBoton= (CButton*) GetDlgItem(IDC_Y);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_Y0);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_INCY);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_YF);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_INCZ);
		pBoton->EnableWindow(false);
	}

	if (m_OtherVariables!=2){
		pBoton= (CButton*) GetDlgItem(IDC_V);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_Z);
		pBoton->EnableWindow(false);
	}

	m_InterpolationDegree.SetRange(0,10);
	m_InterpolationDegree.SetPos(m_iInterpolationDegree);
	UpdateData(false);
	
	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogOptimizationIsolines::OnRadioUnaVariable() 
{
		CButton *pBoton;
		pBoton= (CButton*) GetDlgItem(IDC_Y);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_Y0);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_INCY);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_YF);
		pBoton->EnableWindow(false);
		pBoton= (CButton*) GetDlgItem(IDC_INCZ);
		pBoton->EnableWindow(false);
}

void CDialogOptimizationIsolines::OnRadioDosVariables() 
{
		CButton *pBoton;
		pBoton= (CButton*) GetDlgItem(IDC_Y);
		pBoton->EnableWindow(true);
		pBoton= (CButton*) GetDlgItem(IDC_Y0);
		pBoton->EnableWindow(true);
		pBoton= (CButton*) GetDlgItem(IDC_INCY);
		pBoton->EnableWindow(true);
		pBoton= (CButton*) GetDlgItem(IDC_YF);
		pBoton->EnableWindow(true);
		pBoton= (CButton*) GetDlgItem(IDC_INCZ);
		pBoton->EnableWindow(true);
}

void CDialogOptimizationIsolines::OnZCero() 
{
	CButton *pBoton;

	pBoton= (CButton*) GetDlgItem(IDC_V);
	pBoton->EnableWindow(false);
	pBoton= (CButton*) GetDlgItem(IDC_Z);
	pBoton->EnableWindow(false);
}

void CDialogOptimizationIsolines::OnZActuales() 
{
	CButton *pBoton;

	pBoton= (CButton*) GetDlgItem(IDC_V);
	pBoton->EnableWindow(false);
	pBoton= (CButton*) GetDlgItem(IDC_Z);
	pBoton->EnableWindow(false);
}

void CDialogOptimizationIsolines::OnModificarVariable() 
{
	CButton *pBoton;

	pBoton= (CButton*) GetDlgItem(IDC_V);
	pBoton->EnableWindow(true);
	pBoton= (CButton*) GetDlgItem(IDC_Z);
	pBoton->EnableWindow(true);
}


void CDialogOptimizationIsolines::OnOK() 
{
	UpdateData(true);
	m_iInterpolationDegree= m_InterpolationDegree.GetPos(); 
	
	CDialog::OnOK();
}
