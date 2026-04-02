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

#include <stdafx.h>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DB_Sketch.h"
#include "Vectorization.h"
#include "Refine2D.h"
#include "DetectionManager.h"
#include "DirectInflation.h"
#include "DialogProgress.h"
#include "OptimizationBasedInflation.h"
#include "Refine3D.h"
#include "Tessellate3D.h"

#include "CSG_Modeling.h"
#include "ReconstructionManager.h"


void CReconstructionManager::Reconstruct(CReferDoc* pDoc)
// Manages the entire reconstruction flow:
//		Vectorization
//		Offline 2D refinement
//		Detection of cues and features
//		Inflation
//		Refinement of 3D model
//		CSG modeling
{
	//VECTORIZATION
	//-------------
	//Batch vectorization of the sketch
	if (ReferApp.SketchMode &&
		ReferApp.GeneralParams.bVectorize_Batch){//Partially redundant when online vectorization was active during sketching

		CVectorization V;
		V.VectorizeSketch(pDoc);

		//Reload the drawing offline
		*pDoc->DB_DrawingOff= *pDoc->DB_DrawingOn;
	}

	//Offline refinement of the 2D line-drawing
	if (ReferApp.GeneralParams.bRefine2D_OffLine){
		CRefine2D R2DOL;
		R2DOL.GetRefineDrawing(pDoc->DB_DrawingOff, true);
	}

	//DETECTION OF CUES AND FEATURES
	//------------------------------
	if (pDoc->DB_DrawingOff->GetSizeEdges()+pDoc->DB_DrawingOff->GetSizeArcs() == 0){
		AfxMessageBox("No vectorized 2D model to find cues and features");
		return;
	}

	if(ReferApp.GeneralParams.bDetectCuesAndFeatures){
		CDetectionManager DManager;
		DManager.DetectCuesAndFeatures(pDoc->DB_DrawingOff);
	}

	if (pDoc->DB_DrawingOff->GetSizeEdges()+pDoc->DB_DrawingOff->GetSizeArcs() == 0){
		AfxMessageBox("No vectorized 2D model to reconstruct");
		return;
	}

	//Reload the BRep model database with the offline drawing
	//-------------------------------------------------------
	*pDoc->DB_Brep_Model= *pDoc->DB_DrawingOff;

	//INFLATION
	//---------
	if(ReferApp.GeneralParams.bInflateBRep){
		//Direct inflation
		if(ReferApp.GeneralParams.DirectInflation){
			CDirectInflation DI;
			DI.GetDirectInflation(pDoc->DB_Brep_Model);
		}

		//Optimization-based inflation
		if(ReferApp.GeneralParams.Optimize){
			if (ReferApp.In.iOptAlgorithm != NO_OPTIMIZATION){
				COptimizationInflation OI;
				OI.OptimizationBasedInflation(pDoc); //Optimization cycle
			}
		}
	}

	//3D MODEL REFINEMENT
	//-------------------
	if (ReferApp.GeneralParams.bRefine3D &&
		(ReferApp.R3D.RefineFaces || ReferApp.R3D.AlignFaces)){
		RefineModelBrep(pDoc);
	}

	//3D MODEL tessellation
	//---------------------
	if (ReferApp.GeneralParams.bTessellate3D &&
		(ReferApp.T3D.FacesOutward ||ReferApp.T3D.TriangulateFaces || ReferApp.T3D.NormalsToFaces)){
		TessellateModelBrep(pDoc->DB_Brep_Model);
	}

	//CSG MODELING
	//------------
	pDoc->DB_CSG_Model->Clear();
	pDoc->DB_Deconstruct.clear();
	pDoc->DB_DatumGraph->Clear();

	if(ReferApp.GeneralParams.bBuildModelCSG){
		//Reload the Deconstruct database with the offline drawing
		//(Consecutive drawings will be added while deconstructing!)
		pDoc->DB_Deconstruct.push_back(*pDoc->DB_DrawingOff);

		//Reset the datum graph database with the offline drawing
		pDoc->DB_DrawingOff->CopyVerticesTo(pDoc->DB_DatumGraph);
		pDoc->DB_DrawingOff->CopyEdgesTo(pDoc->DB_DatumGraph);

		//Find the CSG model
		CCSG_Modeling CSG;
		CSG.GetCSG(pDoc);
	}

	//Update status bar
	if(ReferApp.GeneralParams.bInflateBRep){
		pDoc->UpdateStatusBar(pDoc->DB_Brep_Model);
	}
}
