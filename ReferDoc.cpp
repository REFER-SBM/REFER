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

// Implementation of the CReferDoc class
//

#include <stdafx.h>
#include <cfloat>
#include <cassert>
#include <string>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"

#include "WinRf_MainFrame.h"
#include "InOutDXF.h"
#include "InOutRibald.h"
#include "InOutSTL.h"
#include "InOutOBJ.h"
#include "InOutCSG.h"
#include "InOutIGES.h"
//#include "InOutSTEP.h"

#include "DB_Sketch.h"
#include "InOutSketch.h"

#include "ReferDoc.h"

#include "DialogConfigVectorize2D.h"
#include "DialogConfigDetectCuesFeatures.h"
#include "DialogConfigCues.h"
#include "DialogConfigFeatures.h"
#include "DialogConfigRefine2D.h"
#include "DialogConfigInflation.h"
#include "DialogConfigDirectInflation.h"
#include "DialogConfigOptimizationInflation.h"
#include "DialogConfigRefine3D.h"
#include "DialogConfigTessellate3D.h"
#include "DialogConfigCSG.h"

#include "Vectorization.h"
#include "Refine2D.h"
#include "DetectionManager.h"
#include "ReconstructionManager.h"
#include "DirectInflation.h"
#include "DialogProgress.h"
#include "OptimizationBasedInflation.h"

/////////////////////////////////////////////////////////////////////////////
// CReferDoc

IMPLEMENT_DYNCREATE(CReferDoc, CDocument)

BEGIN_MESSAGE_MAP(CReferDoc, CDocument)
	//{{AFX_MSG_MAP(CReferDoc)

	//Files management
	ON_COMMAND(ID_REFER_ADD, OnAddDocument)
	ON_COMMAND(ID_REFER_SAVE, OnReferDocSave)
	ON_COMMAND(ID_REFER_SAVE_AS, OnReferDocSaveAs)

	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)

	//Windows management
	ON_COMMAND(ID_WINDOW_2D_SKETCH, OnViewSketch2D)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_2D_SKETCH, OnUpdateViewSketch2D)

	ON_COMMAND(ID_WINDOW_BREP_MODEL, OnViewModelBrep)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_BREP_MODEL, OnUpdateViewModelBrep)

	ON_COMMAND(ID_WINDOW_CSG_MODEL, OnViewModelCSG)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CSG_MODEL, OnUpdateViewModelCSG)

	ON_COMMAND(ID_WINDOW_OPTIMIZATION, OnViewOptimizacion)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_OPTIMIZATION, OnUpdateViewOptimizacion)

	ON_COMMAND(ID_WINDOW_BACKGROUNDCOLOR, OnSetBackgroundColor)

	ON_COMMAND(ID_WINDOW_TILE_RECTANGLE, OnWindowTileRectangle)

	//Reconstruction
	ON_COMMAND(ID_RECONSTRUCTION3D_NEW, OnNewReconstruction3D)
	ON_UPDATE_COMMAND_UI(ID_RECONSTRUCTION3D_NEW, OnUpdateNewReconstruction3D)

	ON_COMMAND(ID_RECONSTRUCTION3D_UNDO, OnUndoReconstruction3D)
	ON_UPDATE_COMMAND_UI(ID_RECONSTRUCTION3D_UNDO, OnUpdateUndoReconstruction3D)

	//Vectorization
	ON_COMMAND(ID_VECTORIZE_INT, OnVectorizeInt)
	ON_UPDATE_COMMAND_UI(ID_VECTORIZE_INT, OnGetVectorizeInt)

	ON_COMMAND(ID_VECTORIZE_BATCH, OnVectorizeBatch)
	ON_UPDATE_COMMAND_UI(ID_VECTORIZE_BATCH, OnGetVectorizeBatch)

	ON_COMMAND(ID_VECTORIZE_CONFIG, OnConfigVectorize)
	ON_UPDATE_COMMAND_UI(ID_VECTORIZE_CONFIG, OnUpdateConfigVectorize)

	//2D Refinement
	ON_COMMAND(ID_REFINE2D_ONLINE, OnRefine2DOnLine)
	ON_UPDATE_COMMAND_UI(ID_REFINE2D_ONLINE, OnUpdateRefine2DOnLine)

	ON_COMMAND(ID_REFINE2D_OFFLINE, OnRefine2DOffLine)
	ON_UPDATE_COMMAND_UI(ID_REFINE2D_OFFLINE, OnUpdateRefine2DOffLine)

	ON_COMMAND(ID_REFINE_2D_CONFIG, OnConfigRefine2D)
	ON_UPDATE_COMMAND_UI(ID_REFINE_2D_CONFIG, OnUpdateConfigRefine2D)

	//Detection
	ON_COMMAND(ID_DETECT_CUES_FEATURES, OnDetectCuesAndFeatures)
	ON_UPDATE_COMMAND_UI(ID_DETECT_CUES_FEATURES, OnUpdateDetectCuesAndFeatures)

	ON_COMMAND(ID_ERASE_CUES_FEATURES, OnEraseCuesAndFeatures)
	ON_UPDATE_COMMAND_UI(ID_ERASE_CUES_FEATURES, OnUpdateEraseCuesAndFeatures)

	ON_COMMAND(ID_CONFIG_CUES_FEATURES, OnConfigCuesAndFeatures)
	ON_UPDATE_COMMAND_UI(ID_CONFIG_CUES_FEATURES, OnUpdateConfigCuesAndFeatures)

	//Cues
	ON_COMMAND(ID_CUE_VALENCES, OnCueValences)
	ON_UPDATE_COMMAND_UI(ID_CUE_VALENCES, OnUpdateCueValences)

	ON_COMMAND(ID_CUE_PARALLELEDGES, OnCueParallelEdges)
	ON_UPDATE_COMMAND_UI(ID_CUE_PARALLELEDGES, OnUpdateCueParallelEdges)

	ON_COMMAND(ID_CUE_VANISHINGPOINTS, OnCueVanishingPoints)
	ON_UPDATE_COMMAND_UI(ID_CUE_VANISHINGPOINTS, OnUpdateCueVanishingPoints)

	ON_COMMAND(ID_CUE_GRAPHS, OnCueGraphs)
	ON_UPDATE_COMMAND_UI(ID_CUE_GRAPHS, OnUpdateCueGraphs)

	ON_COMMAND(ID_CUE_PERIMETER, OnCuePerimeter)
	ON_UPDATE_COMMAND_UI(ID_CUE_PERIMETER, OnUpdateCuePerimeter)

	ON_COMMAND(ID_CUE_LABELS, OnCueLabels)
	ON_UPDATE_COMMAND_UI(ID_CUE_LABELS, OnUpdateCueLabels)

	ON_COMMAND(ID_CUE_MAIN_AXES, OnCueMainAxes)
	ON_UPDATE_COMMAND_UI(ID_CUE_MAIN_AXES, OnUpdateCueMainAxes)

	ON_COMMAND(ID_CUE_FACES, OnCueFaces)
	ON_UPDATE_COMMAND_UI(ID_CUE_FACES, OnUpdateCueFaces)

	ON_COMMAND(ID_CUE_TYPOLOGY, OnCueTypology)
	ON_UPDATE_COMMAND_UI(ID_CUE_TYPOLOGY, OnUpdateCueTypology)

	ON_COMMAND(ID_CUE_HIDDEN_EDGES, OnCueHiddenEdges)
	ON_UPDATE_COMMAND_UI(ID_CUE_HIDDEN_EDGES, OnUpdateCueHiddenEdges)

	ON_COMMAND(ID_CUE_SYMMETRY, OnCueSymmetry)
	ON_UPDATE_COMMAND_UI(ID_CUE_SYMMETRY, OnUpdateCueSymmetry)

	ON_COMMAND(ID_CUE_LEVELS, OnCueLevels)
	ON_UPDATE_COMMAND_UI(ID_CUE_LEVELS, OnUpdateCueLevels)

	ON_COMMAND(ID_CUE_CONFIG, OnConfigCues)
	ON_UPDATE_COMMAND_UI(ID_CUE_CONFIG, OnUpdateConfigCues)

	//Features
	ON_COMMAND(ID_FEATURE_RIBSANDDIVIDERS, OnFeatureRibsAndDividers)
	ON_UPDATE_COMMAND_UI(ID_FEATURE_RIBSANDDIVIDERS, OnUpdateFeatureRibsAndDividers)

	ON_COMMAND(ID_FEATURE_RAILSANDSLOTS, OnFeatureSlotsAndRails)
	ON_UPDATE_COMMAND_UI(ID_FEATURE_RAILSANDSLOTS, OnUpdateFeatureSlotsAndRails)

	ON_COMMAND(ID_FEATURE_STEPSANDPOCKETS, OnFeatureStepsAndPockets)
	ON_UPDATE_COMMAND_UI(ID_FEATURE_STEPSANDPOCKETS, OnUpdateFeatureStepsAndPockets)

	ON_COMMAND(ID_FEATURE_CHAMFERSANDROUNDS, OnFeatureChamfersAndRounds)    //TBD ChamfersAndRounds
	ON_UPDATE_COMMAND_UI(ID_FEATURE_CHAMFERSANDROUNDS, OnUpdateFeatureChamfersAndRounds)    //TBD ChamfersAndRounds

	ON_COMMAND(ID_FEATURE_CONFIG, OnConfigFeatures)
	ON_UPDATE_COMMAND_UI(ID_FEATURE_CONFIG, OnUpdateConfigFeatures)

	//Inflation
	ON_COMMAND(ID_INFLATION_NEW, OnNewInflation)
	ON_UPDATE_COMMAND_UI(ID_INFLATION_NEW, OnUpdateNewInflation)

	ON_COMMAND(ID_INFLATION_INCREMENTAL, OnIncrementalInflation)
	ON_UPDATE_COMMAND_UI(ID_INFLATION_INCREMENTAL, OnUpdateIncrementalInflation)

	ON_COMMAND(ID_INFLATION_ERASE, OnEraseInflation)
	ON_UPDATE_COMMAND_UI(ID_INFLATION_ERASE, OnUpdateEraseInflation)

	ON_COMMAND(ID_INFLATION_CONFIG, OnConfigInflation)
	ON_UPDATE_COMMAND_UI(ID_INFLATION_CONFIG, OnUpdateConfigInflation)

	//3D Refinement
	ON_COMMAND(ID_REFINE3D, OnRefine3D)
	ON_UPDATE_COMMAND_UI(ID_REFINE3D, OnUpdateRefine3D)

	ON_COMMAND(ID_REFINE3D_FACES, OnRefine3D_Faces)
	ON_UPDATE_COMMAND_UI(ID_REFINE3D_FACES, OnUpdateRefine3D_Faces)

	ON_COMMAND(ID_ALIGN3D_FACES, OnAlign3D_Faces)
	ON_UPDATE_COMMAND_UI(ID_ALIGN3D_FACES, OnUpdateAlign3D_Faces)

	ON_COMMAND(ID_REFINE3D_CONFIG, OnConfigRefine3D)
	ON_UPDATE_COMMAND_UI(ID_REFINE3D_CONFIG, OnUpdateConfigRefine3D)

	//3D Tessellate
	ON_COMMAND(ID_TESSELLATE3D, OnTessellate3D)
	ON_UPDATE_COMMAND_UI(ID_TESSELLATE3D, OnUpdateTessellate3D)

	ON_COMMAND(ID_FACES_OUTWARD, OnFaces_Outward)
	ON_UPDATE_COMMAND_UI(ID_FACES_OUTWARD, OnUpdateFaces_Outward)

	ON_COMMAND(ID_TRIANGULATE_FACES, OnTriangulate_Faces)
	ON_UPDATE_COMMAND_UI(ID_TRIANGULATE_FACES, OnUpdateTriangulate_Faces)

	ON_COMMAND(ID_NORMALS_TO_FACES, OnNormals_To_Faces)
	ON_UPDATE_COMMAND_UI(ID_NORMALS_TO_FACES, OnUpdateNormals_To_Faces)

	ON_COMMAND(ID_TESSELLATE3D_CONFIG, OnConfigTessellate3D)
	ON_UPDATE_COMMAND_UI(ID_TESSELLATE3D_CONFIG, OnUpdateConfigTessellate3D)

	//CSG
	ON_COMMAND(ID_CSG_BUILD, OnCSG_Build)
	ON_UPDATE_COMMAND_UI(ID_CSG_BUILD, OnUpdateCSG_Build)

	ON_COMMAND(ID_CSG_ERASE, OnEraseCSG)
	ON_UPDATE_COMMAND_UI(ID_CSG_ERASE, OnUpdateEraseCSG)

	ON_COMMAND(ID_CSG_CONFIG, OnConfigCSG)
	ON_UPDATE_COMMAND_UI(ID_CSG_CONFIG, OnUpdateConfigCSG)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReferDoc construction/destruction

