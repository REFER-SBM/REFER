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

// DialogDetailsRegularitiesCost.cpp : implementation file
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
#include "DialogDetailsRegularitiesCost.h"
#include "WinRf_GDI_Transform.h"

#include "DialogConfigOptimizationInflation.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesCost dialog


CDialogDetailsRegularitiesCost::CDialogDetailsRegularitiesCost(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogDetailsRegularitiesCost::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogDetailsRegularitiesCost)
	//}}AFX_DATA_INIT
}


void CDialogDetailsRegularitiesCost::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDetailsRegularitiesCost)
	DDX_Control(pDX, IDC_COMBO_CoefVariable_DesviacionEstandarAngulos, m_Variable_DesviacionEstandarAngulos);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_PlanicidadCaras, m_Variable_PlanicidadCaras);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_ParalelismoLineas, m_Variable_ParalelismoLineas);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_ColinealidadLineas, m_Variable_ColinealidadLineas);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_VerticalidadLineas, m_Variable_VerticalidadLineas);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_OrtogonalidadLineas, m_Variable_OrtogonalidadLineas);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_Isometria, m_Variable_Isometria);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_CubicCorners, m_Variable_CubicCorners);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_PerpendicularidadCaras, m_Variable_PerpendicularidadCaras);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_OrtogonalidadFacialOblicua, m_Variable_OrtogonalidadFacialOblicua);
	DDX_Control(pDX, IDC_COMBO_CoefVariable_SimetriaFacialOblicua, m_Variable_SimetriaFacialOblicua);

	DDX_Control(pDX, IDC_COMBO_Coeficiente, m_Coeficiente);
	DDX_Control(pDX, IDC_COMBO_IteracionFinal, m_IteracionFinal);
	DDX_Control(pDX, IDC_COMBO_IteracionInicial, m_IteracionInicial);

	DDX_Text(pDX, IDC_EDIT_CoefFijo_DesviacionEstandarAngulos, m_CoeficienteDesviacionEstandarAngulos);
	DDV_MinMaxDouble(pDX, m_CoeficienteDesviacionEstandarAngulos, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_ParalelismoLineas, m_CoeficienteParalelismoLineas);
	DDV_MinMaxDouble(pDX, m_CoeficienteParalelismoLineas, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_ValorInicial, m_ValorInicial);
	DDV_MinMaxDouble(pDX, m_ValorInicial, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_ValorFinal, m_ValorFinal);
	DDV_MinMaxDouble(pDX, m_ValorFinal, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_Pasos, m_Pasos);
	DDX_Radio(pDX, IDC_RADIO_EstiloEvolucion, m_EstiloEvolucion);
	DDX_Text(pDX, IDC_EDIT_IncrementoProducto, m_IncrementoProducto);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_VerticalidadLineas, m_CoeficienteVerticalidadLineas);
	DDV_MinMaxDouble(pDX, m_CoeficienteVerticalidadLineas, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_PlanicidadCaras, m_CoeficientePlanicidadCaras);
	DDV_MinMaxDouble(pDX, m_CoeficientePlanicidadCaras, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_Isometria, m_CoeficienteIsometria);
	DDV_MinMaxDouble(pDX, m_CoeficienteIsometria, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_CubicCorners, m_CoeficienteCubicCorners);
	DDV_MinMaxDouble(pDX, m_CoeficienteCubicCorners, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_OrtogonalidadFacialOblicua, m_CoeficienteOrtogonalidadFacialOblicua);
	DDV_MinMaxDouble(pDX, m_CoeficienteOrtogonalidadFacialOblicua, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_SimetriaFacialOblicua, m_CoeficienteSimetriaFacialOblicua);
	DDV_MinMaxDouble(pDX, m_CoeficienteSimetriaFacialOblicua, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_OrtogonalidadLineas, m_CoeficienteOrtogonalidadLineas);
	DDV_MinMaxDouble(pDX, m_CoeficienteOrtogonalidadLineas, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_PerpendicularidadCaras, m_CoeficientePerpendicularidadCaras);
	DDV_MinMaxDouble(pDX, m_CoeficientePerpendicularidadCaras, 0., 1.);
	DDX_Text(pDX, IDC_EDIT_CoefFijo_ColinealidadLineas, m_CoeficienteColinealidadLineas);
	DDV_MinMaxDouble(pDX, m_CoeficienteColinealidadLineas, 0., 1.);

	DDX_Check(pDX, IDC_CHECK_EstiloCoef_DesviacionEstandarAngulos, m_EstiloCoef_DesviacionEstandarAngulos);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_PlanicidadCaras, m_EstiloCoef_PlanicidadCaras);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_ParalelismoLineas, m_EstiloCoef_ParalelismoLineas);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_VerticalidadLineas, m_EstiloCoef_VerticalidadLineas);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_ColinealidadLineas, m_EstiloCoef_ColinealidadLineas);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_OrtogonalidadLineas, m_EstiloCoef_OrtogonalidadLineas);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_Isometria, m_EstiloCoef_Isometria);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_CubicCorners, m_EstiloCoef_CubicCorners);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_PerpendicularidadCaras, m_EstiloCoef_PerpendicularidadCaras);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_OrtogonalidadFacialOblicua, m_EstiloCoef_OrtogonalidadFacialObliqua);
	DDX_Check(pDX, IDC_CHECK_EstiloCoef_SimetriaFacialOblicua, m_EstiloCoef_SimetriaFacialOblicua);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDetailsRegularitiesCost, CDialog)
	//{{AFX_MSG_MAP(CDialogDetailsRegularitiesCost)
	ON_BN_CLICKED(IDC_BUTTON_RestoreDefault, OnBUTTONRestoreDefault)
	ON_EN_KILLFOCUS(IDC_EDIT_ValorInicial, OnKillfocusEDITValorInicial)
	ON_EN_KILLFOCUS(IDC_EDIT_Pasos, OnKillfocusEDITPasos)
	ON_EN_KILLFOCUS(IDC_EDIT_ValorFinal, OnKillfocusEDITValorFinal)
	ON_EN_KILLFOCUS(IDC_EDIT_IncrementoProducto, OnKillfocusEDITIncrementoProducto)
	ON_BN_CLICKED(IDC_RADIO_EstiloEvolucion, OnRADIOEstiloEvolucion)
	ON_BN_CLICKED(IDC_RADIO_EstiloEvolucion1, OnRADIOEstiloEvolucion1)
	ON_BN_CLICKED(IDC_RADIO_EstiloEvolucion2, OnRADIOEstiloEvolucion2)
	ON_CBN_SELCHANGE(IDC_COMBO_Coeficiente, OnSelchangeCOMBOCoeficiente)
	ON_BN_CLICKED(IDC_BUTTON_UpdateGraphic, OnUpdateGraphic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesCost message handlers

void CDialogDetailsRegularitiesCost::SetParams(CDataInflation In,
												   CDialogConfigOptimizationInflation *pConfInf)
// Produces local versions of those general parameters that can be modified by this dialog
{
	int i;
	TRegularity Regularity;

	pConfiguracionInflado = pConfInf;

	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_MIN_STD_DEV_ANGLES ];
	m_CoeficienteDesviacionEstandarAngulos = Regularity.dCoeficienteFijo;
	m_Variable_DesviacionEstandarAngulos_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_DesviacionEstandarAngulos = Regularity.bEsCoeficienteVariable;

	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_FACE_PLANARITY ];
	m_CoeficientePlanicidadCaras = Regularity.dCoeficienteFijo;
	m_Variable_PlanicidadCaras_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_PlanicidadCaras = Regularity.bEsCoeficienteVariable;


	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_LINE_PARALLELISM ];
	m_CoeficienteParalelismoLineas = Regularity.dCoeficienteFijo;
	m_Variable_ParalelismoLineas_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_ParalelismoLineas = Regularity.bEsCoeficienteVariable;

	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_LINE_COLLINEARITY ];
	m_CoeficienteColinealidadLineas = Regularity.dCoeficienteFijo;
	m_Variable_ColinealidadLineas_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_ColinealidadLineas = Regularity.bEsCoeficienteVariable;

	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_LINE_VERTICALITY ];
	m_CoeficienteVerticalidadLineas = Regularity.dCoeficienteFijo;
	m_Variable_VerticalidadLineas_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_VerticalidadLineas = Regularity.bEsCoeficienteVariable;


	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_LINE_ORTHOGONALITY ];
	m_CoeficienteOrtogonalidadLineas = Regularity.dCoeficienteFijo;
	m_Variable_OrtogonalidadLineas_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_OrtogonalidadLineas = Regularity.bEsCoeficienteVariable;


	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_ISOMETRY ];
	m_CoeficienteIsometria = Regularity.dCoeficienteFijo;
	m_Variable_Isometria_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_Isometria = Regularity.bEsCoeficienteVariable;


	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_CUBIC_CORNERS ];
	m_CoeficienteCubicCorners = Regularity.dCoeficienteFijo;
	m_Variable_CubicCorners_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_CubicCorners = Regularity.bEsCoeficienteVariable;

	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_FACE_PERPENDICULARITY ];
	m_CoeficientePerpendicularidadCaras = Regularity.dCoeficienteFijo;
	m_Variable_PerpendicularidadCaras_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_PerpendicularidadCaras = Regularity.bEsCoeficienteVariable;


	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY ];
	m_CoeficienteOrtogonalidadFacialOblicua = Regularity.dCoeficienteFijo;
	m_Variable_OrtogonalidadFacialOblicua_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_OrtogonalidadFacialObliqua = Regularity.bEsCoeficienteVariable;

	Regularity = In.Regularities.ListOfRegularities[ REGULARITY_SKEWED_FACIAL_SYMMETRY ];
	m_CoeficienteSimetriaFacialOblicua = Regularity.dCoeficienteFijo;
	m_Variable_SimetriaFacialOblicua_Aux = Regularity.iCoeficienteVariable;
	m_EstiloCoef_SimetriaFacialOblicua = Regularity.bEsCoeficienteVariable;

	// Ajusta los coeficientes
	for( i=COEFFICIENT_A; i<(COEFFICIENT_LAST+1); i +=2) 
		m_ListaCoeficientes[i] = In.Regularities.ListaCoeficientes[i];

	TParametrosCoeficienteRegularidad Coeficiente;
	Coeficiente = In.Regularities.ListaCoeficientes[COEFFICIENT_A];
	m_Coeficiente_Aux = COEFFICIENT_A;
	m_IteracionInicial_Aux = Coeficiente.lIteracionInicialMenu;
	m_IteracionFinal_Aux = Coeficiente.lIteracionFinalMenu;
	m_Pasos = Coeficiente.lPasos;
	m_EstiloEvolucion = Coeficiente.iEstiloEvolucion;
	m_ValorInicial = Coeficiente.dValorInicial;
	m_ValorFinal = Coeficiente.dValorFinal;
	m_IncrementoProducto = Coeficiente.dIncrementoProducto;
}


