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

#if !defined(AFX_DIALOGDETAILSMERGEVERTICES_INCLUDED)
#define AFX_DIALOGDETAILSMERGEVERTICES_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogDetailsMergeVertices.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsMergeVertices dialog

class CDialogDetailsMergeVertices : public CDialog
{
// Construction
public:
	void SetParams(CDataRefine2D R2D);
	void GetParams(CDataRefine2D &R2D);
	CDialogDetailsMergeVertices(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogDetailsMergeVertices)
	enum { IDD = IDD_DIALOG_MergeVertices };
	BOOL m_MergeCubicCorners;
	double m_DistanceMergeCubicCorners;
	BOOL m_MergeAlignedCubicCorners;
	double m_MergeRotationCubicCorners;
	BOOL m_MergeCarefulLineTips;
	double m_MergeMaxDistVertices;
	double m_MergeMaxVerticesRotation;
	double m_ReificationMerit;
	BOOL m_MergeCasualLineTips;
	double m_ReMergeCasualInTol;
	double m_ReMergeCasualOutTol;
	BOOL m_ReMergeVerticesTriplets;
	double m_ReMergeMaxDistTriplets;
	double m_EmergentTripletsMerit;
	BOOL m_ReMergeVerticesDangling;
	double m_ReMergeMaxDistDangling;
	double m_EmergenceDanglingDeMerit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsMergeVertices)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsMergeVertices)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnRestoreDefaultR2D();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSMERGEVERTICES_INCLUDED)
