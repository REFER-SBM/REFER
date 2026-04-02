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

// DialogDetailsLevelsHidden.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DialogDetailsLevelsHidden.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsLevelsHidden dialog


CDialogDetailsLevelsHidden::CDialogDetailsLevelsHidden(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsLevelsHidden::IDD, pParent)
{
}


void CDialogDetailsLevelsHidden::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsLevelsHidden)
	DDX_Radio(pDX, IDC_RADIO_LevelsApproach, m_IncrementoNiveles);
	DDX_Radio(pDX, IDC_CRITERIO_EXPANSION, m_Expansion);
	DDX_Text(pDX, IDC_EDITCoeficienteAlargamiento, m_CoeficienteAlargamiento);
	DDX_Text(pDX, IDC_EDITCoeficienteAlargamiento2, m_CoeficienteAlargamiento2);

	DDX_Text(pDX, IDC_EDIT_Nivel0, m_Nivel0H);
	DDV_MinMaxDouble(pDX, m_Nivel0H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel1, m_Nivel1H);
	DDV_MinMaxDouble(pDX, m_Nivel1H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel2, m_Nivel2H);
	DDV_MinMaxDouble(pDX, m_Nivel2H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel3, m_Nivel3H);
	DDV_MinMaxDouble(pDX, m_Nivel3H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel4, m_Nivel4H);
	DDV_MinMaxDouble(pDX, m_Nivel4H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel10, m_Nivel10H);
	DDV_MinMaxDouble(pDX, m_Nivel10H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel11, m_Nivel11H);
	DDV_MinMaxDouble(pDX, m_Nivel11H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel12, m_Nivel12H);
	DDV_MinMaxDouble(pDX, m_Nivel12H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel13, m_Nivel13H);
	DDV_MinMaxDouble(pDX, m_Nivel13H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel14, m_Nivel14H);
	DDV_MinMaxDouble(pDX, m_Nivel14H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel16, m_Nivel16H);
	DDV_MinMaxDouble(pDX, m_Nivel16H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel5, m_Nivel5H);
	DDV_MinMaxDouble(pDX, m_Nivel5H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel6, m_Nivel6H);
	DDV_MinMaxDouble(pDX, m_Nivel6H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel7, m_Nivel7H);
	DDV_MinMaxDouble(pDX, m_Nivel7H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel8, m_Nivel8H);
	DDV_MinMaxDouble(pDX, m_Nivel8H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel9, m_Nivel9H);
	DDV_MinMaxDouble(pDX, m_Nivel9H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel15, m_Nivel15H);
	DDV_MinMaxDouble(pDX, m_Nivel15H, 0., 150.);
	DDX_Text(pDX, IDC_EDIT_Nivel17, m_Nivel17H);
	DDV_MinMaxDouble(pDX, m_Nivel17H, 0., 150.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsLevelsHidden, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsLevelsHidden)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	ON_BN_CLICKED(IDC_BUTTON_Calculo_Automatico, OnBUTTONAutomatic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsLevelsHidden message handlers

void CDialogDetailsLevelsHidden::SetParams(CDataLevels DIL)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_Nivel0H = DIL.LevelsH[0]*100.;
	m_Nivel1H = DIL.LevelsH[1]*100.;
	m_Nivel2H = DIL.LevelsH[2]*100.;
	m_Nivel3H = DIL.LevelsH[3]*100.;
	m_Nivel4H = DIL.LevelsH[4]*100.;
	m_Nivel5H = DIL.LevelsH[5]*100.;
	m_Nivel6H = DIL.LevelsH[6]*100.;
	m_Nivel7H = DIL.LevelsH[7]*100.;
	m_Nivel8H = DIL.LevelsH[8]*100.;
	m_Nivel9H = DIL.LevelsH[9]*100.;
	m_Nivel10H = DIL.LevelsH[10]*100.;
	m_Nivel11H = DIL.LevelsH[11]*100.;
	m_Nivel12H = DIL.LevelsH[12]*100.;
	m_Nivel13H = DIL.LevelsH[13]*100.;
	m_Nivel14H = DIL.LevelsH[14]*100.;
	m_Nivel15H = DIL.LevelsH[15]*100.;
	m_Nivel16H = DIL.LevelsH[16]*100.;
	m_Nivel17H = DIL.LevelsH[17]*100.;

	m_Expansion = DIL.SpanningTreeLevels;
	if(DIL.IncrementalLevels)
		m_IncrementoNiveles = 0;
	else
		m_IncrementoNiveles = 1;

	m_CoeficienteAlargamiento = DIL.Coef_TreeUp*100.;
	m_CoeficienteAlargamiento2 = DIL.Coef_TreeDown*100.;
}


void CDialogDetailsLevelsHidden::GetParams(CDataLevels &DIL)
// Updates the general parameters that have been modified in the dialog
{
	DIL.LevelsH[0]= m_Nivel0H/100.;
	DIL.LevelsH[1]= m_Nivel1H/100.;
	DIL.LevelsH[2]= m_Nivel2H/100.;
	DIL.LevelsH[3]= m_Nivel3H/100.;
	DIL.LevelsH[4]= m_Nivel4H/100.;
	DIL.LevelsH[5]= m_Nivel5H/100.;
	DIL.LevelsH[6]= m_Nivel6H/100.;
	DIL.LevelsH[7]= m_Nivel7H/100.;
	DIL.LevelsH[8]= m_Nivel8H/100.;
	DIL.LevelsH[9]= m_Nivel9H/100.;
	DIL.LevelsH[10]= m_Nivel10H/100.;
	DIL.LevelsH[11]= m_Nivel11H/100.;
	DIL.LevelsH[12]= m_Nivel12H/100.;
	DIL.LevelsH[13]= m_Nivel13H/100.;
	DIL.LevelsH[14]= m_Nivel14H/100.;
	DIL.LevelsH[15]= m_Nivel15H/100.;
	DIL.LevelsH[16]= m_Nivel16H/100.;
	DIL.LevelsH[17]= m_Nivel17H/100.;

	DIL.SpanningTreeLevels=m_Expansion;

	if(m_IncrementoNiveles ==0)
		DIL.IncrementalLevels = true;
	else
		DIL.IncrementalLevels = false;

	DIL.Coef_TreeUp= m_CoeficienteAlargamiento/100.;
	DIL.Coef_TreeDown= m_CoeficienteAlargamiento2/100.;
}


void CDialogDetailsLevelsHidden::OnBUTTONAutomatic()
{
	double Incremento = 100./17.;
	m_Nivel0H = 0.0;
	m_Nivel1H = Incremento;
	m_Nivel2H = 2*Incremento;
	m_Nivel3H = 3*Incremento;
	m_Nivel4H = 4*Incremento;
	m_Nivel5H = 5*Incremento;
	m_Nivel6H = 6*Incremento;
	m_Nivel7H = 7*Incremento;
	m_Nivel8H = 8*Incremento;
	m_Nivel9H = 9*Incremento;
	m_Nivel10H = 10*Incremento;
	m_Nivel11H = 11*Incremento;
	m_Nivel12H = 12*Incremento;
	m_Nivel13H = 13*Incremento;
	m_Nivel14H = 14*Incremento;
	m_Nivel15H = 15*Incremento;
	m_Nivel16H = 16*Incremento;
	m_Nivel17H = 17*Incremento;

	UpdateData(false);
}


void CDialogDetailsLevelsHidden::OnBUTTONRestoreDefault()
{
	if (AfxMessageBox(IDSL_BY_DEFAULT, MB_YESNO) == IDNO)
		return;
	CDataLevels Initial;
	SetParams(Initial);
	UpdateData(false);
}