CReferDoc::CReferDoc()
{
	// Create DataBases
	DB_InputSketch= NULL;   //Create only in SketchMode
	DB_WorkingSketch= NULL;   //Create only in SketchMode
	DB_DrawingOn= new CDB_Model;
	DB_DrawingOff= new CDB_Model;

	DB_Brep_Model= new CDB_Model;
	DB_Brep_MainAxes= new CDB_Model;
	DB_Brep_Inflation= new CDB_Model;

	DB_CSG_Model= new CDB_CSG;
	DB_DatumGraph= new CDB_Model;

	// Initialize parameters
	m_pFrameSketch2D= NULL;
	m_pFrameModelBrep= NULL;
	m_pFrameModelCSG= NULL;
	m_pFrameOptimization= NULL;

	ReferApp.SketchMode= false;
	ReferApp.BackgroundBitMap= false;

	ReferApp.m_BackgroundColor = RGB(255,255,255);  //Bright white
	ReferApp.m_sTimeCalculating = "";
}


CReferDoc::~CReferDoc()
{
	//Destroy DataBases
	if (DB_InputSketch){
		delete DB_InputSketch;
		DB_InputSketch= NULL;
	}

	if (DB_WorkingSketch){
		delete DB_WorkingSketch;
		DB_WorkingSketch= NULL;
	}

	if (DB_DrawingOn){
		delete DB_DrawingOn;
		DB_DrawingOn= NULL;
	}

	if (DB_DrawingOff){
		delete DB_DrawingOff;
		DB_DrawingOff= NULL;
	}

	if (DB_Brep_Model){
		delete DB_Brep_Model;
		DB_Brep_Model= NULL;
	}

	if (DB_Brep_MainAxes){
		delete DB_Brep_MainAxes;
		DB_Brep_MainAxes= NULL;
	}

	if (DB_Brep_Inflation){
		delete DB_Brep_Inflation;
		DB_Brep_Inflation= NULL;
	}

	if (DB_CSG_Model){
		delete DB_CSG_Model;
		DB_CSG_Model= NULL;
	}
}


CReferDoc * CReferDoc::GetDoc()
//To Get Current CDocument from Anywhere
{
	CMDIChildWnd * pChild = ((CMDIFrameWnd*)(AfxGetApp()->m_pMainWnd))->MDIGetActive();

	if ( !pChild )
		return NULL;

	CDocument * pDoc = pChild->GetActiveDocument();

	if ( !pDoc )
		return NULL;

	// Fail if doc is of wrong kind
	if ( ! pDoc->IsKindOf( RUNTIME_CLASS(CReferDoc)))
		return NULL;

	return (CReferDoc *) pDoc;
}


//Messages inherited from base class/////////////////////////////////////

BOOL CReferDoc::OnNewDocument()
// Called by the framework as part of the File New command (See OnFileNew in Refer.cpp)

//When processing the NewDocument message,
//the program activates the Modeling 2D window in sketching mode
{
	//General NewDocument tasks are done first
	if (!CDocument::OnNewDocument()) return false;  // Creates the sketch window

	// Search the pointer to the frame and store it
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	ASSERT(pos != NULL);
	pView = GetNextView(pos);
	m_pFrameSketch2D = (CMDIChildWnd*) pView->GetParentFrame();

	//Sketching mode
	ReferApp.SketchMode= true;
	DB_InputSketch = new CDB_Sketch(); // Starts a new database to store the input sketch
	DB_WorkingSketch = new CDB_Sketch(); // Starts a new database to store the grouped and segmented sketch

	return true;
}


BOOL CReferDoc::OnOpenDocument(LPCTSTR lpszPathName) 
// Called by ReferApp::OnOpenDocument (See Refer.cpp)

