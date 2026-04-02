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

// DialogDetailsHillClimbing.cpp : implementation file
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

#include "DialogDetailsHillClimbing.h"
#include "DialogHillClimbingPreConfig.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsHillClimbing dialog


CDialogDetailsHillClimbing::CDialogDetailsHillClimbing(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsHillClimbing::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsHillClimbing)
	//}}AFX_DATA_INIT
}


void CDialogDetailsHillClimbing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsHillClimbing)
	DDX_Text(pDX, IDC_EDIT_ThresholdRejectBestCost, m_ThresholdRejectBestCost);
	DDV_MinMaxDouble(pDX, m_ThresholdRejectBestCost, 0., 100.);

	DDX_Text(pDX, IDC_EDIT_IncrementsOfVariables, m_IncrementsOfVariables);

	DDX_Radio(pDX, IDC_RADIO_StyleIncrement, m_EstiloIncremento);

	DDX_Radio(pDX, IDC_RADIO_IncrementoPrefijado, m_IncrementoPrefijado);

	DDX_Text(pDX, IDC_EDIT_LengthInitialStepHC, m_LengthInitialStep);
	DDV_MinMaxDouble(pDX, m_LengthInitialStep, 1.e-003, 200.);

	DDX_Text(pDX, IDC_EDIT_LengthStepReduction, m_LengthStepReduction);
	DDV_MinMaxDouble(pDX, m_LengthStepReduction, 1., 100.);

	DDX_Text(pDX, IDC_EDIT_LengthFinalStep, m_LengthFinalStep);
	DDV_MinMaxDouble(pDX, m_LengthFinalStep, 1.e-003, 200.);

	DDX_Text(pDX, IDC_EDIT_NumeroMaximoIteraciones, m_NumeroMaximoIteraciones);
	DDV_MinMaxLong(pDX, m_NumeroMaximoIteraciones, 0, 10000);

	DDX_Text(pDX, IDC_EDIT_ThresholdRejectBestCostInLevel, m_ThresholdRejectBestCostInLevel);
	DDV_MinMaxDouble(pDX, m_ThresholdRejectBestCostInLevel, 0., 100.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsHillClimbing, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsHillClimbing)
	ON_BN_CLICKED(IDC_RADIO_StyleIncrement, OnRADIOEstiloIncremento)
	ON_BN_CLICKED(IDC_RADIO1, OnRadioIncrementoPrefijado)

	ON_BN_CLICKED(IDC_RADIO_IncrementoPrefijado, OnRADIOIncrementoPrefijado)
	ON_BN_CLICKED(IDC_RADIO2, OnRadioEstiloVariablesMarill)
	ON_BN_CLICKED(IDC_RADIO3, OnRadioEstiloVariableLeclerc)

	ON_BN_CLICKED(IDC_DetailsRestoreValues, OnDetallesRestaurarValores)

	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsHillClimbing message handlers

void CDialogDetailsHillClimbing::SetParams(CDataOptHC DataOptHC)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_NumeroMaximoIteraciones = DataOptHC.iMaxIterations;
	m_ThresholdRejectBestCost = DataOptHC.dThresholdRejectBestCost * 100;
	m_ThresholdRejectBestCostInLevel = DataOptHC.dThresholdRejectBestCostInLevel * 100;
	m_EstiloIncremento = DataOptHC.iStyleIncrement;
	m_IncrementoPrefijado = DataOptHC.iIncrementPrefixed;
	m_LengthInitialStep = DataOptHC.dInitialStep * 100;
	m_LengthFinalStep = DataOptHC.dFinalStep * 100;
	m_LengthStepReduction = DataOptHC.dStepReduction * 100;

	double dDato;
	CString sTexto, sPaso;

	if (m_EstiloIncremento == PARAMETRIC){
		std::vector<double> IncrementsOfVariables;
		EstimateIncrementsForHC(CReferDoc::GetDoc()->DB_Brep_Model,
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
		switch (m_IncrementoPrefijado){
		case USER:
			for(long i=0; i<(long)DataOptHC.IncrementsOfVariables.size(); i++ ) {
				dDato= DataOptHC.IncrementsOfVariables[i];
				sPaso.Format("%i", i+1);
				sTexto.Format("%f\r\n", dDato );
				m_IncrementsOfVariables += sPaso + ")  " + sTexto;
			}
			break;
		case MARILL:
			m_IncrementsOfVariables = "1)  1.0\r\n2)  0.5\r\n3)  0.1";
			break;
		case LECLERC:
			m_IncrementsOfVariables = "1)  0.125\r\n2)  0.0625\r\n3)  0.03125\r\n4)  0.015\r\n5)  0.007";
			break;
		}
	}
}


