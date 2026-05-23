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

// DialogDetailsSimulatedAnnealingMulti.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "ReferDoc.h"
#include "Refer.h"
#include "OptimizationFlow.h"
#include "resource.h"

#include "DialogDetailsSimulatedAnnealingMulti.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSimulatedAnnealingMulti dialog


CDialogDetailsSimulatedAnnealingMulti::CDialogDetailsSimulatedAnnealingMulti(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsSimulatedAnnealingMulti::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsSimulatedAnnealingMulti)
	//}}AFX_DATA_INIT
}


void CDialogDetailsSimulatedAnnealingMulti::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsSimulatedAnnealingMulti)
	DDX_Text(pDX, IDC_EDIT_CriterioCongelacion, m_CriterioCongelacion);
	DDV_MinMaxDouble(pDX, m_CriterioCongelacion, 0., 100.);
	DDX_Text(pDX, IDC_EDIT_DecrementoTemperaturaPotencial, m_DecrementoTemperaturaPotencial);
	DDV_MinMaxDouble(pDX, m_DecrementoTemperaturaPotencial, 0.1, 1.);
	DDX_Text(pDX, IDC_EDIT_NumeroMaximoIteracionesEnEscalon, m_NumeroMaximoIteracionesEnEscalon);
	DDV_MinMaxInt(pDX, m_NumeroMaximoIteracionesEnEscalon, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_CriterioEquilibrio, m_CriterioEquilibrio);
	DDV_MinMaxDouble(pDX, m_CriterioEquilibrio, 0., 100.);
	DDX_Text(pDX, IDC_EDIT_CoefAciertosTemperaturaInicial, m_CoefAciertosTemperaturaInicial);
	DDV_MinMaxDouble(pDX, m_CoefAciertosTemperaturaInicial, 0., 100.);
	DDX_Radio(pDX, IDC_RADIO_StyleIncrement, m_EstiloIncremento);

	DDX_Text(pDX, IDC_EDIT_LengthInitialStep, m_LengthInitialStep);
	DDV_MinMaxDouble(pDX, m_LengthInitialStep, 0., 100.);
	DDX_Text(pDX, IDC_EDIT_LengthStepReduction, m_LengthStepReduction);
	DDV_MinMaxDouble(pDX, m_LengthStepReduction, 0., 100.);
	DDX_Text(pDX, IDC_EDIT_LengthFinalStep, m_LengthFinalStep);
	DDV_MinMaxDouble(pDX, m_LengthFinalStep, 0., 100.);

	DDX_Text(pDX, IDC_EDIT_IncrementsOfVariables, m_IncrementsOfVariables);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsSimulatedAnnealingMulti, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsSimulatedAnnealingMulti)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	ON_BN_CLICKED(IDC_RADIO_StyleIncrement, OnRADIOParametrico)
	ON_BN_CLICKED(IDC_RADIO3, OnRadioPrefijado)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSimulatedAnnealingMulti message handlers


void CDialogDetailsSimulatedAnnealingMulti::SetParams(CDataOptSAM DataOptSAM)
// Produces local versions of those general parameters that can be modified by this dialog
{
	double dDato;
	CString sTexto, sPaso;

	m_CriterioCongelacion = DataOptSAM.dCriterioCongelacion * 100;
	m_CriterioEquilibrio = DataOptSAM.dCriterioEquilibrio * 100;
	m_CoefAciertosTemperaturaInicial = DataOptSAM.dCoefAciertosTemperaturaInicial * 100;
	m_DecrementoTemperaturaPotencial = DataOptSAM.dDecrementoTemperaturaPotencial;
	m_NumeroMaximoIteracionesEnEscalon = DataOptSAM.iMaxIterationsPerLevel;

	m_EstiloIncremento = DataOptSAM.iStyleIncrement;
	m_LengthInitialStep = DataOptSAM.dInitialStep * 100;
	m_LengthStepReduction = DataOptSAM.dStepReduction * 100;
	m_LengthFinalStep = DataOptSAM.dFinalStep * 100;

	if (m_EstiloIncremento == PARAMETRIC){
		std::vector<double> IncrementsOfVariables;
		EstimateIncrementsForSA(CReferDoc::GetDoc()->DB_Brep_Model,
								IncrementsOfVariables,
								m_LengthInitialStep / 100,
								m_LengthFinalStep / 100,
								m_LengthStepReduction / 100);
		m_IncrementsOfVariables = "";
		for(long i=0; i<(long)IncrementsOfVariables.size() ; i++ ) {
			sPaso.Format("%i", i+1);
			sTexto.Format("%f\r\n", IncrementsOfVariables[i]);
			m_IncrementsOfVariables += sPaso + ")  " + sTexto;
		};
	}
	else{
		for(long i=0; i<(long)DataOptSAM.IncrementsOfVariables.size(); i++ ) {
			dDato= DataOptSAM.IncrementsOfVariables[i];
			sPaso.Format("%i", i+1);
			sTexto.Format("%f\r\n", dDato );
			m_IncrementsOfVariables += sPaso + ")  " + sTexto;
		}
	}
}