//When processing the OpenDocument message,
//the program activates the 2D Sketch window in a mode that depends on the type of file:
//    sketching mode, if file extension is ".stk"
//    sketching mode plus background image, if file extension is ".bmp"
//    visualization mode, if file extension is ".dxf"
//    visualization mode plus input translation, if file extension is ".Sketch"
{
	//General OpenDocument tasks are done first
	if (!CDocument::OnOpenDocument(lpszPathName)) return FALSE;
	CString NombreFichero = lpszPathName;

//////------------------------------------------
////	//Dialog to select the file, by filtering the file types
////	CFileDialog dlg(TRUE); // Set to TRUE to construct a File Open dialog box or FALSE to construct a File Save As dialog box
////	dlg.m_ofn.lpstrTitle="Open file";
////	CString sMensaje;
////	sMensaje= "Refer sketch files(*.stk)||*.stk||";
////	sMensaje+= "Refer line-drawing files(*.dxf)||*.dxf||";
////	sMensaje+= "Varley's sketch files(*.Sketch)||*.Sketch||";
////	sMensaje+= "Bitmaps files(*.bmp)||*.bmp||";
////	sMensaje+= "All files(*.*)||*.*||||";
////	dlg.m_ofn.lpstrFilter= sMensaje;
////	if(dlg.DoModal() != IDOK) return FALSE;
////	CString NombreFichero = dlg.GetPathName(); // return full path and filename
//////------------------------------------------

	// Chooses between different types of files to load
	int PosicionPunto = NombreFichero.ReverseFind('.');
	if (NombreFichero.Mid(PosicionPunto+1, 3) == "stk"){
		// Search and store the pointer to the frame
		CView* pView;
		POSITION pos = GetFirstViewPosition();
		ASSERT(pos != NULL);
		pView = GetNextView(pos);
		m_pFrameSketch2D = (CMDIChildWnd*) pView->GetParentFrame();

		// Prepare Sketch DataBase
		//if (DB_InputSketch) delete DB_InputSketch;
		DB_InputSketch= new CDB_Sketch();
		//if (DB_WorkingSketch) delete DB_WorkingSketch;
		DB_WorkingSketch= new CDB_Sketch();

		// Load the sketch
		std::string file= NombreFichero;   //Converts the name of the file into a string
		CInOutSketch IO;
		IO.readSketch(file.c_str(), DB_InputSketch);

		//Sketching mode
		ReferApp.SketchMode= true;

		if(ReferApp.GeneralParams.bVectorize_Int){
			CVectorization V;
			V.VectorizeSketch(this);
		}

		//By default, tidy up this kind of input
		ReferApp.GeneralParams.bRefine2D_OffLine= true;
		ReferApp.R2D.bMergeVertices= true;
	}

	else if (NombreFichero.Mid(PosicionPunto+1, 3) == "bmp"){
		//Search and store the pointer to the frame
		CView* pView;
		POSITION pos = GetFirstViewPosition();
		ASSERT(pos != NULL);
		pView = GetNextView(pos);
		m_pFrameSketch2D = (CMDIChildWnd*) pView->GetParentFrame();

		//Load the bitmap file name
		ReferApp.BackgroundBitMapFile= NombreFichero;

		//Activate background bitmap mode
		ReferApp.BackgroundBitMap= true;

		//Sketching mode
		ReferApp.SketchMode= true;

		//Prepare Sketch DataBase
		//if (DB_InputSketch) delete DB_InputSketch;
		DB_InputSketch= new CDB_Sketch();
		//if (DB_WorkingSketch) delete DB_WorkingSketch;
		DB_WorkingSketch= new CDB_Sketch();

		//By default, do not tidy up this kind of input
		ReferApp.GeneralParams.bRefine2D_OffLine= false;
		ReferApp.R2D.bMergeVertices= false;
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 6) == "Sketch" ||
	        NombreFichero.Mid(PosicionPunto+1, 3) == "txt"){
		// Search and store the pointer to the frame
		CView* pView;
		POSITION pos = GetFirstViewPosition();
		ASSERT(pos != NULL);
		pView = GetNextView(pos);
		m_pFrameSketch2D = (CMDIChildWnd*) pView->GetParentFrame();

		// Load the line-drawing
		// ---------------------
		//Clear the model database
		DB_DrawingOn->Clear();
		DB_DrawingOff->Clear();
		DB_Brep_Model->Clear();

		//Load the new data base with the contents of the Ribald file "lpszPathName"
		InOutRibald fileRibald;
		std::string file= lpszPathName;   //Converts the Name of the file into a string
		if (!fileRibald.readRibald(file.c_str(), DB_DrawingOn))
			return false;
		*DB_DrawingOff= *DB_DrawingOn;
		*DB_Brep_Model= *DB_DrawingOn;

		//Do not start sketching mode
		ReferApp.SketchMode= false;
		DB_InputSketch= NULL;
		DB_WorkingSketch= NULL;
		//DB_InputSketch= new CDB_Sketch();
		//DB_WorkingSketch= new CDB_Sketch();

		//By default, tidy up this kind of input
		ReferApp.GeneralParams.bRefine2D_OffLine= true;
		ReferApp.R2D.bMergeVertices= true;
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "dxf"){
		// Search and store the pointer to the frame
		CView* pView;
		POSITION pos = GetFirstViewPosition();
		ASSERT(pos != NULL);
		pView = GetNextView(pos);
		m_pFrameSketch2D = (CMDIChildWnd*) pView->GetParentFrame();

		// Load the line-drawing
		// ---------------------
		//Clear the model database
		DB_DrawingOn->Clear();
		DB_DrawingOff->Clear();
		DB_Brep_Model->Clear();

		//Load the new data base with the contents of the DXF file "lpszPathName"
		InOutDXF fileDXF;
		std::string file= lpszPathName;   //Converts the Name of the file into a string
		if (!fileDXF.readDXF(file.c_str(), DB_DrawingOn))
			return false;
		*DB_DrawingOff= *DB_DrawingOn;
		*DB_Brep_Model= *DB_DrawingOn;

		//Do not start sketching mode
		ReferApp.SketchMode= false;
		DB_InputSketch= NULL;
		DB_WorkingSketch= NULL;
		//DB_InputSketch= new CDB_Sketch();
		//DB_WorkingSketch= new CDB_Sketch();

		//By default, do not tidy up this kind of input
		ReferApp.GeneralParams.bRefine2D_OffLine= false;
		ReferApp.R2D.bMergeVertices= false;
	}

	else{
		AfxMessageBox("The type of the file was not identified");
	}

	UpdateAllViews(NULL);
	UpdateStatusBar(NombreFichero);

	return true;
}


void CReferDoc::OnFileClose()
//ID_FILE_CLOSE Closes the currently open document.
//CDocument::OnFileClose calls CDocument::SaveModified
//to prompt the user to save the document if it has been modified
//and then calls OnCloseDocument.
//All the closing logic, including destroying the document,
//is done in the OnCloseDocument routine. 
//ID_FILE_CLOSE acts differently from a WM_CLOSE message
//or an SC_CLOSE system command sent to the documents frame window.
//Closing a window will close the document only if that is
//the last frame window showing the document.
//Closing the document with ID_FILE_CLOSE will not only close
//the document but will close down all frame windows showing the document.
{
	// Close optimization framework (if open)
	if (m_pFrameOptimization){
		m_pFrameOptimization->MDIDestroy();
		m_pFrameOptimization= NULL;
	}

	// Close model CSG framework (if open)
	if (m_pFrameModelCSG){
		m_pFrameModelCSG->MDIDestroy();
		m_pFrameModelCSG= NULL;
	}

	// Close model Brep framework (if open)
	if (m_pFrameModelBrep){
		m_pFrameModelBrep->MDIDestroy();
		m_pFrameModelBrep= NULL;
	}

	// Close 2D sketch framework (if open)
	//if (m_pFrameSketch2D){
	//	m_pFrameSketch2D->MDIDestroy();
	//	m_pFrameSketch2D= NULL;
	//}

	///////////////////////WARNING//////////////////////
	// It fails while closing document of DXF type
	// See OpenDocument
	CDocument::OnFileClose();
	////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////
// CReferDoc serialization
/*
void CReferDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
*/
/////////////////////////////////////////////////////////////////////////////
// CReferDoc diagnostics

//#ifdef _DEBUG
//void CReferDoc::AssertValid() const
//{
//	CDocument::AssertValid();
//}
//
//void CReferDoc::Dump(CDumpContext& dc) const
//{
//	CDocument::Dump(dc);
//}
//#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CReferDoc commands

//void CReferDoc::MuestraMensaje(CString sMensaje)
//{
//	CStatusBar* pStatus = (CStatusBar*)
//		ReferApp.m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
//	pStatus->SetPaneText(0, sMensaje);
//}
//
//


void CReferDoc::UpdateStatusBar(CString sName)
// Displays the neme of the file in the Status Bar
{
	//Update status bar
	CStatusBar* pStatus = (CStatusBar*)
		ReferApp.m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);

	//Modifies the sizes of the status bar
	pStatus->SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	pStatus->SetPaneInfo(1, 0, SBPS_POPOUT, 10);
	pStatus->SetPaneInfo(2, 0, SBPS_POPOUT, 400);

	CString sMensaje= "";
	pStatus->SetPaneText(0, sMensaje);
	pStatus->SetPaneText(1, sMensaje);
	pStatus->SetPaneText(2, sName);
}


