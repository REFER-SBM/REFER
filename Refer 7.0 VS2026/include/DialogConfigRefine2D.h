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

#if !defined(AFX_DIALOGCONFIGREFINE2D_INCLUDED_)
#define AFX_DIALOGCONFIGREFINE2D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigRefine2D.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigRefine2D dialog

class CDialogConfigRefine2D : public CDialog
{
// Construction
public:
	CDataOptHC m_DataOptHC_2D;

	CDialogConfigRefine2D(CWnd* pParent = NULL);   // standard constructor
	void SetParams(const CDataRefer &GeneralParams,
				   const CDataRefine2D R2D);
	void GetParams(CDataRefer &GeneralParams,
				   CDataRefine2D &R2D);

// Dialog Data
	//{{AFX_DATA(CDialogConfigRefine2D)
	enum { IDD = IDD_DIALOG_ConfigRefine2D };
	BOOL m_Refine_On_Line;
	BOOL m_Refine_Off_Line;

	BOOL m_MergeVertices;
	long m_MergeVerticesApproach;
	double m_MergeMaxDistVertices;
	double m_MergeMaxVerticesRotation;
	double m_MergeRelaxation;

	BOOL m_Merge_T_Vertices;
	BOOL m_Split_T_Vertices;
	double m_ToleranceSplitTVertices;
	BOOL m_IgnoreShortEdges;
	double m_ToleranceIgnoreShortEdges;
	BOOL m_MergeCollinearEdges;
	double m_ToleranceMergeCollinearEdges;

	BOOL m_SplitNon2ManifoldVertices;
	BOOL m_SplitNon2ManifoldEdges;

	BOOL m_Optimize2D;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigRefine2D)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigRefine2D)
	virtual BOOL OnInitDialog();
	afx_msg void OnBUTTONRestoreDefault();

	afx_msg void OnDetailsMergeVertices();
	afx_msg void OnDetailsHillClimbing2D();
	afx_msg void OnDetailsRegularitiesReconstruction2D();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGREFINE2D_INCLUDED_)
