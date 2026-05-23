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

// DialogDetailsSimulatedAnnealing.cpp : implementation file
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

#include "DialogDetailsSimulatedAnnealing.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSimulatedAnnealing dialog


CDialogDetailsSimulatedAnnealing::CDialogDetailsSimulatedAnnealing(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsSimulatedAnnealing::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsSimulatedAnnealing)
	//}}AFX_DATA_INIT
}


void CDialogDetailsSimulatedAnnealing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsSimulatedAnnealing)
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
	DDX_Radio(pDX, IDC_RADIO_StyleIncrement, m_StyleIncrement);
	DDX_Text(pDX, IDC_EDIT_LengthInitialStep, m_LengthInitialStep);
	DDV_MinMaxDouble(pDX, m_LengthInitialStep, 0., 100.);
	DDX_Text(pDX, IDC_EDIT_LengthStepReduction, m_LengthStepReduction);
	DDV_MinMaxDouble(pDX, m_LengthStepReduction, 0., 99.5);
	DDX_Text(pDX, IDC_EDIT_IncrementsOfVariables, m_IncrementsOfVariables);
	DDX_Text(pDX, IDC_EDIT_LengthFinalStep, m_LengthFinalStep);
	DDV_MinMaxDouble(pDX, m_LengthFinalStep, 0., 100.);
	DDX_Radio(pDX, IDC_RADIO_MethodInitialTemperature, m_MethodInitialTemperature);
	DDX_Text(pDX, IDC_EDIT_Repetitions, m_Repetitions);
	DDV_MinMaxInt(pDX, m_Repetitions, 1, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsSimulatedAnnealing, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsSimulatedAnnealing)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	ON_BN_CLICKED(IDC_RADIO_StyleIncrement, OnRADIOParametrico)
	ON_BN_CLICKED(IDC_RADIO3, OnRadioPrefijado)
	//ON_BN_CLICKED(IDC_DetailsRestoreValues, OnDetallesRestaurarValores)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSimulatedAnnealing message handlers

void CDialogDetailsSimulatedAnnealing::SetParams(CDataOptSA DataOptSA)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_CriterioCongelacion = DataOptSA.dCriterioCongelacion * 100;
	m_CriterioEquilibrio = DataOptSA.dCriterioEquilibrio * 100;
	m_CoefAciertosTemperaturaInicial = DataOptSA.dCoefAciertosTemperaturaInicial * 100;
	m_DecrementoTemperaturaPotencial = DataOptSA.dDecrementoTemperaturaPotencial;
	m_NumeroMaximoIteracionesEnEscalon = DataOptSA.iMaxIterationsPerLevel;

	m_StyleIncrement = DataOptSA.iStyleIncrement;
	m_LengthInitialStep = DataOptSA.dInitialStep * 100;
	m_LengthStepReduction = DataOptSA.dStepReduction * 100;
	m_LengthFinalStep = DataOptSA.dFinalStep * 100;
	m_MethodInitialTemperature = DataOptSA.iMethodInitialTemperature;

	m_Repetitions = DataOptSA.iRepetitions;

	CString sTexto, sPaso;

	if (m_StyleIncrement == PARAMETRIC){
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
		for(long i=0; i<(long)DataOptSA.IncrementsOfVariables.size(); i++ ) {
			double dDato= DataOptSA.IncrementsOfVariables[i];
			sPaso.Format("%i", i+1);
			sTexto.Format("%f\r\n", dDato );
			m_IncrementsOfVariables += sPaso + ")  " + sTexto;
		}
	}
}


bool CDialogDetailsSimulatedAnnealing::GetParams(CDataOptSA &DataOptSA)
// Updates the general parameters that have been modified in the dialog
{
	CString sTexto, sNumero;
	int iPosicionRETURN, iEstado;
	bool bTerminar;
	float dDato;

	std::vector<double> IncrementsOfVariables;

	if (m_StyleIncrement == PARAMETRIC)
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

	DataOptSA.dCriterioCongelacion = m_CriterioCongelacion/100;
	DataOptSA.dCriterioEquilibrio = m_CriterioEquilibrio/100;
	DataOptSA.dCoefAciertosTemperaturaInicial = m_CoefAciertosTemperaturaInicial/100;
	DataOptSA.dDecrementoTemperaturaPotencial = m_DecrementoTemperaturaPotencial;
	DataOptSA.iMaxLevels = (long)IncrementsOfVariables.size();
	DataOptSA.iMaxIterationsPerLevel = m_NumeroMaximoIteracionesEnEscalon;

	DataOptSA.iStyleIncrement = m_StyleIncrement;
	DataOptSA.dInitialStep = m_LengthInitialStep/100;
	DataOptSA.dStepReduction = m_LengthStepReduction/100;
	DataOptSA.dFinalStep = m_LengthFinalStep/100;
	DataOptSA.iMethodInitialTemperature = m_MethodInitialTemperature;

	DataOptSA.iRepetitions = m_Repetitions;

	DataOptSA.IncrementsOfVariables= IncrementsOfVariables;

	return true;
}


void CDialogDetailsSimulatedAnnealing::OnBUTTONRestoreDefault() 
{
	if (AfxMessageBox(IDSL_BY_DEFAULT, MB_YESNO) == IDNO)
		return;
	CDataOptSA Initial;
	SetParams(Initial);
	UpdateData(false);
}


BOOL CDialogDetailsSimulatedAnnealing::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(true);
	CEdit *pCajaEdicion;

	if (m_StyleIncrement == 1){ //Prefijado
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


void CDialogDetailsSimulatedAnnealing::OnRADIOParametrico() 
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


void CDialogDetailsSimulatedAnnealing::OnRadioPrefijado() 
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


//void CDialogDetailsSimulatedAnnealing::OnDetallesRestaurarValores() 
//{
//	CDialogSimulatedAnnealingPreConfig Dlg;
//	Dlg.SetParams(ReferApp.In.DataOptSA);
//
//	if (Dlg.DoModal() == IDOK){
//		Dlg.GetParams(ReferApp.In.DataOptSA);
//	}
//}