void CReferDoc::UpdateStatusBar(CDB_Model* DDBB)
// Displays information in the Status Bar
// summarizing the information of the database pDB
{
	//Information about model
	CString sMensaje= GetPathName() + " : ";
	CString toStringE;    toStringE.Format("%i", DDBB->GetSizeEdges());
	sMensaje = sMensaje + toStringE + " Edges, ";
	CString toStringV;    toStringV.Format("%i", DDBB->GetSizeVertices());
	sMensaje = sMensaje + toStringV + " Vertices, ";
	CString toStringF;    toStringF.Format("%i", DDBB->GetSizeFaces());
	sMensaje = sMensaje + toStringF + " Faces.";

	//Information about typology
	CString sMensaje1= " Object's typology: ";
	switch (ReferApp.Cu.Ty.Typology){
		case Undefined:     sMensaje1 += "NON-DEFINED";    break;
		case Normalon:      sMensaje1 += "NORMALON";       break;
		case Prismatic:     sMensaje1 += "PRISMATIC";      break;
		case Pyramidal:     sMensaje1 += "PYRAMIDAL";      break;
		case QuasiNormalon: sMensaje1 += "QUASI-NORMALON"; break;
		case Undefined_D:   sMensaje1 += "UNDEFINED_D";    break;
		case Undefined_ND:  sMensaje1 += "UNDEFINED_ND";   break;
	}

	//Information about inflation
	CString sMensaje2= " Direct Inflation: ";
	switch (ReferApp.DI.DirectInflationApproach){
		case NO_DIRINFLATE: sMensaje2 += "NO SELECTED";    break;
		case AUTOMATIC: sMensaje2 += "AUTOMATIC";          break;
		case AXONOMETRIC: sMensaje2 += "AXONOMETRIC";      break;
		case LEVELS: sMensaje2 += "LEVELS";                break;
		case RANDOM: sMensaje2 += "RANDOM";                break;
	}
	
	//Update status bar
	CStatusBar* pStatus = (CStatusBar*)
		ReferApp.m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);

	//Modifies the sizes of the status bar
	pStatus->SetPaneInfo(0, 0, SBPS_STRETCH, 0);
	pStatus->SetPaneInfo(1, 0, SBPS_POPOUT, 210);
	pStatus->SetPaneInfo(2, 0, SBPS_POPOUT, 170);

	pStatus->SetPaneText(0, sMensaje);
	pStatus->SetPaneText(1, sMensaje1);
	pStatus->SetPaneText(2, sMensaje2);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Generated message map functions ////////////////////////////////////////


void CReferDoc::OnAddDocument() 
//Opens and reads a .stk document

//Adds its sketch to the current sketch

//Adds the new strokes to the list of selected strokes
//(This is intended to ease subsequent transformation of those strokes)
{
	//Dialog to select the file
	CFileDialog dlg(TRUE); // Set to TRUE to construct a File Open dialog box or FALSE to construct a File Save As dialog box
	dlg.m_ofn.lpstrTitle="Add sketch file";
	dlg.m_ofn.lpstrFilter="Refer sketch files (*.stk)\0*.stk\0";
	if(dlg.DoModal() != IDOK) return;
	CString NombreFichero = dlg.GetPathName(); // return full path and filename

	int PosicionPunto = NombreFichero.ReverseFind('.');
	if (NombreFichero.Mid(PosicionPunto+1, 3) == "stk"){
		// Prepare Sketch DataBase
		long PrevSize= DB_InputSketch->GetNumStrokes();

		// Load the sketch
		std::string file= NombreFichero;	//Converts the name of the file into a string
		CInOutSketch IO;
		IO.readSketch(file.c_str(), DB_InputSketch);

		//Label the new strokes as selected
		long NewSize= DB_InputSketch->GetNumStrokes();

		m_pFrameSketch2D->GetActiveView()->SendMessage(WM_SET_SELECTION, PrevSize, NewSize);
	}

	else{
		AfxMessageBox("Only files containing REFER sketches may be added (stk files)");
		return;
	}

	UpdateAllViews(NULL);
	UpdateStatusBar(NombreFichero);

	return;
}


void CReferDoc::printFeatures(CString NombreFichero, int PosicionPunto)
// Save BRep models of the CSG features as "dxf" files
{
	NombreFichero.Replace(_T(".csg"), _T("_csg"));

	//Loop over all the branches
	long numBranches= DB_CSG_Model->TreeBranchesSize();
	long iFeat= 0;
	for(long i= 0; i<numBranches;  i++){
		//Loop over all the features in the current branch
		long numFeatures= (long)DB_CSG_Model->TreeBranchSize(i);
		for (long j= 0; j<numFeatures; j++){
			//Print current feature
			CFeature Feature= DB_CSG_Model->GetFeature(i, j);

			CString NombreFicheroij= NombreFichero;

			switch (Feature.GetType()){
				case RIB:    {NombreFicheroij += "_RIB";} break;
				case DIVIDER:{NombreFicheroij += "_DIVIDER";} break;
				case SLOT:   {NombreFicheroij += "_SLOT";} break;
				case RAIL:   {NombreFicheroij += "_RAIL";} break;
				case POCKET: {NombreFicheroij += "_POCKET";} break;
				case STEP:   {NombreFicheroij += "_STEP";} break;
				case NODE:   {NombreFicheroij += "_NODE";} break;
				case CORE:   {NombreFicheroij += "_CORE";} break;
			}
			char Aux[20];
			sprintf_s(Aux, sizeof Aux/sizeof *Aux, "%2d", iFeat);
			NombreFicheroij += Aux;

			NombreFicheroij += ".dxf";

			// Loads the DXF file "NombreFichero" with the contents of the i-th feature of the CSG model
			InOutDXF fileDXF;
			CDB_Model F2D= Feature.GetFeature2D();  //Read only
			std::string file= NombreFicheroij;		   //Converts the Name of the file into a string
			if (fileDXF.writeDXF(file.c_str(), &F2D))  //Sends the string as a const char*
				//Updates SetPathName
				SetPathName(NombreFicheroij, true);

			iFeat++;
		}
	}
}


void CReferDoc::printDatumGraph(CString NombreFichero, int PosicionPunto)
// Save BRep models of the datum graph as a "dxf" file
{
	NombreFichero.Replace(_T(".csg"), _T("_csg"));

	//Print datum graph
	NombreFichero += "_DatumGraph";
	NombreFichero += ".dxf";

	// Loads the DXF file "NombreFichero" with the contents of the datum graph of the CSG model
	InOutDXF fileDXF;
	std::string file= NombreFichero;		   //Converts the Name of the file into a string
	if (fileDXF.writeDXF(file.c_str(), DB_DatumGraph))  //Sends the string as a const char*
		//Updates SetPathName
		SetPathName(NombreFichero, true);
}


void CReferDoc::OnReferDocSave() 
// Saves information of the database
// It works in three modes:
//   if the file extension is "stk" the sketch is saved
//   if the file extension is "csg" the CSG model is saved
//   otherwise, the BRep line-drawing is saved into a DXF file or other formats (STL, OBJ, or Varley's)

