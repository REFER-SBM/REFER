//+-----------------------------------------------------------------------------+
//| Copyright (C) 2009 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#if !defined(AFX_DIALOGDETALLESSELECCIONAUTOMATICA_H__9D50B121_2A14_11D3_8C58_00A0C91CC608__INCLUDED_)
#define AFX_DIALOGDETALLESSELECCIONAUTOMATICA_H__9D50B121_2A14_11D3_8C58_00A0C91CC608__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetallesSeleccionAutomatica.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetallesSeleccionAutomatica dialog

class CDialogDetallesSeleccionAutomatica : public CDialog
{
// Construction
public:
	void DevuelveParametros(TParametrosOptimizacion *pParametros);
	void ConstruyePagina(TParametrosOptimizacion *pParametros);
	CDialogDetallesSeleccionAutomatica(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetallesSeleccionAutomatica)
	enum { IDD = IDD_DIALOG_DetallesSeleccionAutomatica };
	double	m_DesviacionParalelismo;
	double	m_FrecuenciaMinima;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetallesSeleccionAutomatica)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetallesSeleccionAutomatica)
	afx_msg void OnBUTTONRestaurarPorDefecto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETALLESSELECCIONAUTOMATICA_H__9D50B121_2A14_11D3_8C58_00A0C91CC608__INCLUDED_)
