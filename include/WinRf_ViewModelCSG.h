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

#if !defined(VIEWMODELCSG_DEFINED)
#define VIEWMODELCSG_DEFINED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// WinRf_ViewModelCSG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelCSG view

enum TMouseModeFeaturesCSG {InactiveModeFeaturesCSG, 
							ZoomModeFeaturesCSG,
							PanModeFeaturesCSG};


class CWinRf_ViewModelCSG : public CScrollView
{
	protected:
		CWinRf_ViewModelCSG();           // protected constructor used by dynamic creation
		DECLARE_DYNCREATE(CWinRf_ViewModelCSG)

	// Attributes
	private:
		CReferDoc* pDoc;
		CReferDoc* GetDocument();

		CClientDC* dc;
		CWinRf_OpenGLDevice OpenGLDeviceModelCSG;
		CWinRf_OpenGLViewModelCSG *OpenGLViewModelCSG;

	// Operations
	private:
		//Parameters and functions to navigate the virtual paper
		double CameraZoom;
		double CameraPanX, CameraPanY;
		double ViewportWide, ViewportHigh;
		double WindowLength, WindowHigh;

		void ResetViewport();
		void ResetViewport(long row, long numRows,
						   long column, long numColumn);
		void Get2DModelBoundingBox(double &wXmin, double &wYmin, double &wXmax, double &wYmax);
		void UpdateWindow();
		void ResetWindow(bool resetZoomPan);

		cGDI_Transform Transf;

		//Parameters and functions to control mouse
		CPoint firstPoint, lastPoint;
		double StartTime;  // Starts measuring time since the buttondown
		TMouseModeFeaturesCSG MouseModeFeatures;

	private:
		//Auxiliar functions
		CDialogPreviousNextCSG *pPrevNextFeature;
		CDialogPreviousNextCSG *pPrevNextDatum;
		CDialogPreviousNextCSG *pPrevNextProfile;
		CDialogPreviousNextCSG *pPrevNextDec;

		//Parameters and functions to display models
		//(3D window)
		bool mode_CSG_Features;
		long NumFeatureInSequence;

		//(2D window)
		bool ViewCSG_Tree;
		bool DisplayDatums;
		bool DisplayDatumsInSequence;
		long NumDatumInSequence;
		bool DisplayProfiles;
		bool DisplayProfilesInSequence;
		long NumProfileInSequence;
		bool DisplayDeconstruction;
		bool DisplayDeconstructionInSequence;
		long NumDeconstructionInSequence;

		//Parameters and functions to display features
		double CSG_Tree_ScaleLabels;
		bool CSG_Tree_DisplayNumOfBranches;
		bool CSG_Tree_DisplayNamesOfFeatures;
		bool CSG_Tree_DisplayMeritsOfFeatures;
		long CSG_Tree_Columns;

		double CSG_Datums_ScaleLabels;
		bool CSG_Datums_DisplayGraph;
		bool CSG_Datums_DisplayNumVertices;
		bool CSG_Datums_DisplayCoordsVertices;
		bool CSG_Datums_DisplayNumEdges;
		bool CSG_Datums_DisplayInputDrawing;
		bool CSG_Datums_DisplayNumOfDatums;
		bool CSG_Datums_DisplayDatumSymmetry;

		double CSG_Dec_ScaleLabels;
		bool CSG_Dec_DisplayLabelDeconstruction;
		bool CSG_Dec_DisplayNumVertices;
		bool CSG_Dec_DisplayCoordsVertices;
		bool CSG_Dec_DisplayNumEdges;
		long CSG_Dec_Columns;

		void TextInRect(CDC *pDC, CString sText, CRect &rectFont);

		void PlotModelTree(CDC* pDC);

		void DisplayDot(CClientDC *dc, CPoint Position, long size);
		POINT2D GetDisplacedPoint(POINT2D p1, POINT2D p2,
										   double d);
		void GDI_DrawMainAxes();
		void GDI_DrawCSGDatums(CDB_Model *DDBB_Path, 
							   CDB_Model *DDBB_Drawing,
							   CDB_CSG *DB_CSG_Model,
							   long NumDatum,
							   int R, int G, int B);
		void GDI_DrawCSGProfiles(CDB_Model *DDBB_Path, 
								 CDB_Model *DDBB_Drawing,
								 CDB_CSG *DB_CSG_Model,
								 long NumProfile,
								 int R, int G, int B);

