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

#if !defined(AFX_DIALOGCONFIGFEATURES_INCLUDED_)
#define AFX_DIALOGCONFIGFEATURES_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigFeatures.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigFeatures dialog

class CDialogConfigFeatures : public CDialog
{
// Construction
public:
	CDialogConfigFeatures(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataRefer GeneralParams, CDataFeatures &Fe);
	void GetParams(CDataRefer &GeneralParams, CDataFeatures &Fe);

// Dialog Data
	//{{AFX_DATA(CDialogConfigFeatures)
	enum { IDD = IDD_DIALOG_ConfigFeatures };
	BOOL	m_DetectFeatures;
	BOOL	m_DetectRibsAndDividers;
	BOOL	m_DetectRailsAndSlots;
	BOOL	m_DetectStepsAndPockets;
	//BOOL	m_DetectChamfersAndRounds;    //TBD ChamfersAndRounds

	long	m_SequenceRibsAndDividers;
	long	m_SequenceRailsAndSlots;
	long	m_SequenceStepsAndPockets;
	long	m_SequenceChamfersAndRounds;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigFeatures)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigFeatures)
	virtual BOOL OnInitDialog();

	afx_msg void OnDetailsRibsAndDividers();
	afx_msg void OnDetailsRailsAndSlots();
	afx_msg void OnDetailsStepsAndPockets();
	//afx_msg void OnDetailsChamfersAndRounds();    //TBD ChamfersAndRounds
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGFEATURES_INCLUDED_)