bool CDialogDetailsHillClimbing::GetParams(CDataOptHC &DataOptHC)
// Updates the general parameters that have been modified in the dialog
{
	CString sTexto, sNumero;
	int iPosicionRETURN, iEstado;
	bool bTerminar;
	float dDato;

	std::vector<double> IncrementsOfVariables;

	if (m_EstiloIncremento == PARAMETRIC)
		EstimateIncrementsForHC(CReferDoc::GetDoc()->DB_Brep_Model,
										  IncrementsOfVariables,
										  m_LengthInitialStep / 100,
										  m_LengthFinalStep / 100,
										  m_LengthStepReduction / 100);
	else{
		switch (m_IncrementoPrefijado){
		case MARILL:
			IncrementsOfVariables.push_back(1.0);
			IncrementsOfVariables.push_back(0.5);
			IncrementsOfVariables.push_back(0.1);
			break;
		case LECLERC:
			IncrementsOfVariables.push_back(0.125);
			IncrementsOfVariables.push_back(0.0625);
			IncrementsOfVariables.push_back(0.03125);
			IncrementsOfVariables.push_back(0.015);
			IncrementsOfVariables.push_back(0.007);
			break;
		case USER:
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
			break;
		}
	}

	if ((long)IncrementsOfVariables.size() == 0){
		return false;
	}

	DataOptHC.iMaxIterations =m_NumeroMaximoIteraciones;
	DataOptHC.dThresholdRejectBestCost = m_ThresholdRejectBestCost / 100;
	DataOptHC.dThresholdRejectBestCostInLevel = m_ThresholdRejectBestCostInLevel / 100;
	DataOptHC.iStyleIncrement = m_EstiloIncremento;
	DataOptHC.iIncrementPrefixed = m_IncrementoPrefijado;
	DataOptHC.dInitialStep = m_LengthInitialStep / 100;
	DataOptHC.dFinalStep = m_LengthFinalStep / 100;
	DataOptHC.dStepReduction = m_LengthStepReduction / 100;

	DataOptHC.IncrementsOfVariables= IncrementsOfVariables;
	
	return true;
}


void CDialogDetailsHillClimbing::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataOptHC Initial;
	SetParams(Initial);
	UpdateData(false);
}


BOOL CDialogDetailsHillClimbing::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CEdit *pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
	CButton *pBoton;
	UpdateData(true);

	if (m_EstiloIncremento == PARAMETRIC){
		pCajaEdicion->EnableWindow(false);
		pBoton = (CButton*) GetDlgItem(IDC_RADIO_IncrementoPrefijado); //Usuario
		pBoton->EnableWindow(false);
		pBoton = (CButton*) GetDlgItem(IDC_RADIO2); //Marill
		pBoton->EnableWindow(false);
		pBoton = (CButton*) GetDlgItem(IDC_RADIO3); //Leclerc
		pBoton->EnableWindow(false);
	}
	else{
		if (m_IncrementoPrefijado == USER)
			pCajaEdicion->EnableWindow(true);
		else
			pCajaEdicion->EnableWindow(false);

		pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthInitialStepHC);
		pCajaEdicion->EnableWindow(false);

		pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthStepReduction);
		pCajaEdicion->EnableWindow(false);

		pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthFinalStep);
		pCajaEdicion->EnableWindow(false);
	}

	return true;  // return TRUE unless you set the focus to a control
               // EXCEPTION: OCX Property Pages should return false
}


