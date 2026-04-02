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

#if !defined(AFX_DIALOGTOOLSDISPLAYSTROKES_INCLUDED_)
#define AFX_DIALOGTOOLSDISPLAYSTROKES_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogSelectNodes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogToolsDisplayStrokes dialog

class CDialogToolsDisplayStrokes : public CDialog
{
// Construction
public:
	void GetParams(long &SelectedNodes, bool &Display_Corners,
				   bool &Display_Line_Tolerance, bool &Display_Arc_Tolerance,
				   bool &Display_Convex_Hull, bool &Display_Merits_Strokes);
	void SetParams(long SelectedNodes, bool Display_Corners,
				   bool Display_Line_Tolerance, bool Display_Arc_Tolerance,
				   bool Display_Convex_Hull, bool Display_Merits_Strokes);
	CDialogToolsDisplayStrokes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogToolsDisplayStrokes)
	enum {IDD = IDD_DIALOG_DISPLAY_STROKES};
	int m_Nodes;
	BOOL m_Display_Corners;
	BOOL m_Display_Line_Tolerance;
	BOOL m_Display_Arc_Tolerance;
	BOOL m_Display_Convex_Hull;
	BOOL m_Display_Merits_Strokes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogToolsDisplayStrokes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogToolsDisplayStrokes)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGTOOLSDISPLAYSTROKES_INCLUDED_)