void CDialogDetailsRegularitiesCost::GetParams(CDataInflation &In)
// Updates the general parameters that have been modified in the dialog
{
	TRegularity Regularity;

	Regularity.dCoeficienteFijo = m_CoeficienteDesviacionEstandarAngulos;
	Regularity.iCoeficienteVariable = m_Variable_DesviacionEstandarAngulos_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_DesviacionEstandarAngulos !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_MIN_STD_DEV_ANGLES ] = Regularity;

	Regularity.dCoeficienteFijo = m_CoeficientePlanicidadCaras;
	Regularity.iCoeficienteVariable = m_Variable_PlanicidadCaras_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_PlanicidadCaras !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_FACE_PLANARITY ] = Regularity;


	Regularity.dCoeficienteFijo = m_CoeficienteParalelismoLineas;
	Regularity.iCoeficienteVariable = m_Variable_ParalelismoLineas_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_ParalelismoLineas !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_LINE_PARALLELISM ] = Regularity;

	Regularity.dCoeficienteFijo = m_CoeficienteColinealidadLineas;
	Regularity.iCoeficienteVariable = m_Variable_ColinealidadLineas_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_ColinealidadLineas !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_LINE_COLLINEARITY ] = Regularity;

	Regularity.dCoeficienteFijo = m_CoeficienteVerticalidadLineas;
	Regularity.iCoeficienteVariable = m_Variable_VerticalidadLineas_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_VerticalidadLineas !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_LINE_VERTICALITY ] = Regularity;


	Regularity.dCoeficienteFijo = m_CoeficienteOrtogonalidadLineas;
	Regularity.iCoeficienteVariable = m_Variable_OrtogonalidadLineas_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_OrtogonalidadLineas !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_LINE_ORTHOGONALITY ] = Regularity;


	Regularity.dCoeficienteFijo = m_CoeficienteIsometria;
	Regularity.iCoeficienteVariable = m_Variable_Isometria_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_Isometria !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_ISOMETRY ] = Regularity;


	Regularity.dCoeficienteFijo = m_CoeficienteCubicCorners;
	Regularity.iCoeficienteVariable = m_Variable_CubicCorners_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_CubicCorners !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_CUBIC_CORNERS ] = Regularity;

	Regularity.dCoeficienteFijo = m_CoeficientePerpendicularidadCaras;
	Regularity.iCoeficienteVariable = m_Variable_PerpendicularidadCaras_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_PerpendicularidadCaras !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_FACE_PERPENDICULARITY ] = Regularity;


	Regularity.dCoeficienteFijo = m_CoeficienteOrtogonalidadFacialOblicua;
	Regularity.iCoeficienteVariable = m_Variable_OrtogonalidadFacialOblicua_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_OrtogonalidadFacialObliqua !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY ] = Regularity;

	Regularity.dCoeficienteFijo = m_CoeficienteSimetriaFacialOblicua;
	Regularity.iCoeficienteVariable = m_Variable_SimetriaFacialOblicua_Aux;
	Regularity.bEsCoeficienteVariable = m_EstiloCoef_SimetriaFacialOblicua !=0;
	In.Regularities.ListOfRegularities[ REGULARITY_SKEWED_FACIAL_SYMMETRY ] = Regularity;

	for(long i=COEFFICIENT_A; i<(COEFFICIENT_LAST+1); i +=2) 
		In.Regularities.ListaCoeficientes[i] = m_ListaCoeficientes[i];

	TParametrosCoeficienteRegularidad Coeficiente;
	Coeficiente.lIteracionInicialMenu = m_IteracionInicial_Aux;
	Coeficiente.lIteracionFinalMenu = m_IteracionFinal_Aux;
	Coeficiente.lPasos = m_Pasos;
	Coeficiente.iEstiloEvolucion = m_EstiloEvolucion;
	Coeficiente.dValorInicial = m_ValorInicial;
	Coeficiente.dValorFinal = m_ValorFinal;
	Coeficiente.dIncrementoProducto = m_IncrementoProducto;
	In.Regularities.ListaCoeficientes[ m_Coeficiente_Aux * 2 ] = Coeficiente;
}


