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

#if !defined(AFX_DIALOGCONFIGCUES_INCLUDED_)
#define AFX_DIALOGCONFIGCUES_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigCues.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigCues dialog

class CDialogConfigCues : public CDialog
{
// Construction
public:
	CDialogConfigCues(CWnd* pParent = NULL);
	void SetParams(CDataRefer GeneralParamss);
	void GetParams(CDataRefer &GeneralParams);

// Dialog Data
	//{{AFX_DATA(CDialogConfigCues)
	enum { IDD = IDD_DIALOG_ConfigCues };
	BOOL	m_DetectCues;

	BOOL	m_DetectValences;
	BOOL	m_DetectParallelEdges;
	BOOL	m_DetectVanishingPoints;

	BOOL	m_DetectGraphs;
	BOOL	m_DetectPerimeter;
	BOOL	m_DetectLabels;
	BOOL	m_DetectMainAxes;
	BOOL	m_DetectFaces;
	BOOL	m_DetectTypology;
	BOOL	m_DetectHiddenEdges;
	BOOL	m_DetectSymmetry;

	BOOL	m_DetectLevels;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigCues)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigCues)
	virtual BOOL OnInitDialog();

	afx_msg void OnDetailsValences();
	afx_msg void OnDetailsParallelEdges();
	afx_msg void OnDetailsVanishingPoints();

	afx_msg void OnDetailsSubGraphs();
	afx_msg void OnDetailsPerimeter();
	afx_msg void OnDetailsLabels();
	afx_msg void OnDetailsMainAxes();
	afx_msg void OnDetailsFaces();
	afx_msg void OnDetailsTypology();
	afx_msg void OnDetailsHiddenEdges();
	afx_msg void OnDetailsSymmetry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGCUES_INCLUDED_)
