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

// DialogConfigDisplayCSG.cpp : implementation file
//

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "ReferDoc.h"

#include "DialogConfigDisplayCSG.h"

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDisplayCSG dialog


CDialogConfigDisplayCSG::CDialogConfigDisplayCSG(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogConfigDisplayCSG::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogConfigDisplayCSG)
	//}}AFX_DATA_INIT
}


void CDialogConfigDisplayCSG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogConfigCSG)
	DDX_Text(pDX, IDC_CSG_Tree_ScaleLabels, m_CSG_Tree_ScaleLabels);
	DDX_Check(pDX, IDC_CSG_Tree_DisplayNumOfBranches, m_CSG_Tree_DisplayNumOfBranches);
	DDX_Check(pDX, IDC_CSG_Tree_DisplayNamesOfFeatures, m_CSG_Tree_DisplayNamesOfFeatures);
	DDX_Check(pDX, IDC_CSG_Tree_DisplayMeritsOfFeatures, m_CSG_Tree_DisplayMeritsOfFeatures);
	DDX_Text(pDX, IDC_CSG_Tree_Columns, m_CSG_Tree_Columns);

	DDX_Text(pDX, IDC_CSG_Datums_ScaleLabels, m_CSG_Datums_ScaleLabels);
	DDX_Check(pDX, IDC_CSG_Datums_DisplayGraph, m_CSG_Datums_DisplayGraph);
	DDX_Check(pDX, IDC_CSG_Datums_DisplayNumVertices, m_CSG_Datums_DisplayNumVertices);
	DDX_Check(pDX, IDC_CSG_Datums_DisplayCoordsVertices, m_CSG_Datums_DisplayCoordsVertices);
	DDX_Check(pDX, IDC_CSG_Datums_DisplayNumEdges, m_CSG_Datums_DisplayNumEdges);
	DDX_Check(pDX, IDC_CSG_Datums_DisplayInputDrawing, m_CSG_Datums_DisplayInputDrawing);
	DDX_Check(pDX, IDC_CSG_Datums_DisplayNumOfDatums, m_CSG_Datums_DisplayNumOfDatums);
	DDX_Check(pDX, IDC_CSG_Datums_DisplayDatumSymmetry, m_CSG_Datums_DisplayDatumSymmetry);

	DDX_Text(pDX, IDC_CSG_Dec_ScaleLabels, m_CSG_Dec_ScaleLabels);
	DDX_Check(pDX, IDC_CSG_Dec_DisplayLabelDeconstruction, m_CSG_Dec_DisplayLabelDeconstruction);
	DDX_Check(pDX, IDC_CSG_Dec_DisplayNumVertices, m_CSG_Dec_DisplayNumVertices);
	DDX_Check(pDX, IDC_CSG_Dec_DisplayCoordsVertices, m_CSG_Dec_DisplayCoordsVertices);
	DDX_Check(pDX, IDC_CSG_Dec_DisplayNumEdges, m_CSG_Dec_DisplayNumEdges);
	DDX_Text(pDX, IDC_CSG_Dec_Columns, m_CSG_Dec_Columns);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogConfigDisplayCSG, CDialog)
	//{{AFX_MSG_MAP(CDialogConfigDisplayCSG)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogConfigDisplayCSG message handlers

void CDialogConfigDisplayCSG::SetParams(double CSG_Tree_ScaleLabels,
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
										long CSG_Dec_Columns)
