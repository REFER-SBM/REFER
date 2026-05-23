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

#if !defined(DIALOGOPTIMIZATIONISOLINES_INCLUDED)
	#define DIALOGOPTIMIZATIONISOLINES_INCLUDED

	#if _MSC_VER >= 1000
		#pragma once
	#endif // _MSC_VER >= 1000
	// DialogOptimizationIsolines.h : header file
	//

	/////////////////////////////////////////////////////////////////////////////
	// CDialogOptimizationIsolines dialog

	class CDialogOptimizationIsolines : public CDialog
	{
		// Construction
		public:
			int m_iInterpolationDegree;

			void GetParams(CParamsIsolines* pParamsIsolines);
			void SetParams(CParamsIsolines* pParamsIsolines);
			CDialogOptimizationIsolines(CWnd* pParent = NULL);   // standard constructor

		// Dialog Data
			//{{AFX_DATA(CDialogOptimizationIsolines)
			enum { IDD = IDD_DIALOG_OptimizationIsolines };
			CSliderCtrl m_InterpolationDegree;
			int		m_X;
			int		m_Y;
			double	m_INCX;
			double	m_INCY;
			double	m_X0;
			double	m_XF;
			double	m_Y0;
			double	m_YF;
			int		m_TG;
			double	m_INCZ;
			int		m_OtherVariables;
			double	m_Z;
			int		m_V;
			int		m_Color;
			//}}AFX_DATA

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CDialogOptimizationIsolines)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			//}}AFX_VIRTUAL

		// Implementation
		protected:
			// Generated message map functions
			//{{AFX_MSG(CDialogOptimizationIsolines)
			virtual BOOL OnInitDialog();
			afx_msg void OnRadioUnaVariable();
			afx_msg void OnRadioDosVariables();
			afx_msg void OnZActuales();
			afx_msg void OnModificarVariable();
			afx_msg void OnZCero();
			virtual void OnOK();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(DIALOGOPTIMIZATIONISOLINES_INCLUDED)
