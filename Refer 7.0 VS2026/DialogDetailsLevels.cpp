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

// DialogDetailsLevels.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "DataRefer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DialogDetailsLevels.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsLevels dialog


CDialogDetailsLevels::CDialogDetailsLevels(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsLevels::IDD, pParent)
{
}


void CDialogDetailsLevels::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsLevels)
	DDX_Radio(pDX, IDC_RADIO_LevelsApproach, m_IncrementoNiveles);
	DDX_Radio(pDX, IDC_CRITERIO_EXPANSION, m_Expansion);
	DDX_Text(pDX, IDC_EDITCoeficienteAlargamiento, m_CoeficienteAlargamiento);
	DDX_Text(pDX, IDC_EDITCoeficienteAlargamiento2, m_CoeficienteAlargamiento2);

	DDX_Text(pDX, IDC_EDIT_Nivel0, m_Nivel0);
	DDX_Text(pDX, IDC_EDIT_Nivel1, m_Nivel1);
	DDX_Text(pDX, IDC_EDIT_Nivel2, m_Nivel2);
	DDX_Text(pDX, IDC_EDIT_Nivel3, m_Nivel3);
	DDX_Text(pDX, IDC_EDIT_Nivel4, m_Nivel4);
	DDX_Text(pDX, IDC_EDIT_Nivel5, m_Nivel5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsLevels, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsLevels)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	ON_BN_CLICKED(IDC_BUTTON_Calculo_Automatico, OnBUTTONAutomatic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsLevels message handlers


void CDialogDetailsLevels::SetParams(CDataLevels DIL)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Nivel0 = DIL.LevelsV[0]*100.;
	m_Nivel1 = DIL.LevelsV[1]*100.;
	m_Nivel2 = DIL.LevelsV[2]*100.;
	m_Nivel3 = DIL.LevelsV[3]*100.;
	m_Nivel4 = DIL.LevelsV[4]*100.;
	m_Nivel5 = DIL.LevelsV[5]*100.;

	m_Expansion = DIL.SpanningTreeLevels;
	if(DIL.IncrementalLevels)
		m_IncrementoNiveles= 0;
	else
		m_IncrementoNiveles= 1;
	m_CoeficienteAlargamiento = DIL.Coef_TreeUp*100.;
	m_CoeficienteAlargamiento2 = DIL.Coef_TreeDown*100.;
}


void CDialogDetailsLevels::GetParams(CDataLevels &DIL)
// Updates the general parameters that have been modified in the dialog
{
	DIL.LevelsV[0]= m_Nivel0/100.;
	DIL.LevelsV[1]= m_Nivel1/100.;
	DIL.LevelsV[2]= m_Nivel2/100.;
	DIL.LevelsV[3]= m_Nivel3/100.;
	DIL.LevelsV[4]= m_Nivel4/100.;
	DIL.LevelsV[5]= m_Nivel5/100.;

	DIL.SpanningTreeLevels=m_Expansion;

	if (m_IncrementoNiveles==0)
		DIL.IncrementalLevels= true;
	else
		DIL.IncrementalLevels= false;

	DIL.Coef_TreeUp=m_CoeficienteAlargamiento/100.;
	DIL.Coef_TreeDown=m_CoeficienteAlargamiento2/100.;
}


void CDialogDetailsLevels::OnBUTTONAutomatic()
{
	double Incremento = 100./5.;
	m_Nivel0 = 0.0;
	m_Nivel1 = Incremento;
	m_Nivel2 = 2*Incremento;
	m_Nivel3 = 3*Incremento;
	m_Nivel4 = 4*Incremento;
	m_Nivel5 = 5*Incremento;

	UpdateData(false);
}


void CDialogDetailsLevels::OnBUTTONRestoreDefault()
{
	if(AfxMessageBox(IDSL_BY_DEFAULT, MB_YESNO) == IDNO)
		return;
	CDataLevels Initial;
	SetParams(Initial);
	UpdateData(false);
}