		void PlotModelDeconstruction(CDC* pDC);

		void GDI_DrawCSGDrawing(CDB_Model *DDBB, int R, int G, int B);

		//void DetermineFeaturesCSGVolume(CDB_Model *DB_Aux);
		void OnDrawModelTreeCSG(CDC* pDC);

		void DetermineModelCSGVolume(CDB_Model *DB_Aux);
		void OnDrawFeaturesCSG(CDC* pDC);

		enum {Inactive, CalculatingSpeed, SelfRotation} m_EstateTimer;
		UINT m_Timer;
		DWORD m_dwMilliseconds;
		CPoint m_PosMousePrev,
			   m_PosMouseInitial,
			   m_PosMouseFromTimer;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CWinRf_ViewModelCSG)
		protected:
		virtual void OnDraw(CDC* pDC);      // overridden to draw this view
		virtual void OnInitialUpdate();     // first time after construct
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		virtual ~CWinRf_ViewModelCSG();
		//#ifdef _DEBUG
		//	virtual void AssertValid() const;
		//	virtual void Dump(CDumpContext& dc) const;
		//#endif

		// Generated message map functions
		//{{AFX_MSG(CWinRf_ViewModelCSG)
		//Window
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnDestroy();

		//Toolbar CSG Model
		afx_msg void OnViewCSG_FeaturesInSequence();
		afx_msg void OnUpdateViewCSG_FeaturesInSequence(CCmdUI* pCmdUI);
		afx_msg void OnDisplayNextFeatureInSequence();
		afx_msg void OnDisplayPreviousFeatureInSequence();

		afx_msg void OnViewModelCSG_Tree();
		afx_msg void OnUpdateViewModelCSG_Tree(CCmdUI* pCmdUI);

		afx_msg void OnDisplayDatums();
		afx_msg void OnUpdateDisplayDatums(CCmdUI* pCmdUI);

		afx_msg void OnDisplayDatumsInSequence();
		afx_msg void OnUpdateDisplayDatumsInSequence(CCmdUI* pCmdUI);
		afx_msg void OnDisplayNextDatumInSequence();
		afx_msg void OnDisplayPreviousDatumInSequence();

		afx_msg void OnDisplayProfiles();
		afx_msg void OnUpdateDisplayProfiles(CCmdUI* pCmdUI);

		afx_msg void OnDisplayProfilesInSequence();
		afx_msg void OnUpdateDisplayProfilesInSequence(CCmdUI* pCmdUI);
		afx_msg void OnDisplayNextProfileInSequence();
		afx_msg void OnDisplayPreviousProfileInSequence();

		afx_msg void OnDisplayDeconstruction();
		afx_msg void OnUpdateDisplayDeconstruction(CCmdUI* pCmdUI);

		afx_msg void OnDisplayDeconstructionInSequence();
		afx_msg void OnUpdateDisplayDeconstructionInSequence(CCmdUI* pCmdUI);
		afx_msg void OnDisplayNextDeconstructionInSequence();
		afx_msg void OnDisplayPreviousDeconstructionInSequence();

		afx_msg void OnDisplayZoomReset();
		afx_msg void OnUpdateDisplayZoomReset(CCmdUI* pCmdUI);

		afx_msg void OnConfigDisplayCSG();
		afx_msg void OnUpdateConfigDisplayCSG(CCmdUI* pCmdUI);

		//Mouse
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

		afx_msg void OnMouseMove(UINT nFlags, CPoint point);

		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};


#ifndef _DEBUG  // debug version in CWinRf_ViewModelCSG.cpp
inline CReferDoc* CWinRf_ViewModelCSG::GetDocument()
   { return (CReferDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////
#endif //VIEWMODELCSG_DEFINED