// If the current pathname is empty, a default name is assigned (Refer followed by a natural number)
{
	// Obtains the current pathname and file name
	CString NombreFichero = GetPathName(); // Is empty if the document has not been saved
										   // or does not have a disk file associated with it

	// Obtains a new pathname and file name
	if(NombreFichero.IsEmpty()){
		CString type, extension;
		CString sMensaje1, sMensaje2;
		if (ReferApp.SketchMode){
			//type= "stk";
			extension= ".stk";

			sMensaje1= "Refer sketch files(*.stk)||*.stk||";
			sMensaje1+= "Refer line-drawing files(*.dxf)||*.dxf||";
			sMensaje1+= "Varley's sketch files(*.Sketch)||*.Sketch||";
			sMensaje1+= "STL files(*.stl)||*.stl||";
			sMensaje1+= "OBJ files(*.obj)||*.obj||";
			sMensaje1+= "IGES files(*.igs)||*.igs||";
			sMensaje1+= "CSG files(*.csg)||*.csg||";
			sMensaje1+= "All files(*.*)||*.*||||";

		}
		else{
			//type= "dxf";
			extension= "(Rf).dxf";

			sMensaje1= "Refer line-drawing files(*.dxf)||*.dxf||";
			sMensaje1+= "Refer sketch files(*.stk)||*.stk||";
			sMensaje1+= "Varley's sketch files(*.Sketch)||*.Sketch||";
			sMensaje1+= "STL files(*.stl)||*.stl||";
			sMensaje1+= "OBJ files(*.obj)||*.obj||";
			sMensaje1+= "IGES files(*.igs)||*.igs||";
			sMensaje1+= "CSG files(*.csg)||*.csg||";
			sMensaje1+= "All files(*.*)||*.*||||";
		}
		CFileDialog DlgSave(false,  // Set to TRUE to construct a File Open dialog box or FALSE to construct a File Save As dialog box
							type,  // The default filename extension
							GetTitle()+extension,  // The initial filename that appears in the filename edit box
							OFN_HIDEREADONLY || OFN_OVERWRITEPROMPT,  //  flags to customize the dialog box
							sMensaje1, //A series of string pairs that specify filters you can apply to the file
							NULL );  // default size of 0 means that the MFC code will determine the proper dialog box size 
		if (DlgSave.DoModal() != IDOK)
			return;
		NombreFichero = DlgSave.GetPathName();
	};

	// Chooses between saving the sketch, the line-drawing, the B-Rep model, or the CSG model
	int PosicionPunto = NombreFichero.ReverseFind('.');
	if(NombreFichero.Mid(PosicionPunto+1, 3) == "stk"){
		// Save the sketch
		long i= 0;
		CInOutSketch* InOutSketch= new CInOutSketch();
			std::string file= NombreFichero;		//Converts the Name of the file into a string
			InOutSketch->writeSketch(file.c_str(), DB_InputSketch);
		delete InOutSketch;
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 6) == "Sketch"){
		// Save the line-drawing in the Varley's format
		// Loads the Ribald file "NombreFichero" with the contents of the B-Rep model data base
		InOutRibald fileRibald;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileRibald.writeRibald(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "stl"){
		// Save the triangulated faces in the STL format
		// Loads the STL file "NombreFichero" with the contents of the B-Rep model data base
		InOutSTL fileSTL;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileSTL.writeSTL(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "obj"){
		// Save the B-Rep model in the OBJ format
		// Loads the OBJ file "NombreFichero" with the contents of the B-Rep model data base
		InOutOBJ fileOBJ;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileOBJ.writeOBJ(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "igs" ||
			NombreFichero.Mid(PosicionPunto+1, 3) == "IGS"){
		// Save the IGES model in the IGES format
		// Loads the IGES file "NombreFichero" with the contents of the IGES model data base
		InOutIGES fileIGES;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileIGES.writeIGES(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "csg" ||
			NombreFichero.Mid(PosicionPunto+1, 3) == "CSG"){
		// Save the CSG model in the CSG format
		// Loads the CSG file "NombreFichero" with the contents of the CSG model data base
		InOutCSG fileCSG;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileCSG.writeCSG(file.c_str(), DB_CSG_Model, DB_DatumGraph))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);

		//Save features
		if (AfxMessageBox( "Save features as B-Rep files?", MB_YESNO ) == IDYES )
			printFeatures(NombreFichero, PosicionPunto);

		//Save datum graph
		if (AfxMessageBox( "Save datum graph?", MB_YESNO ) == IDYES )
			printDatumGraph(NombreFichero, PosicionPunto);
	}

	else{
		// Save the line-drawing

		// Ensures that the file name ends with "(Rf)" and the extension "dxf"
		if ((PosicionPunto == -1) || (PosicionPunto < 5))
			NombreFichero += "(Rf).dxf";
		else
			if ( NombreFichero.Mid(PosicionPunto-4, 4) != "(Rf)" )
				NombreFichero = NombreFichero.Left(PosicionPunto) + "(Rf).dxf";

		// Loads the DXF file "NombreFichero" with the contents of the model data base
		InOutDXF fileDXF;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileDXF.writeDXF(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	return;
}


void CReferDoc::OnReferDocSaveAs() 
// Saves information of the database
// It works in three modes:
//   if the file extension is "stk" the sketch is saved
//   if the file extension is "csg" the CSG model is saved
//   otherwise, the BRep line-drawing is saved into a DXF file or other formats (STL, OBJ, or Varley's)
{
	// Asks for the name and extension of the new file
	CString type;
	CString sMensaje1, sMensaje2;
	if (ReferApp.SketchMode){
		//Default type= "stk";

		sMensaje1= "Refer sketch files(*.stk)||*.stk||";
		sMensaje1+= "Refer line-drawing files(*.dxf)||*.dxf||";
		sMensaje1+= "Varley's sketch files(*.Sketch)||*.Sketch||";
		sMensaje1+= "STL files(*.stl)||*.stl||";
		sMensaje1+= "OBJ files(*.obj)||*.obj||";
		sMensaje1+= "IGES files(*.igs)||*.igs||";
		sMensaje1+= "CSG files(*.csg)||*.csg||";
		sMensaje1+= "All files(*.*)||*.*||||";
	}
	else{
		//Default type= "dxf";

		sMensaje1= "Refer line-drawing files(*.dxf)||*.dxf||";
		sMensaje1+= "Refer sketch files(*.stk)||*.stk||";
		sMensaje1+= "Varley's sketch files(*.Sketch)||*.Sketch||";
		sMensaje1+= "STL files(*.stl)||*.stl||";
		sMensaje1+= "OBJ files(*.obj)||*.obj||";
		sMensaje1+= "IGES files(*.igs)||*.igs||";
		sMensaje1+= "CSG files(*.csg)||*.csg||";
		sMensaje1+= "All files(*.*)||*.*||||";
	}

	CFileDialog DlgSave(false,  // Set to TRUE to construct a File Open dialog box or FALSE to construct a File Save As dialog box
						type,  // The default filename extension
						NULL,  // The initial filename that appears in the filename edit box
						OFN_HIDEREADONLY || OFN_OVERWRITEPROMPT,  //  flags to customize the dialog box
						sMensaje1, //A series of string pairs that specify filters you can apply to the file
						NULL );  // default size of 0 means that the MFC code will determine the proper dialog box size 
	if (DlgSave.DoModal() != IDOK)
		return;

	CString NombreFichero = DlgSave.GetPathName();

	// Chooses between saving the sketch, the line-drawing, the B-Rep model, or the CSG model
	int PosicionPunto = NombreFichero.ReverseFind('.');
	if ( NombreFichero.Mid(PosicionPunto+1, 3) == "stk" ){
		// Save the sketch
		long i= 0;
		CInOutSketch* InOutSketch= new CInOutSketch();
			std::string file= NombreFichero;		//Converts the Name of the file into a string
			InOutSketch->writeSketch(file.c_str(), DB_InputSketch);
			delete InOutSketch;
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 6) == "Sketch"){
		// Save the file drawing in the Varley's format
		// Loads the Ribald file "NombreFichero" with the contents of the B-Rep model data base
		InOutRibald fileRibald;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileRibald.writeRibald(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "stl"){
		// Save the Tessellated faces in the STL format
		// Loads the STL file "NombreFichero" with the contents of the B-Rep model data base
		InOutSTL fileSTL;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileSTL.writeSTL(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "obj"){
		// Save the B-Rep model in the OBJ format
		// Loads the OBJ file "NombreFichero" with the contents of the B-Rep model data base
		InOutOBJ fileOBJ;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileOBJ.writeOBJ(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "igs" ||
			NombreFichero.Mid(PosicionPunto+1, 3) == "IGS"){
		// Save the B-Rep model in the IGES format
		// Loads the IGES file "NombreFichero" with the contents of the B-Rep model data base
		InOutIGES fileIGES;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileIGES.writeIGES(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);
	}

	else if(NombreFichero.Mid(PosicionPunto+1, 3) == "csg" ||
			NombreFichero.Mid(PosicionPunto+1, 3) == "CSG"){
		// Save the CSG model in the REFER format
		// Loads the CSG file "NombreFichero" with the contents of the CSG model data base
		InOutCSG fileCSG;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileCSG.writeCSG(file.c_str(), DB_CSG_Model, DB_DatumGraph))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName(NombreFichero, true);

		//Save features
		if (AfxMessageBox( "Save features as B-Rep files?", MB_YESNO ) == IDYES )
			printFeatures(NombreFichero, PosicionPunto);

		//Save datum graph
		if (AfxMessageBox( "Save datum graph?", MB_YESNO ) == IDYES )
			printDatumGraph(NombreFichero, PosicionPunto);
	}

	else{
		// Save the line-drawing
		// Ensures that the file name ends with "(Rf)" and the extension "dxf"
		if ((PosicionPunto == -1) || (PosicionPunto < 5))
			NombreFichero += "(Rf).dxf";
		else
			if ( NombreFichero.Mid(PosicionPunto-4, 4) != "(Rf)" )
				NombreFichero = NombreFichero.Left(PosicionPunto) + "(Rf).dxf";

		// Loads the DXF file "NombreFichero" with the contents of the model data base
		InOutDXF fileDXF;
		std::string file= NombreFichero;					//Converts the Name of the file into a string
		if (fileDXF.writeDXF(file.c_str(), DB_Brep_Model))  //Sends the string as a const char*
			//Updates SetPathName
			SetPathName( NombreFichero, true );
	}
	return;
}


void CReferDoc::OnViewSketch2D()
// Switches the Sketch2D window
{
	if (m_pFrameSketch2D){
		if ( AfxMessageBox( "This operation will close ALL instances/views related to the active sketch? Do you want to continue?", MB_YESNO ) == IDNO )
			return;
		OnFileClose();

		//m_pFrameSketch2D->MDIDestroy();
		//m_pFrameSketch2D= NULL;
	}
	else {
		CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
		m_pFrameSketch2D = pFrame->OnCreateWindowSketch2D(this);
	};
}


void CReferDoc::OnUpdateViewSketch2D(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrameSketch2D!=NULL);
}


void CReferDoc::OnViewModelBrep()
// Switches the Model BRep window
{
	if (m_pFrameModelBrep){
		m_pFrameModelBrep->MDIDestroy();
		m_pFrameModelBrep= NULL;
	}
	else {
		CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
		m_pFrameModelBrep = pFrame->OnCreateWindowModelBrep(this);
	};
}


void CReferDoc::OnUpdateViewModelBrep(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrameModelBrep!=NULL);
}


void CReferDoc::OnViewModelCSG()
// Switches the model CSG window
{
	if (m_pFrameModelCSG){
		m_pFrameModelCSG->MDIDestroy();
		m_pFrameModelCSG= NULL;
	}
	else {
		CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
		m_pFrameModelCSG = pFrame->OnCreateWindowModelCSG(this);
	};

}


void CReferDoc::OnUpdateViewModelCSG(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrameModelCSG!=NULL);
}


void CReferDoc::OnViewOptimizacion() 
// Switches the Optimization window
{
	if (m_pFrameOptimization){
		m_pFrameOptimization->MDIDestroy();
		m_pFrameOptimization= NULL;
	}
	else {
		CWinRf_MainFrame *pFrame = (CWinRf_MainFrame *) ReferApp.m_pMainWnd;
		m_pFrameOptimization = pFrame->OnCreateWindowOptimization(this);
	}
}


void CReferDoc::OnUpdateViewOptimizacion(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pFrameOptimization!=NULL);
}


