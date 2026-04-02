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

#if !defined(AFX_DIALOGCONFIGDETECTCUESFEATURES_INCLUDED_)
#define AFX_DIALOGCONFIGDETECTCUESFEATURES_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigDetectCuesFeatures.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDetectCuesFeatures dialog

class CDialogConfigDetectCuesFeatures : public CDialog
{
// Construction
public:
	CDialogConfigDetectCuesFeatures(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataRefer GeneralParams);
	void GetParams(CDataRefer &GeneralParams);

// Dialog Data
	//{{AFX_DATA(CDialogConfigDetectCuesFeatures)
	enum { IDD = IDD_DIALOG_ConfigDetectCuesFeatures };
	BOOL	m_DetectCues;
	BOOL	m_DetectFeatures;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigDetectCuesFeatures)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigDetectCuesFeatures)
	virtual BOOL OnInitDialog();

	afx_msg void OnConfigCues();
	afx_msg void OnConfigFeatures();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGDETECTCUESFEATURES_INCLUDED_)
