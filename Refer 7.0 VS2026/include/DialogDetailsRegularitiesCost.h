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

#if !defined(AFX_DIALOGDETAILSREGULARITIESCOST_INCLUDED)
#define AFX_DIALOGDETAILSREGULARITIESCOST_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDialogConfigOptimizationInflation;

// DialogDetailsRegularitiesCost.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsRegularitiesCost dialog

class CDialogDetailsRegularitiesCost : public CDialog
{
// Construction
public:
	void SetParams(CDataInflation In, CDialogConfigOptimizationInflation *pConfInf);
	void GetParams(CDataInflation &In);
	void ActualizaComboBox();
	CDialogDetailsRegularitiesCost(CWnd* pParent = NULL);   // standard constructor
	TParametrosCoeficienteRegularidad m_ListaCoeficientes[COEFFICIENT_LAST+1];
	CDialogConfigOptimizationInflation *pConfiguracionInflado;
	int m_Variable_DesviacionEstandarAngulos_Aux,
		 m_Variable_ParalelismoLineas_Aux,
		 m_Variable_VerticalidadLineas_Aux,
		 m_Coeficiente_Aux,
		 m_IteracionFinal_Aux,
		 m_IteracionInicial_Aux,
		 m_Variable_PlanicidadCaras_Aux,
		 m_Variable_Isometria_Aux,
		 m_Variable_CubicCorners_Aux,
		 m_Variable_OrtogonalidadFacialOblicua_Aux,
		 m_Variable_SimetriaFacialOblicua_Aux,
		 m_Variable_OrtogonalidadLineas_Aux,
		 m_Variable_PerpendicularidadCaras_Aux,
		 m_Variable_ColinealidadLineas_Aux;


// Dialog Data
	//{{AFX_DATA(CDialogDetailsRegularitiesCost)
	enum { IDD = IDD_DIALOG_DetailsRegularitiesCost };
	CComboBox	m_Variable_DesviacionEstandarAngulos;
	CComboBox	m_Variable_PlanicidadCaras;
	CComboBox	m_Variable_ParalelismoLineas;
	CComboBox	m_Variable_ColinealidadLineas;
	CComboBox	m_Variable_VerticalidadLineas;
	CComboBox	m_Variable_OrtogonalidadLineas;
	CComboBox	m_Variable_Isometria;
	CComboBox	m_Variable_CubicCorners;
	CComboBox	m_Variable_PerpendicularidadCaras;
	CComboBox	m_Variable_OrtogonalidadFacialOblicua;
	CComboBox	m_Variable_SimetriaFacialOblicua;

	CComboBox	m_Coeficiente;
	CComboBox	m_IteracionFinal;
	CComboBox	m_IteracionInicial;

	double	m_ValorInicial;
	double	m_ValorFinal;
	long	m_Pasos;
	int		m_EstiloEvolucion;
	double	m_IncrementoProducto;

	double	m_CoeficienteDesviacionEstandarAngulos;
	BOOL	m_EstiloCoef_DesviacionEstandarAngulos;
	double	m_CoeficientePlanicidadCaras;
	BOOL	m_EstiloCoef_PlanicidadCaras;
	double	m_CoeficienteParalelismoLineas;
	BOOL	m_EstiloCoef_ParalelismoLineas;
	double	m_CoeficienteColinealidadLineas;
	BOOL	m_EstiloCoef_ColinealidadLineas;
	double	m_CoeficienteVerticalidadLineas;
	BOOL	m_EstiloCoef_VerticalidadLineas;
	double	m_CoeficienteOrtogonalidadLineas;
	BOOL	m_EstiloCoef_OrtogonalidadLineas;
	double	m_CoeficienteIsometria;
	BOOL	m_EstiloCoef_Isometria;
	double	m_CoeficienteCubicCorners;
	BOOL	m_EstiloCoef_CubicCorners;
	double	m_CoeficientePerpendicularidadCaras;
	BOOL	m_EstiloCoef_PerpendicularidadCaras;
	double	m_CoeficienteOrtogonalidadFacialOblicua;
	BOOL	m_EstiloCoef_OrtogonalidadFacialObliqua;
	double	m_CoeficienteSimetriaFacialOblicua;
	BOOL	m_EstiloCoef_SimetriaFacialOblicua;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsRegularitiesCost)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsRegularitiesCost)
	virtual BOOL OnInitDialog();
	afx_msg void OnBUTTONRestoreDefault();
	afx_msg void OnKillfocusEDITValorInicial();
	afx_msg void OnKillfocusEDITPasos();
	afx_msg void OnKillfocusEDITValorFinal();
	afx_msg void OnKillfocusEDITIncrementoProducto();
	afx_msg void OnRADIOEstiloEvolucion();
	afx_msg void OnRADIOEstiloEvolucion1();
	afx_msg void OnRADIOEstiloEvolucion2();
	afx_msg void OnSelchangeCOMBOCoeficiente();
	afx_msg void OnUpdateGraphic();
	afx_msg void OnPaint();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSREGULARITIESCOST_INCLUDED)
