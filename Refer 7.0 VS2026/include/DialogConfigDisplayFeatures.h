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

#if !defined(AFX_DIACONFIGDISPLAYFEATURES_INCLUDED_)
#define AFX_DIACONFIGDISPLAYFEATURES_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogSelectTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogSelectTree dialog

class CDialogConfigDisplayFeatures : public CDialog
{
// Construction
public:
	CDialogConfigDisplayFeatures(CWnd* pParent = NULL);   // standard constructor
	void GetParams(double &ScaleFeatureLabels,
				   bool &DisplayNamesOfFeatures, 
				   bool &DisplayMeritsOfFeatures, 
				   bool &DisplayContainerFaces);
	void SetParams(double ScaleFeatureLabels,
				   bool DisplayNamesOfFeatures, 
				   bool DisplayMeritsOfFeatures, 
				   bool DisplayContainerFaces);

// Dialog Data
	//{{AFX_DATA(CDialogConfigDisplayFeatures)
	enum {IDD = IDD_DIALOG_DISPLAY_FEATURES};
	double m_ScaleFeatureLabels;
	BOOL m_DisplayNamesOfFeatures;
	BOOL m_DisplayMeritsOfFeatures;
	BOOL m_DisplayContainerFaces;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigDisplayFeatures)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigDisplayFeatures)
	virtual BOOL OnInitDialog();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIACONFIGDISPLAYFEATURES_INCLUDED_)