bool CDialogDetailsSimulatedAnnealingMulti::GetParams(CDataOptSAM &DataOptSAM)
// Updates the general parameters that have been modified in the dialog
{
	CString sTexto, sNumero;
	int iPosicionRETURN, iEstado;
	bool bTerminar;
	float dDato;

	std::vector<double> IncrementsOfVariables;

	if (m_EstiloIncremento == PARAMETRIC)
		EstimateIncrementsForSA(CReferDoc::GetDoc()->DB_Brep_Model,
								IncrementsOfVariables,
								m_LengthInitialStep / 100,
								m_LengthFinalStep / 100,
								m_LengthStepReduction / 100);
	else{
		sTexto = m_IncrementsOfVariables;
		if (sTexto.IsEmpty())
			bTerminar = true;
		else
			bTerminar = false;

		// Mientras queden lineas...
		while (bTerminar == false) {
			iPosicionRETURN = sTexto.Find(')');       // Saca la primera linea
			sTexto = sTexto.Mid( iPosicionRETURN + 1 );
			iPosicionRETURN = sTexto.Find('\n');       // Saca la primera linea
			if (iPosicionRETURN == -1)                 // (si no se encuentra saca todo el texto)
				iPosicionRETURN = sTexto.GetLength();
			sNumero = sTexto.Left( iPosicionRETURN );  // transforma al valor numerico y lo inserta
			iEstado = sscanf_s(sNumero, "%f", &dDato);
			if ((iEstado != 0) && (iEstado != EOF))
				IncrementsOfVariables.push_back(dDato);
			if ( iPosicionRETURN < sTexto.GetLength()-1 )   // Si quedan lineas, eliminar la primera, ...
				sTexto = sTexto.Mid( iPosicionRETURN + 1 );
			else
				bTerminar = true;                            // ... y si no quedan lineas, terminar.
		};
	}
	if ((long)IncrementsOfVariables.size() == 0){
		return false;
	}

	DataOptSAM.dCriterioCongelacion = m_CriterioCongelacion/100;
	DataOptSAM.dCriterioEquilibrio = m_CriterioEquilibrio/100;
	DataOptSAM.dCoefAciertosTemperaturaInicial = m_CoefAciertosTemperaturaInicial/100;
	DataOptSAM.dDecrementoTemperaturaPotencial = m_DecrementoTemperaturaPotencial;
	DataOptSAM.iMaxLevels = (long)IncrementsOfVariables.size();
	DataOptSAM.iMaxIterationsPerLevel = m_NumeroMaximoIteracionesEnEscalon;

	DataOptSAM.iStyleIncrement = m_EstiloIncremento;
	DataOptSAM.dInitialStep = m_LengthInitialStep/100;
	DataOptSAM.dStepReduction = m_LengthStepReduction/100;
	DataOptSAM.dFinalStep = m_LengthFinalStep/100;

	DataOptSAM.IncrementsOfVariables= IncrementsOfVariables;

	return true;
}


void CDialogDetailsSimulatedAnnealingMulti::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataOptSAM Initial;
	SetParams(Initial);
	UpdateData(false);
}


BOOL CDialogDetailsSimulatedAnnealingMulti::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(true);
	CEdit *pCajaEdicion;

	if (m_EstiloIncremento == 1){ //Prefijado
		pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthInitialStep);
		pCajaEdicion->EnableWindow(false);
		pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthStepReduction);
		pCajaEdicion->EnableWindow(false);
		pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthFinalStep);
		pCajaEdicion->EnableWindow(false);
	}
	else{ //Parametrico
		pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
		pCajaEdicion->EnableWindow(false);
	}
	
	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogDetailsSimulatedAnnealingMulti::OnRADIOParametrico() 
{
	CString sTexto, sPaso;

	CEdit *pCajaEdicion;
	UpdateData(true);

	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthInitialStep);
	pCajaEdicion->EnableWindow(true);
	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthStepReduction);
	pCajaEdicion->EnableWindow(true);
	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthFinalStep);
	pCajaEdicion->EnableWindow(true);
	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
	pCajaEdicion->EnableWindow(false);

	std::vector<double> IncrementsOfVariables;
	EstimateIncrementsForSA(CReferDoc::GetDoc()->DB_Brep_Model,
							IncrementsOfVariables,
							m_LengthInitialStep / 100,
							m_LengthFinalStep / 100,
							m_LengthStepReduction / 100);
	m_IncrementsOfVariables = "";
	for(long i=0; i<(long)IncrementsOfVariables.size() ; i++ ) {
		sPaso.Format("%i", i+1);
		sTexto.Format("%f\r\n", IncrementsOfVariables[i]);
		m_IncrementsOfVariables += sPaso + ")  " + sTexto;
	};
	UpdateData(false);
}


void CDialogDetailsSimulatedAnnealingMulti::OnRadioPrefijado() 
{
	CEdit *pCajaEdicion;
	UpdateData(true);

	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthInitialStep);
	pCajaEdicion->EnableWindow(false);
	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthStepReduction);
	pCajaEdicion->EnableWindow(false);
	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthFinalStep);
	pCajaEdicion->EnableWindow(false);
	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
	pCajaEdicion->EnableWindow(true);
}
