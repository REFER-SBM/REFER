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

// ReferDoc.h : interface of the CReferDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFERDOC_INCLUDED_)
#define AFX_REFERDOC_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// External classes are declared to avoid using #includes
class CWinRf_ViewSketch2D;
class CWinRf_ViewModelBrep;
class CWinRf_ViewModelCSG;
class CWinRf_ViewOptimization;

class CDB_Sketch;
class CDB_CSG;


class CReferDoc : public CDocument
{
	protected:
		CReferDoc();
		DECLARE_DYNCREATE(CReferDoc)

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CReferDoc)
	public:
		virtual BOOL OnNewDocument();
		virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
		virtual ~CReferDoc();

		//#ifdef _DEBUG
		//	virtual void AssertValid() const;
		//	virtual void Dump(CDumpContext& dc) const;
		//#endif
		//}}AFX_VIRTUAL

	public:
		static CReferDoc* GetDoc();

		CMDIChildWnd* m_pFrameSketch2D;
		CMDIChildWnd* m_pFrameModelBrep;
		CMDIChildWnd* m_pFrameModelCSG;
		CMDIChildWnd* m_pFrameOptimization;

		CWinRf_ViewSketch2D* m_pViewSketch2D;
		CWinRf_ViewModelCSG* m_pViewModelCSG;

	// Implementation

	public:
		void RefreshWindows();
		void RefreshWindowSketch2D();
		void RefreshWindowModelBrep();
		void RefreshWindowModelCSG();
		void RefreshWindowOptimization();

		void UpdateStatusBar(CString sName);
		void UpdateStatusBar(CDB_Model* DDBB);

	public:
		//Databases are defined here to allow multithread
		//(multiple documents sharing the same execution)

		//Sketch databases
		CDB_Sketch* DB_InputSketch;
		CDB_Sketch* DB_WorkingSketch;

		//LineDrawing databases
		CDB_Model* DB_DrawingOn;   //With online refinements
		CDB_Model* DB_DrawingOff;  //With offline refinements

		//B-Rep model databases
		CDB_Model* DB_Brep_Model;
		CDB_Model* DB_Brep_MainAxes;  //Auxiliar database for main axis oriented BRep models
		CDB_Model* DB_Brep_Inflation; //Auxiliary database to store inflation intermediate models

		//CSG model databases
		CDB_CSG* DB_CSG_Model;                  //Procedural model
		//CSG internal information made`public here for visualization
		std::vector <CDB_Model> DB_Deconstruct; //Group of deconstructed drawings
		CDB_Model* DB_DatumGraph;  //Graph to calculate datum distances and profiles

	// Generated message map functions
	protected:
		//{{AFX_MSG(CReferDoc)

		void printFeatures(CString NombreFichero, int PosicionPunto);
		void printDatumGraph(CString NombreFichero, int PosicionPunto);

		//Files management
		afx_msg void OnAddDocument();
		afx_msg void OnReferDocSave();
		afx_msg void OnReferDocSaveAs();

		afx_msg void OnFileClose();

		//Windows management
		afx_msg void OnViewSketch2D();
		afx_msg void OnUpdateViewSketch2D(CCmdUI* pCmdUI);

		afx_msg void OnViewModelBrep();
		afx_msg void OnUpdateViewModelBrep(CCmdUI* pCmdUI);

		afx_msg void OnViewModelCSG();
		afx_msg void OnUpdateViewModelCSG(CCmdUI* pCmdUI);

		afx_msg void OnViewOptimizacion();
		afx_msg void OnUpdateViewOptimizacion(CCmdUI* pCmdUI);

		afx_msg void OnSetBackgroundColor();

		afx_msg void OnWindowTileRectangle();

		//Reconstruction
		afx_msg void OnNewReconstruction3D();
		afx_msg void OnUpdateNewReconstruction3D(CCmdUI* pCmdUI);

		afx_msg void OnUndoReconstruction3D();
		afx_msg void OnUpdateUndoReconstruction3D(CCmdUI* pCmdUI);

		//Cues
		afx_msg void OnCueGraphs();
		afx_msg void OnUpdateCueGraphs(CCmdUI* pCmdUI);

		afx_msg void OnCueLabels();
		afx_msg void OnUpdateCueLabels(CCmdUI* pCmdUI);

		afx_msg void OnCueValences();
		afx_msg void OnUpdateCueValences(CCmdUI* pCmdUI);

		afx_msg void OnCuePerimeter();
		afx_msg void OnUpdateCuePerimeter(CCmdUI* pCmdUI);

		afx_msg void OnCueParallelEdges();
		afx_msg void OnUpdateCueParallelEdges(CCmdUI* pCmdUI);

		afx_msg void OnCueVanishingPoints();
		afx_msg void OnUpdateCueVanishingPoints(CCmdUI* pCmdUI);

		afx_msg void OnCueMainAxes();
		afx_msg void OnUpdateCueMainAxes(CCmdUI* pCmdUI);

		afx_msg void OnCueTypology();
		afx_msg void OnUpdateCueTypology(CCmdUI* pCmdUI);

		afx_msg void OnCueFaces();
		afx_msg void OnUpdateCueFaces(CCmdUI* pCmdUI);

		afx_msg void OnCueHiddenEdges();
		afx_msg void OnUpdateCueHiddenEdges(CCmdUI* pCmdUI);

		afx_msg void OnCueLevels();
		afx_msg void OnUpdateCueLevels(CCmdUI* pCmdUI);

		afx_msg void OnCueSymmetry();
		afx_msg void OnUpdateCueSymmetry(CCmdUI* pCmdUI);

		//Features
		afx_msg void OnFeatureRibsAndDividers();
		afx_msg void OnUpdateFeatureRibsAndDividers(CCmdUI* pCmdUI);

		afx_msg void OnFeatureSlotsAndRails();
		afx_msg void OnUpdateFeatureSlotsAndRails(CCmdUI* pCmdUI);

		afx_msg void OnFeatureStepsAndPockets();
		afx_msg void OnUpdateFeatureStepsAndPockets(CCmdUI* pCmdUI);

		afx_msg void OnFeatureChamfersAndRounds();    //TBD ChamfersAndRounds
		afx_msg void OnUpdateFeatureChamfersAndRounds(CCmdUI* pCmdUI);    //TBD ChamfersAndRounds

		//Vectorization
		afx_msg void OnVectorizeInt();
		afx_msg void OnGetVectorizeInt(CCmdUI* pCmdUI);

		afx_msg void OnVectorizeBatch();
		afx_msg void OnGetVectorizeBatch(CCmdUI* pCmdUI);

		afx_msg void OnConfigVectorize();
		afx_msg void OnUpdateConfigVectorize(CCmdUI* pCmdUI);

		//2D Refinement
		afx_msg void OnRefine2DOnLine();
		afx_msg void OnUpdateRefine2DOnLine(CCmdUI* pCmdUI);

		afx_msg void OnRefine2DOffLine();
		afx_msg void OnUpdateRefine2DOffLine(CCmdUI* pCmdUI);

		afx_msg void OnConfigRefine2D();
		afx_msg void OnUpdateConfigRefine2D(CCmdUI* pCmdUI);

		//Detection
		afx_msg void OnDetectCuesAndFeatures();
		afx_msg void OnUpdateDetectCuesAndFeatures(CCmdUI* pCmdUI);

		afx_msg void OnEraseCuesAndFeatures();
		afx_msg void OnUpdateEraseCuesAndFeatures(CCmdUI* pCmdUI);

		afx_msg void OnConfigCuesAndFeatures();
		afx_msg void OnUpdateConfigCuesAndFeatures(CCmdUI* pCmdUI);

		afx_msg void OnConfigCues();
		afx_msg void OnUpdateConfigCues(CCmdUI* pCmdUI);

		afx_msg void OnConfigFeatures();
		afx_msg void OnUpdateConfigFeatures(CCmdUI* pCmdUI);

		//Inflation
		afx_msg void OnNewInflation();
		afx_msg void OnUpdateNewInflation(CCmdUI* pCmdUI);

		afx_msg void OnIncrementalInflation();
		afx_msg void OnUpdateIncrementalInflation(CCmdUI* pCmdUI);

		afx_msg void OnEraseInflation();
		afx_msg void OnUpdateEraseInflation(CCmdUI* pCmdUI);

		afx_msg void OnConfigInflation();
		afx_msg void OnUpdateConfigInflation(CCmdUI* pCmdUI);

		//3D Refinement
		afx_msg void OnRefine3D();
		afx_msg void OnUpdateRefine3D(CCmdUI* pCmdUI);

		afx_msg void OnRefine3D_Faces();
		afx_msg void OnUpdateRefine3D_Faces(CCmdUI* pCmdUI);

		afx_msg void OnAlign3D_Faces();
		afx_msg void OnUpdateAlign3D_Faces(CCmdUI* pCmdUI);

		afx_msg void OnConfigRefine3D();
		afx_msg void OnUpdateConfigRefine3D(CCmdUI* pCmdUI);

		//3D Tessellation
		afx_msg void OnTessellate3D();
		afx_msg void OnUpdateTessellate3D(CCmdUI* pCmdUI);

		afx_msg void OnFaces_Outward();
		afx_msg void OnUpdateFaces_Outward(CCmdUI* pCmdUI);

		afx_msg void OnTriangulate_Faces();
		afx_msg void OnUpdateTriangulate_Faces(CCmdUI* pCmdUI);

		afx_msg void OnNormals_To_Faces();
		afx_msg void OnUpdateNormals_To_Faces(CCmdUI* pCmdUI);

		afx_msg void OnConfigTessellate3D();
		afx_msg void OnUpdateConfigTessellate3D(CCmdUI* pCmdUI);

		//CSG
		afx_msg void OnCSG_Build();
		afx_msg void OnUpdateCSG_Build(CCmdUI* pCmdUI);

		afx_msg void OnEraseCSG();
		afx_msg void OnUpdateEraseCSG(CCmdUI* pCmdUI);

		afx_msg void OnConfigCSG();
		afx_msg void OnUpdateConfigCSG(CCmdUI* pCmdUI);

		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REFERDOC_INCLUDED_)
