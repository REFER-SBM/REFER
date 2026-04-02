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

#if !defined(AFX_DIALOGSETTINGSREFER_INCLUDED)
#define AFX_DIALOGSETTINGSREFER_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfiguracionRefer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogSettingsRefer dialog

class CDialogSettingsRefer : public CDialog
{
// Construction
public:
	void SetParams();
	void GetParams();
	CDialogSettingsRefer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogSettingsRefer)
	enum { IDD = IDD_DIALOG_SettingsRefer };
	BOOL m_Refine_On_Line;
	BOOL m_Refine_Off_Line;
	BOOL m_Detect_CuesAndFeatures;

	BOOL m_Detect_Cues;
	BOOL m_Detect_Valences;
	BOOL m_Detect_ParallelEdges;
	BOOL m_Detect_VanishingPoints;
	BOOL m_Detect_Graphs;
	BOOL m_Detect_Perimeter;
	BOOL m_Detect_Labels;
	BOOL m_Detect_MainAxes;
	BOOL m_Detect_Faces;
	BOOL m_Detect_Typology;
	BOOL m_Detect_HiddenEdges;
	BOOL m_Detect_Symmetry;
	BOOL m_Detect_Levels;

	BOOL m_Detect_Features;
	BOOL m_Detect_RibsAndDividers;
	BOOL m_Detect_RailsAndSlots;
	BOOL m_Detect_StepsAndPockets;

	BOOL m_InflateBRep;
	BOOL m_Direct_Inflation;
	BOOL m_Optimization_based_Inflation;
	BOOL m_CSG_Modeling;
	BOOL m_Refine_3D;
	BOOL m_Tessellate_3D;
	int m_MRU_Files;
	BOOL m_IGS_Type_143;
	BOOL m_IGS_NgonFaces;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogSettingsRefer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogSettingsRefer)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGSETTINGSREFER_INCLUDED)