void CDialogDetailsRegularitiesCost::ActualizaComboBox()
{
	m_Variable_DesviacionEstandarAngulos.SetCurSel( m_Variable_DesviacionEstandarAngulos_Aux );
	m_Variable_ParalelismoLineas.SetCurSel( m_Variable_ParalelismoLineas_Aux );
	m_Variable_VerticalidadLineas.SetCurSel( m_Variable_VerticalidadLineas_Aux );
	m_Variable_PlanicidadCaras.SetCurSel( m_Variable_PlanicidadCaras_Aux );
	m_Variable_Isometria.SetCurSel( m_Variable_Isometria_Aux );
	m_Variable_CubicCorners.SetCurSel( m_Variable_CubicCorners_Aux );
	m_Variable_OrtogonalidadFacialOblicua.SetCurSel( m_Variable_OrtogonalidadFacialOblicua_Aux );
	m_Variable_OrtogonalidadLineas.SetCurSel( m_Variable_OrtogonalidadLineas_Aux );
	m_Variable_PerpendicularidadCaras.SetCurSel( m_Variable_PerpendicularidadCaras_Aux );
	m_Variable_ColinealidadLineas.SetCurSel( m_Variable_ColinealidadLineas_Aux );
	m_Variable_SimetriaFacialOblicua.SetCurSel( m_Variable_SimetriaFacialOblicua_Aux );

	m_Coeficiente.SetCurSel( m_Coeficiente_Aux );
	m_IteracionFinal.SetCurSel( m_IteracionFinal_Aux );
	m_IteracionInicial.SetCurSel( m_IteracionInicial_Aux );
}

