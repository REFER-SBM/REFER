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

#if !defined(AFX_DIALOGCONFIGDISPLAYCSG_INCLUDED_)
#define AFX_DIALOGCONFIGDISPLAYCSG_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DialogConfigDisplayCSG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDisplayCSG dialog

class CDialogConfigDisplayCSG : public CDialog
{
// Construction
public:
	CDialogConfigDisplayCSG(CWnd* pParent = NULL);   // standard constructor
	void GetParams(double &CSG_Tree_ScaleLabels,
				   bool &CSG_Tree_DisplayNumOfBranches, 
				   bool &CSG_Tree_DisplayNamesOfFeatures, 
				   bool &CSG_Tree_DisplayMeritsOfFeatures, 
				   long &CSG_Tree_Columns,
				   double &CSG_Datums_ScaleLabels,
				   bool &CSG_Datums_DisplayGraph,
				   bool &CSG_Datums_DisplayNumVertices,
				   bool &CSG_Datums_DisplayCoordsVertices,
				   bool &CSG_Datums_DisplayNumEdges,
				   bool &CSG_Datums_DisplayInputDrawing,
				   bool &CSG_Datums_DisplayNumOfDatums,
				   bool &CSG_Datums_DisplayDatumSymmetry,
				   double &CSG_Dec_ScaleLabels,
				   bool &CSG_Dec_DisplayLabelDeconstruction, 
				   bool &CSG_Dec_DisplayNumVertices,
				   bool &CSG_Dec_DisplayCoordsVertices,
				   bool &CSG_Dec_DisplayNumEdges,
				   long &CSG_Dec_Columns);

	void SetParams(double CSG_Tree_ScaleLabels,
				   bool CSG_Tree_DisplayNumOfBranches, 
				   bool CSG_Tree_DisplayNamesOfFeatures, 
				   bool CSG_Tree_DisplayMeritsOfFeatures, 
				   long CSG_Tree_Columns,
				   double CSG_Datums_ScaleLabels,
				   bool CSG_Datums_DisplayGraph,
				   bool CSG_Datums_DisplayNumVertices,
				   bool CSG_Datums_DisplayCoordsVertices,
				   bool CSG_Datums_DisplayNumEdges,
				   bool CSG_Datums_DisplayInputDrawing,
				   bool CSG_Datums_DisplayNumOfDatums,
				   bool CSG_Datums_DisplayDatumSymmetry,
				   double CSG_Dec_ScaleLabels,
				   bool CSG_Dec_DisplayLabelDeconstruction, 
				   bool CSG_Dec_DisplayNumVertices,
				   bool CSG_Dec_DisplayCoordsVertices,
				   bool CSG_Dec_DisplayNumEdges,
				   long CSG_Dec_Columns);

// Dialog Data
	//{{AFX_DATA(CDialogConfigDisplayCSG)
	enum { IDD = IDD_DIALOG_ConfigDisplayCSG };
	double m_CSG_Tree_ScaleLabels;
	BOOL m_CSG_Tree_DisplayNumOfBranches;
	BOOL m_CSG_Tree_DisplayNamesOfFeatures;
	BOOL m_CSG_Tree_DisplayMeritsOfFeatures;
	long m_CSG_Tree_Columns;

	double m_CSG_Datums_ScaleLabels;
	BOOL m_CSG_Datums_DisplayGraph;
	BOOL m_CSG_Datums_DisplayNumVertices;
	BOOL m_CSG_Datums_DisplayCoordsVertices;
	BOOL m_CSG_Datums_DisplayNumEdges;
	BOOL m_CSG_Datums_DisplayInputDrawing;
	BOOL m_CSG_Datums_DisplayNumOfDatums;
	BOOL m_CSG_Datums_DisplayDatumSymmetry;

	double m_CSG_Dec_ScaleLabels;
	BOOL m_CSG_Dec_DisplayLabelDeconstruction;
	BOOL m_CSG_Dec_DisplayNumVertices;
	BOOL m_CSG_Dec_DisplayCoordsVertices;
	BOOL m_CSG_Dec_DisplayNumEdges;
	long m_CSG_Dec_Columns;

	BOOL m_CSG_Inflation;
	BOOL m_CSG_Inflation_Direct;
	BOOL m_CSG_Inflation_Optimization;

	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogConfigDisplayCSG)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDialogConfigDisplayCSG)
	virtual BOOL OnInitDialog();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGCONFIGDISPLAYCSG_INCLUDED_)