void CDialogDetailsHillClimbing::OnRadioIncrementoPrefijado()
{
	CButton *pBoton;
	CEdit *pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
	UpdateData(true);

	pBoton = (CButton*) GetDlgItem(IDC_RADIO_IncrementoPrefijado); //Usuario
	pBoton->EnableWindow(true);
	pBoton = (CButton*) GetDlgItem(IDC_RADIO2); //Marill
	pBoton->EnableWindow(true);
	pBoton = (CButton*) GetDlgItem(IDC_RADIO3); //Leclerc
	pBoton->EnableWindow(true);

	switch (m_IncrementoPrefijado){
	case USER:
		pCajaEdicion->EnableWindow(true);
		break;
	case MARILL:
		m_IncrementsOfVariables = "1)  1.0\r\n2)  0.5\r\n3)  0.1";
		UpdateData(false);
		break;
	case LECLERC:
		m_IncrementsOfVariables = "1)  0.125\r\n2)  0.0625\r\n3)  0.03125\r\n4)  0.015\r\n5)  0.007";
		UpdateData(false);
		break;
	}

	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthInitialStepHC);
	pCajaEdicion->EnableWindow(false);

	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthStepReduction);
	pCajaEdicion->EnableWindow(false);

	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthFinalStep);
	pCajaEdicion->EnableWindow(false);
}

void CDialogDetailsHillClimbing::OnRADIOEstiloIncremento() //Parametric
{
	UpdateData(false);

	std::vector<double> IncrementsOfVariables;
	EstimateIncrementsForHC(CReferDoc::GetDoc()->DB_Brep_Model,
									  IncrementsOfVariables,
									  m_LengthInitialStep / 100,
									  m_LengthFinalStep / 100,
									  m_LengthStepReduction / 100);

	m_IncrementsOfVariables = "";
	CString sTexto, sPaso;

	for(long i=0; i<(long)IncrementsOfVariables.size() ; i++ ) {
		sPaso.Format("%i", i+1);
		sTexto.Format("%f\r\n", IncrementsOfVariables[i]);
		m_IncrementsOfVariables += sPaso + ")  " + sTexto;
	};

	UpdateData(false);
	CEdit *pCajaEdicion;
	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
	pCajaEdicion->EnableWindow(false);

	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthInitialStepHC);
	pCajaEdicion->EnableWindow(true);

	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthStepReduction);
	pCajaEdicion->EnableWindow(true);

	pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_LengthFinalStep);
	pCajaEdicion->EnableWindow(true);

	CButton *pBoton;
	pBoton = (CButton*) GetDlgItem(IDC_RADIO_IncrementoPrefijado); //User
	pBoton->EnableWindow(false);
	pBoton = (CButton*) GetDlgItem(IDC_RADIO2); //Marill
	pBoton->EnableWindow(false);
	pBoton = (CButton*) GetDlgItem(IDC_RADIO3); //Leclerc
	pBoton->EnableWindow(false);
}


void CDialogDetailsHillClimbing::OnRadioEstiloVariablesMarill() 
{
	UpdateData(true);
	m_IncrementsOfVariables = "1)  1.0\r\n2)  0.5\r\n3)  0.1";
	UpdateData(false);
	CEdit *pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
	pCajaEdicion->EnableWindow(false);
}


void CDialogDetailsHillClimbing::OnRadioEstiloVariableLeclerc() 
{
	UpdateData(true);
	m_IncrementsOfVariables = "1)  0.125\r\n2)  0.0625\r\n3)  0.03125\r\n4)  0.015\r\n5)  0.007";
	UpdateData(false);
	CEdit *pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
	pCajaEdicion->EnableWindow(false);
}


void CDialogDetailsHillClimbing::OnRADIOIncrementoPrefijado() 
{
	CEdit *pCajaEdicion = (CEdit*) GetDlgItem(IDC_EDIT_IncrementsOfVariables);
	pCajaEdicion->EnableWindow(true);
}


void CDialogDetailsHillClimbing::OnDetallesRestaurarValores() 
{
	CDialogHillClimbingPreConfig Dlg;
	Dlg.SetParams(ReferApp.In.DataOptHC);

	if (Dlg.DoModal() == IDOK){
		Dlg.GetParams(ReferApp.In.DataOptHC);
	}
}