BOOL CDialogDetailsRegularitiesCost::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ActualizaComboBox();
	UpdateData(false);
	
	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

void CDialogDetailsRegularitiesCost::OnBUTTONRestoreDefault() 
{
	if ( AfxMessageBox( IDSL_BY_DEFAULT, MB_YESNO ) == IDNO )
		return;
	CDataInflation initial;
	SetParams(initial, pConfiguracionInflado );
	ActualizaComboBox();
	UpdateData( false );
	OnUpdateGraphic();
}

void CDialogDetailsRegularitiesCost::OnKillfocusEDITValorInicial() 
{
	OnUpdateGraphic();	
}

void CDialogDetailsRegularitiesCost::OnKillfocusEDITPasos() 
{
	OnUpdateGraphic();	
}

void CDialogDetailsRegularitiesCost::OnKillfocusEDITValorFinal() 
{
	OnUpdateGraphic();	
}

void CDialogDetailsRegularitiesCost::OnKillfocusEDITIncrementoProducto() 
{
	OnUpdateGraphic();	
}

void CDialogDetailsRegularitiesCost::OnRADIOEstiloEvolucion() 
{
	OnUpdateGraphic();	
}

void CDialogDetailsRegularitiesCost::OnRADIOEstiloEvolucion1() 
{
	OnUpdateGraphic();	
}