void CReferDoc::OnSetBackgroundColor()
{
	CColorDialog Dlg( ReferApp.m_BackgroundColor );

	if (Dlg.DoModal() == IDOK){
		ReferApp.m_BackgroundColor = Dlg.GetColor();
		RefreshWindows();
	};
}


void CReferDoc::OnWindowTileRectangle() 
// Arranges the windows in tile mode
// and activates the last one
{
	CMDIFrameWnd* pMDIFrameWnd;

	long numwindows= 0;
	// Windows must be activated in reverse order
	//(The appeareance order is oposite to activation order)
	// So, the last window remains activated
	if (m_pFrameModelCSG){
		m_pFrameModelCSG->MDIActivate();
		pMDIFrameWnd = ((CMDIChildWnd*)m_pFrameModelCSG)->GetMDIFrame();
		numwindows++;
	}
	if (m_pFrameOptimization){
		m_pFrameOptimization->MDIActivate();
		pMDIFrameWnd = ((CMDIChildWnd*)m_pFrameOptimization)->GetMDIFrame();
		numwindows++;
	}
	if (m_pFrameModelBrep){
		m_pFrameModelBrep->MDIActivate();
		pMDIFrameWnd = ((CMDIChildWnd*)m_pFrameModelBrep)->GetMDIFrame();
		numwindows++;
	}
	if (m_pFrameSketch2D){
		m_pFrameSketch2D->MDIActivate();
		pMDIFrameWnd = ((CMDIChildWnd*)m_pFrameSketch2D)->GetMDIFrame();
		numwindows++;
	}

	//Arrange all of the MDI child windows in a tile format
	if (pMDIFrameWnd != NULL){
		if(numwindows==3 && m_pFrameSketch2D && m_pFrameModelBrep && m_pFrameOptimization){
			//Arrange a personal tile mode for optimization visualization
			CRect parent;
			//pMDIFrameWnd->GetClientRect(&parent);
			//Since toolbars and status bar reduce the client area,
			//it is better to maximize one child window and get its dimensions
			m_pFrameSketch2D->MDIMaximize( );
			m_pFrameSketch2D->GetClientRect(&parent);

			CRect w;
			w.left= parent.left;
			w.right= parent.right/2;
			w.top= parent.top;
			w.bottom= parent.bottom;
			m_pFrameSketch2D->MoveWindow(w.left, w.top, w.Width(), w.Height(), TRUE);

			w.left= parent.left + parent.Width()/2;
			w.right= parent.right;
			w.top= parent.top;
			w.bottom= parent.top + parent.Height()/2;
			m_pFrameModelBrep->MoveWindow(w.left, w.top, w.Width(), w.Height(), TRUE);

			w.left= parent.left + parent.Width()/2;
			w.right= parent.right;
			w.top= parent.top + parent.Height()/2;
			w.bottom= parent.bottom;
			m_pFrameOptimization->MoveWindow(w.left, w.top, w.Width(), w.Height(), TRUE);

		}
		else{
			//Default tile arrangement
			pMDIFrameWnd->MDITile(MDITILE_VERTICAL);
		}
	}
}


void CReferDoc::RefreshWindows()
//Message to redraw all the documents' windows
{
	RefreshWindowSketch2D();
	RefreshWindowModelBrep();
	RefreshWindowModelCSG();
	RefreshWindowOptimization();
}


void CReferDoc::RefreshWindowSketch2D()
//Message to redraw the window, if active
{
	if (m_pFrameSketch2D)
		m_pFrameSketch2D->GetActiveView()->Invalidate();
}


void CReferDoc::RefreshWindowModelBrep()
//Message to redraw the window, if active
{
	if (m_pFrameModelBrep)
		m_pFrameModelBrep->GetActiveView()->Invalidate();
}


void CReferDoc::RefreshWindowModelCSG()
//Message to redraw the window, if active
{
	if (m_pFrameModelCSG)
		m_pFrameModelCSG->GetActiveView()->Invalidate();
}


void CReferDoc::RefreshWindowOptimization()
//Message to redraw the window, if active
{
	if (m_pFrameOptimization)
		m_pFrameOptimization->GetActiveView()->Invalidate();	
}

///////////////////////////////RECONSTRUCTION///////////////////////////////////////////////////////////

void CReferDoc::OnNewReconstruction3D()
//Start the 3D reconstruction process
{
	//Reconstruction process
	CReconstructionManager RM;
	RM.Reconstruct(this);

	//Update child windows
	if(ReferApp.GeneralParams.bInflateBRep){
		if (m_pFrameModelBrep)
			m_pFrameModelBrep->MDIActivate(); // Activates the Model BRep window
		else{
			OnViewModelBrep();  // Opens the model BRep window
		}

		//Close CSG window, if open
		if (!ReferApp.GeneralParams.bBuildModelCSG && m_pFrameModelCSG){
			m_pFrameModelCSG->MDIDestroy();
			m_pFrameModelCSG= NULL;
		}
	}

	if(ReferApp.GeneralParams.bBuildModelCSG){
		if (m_pFrameModelCSG){
			m_pFrameModelCSG->MDIActivate(); // Activates the CSG window
		}
		else{
			OnViewModelCSG();  // Opens the model CSG window
		}

		//Close B-Rep window, if open
		//if (!ReferApp.GeneralParams.bInflateBRep && m_pFrameModelBrep){
		if (m_pFrameModelBrep){
			m_pFrameModelBrep->MDIDestroy();
			m_pFrameModelBrep= NULL;
		}
	}

	OnWindowTileRectangle();
	//RefreshWindows();    //Warning, On_Draw are only called after finishing this fucntion!!!

	////Reconstruction process
	//CReconstructionManager RM;
	//RM.Reconstruct(this);

	RefreshWindows();
}


void CReferDoc::OnUpdateNewReconstruction3D(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}


void CReferDoc::OnUndoReconstruction3D()
//Removes previous 3D reconstructions from database
{
	DB_Brep_Model->Clear();

	RefreshWindows();
}


void CReferDoc::OnUpdateUndoReconstruction3D(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(DB_Brep_Model->GetSizeVertices() != 0);
}
///////////////////////////////END RECONSTRUCTION///////////////////////////////////////////////////////

///////////////////////////////VECTORIZE////////////////////////////////////////////////////////////////

void CReferDoc::OnVectorizeInt()
{
	ReferApp.GeneralParams.bVectorize_Int=!ReferApp.GeneralParams.bVectorize_Int;

	if(ReferApp.GeneralParams.bVectorize_Int){
		//Update vectorization
		CVectorization V;
		V.VectorizeSketch(this);

		if(ReferApp.GeneralParams.bRefine2D_OnLine){
			//Update online refinement
			CRefine2D R2D;
			R2D.GetRefineDrawing(DB_DrawingOn, false);
		}

		RefreshWindows();
	}

	else{
		//Delete vectorization
		CVectorization V;
		V.ClearVectorize(this);

		RefreshWindows();
	}
}


