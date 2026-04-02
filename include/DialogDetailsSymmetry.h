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

#if !defined(AFX_DIALOGDETAILSSYMMETRY_INCLUDED)
#define AFX_DIALOGDETAILSSYMMETRY_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DialogDetailsSymmetry.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogDetailsSymmetry dialog

class CDialogDetailsSymmetry : public CDialog
{
// Construction
public:
	CDialogDetailsSymmetry(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataSymmetry SY);
	void GetParams(CDataSymmetry &SY);


// Dialog Data
	//{{AFX_DATA(CDialogDetailsSymmetry)
	enum { IDD = IDD_DIALOG_Symmetry };
	BOOL m_DetectNormalCrossing;
	double m_MaxParallelCrossing;
	double m_MinParallelCrossing;
	double m_MeritWeight_1;

	BOOL m_DetectDistorted;
	double m_MinSymDistortion;
	double m_MaxSymDistortion;
	double m_MaxParallelFaceSymLines;
	double m_MinParallelFaceSymLines;
	double m_MeritWeight_2;

	BOOL m_SymmetricalEdges;
	double m_MaxParallelEdgeSymLines;
	double m_MinParallelEdgeSymLines;
	double m_MeritWeight_3;

	BOOL m_PrioritizeVertical;
	double m_minVerticalThreshold;
	double m_maxVerticalThreshold;
	double m_MeritWeight_4;

	BOOL m_RemoveBadMerit;
	double m_MinMerit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogDetailsSymmetry)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogDetailsSymmetry)
	afx_msg void OnBUTTONDetailsSymmetryDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGDETAILSSYMMETRY_INCLUDED)
