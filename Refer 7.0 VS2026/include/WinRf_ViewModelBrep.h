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

#if !defined(VIEWMODELBREP_DEFINED)
#define VIEWMODELBREP_DEFINED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// WinRf_ViewModelBrep.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWinRf_ViewModelBrep view

class CWinRf_ViewModelBrep : public CScrollView
{
	protected:
		CWinRf_ViewModelBrep();           // protected constructor used by dynamic creation
		DECLARE_DYNCREATE(CWinRf_ViewModelBrep)

	// Attributes
	private:
		CReferDoc* pDoc;
		CReferDoc* GetDocument();

		CClientDC* dc;
		CWinRf_OpenGLDevice OpenGLDeviceModelBrep;
		CWinRf_OpenGLViewModelBrep *OpenGLViewModelBrep;

		CWinRf_OpenGLDevice OpenGLDeviceInflation;
		CWinRf_OpenGLViewInflation *OpenGLViewInflation;

	// Operations
	private:
		bool mode_Model_Brep;

		bool bShowNeckerReversal;

		bool bShowAxesModelBrep;
		bool bMainAxes;

		bool bShowEdges3D;
		bool bThickEdges;

		bool bShowFaces3D;
		bool bShowTriangulatedFaces3D;
		bool bShowFaceNormals3D;

		void DetermineModelBrepVolume(CDB_Model *DB_Aux);
		void OnDrawModelBrep(CDC* pDC);

		void DetermineInflationVolume();
		void OnDrawInflation(CDC* pDC);

		enum {Inactive, CalculatingSpeed, SelfRotation} m_EstateTimer;
		UINT m_Timer;
		DWORD m_dwMilliseconds;
		CPoint m_PosMousePrev,
			   m_PosMouseInitial,
			   m_PosMouseFromTimer;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinRf_ViewModelBrep)
	protected:
		virtual void OnDraw(CDC* pDC);      // overridden to draw this view
		virtual void OnInitialUpdate();     // first time after construct
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		virtual ~CWinRf_ViewModelBrep();
		//#ifdef _DEBUG
		//	virtual void AssertValid() const;
		//	virtual void Dump(CDumpContext& dc) const;
		//#endif

		// Generated message map functions
		//{{AFX_MSG(CWinRf_ViewModelBrep)
		//Window
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSetFocus(CWnd* pOldWnd);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnDestroy();

		//Toolbar Brep Model
		afx_msg void OnViewInflation();
		afx_msg void OnUpdateViewInflation(CCmdUI* pCmdUI);

		afx_msg void OnConfigureViewInflation();
		afx_msg void OnUpdateConfigureViewInflation(CCmdUI* pCmdUI);

		afx_msg void OnViewEvolution();
		afx_msg void OnUpdateViewEvolution(CCmdUI* pCmdUI);

		afx_msg void OnViewNeckerReversal();
		afx_msg void OnUpdateViewNeckerReversal(CCmdUI* pCmdUI);

		afx_msg void OnViewModelInMainAxes();
		afx_msg void OnUpdateViewModelInMainAxes(CCmdUI* pCmdUI);

		afx_msg void OnViewFront();
		afx_msg void OnUpdateViewFront(CCmdUI* pCmdUI);

		afx_msg void OnViewTop();
		afx_msg void OnUpdateViewTop(CCmdUI* pCmdUI);

		afx_msg void OnViewSide();
		afx_msg void OnUpdateViewSide(CCmdUI* pCmdUI);

		afx_msg void OnViewIsometric();
		afx_msg void OnUpdateViewIsometric(CCmdUI* pCmdUI);

		afx_msg void OnViewAxes3D();
		afx_msg void OnUpdateViewAxes3D(CCmdUI* pCmdUI);

		afx_msg void OnViewEdges3D();
		afx_msg void OnUpdateViewEdges3D(CCmdUI* pCmdUI);

		afx_msg void OnDisplayThickEdges();
		afx_msg void OnUpdateDisplayThickEdges(CCmdUI* pCmdUI);

		afx_msg void OnViewFaces3D();
		afx_msg void OnUpdateViewFaces3D(CCmdUI* pCmdUI);

		afx_msg void OnViewTriangulatedFaces3D();
		afx_msg void OnUpdateViewTriangulatedFaces3D(CCmdUI* pCmdUI);

		afx_msg void OnViewFaceNormals3D();
		afx_msg void OnUpdateViewFaceNormals3D(CCmdUI* pCmdUI);

		//Mouse
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnTimer(UINT_PTR nIDEvent);
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WinRf_ViewSketch2D.cpp
inline CReferDoc* CWinRf_ViewModelBrep::GetDocument()
   { return (CReferDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(VIEWMODELBREP_DEFINED)