void CReferDoc::OnGetVectorizeInt(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(ReferApp.GeneralParams.bVectorize_Int); 
}


void CReferDoc::OnVectorizeBatch()
{
	ReferApp.GeneralParams.bVectorize_Batch=!ReferApp.GeneralParams.bVectorize_Batch;

	DB_Brep_Model->Clear();
	if(!ReferApp.GeneralParams.bVectorize_Int &&
	   ReferApp.SketchMode &&
	   DB_DrawingOff->GetSizeEdges() != 0){
		DB_DrawingOff->Clear();
	}

	RefreshWindows();
}


void CReferDoc::OnGetVectorizeBatch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(ReferApp.GeneralParams.bVectorize_Batch); 
}


void CReferDoc::OnConfigVectorize() 
{
	CDialogConfigVectorize2D Dlg;
	Dlg.SetParams(ReferApp.GeneralParams, ReferApp.Ve);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.GeneralParams, ReferApp.Ve);

		CVectorization V;
		V.ClearVectorize(this);
	}
}


void CReferDoc::OnUpdateConfigVectorize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}
///////////////////////////////END VECTORIZE////////////////////////////////////////////////////////////

///////////////////////////////2D REFINE////////////////////////////////////////////////////////////////

void CReferDoc::OnRefine2DOnLine() 
{
	ReferApp.GeneralParams.bRefine2D_OnLine=!ReferApp.GeneralParams.bRefine2D_OnLine;

	if(ReferApp.GeneralParams.bRefine2D_OnLine){
		//Add online refinement to the vectorized line-drawing
		CRefine2D R2D;
		R2D.GetRefineDrawing(DB_DrawingOn, false);
	}
	else{
		//Re-vectorize to remove the refinement
		CVectorization V;
		V.VectorizeSketch(this);
	}

	RefreshWindows();
}


void CReferDoc::OnUpdateRefine2DOnLine(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(ReferApp.GeneralParams.bRefine2D_OnLine);
	pCmdUI->Enable(ReferApp.GeneralParams.bVectorize_Int);
}


void CReferDoc::OnRefine2DOffLine() 
{
	ReferApp.GeneralParams.bRefine2D_OffLine=!ReferApp.GeneralParams.bRefine2D_OffLine;
}


void CReferDoc::OnUpdateRefine2DOffLine(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( ReferApp.GeneralParams.bRefine2D_OffLine); 
}


void CReferDoc::OnConfigRefine2D()
{
	CDialogConfigRefine2D Dlg;
	Dlg.SetParams(ReferApp.GeneralParams, ReferApp.R2D);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.GeneralParams, ReferApp.R2D);
	}
}


void CReferDoc::OnUpdateConfigRefine2D(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}

///////////////////////////////END REFINE 2D////////////////////////////////////////////////////////////////

///////////////////////////////CUES AND FEATURES////////////////////////////////////////////////////////////

void CReferDoc::OnDetectCuesAndFeatures()
{
	ReferApp.GeneralParams.bDetectCuesAndFeatures=!ReferApp.GeneralParams.bDetectCuesAndFeatures;
}


void CReferDoc::OnUpdateDetectCuesAndFeatures(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(ReferApp.GeneralParams.bDetectCuesAndFeatures);
}


void CReferDoc::OnEraseCuesAndFeatures()
//Removes previouly detecteted cues and features
{
	CDetectionManager DManager;
	DManager.EraseCuesAndFeatures();
}


void CReferDoc::OnUpdateEraseCuesAndFeatures(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(false);
}


void CReferDoc::OnConfigCuesAndFeatures()
{
	CDialogConfigDetectCuesFeatures Dlg;
	Dlg.SetParams(ReferApp.GeneralParams);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.GeneralParams);
	}
}


void CReferDoc::OnUpdateConfigCuesAndFeatures(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(false);
}
///////////////////////////////END CUES AND FEATURES///////////////////////////////////////////////

///////////////////////////////CUES////////////////////////////////////////////////////////////////

void CReferDoc::OnCueValences()
{
	ReferApp.GeneralParams.DetectValences= !ReferApp.GeneralParams.DetectValences;
}


void CReferDoc::OnUpdateCueValences(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectValences);
}


void CReferDoc::OnCueParallelEdges() 
// Switches the buton to detect parallel edges
{
	ReferApp.GeneralParams.DetectParallelEdges= !ReferApp.GeneralParams.DetectParallelEdges;
}

void CReferDoc::OnUpdateCueParallelEdges(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectParallelEdges);
}


void CReferDoc::OnCueVanishingPoints() 
//Switches the buton to detect vanishing points
{
	ReferApp.GeneralParams.DetectVanishingPoints= !ReferApp.GeneralParams.DetectVanishingPoints;
}

void CReferDoc::OnUpdateCueVanishingPoints(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectVanishingPoints);
}


void CReferDoc::OnCueGraphs()
{
	ReferApp.GeneralParams.DetectGraphs= !ReferApp.GeneralParams.DetectGraphs;
}


void CReferDoc::OnUpdateCueGraphs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectGraphs);
}


void CReferDoc::OnCuePerimeter() 
{
	ReferApp.GeneralParams.DetectPerimeter= !ReferApp.GeneralParams.DetectPerimeter;
}


void CReferDoc::OnUpdateCuePerimeter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues &&
					ReferApp.GeneralParams.DetectGraphs) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectPerimeter);
}


void CReferDoc::OnCueLabels()
{
	ReferApp.GeneralParams.DetectLabels= !ReferApp.GeneralParams.DetectLabels;
}


void CReferDoc::OnUpdateCueLabels(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues &&
					ReferApp.GeneralParams.DetectGraphs &&
					ReferApp.GeneralParams.DetectPerimeter) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectLabels);
}


void CReferDoc::OnCueMainAxes() 
{
	ReferApp.GeneralParams.DetectMainAxes= !ReferApp.GeneralParams.DetectMainAxes;
}


void CReferDoc::OnUpdateCueMainAxes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues &&
					ReferApp.GeneralParams.DetectGraphs) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectMainAxes);
}


void CReferDoc::OnCueFaces() 
//Switches the buton to detect faces
{
	ReferApp.GeneralParams.DetectFaces= !ReferApp.GeneralParams.DetectFaces;
}

void CReferDoc::OnUpdateCueFaces(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues &&
					ReferApp.GeneralParams.DetectGraphs) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectFaces);
}


void CReferDoc::OnCueTypology() 
{
	ReferApp.GeneralParams.DetectTypology= !ReferApp.GeneralParams.DetectTypology;

	////Initialize Typology
	//if (ReferApp.GeneralParams.DetectTypology)
	//	Typology= DetectTypology(this);
	//else Typology= Indefinido;
}

void CReferDoc::OnUpdateCueTypology(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues &&
					ReferApp.GeneralParams.DetectGraphs) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectTypology);
}


void CReferDoc::OnCueHiddenEdges() 
{
	ReferApp.GeneralParams.DetectHiddenEdges= !ReferApp.GeneralParams.DetectHiddenEdges;
}


void CReferDoc::OnUpdateCueHiddenEdges(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues &&
					ReferApp.GeneralParams.DetectGraphs && 
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectHiddenEdges);
}


void CReferDoc::OnCueSymmetry() 
//Detects bilateral symmetry
{
	ReferApp.GeneralParams.DetectSymmetry= !ReferApp.GeneralParams.DetectSymmetry;
}


void CReferDoc::OnUpdateCueSymmetry(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues &&
					ReferApp.GeneralParams.DetectGraphs && 
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectSymmetry);
}


void CReferDoc::OnCueLevels() 
//Detects levels of vertices
{
	ReferApp.GeneralParams.DetectLevels= !ReferApp.GeneralParams.DetectLevels;
}


void CReferDoc::OnUpdateCueLevels(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectCues) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectLevels);
}


void CReferDoc::OnConfigCues() 
{
	CDialogConfigCues Dlg;
	Dlg.SetParams(ReferApp.GeneralParams);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.GeneralParams);
	}
}


void CReferDoc::OnUpdateConfigCues(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}
///////////////////////////////END CUES////////////////////////////////////////////////////////////////

///////////////////////////////FEATURES////////////////////////////////////////////////////////////////

void CReferDoc::OnFeatureRibsAndDividers() 
{
	ReferApp.GeneralParams.DetectRibsAndDividers= !ReferApp.GeneralParams.DetectRibsAndDividers;
}