void CDialogDetailsRegularitiesCost::OnRADIOEstiloEvolucion2() 
{
	OnUpdateGraphic();	
}

void CDialogDetailsRegularitiesCost::OnSelchangeCOMBOCoeficiente() 
{
	TParametrosCoeficienteRegularidad Coeficiente;

	// Modifica el coeficiente anterior
	UpdateData(true);
	Coeficiente.lIteracionInicialMenu = m_IteracionInicial.GetCurSel();
	Coeficiente.lIteracionFinalMenu = m_IteracionFinal.GetCurSel();
	Coeficiente.lPasos = m_Pasos;
	Coeficiente.iEstiloEvolucion = m_EstiloEvolucion;
	Coeficiente.dValorInicial = m_ValorInicial;
	Coeficiente.dValorFinal = m_ValorFinal;
	Coeficiente.dIncrementoProducto = m_IncrementoProducto;
	m_ListaCoeficientes[ m_Coeficiente_Aux *2 ] = Coeficiente;

	// Muestra los parametros del coeficiente actual
	m_Coeficiente_Aux = m_Coeficiente.GetCurSel();
	Coeficiente = m_ListaCoeficientes[ m_Coeficiente_Aux * 2 ];
	m_IteracionInicial.SetCurSel( Coeficiente.lIteracionInicialMenu );
	m_IteracionFinal.SetCurSel( Coeficiente.lIteracionFinalMenu );
	m_Pasos = Coeficiente.lPasos;
	m_EstiloEvolucion = Coeficiente.iEstiloEvolucion;
	m_ValorInicial = Coeficiente.dValorInicial;
	m_ValorFinal = Coeficiente.dValorFinal;
	m_IncrementoProducto = Coeficiente.dIncrementoProducto;
	UpdateData(false);

	OnUpdateGraphic();
}


