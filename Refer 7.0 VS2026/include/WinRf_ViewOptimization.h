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

#if !defined(VIEW_OPTIMIZATION_VISITED)
#define VIEW_OPTIMIZATION_VISITED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// WinRf_ViewOptimization.h : header file
//


class CDialogOptimizationPan;


/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewOptimization view

class CWinRf_ViewOptimization : public CScrollView
{
protected:
	CWinRf_ViewOptimization();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CWinRf_ViewOptimization)

// Operations
private:
	bool bZoom;
	bool PanDefined;
	CDialogOptimizationPan *pEncuadre;
	bool CurveCalculationInProgress;
	bool ResetCurves;

	//Grid of points of the map
	std::vector<double> Xgrid;
	std::vector<double> Ygrid;
	std::vector<double> Fgrid;

	CGraphicStyle m_GraphicStyle;

	CPoint Zoom[5];

	CParamsIsolines* pParamsIsolines;

	void SubSpaceTitles(CDC * pDC, CString &sTitleAxisX, CString &sTitleAxisY);

	void ResetZoomPan();
	void RecalculateGrid();

	void RefineSpline(std::vector<double> &SplineXgrid,
					  std::vector<double> &SplineYgrid,
					  std::vector<double> &XPuntosControl,
					  std::vector<double> &YPuntosControl,
					  int NSSG);
	void DrawSpline(std::vector<double> &XPuntosControl,
					std::vector<double> &YPuntosControl,
					COLORREF ColorCurvas,
					CDC * pDC,
					cGDI_Transform TransfEjeX,
					cGDI_Transform TransfEjeY,
					int InterpolationDegree, double Z, double Zmax);

	//std::vector<double> CopyXCutPoints;
	//std::vector<double> CopyYCutPoints;

	bool CutLeft(int lIndice, int lIndiceY,
				 double *pXp, double *pYp,
				 std::vector<double> &CopyXCutPoints,
				 std::vector<double> &CopyYCutPoints);
	bool CutRight(int lIndice, int lIndiceY, 
				  double *pXp, double *pYp,
				  std::vector<double> &CopyXCutPoints,
				  std::vector<double> &CopyYCutPoints);
	bool CutDown(int lIndice, int lIndiceY,
				 double *pXp, double *pYp,
				 std::vector<double> &CopyXCutPoints,
				 std::vector<double> &CopyYCutPoints);
	bool CutUp(int lIndice, int lIndiceY,
			   double *pXp, double *pYp,
			   std::vector<double> &CopyXCutPoints,
			   std::vector<double> &CopyYCutPoints);

	void DrawIsolinesAxisY(CDC * pDC,
						   CRect rect,
						   long iBorderLeft, long iBorderRight,
						   long iBorderUp, long iBorderDown,
						   double Xmin, double Ymin, double Xmax, double Ymax,
						   double dRangeX, double dRangeY,
						   COLORREF ColorEjeX, COLORREF ColorEjeY,
						   double dFactorAmpliacionRangos,
						   CString sTitleAxisY,
						   cGDI_Transform &TransfEjeY);
	void DrawIsolinesAxisX(CDC * pDC,
						   CRect rect,
						   long iBorderLeft, long iBorderRight,
						   long iBorderUp, long iBorderDown,
						   double Xmin, double Ymin, double Xmax, double Ymax,
						   double dRangeX, double dRangeY,
						   COLORREF ColorEjeX, COLORREF ColorEjeY,
						   double dFactorAmpliacionRangos,
						   CString sTitleAxisX,
						   cGDI_Transform &TransfEjeX);

	std::vector<double> XPuntosCorte;
	std::vector<double> YPuntosCorte;

	void DetectIsolines1(CDC * pDC,
						 COLORREF ColorCurvas,
						 cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
						 int InterpolationDegree,
						 double Z, double Zmax,
						 std::vector<double> &CopyXCutPoints,
						 std::vector<double> &CopyYCutPoints);
	void DetectIsolines2(CDC * pDC,
						 COLORREF ColorCurvas,
						 cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
						 int InterpolationDegree,
						 double Z, double Zmax,
						 std::vector<double> &CopyXCutPoints,
						 std::vector<double> &CopyYCutPoints);
	void DetectIsolines3(CDC * pDC,
						 COLORREF ColorCurvas,
						 cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
						 int InterpolationDegree,
						 double Z, double Zmax,
						 std::vector<double> &CopyXCutPoints,
						 std::vector<double> &CopyYCutPoints);
	void DetectIsolines4(CDC * pDC,
						 COLORREF ColorCurvas,
						 cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
						 int InterpolationDegree,
						 double Z, double Zmax,
						 std::vector<double> &CopyXCutPoints,
						 std::vector<double> &CopyYCutPoints);
	void DetectIsolines5(CDC * pDC,
						 COLORREF ColorCurvas,
						 cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
						 int InterpolationDegree,
						 double Z, double Zmax,
						 double Xmin, double Xmax, double Ymin, double Ymax,
						 std::vector<double> &CopyXCutPoints,
						 std::vector<double> &CopyYCutPoints);
	void DetectIsolines6(CDC * pDC,
						 COLORREF ColorCurvas,
						 cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
						 int InterpolationDegree,
						 double Z, double Zmax,
						 double Xmin, double Xmax, double Ymin, double Ymax,
						 std::vector<double> &CopyXCutPoints,
						 std::vector<double> &CopyYCutPoints);
	void DetectIsolines7(CDC * pDC,
						 COLORREF ColorCurvas,
						 cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY,
						 int InterpolationDegree,
						 double Z, double Zmax,
						 double Xmin, double Xmax, double Ymin, double Ymax,
						 std::vector<double> &CopyXCutPoints,
						 std::vector<double> &CopyYCutPoints);
	void DrawIsoLines(CDC * pDC, CRect rect,
					  int DInterpolationDegree,
					  long iBorderLeft, long iBorderRight,
					  long iBorderUp, long iBorderDown,
					  double Xmin, double Xmax,
					  double Ymin, double Ymax,
					  double Zmin, double Zmax,
					  cGDI_Transform TransfEjeX, cGDI_Transform TransfEjeY);
	void DrawLegendDesignSubSpace(CDC *pDC, CRect rect,
								  long iBorderRight, long iBorderUp, COLORREF ColorEjeY);

	void MapSubSpace(CDC *pDC, CString sTitleAxisX, CString sTitleAxisY);
	void DrawSubSpace(CDC *pDC);

	void PlotCost(CDC *pDC, std::vector<double> Data,
				  CDataInflation In,
				  CString sTitleAxisYMain,
				  CString sTitleAxisYAux, 
				  CString sTitleAxisX);

	void PlotCostsMulti(CDC *pDC, std::vector<double> Data,
						CDataInflation In,
						CString sTitleAxisYMain,
						CString sTitleAxisYAux,
						CString sTitleAxisX);

	void PlotVariablesZ(CDC *pDC, std::vector<double> Data, 
						CDataInflation In,
						CString sTitleAxisYMain,
						CString sTitleAxisX);
	void PlotDiagrams(CDC *pDC);

	CReferDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinRf_ViewOptimization)
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CWinRf_ViewOptimization();
	//#ifdef _DEBUG
	//	virtual void AssertValid() const;
	//	virtual void Dump(CDumpContext& dc) const;
	//#endif

	// Generated message map functions
	//{{AFX_MSG(CWinRf_ViewOptimization)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	//Toolbar Optimization
	afx_msg void OnViewCost();
	afx_msg void OnUpdateViewCost(CCmdUI* pCmdUI);

	afx_msg void OnViewPartialCosts();
	afx_msg void OnUpdateViewPartialCosts(CCmdUI* pCmdUI);

	afx_msg void OnViewZ();
	afx_msg void OnUpdateViewZ(CCmdUI* pCmdUI);

	afx_msg void OnViewSomeZs();
	afx_msg void OnUpdateViewSomeZs(CCmdUI* pCmdUI);

	afx_msg void OnViewIsolines();
	afx_msg void OnUpdateViewIsolines(CCmdUI* pCmdUI);

	afx_msg void OnShowLegend();
	afx_msg void OnUpdateShowLegend(CCmdUI* pCmdUI);

	afx_msg void OnLabels();
	afx_msg void OnUpdateLabels(CCmdUI* pCmdUI);

	afx_msg void OnViewConfigEvolutionVariablesOpt();
	afx_msg void OnUpdateViewConfigEvolutionVariablesOpt(CCmdUI* pCmdUI);

	afx_msg void OnConfigIsolines();
	afx_msg void OnUpdateConfigIsolines(CCmdUI* pCmdUI);

	afx_msg void OnZoomAmpliar();
	afx_msg void OnUpdateZoomAmpliar(CCmdUI* pCmdUI);

	afx_msg void OnZoomReducir();
	afx_msg void OnUpdateZoomReducir(CCmdUI* pCmdUI);

	afx_msg void OnZoomTodo();
	afx_msg void OnUpdateZoomTodo(CCmdUI* pCmdUI);

	afx_msg void OnEncuadre();
	afx_msg void OnUpdateEncuadre(CCmdUI* pCmdUI);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#ifndef _DEBUG  // debug version in ViewOptimization.cpp
inline CReferDoc* CWinRf_ViewOptimization::GetDocument()
   { return (CReferDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(VIEW_OPTIMIZATION_VISITED)
