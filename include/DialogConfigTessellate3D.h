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

#if !defined(AFX_DIALOGCONFIGTESSELLATE3D_INCLUDED_)
#define AFX_DIALOGCONFIGTESSELLATE3D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigTessellate3D.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigTessellate3D dialog

class CDialogConfigTessellate3D : public CDialog
{
// Construction
public:
	CDialogConfigTessellate3D(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataTessellate3D T3D);
	void GetParams(CDataTessellate3D &T3D);

// Dialog Data
	//{{AFX_DATA(CDialogConfigTessellate3D)
	enum { IDD = IDD_DIALOG_ConfigTessellate3D };
	BOOL	m_Tessellate3D;
	BOOL	m_FacesOutward;
	BOOL	m_TriangulateFaces;
	BOOL	m_NormalsToFaces;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigTessellate3D)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigTessellate3D)
	virtual BOOL OnInitDialog();
	afx_msg void OnBUTTONRestoreDefault();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGTESSELLATE3D_INCLUDED_)