void CDialogDetailsRegularitiesCost::OnUpdateGraphic() 
// Uses GDI to draw a diagram of current regulatities' coefficient
// inside the control IDC_STATIC_VENTANA_DIBUJO
{
	cGDI_Transform Transf;
	CRect rect;
	CPoint Punto;
	CString sTexto;
	int iCoeficiente, i;
	TParametrosCoeficienteRegularidad Coeficiente;

	// Primero modifica el coeficiente actual
	UpdateData(true);
	Coeficiente.lIteracionInicialMenu = m_IteracionInicial.GetCurSel();
	Coeficiente.lIteracionFinalMenu = m_IteracionFinal.GetCurSel();
	Coeficiente.lPasos = m_Pasos;
	Coeficiente.iEstiloEvolucion = m_EstiloEvolucion;
	Coeficiente.dValorInicial = m_ValorInicial;
	Coeficiente.dValorFinal = m_ValorFinal;
	Coeficiente.dIncrementoProducto = m_IncrementoProducto;
	m_ListaCoeficientes[ m_Coeficiente_Aux *2 ] = Coeficiente;

	// Ajusta cual es el numero maximo de iteraciones, mirando las otras paginas de propiedades
	int iMaxIterations= 0;
	////////switch (pConfiguracionInflado->m_Parametros.iOptAlgorithm) {
	////////	case NO_OPTIMIZATION :
	////////		iMaxIterations = 0;
	////////		break;

	////////	case HILL_CLIMBING : {
	////////		int iNumeroIncrementosVariables;
	////////		iNumeroIncrementosVariables= (long)pConfiguracionInflado->m_DataOptHC.IncrementsOfVariables.size();
	////////		// Fijarse que 'iNumeroIncrementosVariables' siempre vale al menos 1.
	////////		if (iNumeroIncrementosVariables == 0)
	////////			iNumeroIncrementosVariables = 1;

	////////		iMaxIterations = pConfiguracionInflado->m_DataOptHC.iMaxIterations *
	////////											iNumeroIncrementosVariables;
	////////		break;
	////////		};
	////////	case SIMULATED_ANNEALING :
	////////		iMaxIterations = pConfiguracionInflado->m_DataOptSA.iMaxLevels;
	////////		break;

	////////	case SIMULATED_ANNEALING_MULTI :
	////////		iMaxIterations = pConfiguracionInflado->m_DataOptSAM.iMaxLevels;
	////////		break;
	////////};

	// Prepare the drawing area
	CStatic* pStatic = (CStatic*) GetDlgItem(IDC_STATIC_VENTANA_DIBUJO);
	pStatic->GetClientRect( &rect );
	CClientDC dc(pStatic);
	dc.FillSolidRect( rect, RGB( 255, 255, 255) );

	// Prepare Pens and Fonts to draw the graphic
	CPen Pluma;
	COLORREF Color,
			 ColorEjes = RGB( 200, 200, 200 );
	CFont Fuente, *pFuenteAnterior;
	Fuente.CreatePointFont( 7*10, "Arial", &dc);  //Initializes a CFont with the specified height, measured in tenths of a point, and typeface
	pFuenteAnterior = (CFont *) dc.SelectObject(&Fuente);
	dc.SetBkMode(TRANSPARENT);

	// Prepare the Window-Viewport transformation
	double dDatoMayor = 1;
	double dDatoMenor = 0;
	double dRangeY = dDatoMayor - dDatoMenor;

	int iBorderLeft = 8;
	int iBorderRight = 10;
	int iBorderUp = 5;
	int iBorderDown = 12;

	Transf.GDI_SetViewport(iBorderLeft, iBorderUp,
						   rect.right-iBorderRight, rect.bottom-iBorderDown);

	Transf.GDI_SetWorld( 0,(double)dDatoMenor,(double)iMaxIterations,(double)dDatoMayor);

	// Draw the axes
	dc.SetTextAlign(TA_TOP | TA_RIGHT);
	Pluma.CreatePen(PS_SOLID, 1, RGB(50, 50, 50) );
	dc.SelectObject(&Pluma);
	Punto = Transf.GDI_WorldToViewport(0, 1); // Eje X1
	dc.MoveTo(Punto.x, Punto.y);
	dc.TextOut(Punto.x, Punto.y-5, "1");
	Punto = Transf.GDI_WorldToViewport(0, 0); // Eje Y
	dc.LineTo(Punto.x, Punto.y );
	dc.TextOut(Punto.x, Punto.y-7, "0");
	dc.LineTo(Punto.x, Punto.y );
	dc.TextOut(Punto.x+5, Punto.y+2, "0");
	Punto = Transf.GDI_WorldToViewport(iMaxIterations, 0); // Eje X2
	dc.LineTo(Punto.x, Punto.y);
	sTexto.Format("%i", iMaxIterations);
	dc.TextOut(Punto.x+5, Punto.y+2, sTexto);
	Punto = Transf.GDI_WorldToViewport(iMaxIterations, 1); // Eje X2
	dc.LineTo(Punto.x, Punto.y);
	Pluma.DeleteObject();

	//Draw all active coefficients
	dc.SetTextAlign(TA_BOTTOM | TA_RIGHT);
	for ( iCoeficiente=0; iCoeficiente < (COEFFICIENT_LAST+1) ; iCoeficiente+= 2 ) {
		Coeficiente = m_ListaCoeficientes[ iCoeficiente ];
		if (Coeficiente.iEstiloEvolucion != EVOLUTION_STYLE_INACTIVE) {

			// Dibuja el primer punto del coeficiente elegido
			PrepareCoefficientsRegularities( m_ListaCoeficientes, iMaxIterations );
			UpdateCoefficientsRegularities( m_ListaCoeficientes, 0 );
			Coeficiente = m_ListaCoeficientes[ iCoeficiente ];

			if (Coeficiente.iEstiloEvolucion == EVOLUTION_STYLE_NOEVOLUTION)
				Punto = Transf.GDI_WorldToViewport( Coeficiente.lIteracionInicial, Coeficiente.dValorActual );
			else
				Punto = Transf.GDI_WorldToViewport( 0,	Coeficiente.dValorActual );
			Color = RGB(0, 255 - iCoeficiente*255/COEFFICIENT_LAST, 0);
			Pluma.CreatePen( PS_SOLID, 1, Color );
			dc.SelectObject( &Pluma );
			dc.SetTextColor( Color );
			sTexto = (char) __toascii(65 + iCoeficiente/2);
			dc.MoveTo( Punto.x, Punto.y );

			if (Coeficiente.iEstiloEvolucion == EVOLUTION_STYLE_NOEVOLUTION) {
				Punto = Transf.GDI_WorldToViewport( Coeficiente.lIteracionFinal, Coeficiente.dValorActual );
				dc.LineTo( Punto.x, Punto.y );
			}
			else {
				// Dibuja la evolucion del coeficiente elegido
				for( i=1; i < iMaxIterations; i++ ){
					UpdateCoefficientsRegularities( m_ListaCoeficientes, i );
					Coeficiente = m_ListaCoeficientes[ iCoeficiente ];
					Punto = Transf.GDI_WorldToViewport( i, Coeficiente.dValorActual );

					// Comprueba los limites
					if (Punto.x < iBorderLeft)  Punto.x = iBorderLeft;
					if (Punto.x > rect.right-iBorderRight)  Punto.x = rect.right-iBorderRight;
					if (Punto.y < iBorderUp)  Punto.y = iBorderUp;
					if (Punto.y > rect.bottom-iBorderDown)  Punto.y = rect.bottom-iBorderDown;

					dc.LineTo( Punto.x, Punto.y );
					if ( i == iMaxIterations/(iCoeficiente+2) )
						dc.TextOut( Punto.x, Punto.y, sTexto );
				};
			};
			dc.TextOut( Punto.x+9, Punto.y+7, sTexto );
			Pluma.DeleteObject();
		};
	};
}


