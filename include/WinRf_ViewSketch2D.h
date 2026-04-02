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

// WinRf_ViewSketch2D.h : interface of the CWinRf_ViewSketch2D class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWSKETCH2D_INCLUDED)
	#define AFX_VIEWSKETCH2D_INCLUDED

	#if _MSC_VER >= 1000
	#pragma once
	#endif // _MSC_VER >= 1000

	enum TMouseMode {InactiveMode, 
					 SketchMode, LineMode, 
					 ZoomMode, PanMode,
					 SelectMode, EraseMode};

	class CWinRf_ViewSketch2D : public CScrollView
	{
		protected: // create from serialization only
			CWinRf_ViewSketch2D();
			DECLARE_DYNCREATE(CWinRf_ViewSketch2D)

		// Attributes
		private:
			CReferDoc* pDoc;
			CReferDoc* GetDocument();

		private:
			//Parameters and functions to navigate the virtual paper
			double CameraZoom;
			double CameraPanX, CameraPanY;
			double ViewportWide, ViewportHigh;
			double WindowLength, WindowHigh;

			void ResetViewport();
			void Get2DModelBoundingBox(double &wXmin, double &wYmin, double &wXmax, double &wYmax);
			void UpdateWindow();
			void ResetWindow();

			cGDI_Transform Transf;

			//Parameters and functions to control mouse
			CPoint firstPoint, lastPoint;
			double StartTime;  // Starts measuring time since the buttondown
			TMouseMode MouseMode;

			CStroke CurrentStroke;
			RECTANGLE2D CurrentRect;

			COLORREF ColorStroke;
			COLORREF TextLabelsColor;

			double TextLabelsSizeScale;

			void AddPointToStroke(CPoint point, double CurrentTime);

		private:
			//Auxiliar functions
			CDialogPreviousNext *pPrevNextPE;
			CDialogPreviousNext *pPrevNextVP;
			CDialogPreviousNext *pPrevNextSG;
			CDialogPreviousNext *pPrevNextPer;
			CDialogPreviousNext *pPrevNextFace;
			CDialogPreviousNext *pPrevNextSP;
			CDialogPreviousNext *pPrevNextTree;
			CDialogPreviousNext *pPrevNextSym;

			void EllipseToBezier(double left, double right, double bottom, double top,
								 std::vector <POINT2D> &E);
			void UnitAngleArcToBezier(double angleA, double angleB,
									  POINT2D (&quadE)[4]);
			void EllipseArcToBezier(double Ra, double Rb, 
									double StartAngle, double EndAngle,
									std::vector <POINT2D> &E);
			void DisplayDot(CClientDC *dc, CPoint Position, long size);

			POINT2D GetDisplacedPoint(POINT2D p1, POINT2D p2,
										   double d);

			long RemoveLines(CPoint point);
			bool GetLine();

			//Parameters and functions to display models
			bool DisplayInputSketch;
			bool DisplayWorkingSketch;
			bool DisplayOnLine;
			bool DisplayOffLine;
			bool DisplayModelBrep;

			long SelectedNodes; //0 displays original nodes, 1 displays resampled nodes
			bool Display_Corners;
			bool Display_Line_Tolerance;
			bool Display_Arc_Tolerance;
			bool Display_Convex_Hull;
			bool Display_Merits_Strokes;

			bool DisplayNodesAndCorners;
			bool DisplaySlidingStrips;
			bool DisplayToleranceBands;

			bool DisplayInBlack;

			bool DisplayNumStrokes;

			bool DisplayNumVertices;
			bool DisplayMeritsOfVertices;
			bool DisplayCoordVertices;
			bool DisplayNumEdges;

			bool DisplayDashedStrokes;

			void GDI_DrawDashedStrokes();
			void GDI_DrawSubGraphsInSequence();

			void GDI_DrawBackGroundImage();
			void GDI_Draw2DSketch(CDB_Sketch *DB_Sketch);
			void GDI_Draw2DDrawing(CDB_Model *DDBB, int R, int G, int B);

			//Parameters and functions to display cues
			bool DisplaySubGraphsInSequence;
			long NumSubGraphInSequence;

			bool DisplayValences;
			bool DisplayVertexLabelling;
			bool DisplayLabelsEdges;
			bool DisplayPerimeterInSequence;
			long NumPerimeterInSequence;
			bool DisplayParallelEdges;
			bool DisplayPEInSequence;
			long NumPEInSequence;
			bool DisplayVanishingPoints;
			bool DisplayVPInSequence;
			long NumVPInSequence;
			bool DisplayMainAxes;
			bool DisplayFaces;
			bool DisplayFacesInSequence;
			long NumFaceInSequence;
			bool DisplayHiddenEdges;
			bool DisplayHiddenArcs;
			bool DisplayLevels;
			bool DisplaySymmetryAxesInFaces;
			bool DisplaySymmetryPlanes;
			bool DisplaySymmetryPlanesInSequence;
			long NumSymmetryPlaneInSequence;
			bool DisplaySymmetrySubGraphsInSequence;
			long NumSymmetrySubGraphInSequence;
			std::vector<CDB_Model> SymmetrySubGraphs;

			bool DisplayNormalon;

			bool DisplayTree;
			long SelectedTree; //0 displays axonometric tree, 1 displays levels tree
			long NumBranches;

			void GDI_DrawLabels(CDB_Model *DDBB_2D);
			void GDI_DrawVertexLabelling();
			void GDI_DrawLabelsEdges();
			void GDI_DrawPerimeterInSequence();
			void GDI_DrawParallelEdges();
			void GDI_DrawPEInSequence();
			void GDI_DrawVanishingPoints();
			void GDI_DrawVPInSequence();
			void GDI_DrawMainAxes();
			void GDI_DrawImplosionFaces();
			void GDI_DrawFacesInSequence();
			void GDI_DrawSymmetryPlanes();
			void GDI_DrawSymmetryPlanesInSequence();
			void GDI_DrawSymmetrySubGraphsInSequence();
			void GDI_DrawSymmetryAxesInFaces();

			void GDI_DrawNormalonLines(CDB_Model *DDBB);
			void GDI_DrawSpanningTree();

			void GDI_DrawNodesAndVertices(CDB_Sketch *DB_Sketch);

			//Parameters and functions to display features
			bool DisplayRibsAndDividers;
			bool DisplayRailsAndSlots;
			bool DisplayStepsAndPockets;
			bool DisplayChamfersAndRounds;    //TBD ChamfersAndRounds

			double ScaleFeatureLabels;
			bool DisplayNamesOfFeatures;
			bool DisplayMeritsOfFeatures;
			bool DisplayContainerFaces;

			void GDI_DrawFeatures(long numFeatures,
								  std::vector<long> Faces,
								  std::vector<double> Merits,
								  std::vector<long> ContainerFaces,
								  CString label,
								  long RedH, long GreenH, long BlueH,
								  long RedL, long GreenL, long BlueL);
			void GDI_DrawRibsAndDividers();
			void GDI_DrawRailsAndSlots();
			void GDI_DrawStepsAndPockets();
			void GDI_DrawChamfersAndRounds();    //TBD ChamfersAndRounds

			void GetRectangularStrip(double A, double B, double sinAlpha, double cosAlpha,
									 double CentroidX, double CentroidY, CPoint* P);
			void GDI_DrawSlidingStrips(CDB_Sketch *DB_Sketch);
			void GDI_DrawToleranceBands(CDB_Sketch *DB_Sketch);
			void project3DModel(CDB_Model *DB_Brep_Model_2D);
			void GDI_DrawSketch2D();

			//Parameters and functions to edit sketches
			long Eraser_Radius;
			long Eraser_Type;

			std::vector<long> Selection;
			bool LiangBarsky (RECTANGLE2D Limit,
							  POINT2D src0, POINT2D src1);
			POINT2D GetDisplacement();

			bool SelectStrokes(CPoint point, std::vector<long> &Selection);
			bool SelectStrokes(RECTANGLE2D rect, std::vector<long> &Selection);

			void GetErasedSketch(CPoint EraserLocation);
			void ManageEraserMove();

			void Distort2DDrawing();
			void Rotate2DSketch(double angle);
			void Rotate2DDrawing(double angle);

			void EagerVectorizationUpdate();
			void EagerVectorizeStroke(CStroke &InputStroke);

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CWinRf_ViewSketch2D)
		protected:
			virtual void OnDraw(CDC* pDC);  // overridden to draw this view
			virtual void OnInitialUpdate(); // called first time after construct
			virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
			//}}AFX_VIRTUAL

		// Implementation
		protected:
			virtual ~CWinRf_ViewSketch2D();
			//#ifdef _DEBUG
			//	virtual void AssertValid() const;
			//	virtual void Dump(CDumpContext& dc) const;
			//#endif

			// Generated message map functions
			//{{AFX_MSG(CWinRf_ViewSketch2D)
			//Window
			afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
			afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg void OnSetFocus(CWnd* pOldWnd);
			afx_msg BOOL OnEraseBkgnd(CDC* pDC);
			afx_msg void OnDestroy();

			//Mouse
			afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
			afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
			afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
			afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
			afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
			afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
			afx_msg void OnMouseMove(UINT nFlags, CPoint point);
			afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
			afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

			//Keyboard
			afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

			//Toolbar Sketch2D
			afx_msg void OnDisplayInputSketch();
			afx_msg void OnUpdateDisplayInputSketch(CCmdUI* pCmdUI);
			afx_msg void OnDisplayWorkingSketch();
			afx_msg void OnUpdateDisplayWorkingSketch(CCmdUI* pCmdUI);
			afx_msg void OnDisplayDrawing2DOnLine();
			afx_msg void OnUpdateDisplayDrawing2DOnLine(CCmdUI* pCmdUI);
			afx_msg void OnDisplayDrawing2DOffLine();
			afx_msg void OnUpdateDisplayDrawing2DOffLine(CCmdUI* pCmdUI);
			afx_msg void OnDisplayModelBrep();
			afx_msg void OnUpdateDisplayModelBrep(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNodesAndCorners();
			afx_msg void OnUpdateDisplayNodesAndCorners(CCmdUI* pCmdUI);
			afx_msg void OnDisplaySlidingStrips();
			afx_msg void OnUpdateDisplaySlidingStrips(CCmdUI* pCmdUI);
			afx_msg void OnDisplayToleranceBands();
			afx_msg void OnUpdateDisplayToleranceBands(CCmdUI* pCmdUI);
			afx_msg void OnToolsDisplayStrokes();
			afx_msg void OnUpdateToolsDisplayStrokes(CCmdUI* pCmdUI);

			afx_msg void OnZoom100();

			afx_msg void OnDisplayInBlack();
			afx_msg void OnUpdateDisplayInBlack(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNumStrokes();
			afx_msg void OnUpdateDisplayNumStrokes(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNumVertices();
			afx_msg void OnUpdateDisplayNumVertices(CCmdUI* pCmdUI);
			afx_msg void OnDisplayMeritsOfVertices();
			afx_msg void OnUpdateDisplayMeritsOfVertices(CCmdUI* pCmdUI);
			afx_msg void OnDisplayCoordinates();
			afx_msg void OnUpdateDisplayCoordinates(CCmdUI* pCmdUI);

			afx_msg void OnSelectLabelsColor();
			afx_msg void OnSelectLabelsFont();

			afx_msg void OnDisplayNumedges();
			afx_msg void OnUpdateDisplayNumedges(CCmdUI* pCmdUI);

			//Toolbar Sketch2D (Cues)
			afx_msg void OnDisplayDashedStrokes();
			afx_msg void OnUpdateDisplayDashedStrokes(CCmdUI* pCmdUI);
			afx_msg void OnDisplaySubGraphsInSequence();
			afx_msg void OnUpdateDisplaySubGraphsInSequence(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNextSubGraphInSequence();
			afx_msg void OnDisplayPreviousSubGraphInSequence();
			afx_msg void OnDisplayValences();
			afx_msg void OnUpdateDisplayValences(CCmdUI* pCmdUI);
			afx_msg void OnDisplayLabelsVertices();
			afx_msg void OnUpdateDisplayLabelsVertices(CCmdUI* pCmdUI);
			afx_msg void OnDisplayLabelsEdges();
			afx_msg void OnUpdateDisplayLabelsEdges(CCmdUI* pCmdUI);
			afx_msg void OnDisplayPerimeterInSequence();
			afx_msg void OnUpdateDisplayPerimeterInSequence(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNextPerimeterInSequence();
			afx_msg void OnDisplayPreviousPerimeterInSequence();
			afx_msg void OnDisplayParallelEdges();
			afx_msg void OnUpdateDisplayParallelEdges(CCmdUI* pCmdUI);
			afx_msg void OnDisplayPEInSequence();
			afx_msg void OnUpdateDisplayPEInSequence(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNextPEInSequence();
			afx_msg void OnDisplayPreviousPEInSequence();
			afx_msg void OnDisplayVanishingPoints();
			afx_msg void OnUpdateDisplayVanishingPoints(CCmdUI* pCmdUI);
			afx_msg void OnDisplayVPInSequence();
			afx_msg void OnUpdateDisplayVPInSequence(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNextVPInSequence();
			afx_msg void OnDisplayPreviousVPInSequence();
			afx_msg void OnDisplayMainAxes();
			afx_msg void OnUpdateDisplayMainAxes(CCmdUI* pCmdUI);
			afx_msg void OnDisplayImplosionFaces();
			afx_msg void OnUpdateDisplayImplosionFaces(CCmdUI* pCmdUI);
			afx_msg void OnDisplayFacesInSequence();
			afx_msg void OnUpdateDisplayFacesInSequence(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNextFaceInSequence();
			afx_msg void OnDisplayPreviousFaceInSequence();
			afx_msg void OnDisplayHiddenEdges();
			afx_msg void OnUpdateDisplayHiddenEdges(CCmdUI* pCmdUI);
			afx_msg void OnDisplayLevels();
			afx_msg void OnUpdateDisplayLevels(CCmdUI* pCmdUI);
			afx_msg void OnDisplaySymmetryPlanes();
			afx_msg void OnUpdateDisplaySymmetryPlanes(CCmdUI* pCmdUI);
			afx_msg void OnDisplaySymmetryPlanesInSequence();
			afx_msg void OnUpdateDisplaySymmetryPlanesInSequence(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNextSymmetryPlaneInSequence();
			afx_msg void OnDisplayPreviousSymmetryPlaneInSequence();
			afx_msg void OnDisplaySymmetrySubGraphsInSequence();
			afx_msg void OnUpdateDisplaySymmetrySubGraphsInSequence(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNextSymmetrySubGraphInSequence();
			afx_msg void OnDisplayPreviousSymmetrySubGraphInSequence();
			afx_msg void OnDisplaySymmetryAxesInFaces();
			afx_msg void OnUpdateDisplaySymmetryAxesInFaces(CCmdUI* pCmdUI);
			afx_msg void OnDisplayNormalon();
			afx_msg void OnUpdateDisplayNormalon(CCmdUI* pCmdUI);
			afx_msg void OnDisplayTree();
			afx_msg void OnUpdateDisplayTree(CCmdUI* pCmdUI);
			afx_msg void OnTreeForward();
			afx_msg void OnTreeBackward();
			afx_msg void OnSelectTree();
			afx_msg void OnUpdateSelectTree(CCmdUI* pCmdUI);

			//Toolbar View Features
			afx_msg void OnDisplayRibsAndDividers();
			afx_msg void OnUpdateDisplayRibsAndDividers(CCmdUI* pCmdUI);
			afx_msg void OnDisplayRailsAndSlots();
			afx_msg void OnUpdateDisplayRailsAndSlots(CCmdUI* pCmdUI);
			afx_msg void OnDisplayStepsAndPockets();
			afx_msg void OnUpdateDisplayStepsAndPockets(CCmdUI* pCmdUI);
			afx_msg void OnDisplayChamfersAndRounds();    //TBD ChamfersAndRounds
			afx_msg void OnUpdateDisplayChamfersAndRounds(CCmdUI* pCmdUI);    //TBD ChamfersAndRounds
			afx_msg void OnConfigDisplayFeatures();
			afx_msg void OnUpdateConfigDisplayFeatures(CCmdUI* pCmdUI);

			//Toolbar Modes
			afx_msg void OnSketchStrokes();
			afx_msg void OnUpdateSketchStrokes(CCmdUI* pCmdUI);
			afx_msg void OnSelectStrokes();
			afx_msg void OnUpdateSelectStrokes(CCmdUI* pCmdUI);
			afx_msg void OnDrawLines();
			afx_msg void OnUpdateDrawLines(CCmdUI* pCmdUI);
			afx_msg void OnEraseStrokes();
			afx_msg void OnUpdateEraseStrokes(CCmdUI* pCmdUI);
			afx_msg void OnEraseStrokesTools();
			afx_msg void OnUpdateEraseStrokesTools(CCmdUI* pCmdUI);

			//Toolbar Edition
			afx_msg void OnTranslateStrokes();
			afx_msg void OnUpdateTranslateStrokes(CCmdUI* pCmdUI);
			afx_msg void OnCopyStrokes();
			afx_msg void OnUpdateCopyStrokes(CCmdUI* pCmdUI);
			afx_msg void OnRotateStrokes();
			afx_msg void OnUpdateRotateStrokes(CCmdUI* pCmdUI);
			afx_msg void OnScaleStrokes();
			afx_msg void OnUpdateScaleStrokes(CCmdUI* pCmdUI);
			afx_msg void OnFilterStrokes();
			afx_msg void OnUpdateFilterStrokes(CCmdUI* pCmdUI);

			afx_msg void OnRotate2Ddrawing();
			afx_msg void OnUpdateRotate2Ddrawing(CCmdUI* pCmdUI);
			afx_msg void OnDistort2DDrawing();
			afx_msg void OnUpdateDistort2DDrawing(CCmdUI* pCmdUI);

			//}}AFX_MSG
			afx_msg LRESULT OnSetSelection(WPARAM n1, LPARAM n2);
			afx_msg LRESULT OnGetSelection(WPARAM wparam, LPARAM lparam);
			afx_msg LRESULT OnGetPixelSize(WPARAM wparam, LPARAM lparam);
			DECLARE_MESSAGE_MAP()
	};

	#ifndef _DEBUG  // debug version in WinRf_ViewSketch2D.cpp
	inline CReferDoc* CWinRf_ViewSketch2D::GetDocument()
	   { return (CReferDoc*)m_pDocument; }
	#endif

	/////////////////////////////////////////////////////////////////////////////

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEWSKETCH2D_INCLUDED)
