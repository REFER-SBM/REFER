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

#if !defined(AFX_DIALOGCONFIGVECTORIZE2D_INCLUDED_)
#define AFX_DIALOGCONFIGVECTORIZE2D_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigVectorize2D.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigVectorize2D dialog

class CDialogConfigVectorize2D : public CDialog
{
// Construction
public:
	CDialogConfigVectorize2D(CWnd* pParent = NULL);   // standard constructor
	void SetParams(CDataRefer GeneralParams, CDataVectorize Ve);
	void GetParams(CDataRefer &GeneralParams, CDataVectorize &Ve);

// Dialog Data
	//{{AFX_DATA(CDialogConfigVectorize2D)
	enum { IDD = IDD_DIALOG_ConfigVectorize2D };
	BOOL m_Vectorize_Int;
	BOOL m_Vectorize_Batch;

	BOOL m_ParseGestures;
	int m_StrokeCornersApproach;

	int m_RecognitionApproach;   // 0= REFER, 1= CALI

	BOOL m_GroupOvertracedStrokes;
	BOOL m_GroupNonSolidStrokes;
	BOOL m_SegmentStrokes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigVectorize2D)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigVectorize2D)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdate();
	afx_msg void OnDetailsStrokeVectorize();
	afx_msg void OnDetailsGroupOvertracedStrokes();
	afx_msg void OnDetailsGroupNonSolidStrokes();
	afx_msg void OnBUTTONRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGVECTORIZE2D_INCLUDED_)