// Produces local versions of those general parameters that can be modified by this dialog
{
	m_CSG_Tree_ScaleLabels = CSG_Tree_ScaleLabels;
	m_CSG_Tree_DisplayNumOfBranches = CSG_Tree_DisplayNumOfBranches;
	m_CSG_Tree_DisplayNamesOfFeatures = CSG_Tree_DisplayNamesOfFeatures;
	m_CSG_Tree_DisplayMeritsOfFeatures = CSG_Tree_DisplayMeritsOfFeatures;
	m_CSG_Tree_Columns = CSG_Tree_Columns;

	m_CSG_Datums_ScaleLabels = CSG_Datums_ScaleLabels;
	m_CSG_Datums_DisplayGraph = CSG_Datums_DisplayGraph;
	m_CSG_Datums_DisplayNumVertices = CSG_Datums_DisplayNumVertices;
	m_CSG_Datums_DisplayCoordsVertices = CSG_Datums_DisplayCoordsVertices;
	m_CSG_Datums_DisplayNumEdges = CSG_Datums_DisplayNumEdges;
	m_CSG_Datums_DisplayInputDrawing = CSG_Datums_DisplayInputDrawing;
	m_CSG_Datums_DisplayNumOfDatums = CSG_Datums_DisplayNumOfDatums;
	m_CSG_Datums_DisplayDatumSymmetry = CSG_Datums_DisplayDatumSymmetry;

	m_CSG_Dec_ScaleLabels = CSG_Dec_ScaleLabels;
	m_CSG_Dec_DisplayLabelDeconstruction = CSG_Dec_DisplayLabelDeconstruction;
	m_CSG_Dec_DisplayNumVertices = CSG_Dec_DisplayNumVertices;
	m_CSG_Dec_DisplayCoordsVertices = CSG_Dec_DisplayCoordsVertices;
	m_CSG_Dec_DisplayNumEdges = CSG_Dec_DisplayNumEdges;
	m_CSG_Dec_Columns = CSG_Dec_Columns;
}


void CDialogConfigDisplayCSG::GetParams(double &CSG_Tree_ScaleLabels,
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
										long &CSG_Dec_Columns)
{
	CSG_Tree_ScaleLabels = m_CSG_Tree_ScaleLabels;
	CSG_Tree_DisplayNumOfBranches = (m_CSG_Tree_DisplayNumOfBranches != 0);
	CSG_Tree_DisplayNamesOfFeatures = (m_CSG_Tree_DisplayNamesOfFeatures != 0);
	CSG_Tree_DisplayMeritsOfFeatures = (m_CSG_Tree_DisplayMeritsOfFeatures != 0);
	CSG_Tree_Columns = m_CSG_Tree_Columns;

	CSG_Datums_ScaleLabels = m_CSG_Datums_ScaleLabels;
	CSG_Datums_DisplayGraph = (m_CSG_Datums_DisplayGraph != 0);
	CSG_Datums_DisplayNumVertices = (m_CSG_Datums_DisplayNumVertices != 0);
	CSG_Datums_DisplayCoordsVertices = (m_CSG_Datums_DisplayCoordsVertices != 0);
	CSG_Datums_DisplayNumEdges = (m_CSG_Datums_DisplayNumEdges != 0);
	CSG_Datums_DisplayInputDrawing = (m_CSG_Datums_DisplayInputDrawing != 0);
	CSG_Datums_DisplayNumOfDatums = (m_CSG_Datums_DisplayNumOfDatums != 0);
	CSG_Datums_DisplayDatumSymmetry = (m_CSG_Datums_DisplayDatumSymmetry != 0);

	CSG_Dec_ScaleLabels = m_CSG_Dec_ScaleLabels;
	CSG_Dec_DisplayLabelDeconstruction = (m_CSG_Dec_DisplayLabelDeconstruction != 0);
	CSG_Dec_DisplayNumVertices = (m_CSG_Dec_DisplayNumVertices != 0);
	CSG_Dec_DisplayCoordsVertices = (m_CSG_Dec_DisplayCoordsVertices != 0);
	CSG_Dec_DisplayNumEdges = (m_CSG_Dec_DisplayNumEdges != 0);
	CSG_Dec_Columns = m_CSG_Dec_Columns;
}


BOOL CDialogConfigDisplayCSG::OnInitDialog() 
{
	CDialog::OnInitDialog();

	return true;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
