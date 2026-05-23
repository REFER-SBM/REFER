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

#if !defined(AFX_DIALOGDETALLESDETECTARCARAS_H__4336BF11_7581_11D3_9BD9_00508B0BD793__INCLUDED_)
#define AFX_DIALOGDETALLESDETECTARCARAS_H__4336BF11_7581_11D3_9BD9_00508B0BD793__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsFaces.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsFaces dialog

class CDialogDetailsFaces : public CDialog
{
// Construction
public:
	void SetParams(CDataCues Cu);
	void GetParams(CDataCues &Cu);
	CDialogDetailsFaces(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsFaces)
	enum { IDD = IDD_DIALOG_DetailsDetectFaces };
	double	m_EdgeCollinearityAngleThreshold;
	int		m_DetectFacesApproach;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsFaces)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsFaces)
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETALLESDETECTARCARAS_H__4336BF11_7581_11D3_9BD9_00508B0BD793__INCLUDED_)