void CDialogDetailsRegularitiesCost::OnPaint() 
{
	CPaintDC dc(this); // Removes WM_PAINT message from the queue

	// Draw the diagram of the current regulatities' coefficient
	// inside the control IDC_STATIC_VENTANA_DIBUJO
	OnUpdateGraphic();
}


void CDialogDetailsRegularitiesCost::OnOK() 
{
	UpdateData(true);
	m_Variable_DesviacionEstandarAngulos_Aux = m_Variable_DesviacionEstandarAngulos.GetCurSel();
	m_Variable_ParalelismoLineas_Aux = m_Variable_ParalelismoLineas.GetCurSel();
	m_Variable_VerticalidadLineas_Aux = m_Variable_VerticalidadLineas.GetCurSel();
	m_Variable_PlanicidadCaras_Aux = m_Variable_PlanicidadCaras.GetCurSel();
	m_Variable_Isometria_Aux = m_Variable_Isometria.GetCurSel();
	m_Variable_CubicCorners_Aux = m_Variable_CubicCorners.GetCurSel();
	m_Variable_OrtogonalidadFacialOblicua_Aux = m_Variable_OrtogonalidadFacialOblicua.GetCurSel();
	m_Variable_SimetriaFacialOblicua_Aux = m_Variable_SimetriaFacialOblicua.GetCurSel();
	m_Variable_OrtogonalidadLineas_Aux = m_Variable_OrtogonalidadLineas.GetCurSel();
	m_Variable_PerpendicularidadCaras_Aux = m_Variable_PerpendicularidadCaras.GetCurSel();
	m_Variable_ColinealidadLineas_Aux = m_Variable_ColinealidadLineas.GetCurSel();

	m_Coeficiente_Aux = m_Coeficiente.GetCurSel();
	m_IteracionFinal_Aux = m_IteracionFinal.GetCurSel();
	m_IteracionInicial_Aux = m_IteracionInicial.GetCurSel();

	CDialog::OnOK();
}