void CReferDoc::OnUpdateFeatureRibsAndDividers(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectFeatures &&
					ReferApp.GeneralParams.DetectGraphs&& 
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectRibsAndDividers);
}


void CReferDoc::OnFeatureSlotsAndRails() 
{
	ReferApp.GeneralParams.DetectRailsAndSlots= !ReferApp.GeneralParams.DetectRailsAndSlots;
}


void CReferDoc::OnUpdateFeatureSlotsAndRails(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectFeatures &&
					ReferApp.GeneralParams.DetectGraphs&& 
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectRailsAndSlots);
}


void CReferDoc::OnFeatureStepsAndPockets()
{
	ReferApp.GeneralParams.DetectStepsAndPockets= !ReferApp.GeneralParams.DetectStepsAndPockets;
}


void CReferDoc::OnUpdateFeatureStepsAndPockets(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
					ReferApp.GeneralParams.DetectFeatures &&
					ReferApp.GeneralParams.DetectGraphs&& 
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.GeneralParams.DetectStepsAndPockets);
}


void CReferDoc::OnFeatureChamfersAndRounds()   //TBD ChamfersAndRounds
{
	ReferApp.GeneralParams.DetectChamfersAndRounds= !ReferApp.GeneralParams.DetectChamfersAndRounds;
}


void CReferDoc::OnUpdateFeatureChamfersAndRounds(CCmdUI* pCmdUI)   //TBD ChamfersAndRounds 
{
	pCmdUI->Enable(false);
	//pCmdUI->Enable((ReferApp.GeneralParams.bDetectCuesAndFeatures &&
	//				ReferApp.GeneralParams.DetectFeatures &&
	//				ReferApp.GeneralParams.DetectGraphs&& 
	//				ReferApp.GeneralParams.DetectFaces) != 0);
	//pCmdUI->SetCheck(ReferApp.GeneralParams.DetectChamfersAndRounds);
}


void CReferDoc::OnConfigFeatures() 
{
	CDialogConfigFeatures Dlg;
	Dlg.SetParams(ReferApp.GeneralParams, ReferApp.Fe);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.GeneralParams, ReferApp.Fe);
	}
}


void CReferDoc::OnUpdateConfigFeatures(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(true);
}
///////////////////////////////END FEATURES//////////////////////////////////////////////////

//////////////////////////////INFLATION//////////////////////////////////////////////////////

void CReferDoc::OnNewInflation()
{
	ReferApp.GeneralParams.bInflateBRep = !ReferApp.GeneralParams.bInflateBRep;

	//if (m_pFrameModelBrep)
	//	m_pFrameModelBrep->MDIActivate(); // Activates the model BRep window
	//else
	//	OnViewModelBrep();  // Opens the model BRep window
}


void CReferDoc::OnUpdateNewInflation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ReferApp.GeneralParams.bDetectCuesAndFeatures);
	pCmdUI->SetCheck(ReferApp.GeneralParams.bInflateBRep);
}


void CReferDoc::OnIncrementalInflation()
//Execute an incremental inflation process
//(This is an inflation departing from the current 3D model)
{
	//Direct inflation
	if(ReferApp.GeneralParams.DirectInflation){
		CDirectInflation DI;
		DI.GetDirectInflation(DB_Brep_Model);
	}

	//Optimization-based inflation
	if(ReferApp.GeneralParams.Optimize){
		if (ReferApp.In.iOptAlgorithm != NO_OPTIMIZATION){
			COptimizationInflation OI;
			OI.OptimizationBasedInflation(this); //Optimization cycle
		}
	}

	RefreshWindows();
}


void CReferDoc::OnUpdateIncrementalInflation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(DB_Brep_Model->GetSizeVertices() != 0);
}


void CReferDoc::OnEraseInflation()
//Removes current inflation by reseting the z coordinates of the vertices
{
	DB_Brep_Model->SetAllVertex_Z_ToZero();

	RefreshWindows();
}


void CReferDoc::OnUpdateEraseInflation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(DB_Brep_Model->GetSizeVertices() != 0);
}


void CReferDoc::OnConfigInflation()
{
	CDialogConfigInflation Dlg;
	Dlg.SetParams(ReferApp.GeneralParams);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.GeneralParams);
	}
}


void CReferDoc::OnUpdateConfigInflation(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

//////////////////////////////END INFLATION///////////////////////////////////////////////////


//////////////////////////////REFINE 3D///////////////////////////////////////////////////////

void CReferDoc::OnRefine3D()
{
	ReferApp.GeneralParams.bRefine3D = !ReferApp.GeneralParams.bRefine3D;
}


void CReferDoc::OnUpdateRefine3D(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(ReferApp.GeneralParams.bRefine3D);
}


void CReferDoc::OnRefine3D_Faces()
{
	ReferApp.R3D.RefineFaces = !ReferApp.R3D.RefineFaces;
}


void CReferDoc::OnUpdateRefine3D_Faces(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((ReferApp.GeneralParams.bRefine3D &&
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.R3D.RefineFaces);
}


void CReferDoc::OnAlign3D_Faces()
{
	ReferApp.R3D.AlignFaces = !ReferApp.R3D.AlignFaces;
}


void CReferDoc::OnUpdateAlign3D_Faces(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((ReferApp.GeneralParams.bRefine3D &&
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.R3D.AlignFaces);
}


void CReferDoc::OnConfigRefine3D()
{
	CDialogConfigRefine3D Dlg;
	Dlg.SetParams(ReferApp.R3D);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.R3D);
	}
}


void CReferDoc::OnUpdateConfigRefine3D(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}
//////////////////////////////END 3DREFINE///////////////////////////////////////////////////

//////////////////////////////3D TESSELLATE///////////////////////////////////////////////////////

void CReferDoc::OnTessellate3D()
{
	ReferApp.GeneralParams.bTessellate3D = !ReferApp.GeneralParams.bTessellate3D;
}


void CReferDoc::OnUpdateTessellate3D(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(ReferApp.GeneralParams.bTessellate3D);
}


void CReferDoc::OnFaces_Outward()
{
	ReferApp.T3D.FacesOutward = !ReferApp.T3D.FacesOutward;
}


void CReferDoc::OnUpdateFaces_Outward(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((ReferApp.GeneralParams.bTessellate3D && 
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.T3D.FacesOutward);
}


void CReferDoc::OnTriangulate_Faces()
{
	ReferApp.T3D.TriangulateFaces = !ReferApp.T3D.TriangulateFaces;
}


void CReferDoc::OnUpdateTriangulate_Faces(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((ReferApp.GeneralParams.bTessellate3D && 
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.T3D.TriangulateFaces);
}


void CReferDoc::OnNormals_To_Faces()
{
	ReferApp.T3D.NormalsToFaces = !ReferApp.T3D.NormalsToFaces;
}


void CReferDoc::OnUpdateNormals_To_Faces(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((ReferApp.GeneralParams.bTessellate3D && 
					ReferApp.GeneralParams.DetectFaces) != 0);
	pCmdUI->SetCheck(ReferApp.T3D.NormalsToFaces);
}


void CReferDoc::OnConfigTessellate3D()
{
	CDialogConfigTessellate3D Dlg;
	Dlg.SetParams(ReferApp.T3D);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.T3D);
	}
}


void CReferDoc::OnUpdateConfigTessellate3D(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}
//////////////////////////////END 3D TESSELLATE///////////////////////////////////////////////////

//////////////////////////////CSG/////////////////////////////////////////////////////////////

void CReferDoc::OnCSG_Build()
{
	ReferApp.GeneralParams.bBuildModelCSG = !ReferApp.GeneralParams.bBuildModelCSG;
}


void CReferDoc::OnUpdateCSG_Build(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ReferApp.GeneralParams.bDetectCuesAndFeatures &&
				   (ReferApp.GeneralParams.DetectRibsAndDividers ||
				    ReferApp.GeneralParams.DetectRailsAndSlots ||
				    //ReferApp.GeneralParams.DetectChamgfersAndRounds ||
				    ReferApp.GeneralParams.DetectStepsAndPockets));
	pCmdUI->SetCheck(ReferApp.GeneralParams.bBuildModelCSG);
}


void CReferDoc::OnEraseCSG()
{
	DB_Deconstruct.clear();
	DB_CSG_Model->Clear();
}


void CReferDoc::OnUpdateEraseCSG(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ReferApp.GeneralParams.bDetectCuesAndFeatures);
}


void CReferDoc::OnConfigCSG()
{
	CDialogConfigCSG Dlg;
	Dlg.SetParams(ReferApp.CSG);

	if (Dlg.DoModal() == IDOK) {
		Dlg.GetParams(ReferApp.CSG);
	}
}


void CReferDoc::OnUpdateConfigCSG(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}
//////////////////////////////END CSG/////////////////////////////////////////////////////////
