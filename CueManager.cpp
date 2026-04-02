
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

#include "CueValences.h"
#include "CueParallelEdges.h"
#include "CueVanishingPoints.h"

#include "CueSubGraphs.h"
#include "CueLabels.h"
#include "CuePerimeter.h"
#include "CueMainAxes.h"
#include "CueTypology.h"
#include "CueFaces.h"
#include "CueHiddenEdges.h" 
#include "CueSymmetry.h"

#include "CueLevels.h"

#include "CueManager.h"


CCueManager::CCueManager()
{
	//Set flags as configured though the interactive menu
	DetectValences = ReferApp.GeneralParams.DetectValences;
	DetectParallelEdges = ReferApp.GeneralParams.DetectParallelEdges;
	DetectVanishingPoints = ReferApp.GeneralParams.DetectVanishingPoints;
	DetectGraphs = ReferApp.GeneralParams.DetectGraphs;
	DetectPerimeter = ReferApp.GeneralParams.DetectPerimeter;
	DetectLabels = ReferApp.GeneralParams.DetectLabels;
	DetectMainAxes = ReferApp.GeneralParams.DetectMainAxes;
	DetectFaces = ReferApp.GeneralParams.DetectFaces;
	DetectTypology = ReferApp.GeneralParams.DetectTypology;
	DetectHiddenEdges = ReferApp.GeneralParams.DetectHiddenEdges;
	DetectSymmetry = ReferApp.GeneralParams.DetectSymmetry;
	DetectLevels = ReferApp.GeneralParams.DetectLevels;

	//CCueMainAxes MainAxes;
}


CCueManager::CCueManager(bool Set_DetectValences,
						bool Set_DetectParallelEdges,
						bool Set_DetectVanishingPoints,
						bool Set_DetectGraphs,
						bool Set_DetectPerimeter,
						bool Set_DetectLabels,
						bool Set_DetectMainAxes,
						bool Set_DetectFaces,
						bool Set_DetectTypology,
						bool Set_DetectHiddenEdges,
						bool Set_DetectSymmetry,
						bool Set_DetectLevels)
:
	DetectValences (Set_DetectValences),
	DetectParallelEdges (Set_DetectParallelEdges),
	DetectVanishingPoints (Set_DetectVanishingPoints),
	DetectGraphs (Set_DetectGraphs),
	DetectPerimeter (Set_DetectPerimeter),
	DetectLabels (Set_DetectLabels),
	DetectMainAxes (Set_DetectMainAxes),
	DetectFaces (Set_DetectFaces),
	DetectTypology (Set_DetectTypology),
	DetectHiddenEdges (Set_DetectHiddenEdges),
	DetectSymmetry (Set_DetectSymmetry),
	DetectLevels (Set_DetectLevels)
{
}


CCueManager::~CCueManager()
{
	//CCueMainAxes MainAxes;
}


void CCueManager::EraseCues()
// Removes previously calculated cues
{
	//Pointer to the document that contains the databases
	CReferDoc* pDoc= CReferDoc::GetDoc();

	//Cues that do not need sub-graphs
	//--------------------------------
	//Clear previous valences
	//(Valence is the number of edges connected to the vertex)
	ReferApp.Cu.Va.Valences.clear();

	//Clear previous vanishing points
	ReferApp.Cu.VP.CandidateVP.clear();
	ReferApp.Cu.VP.ClusterOfVPs.clear();

	ReferApp.Cu.VP.MeanVP.clear();

	//Calculate sub-graphs
	//--------------------
	ReferApp.Cu.SG.NumSubgraphs= 0;
	ReferApp.Cu.SG.SubgraphForVertex.clear();
	ReferApp.Cu.SG.SubgraphForEdge.clear();
	ReferApp.Cu.SG.Outer= 0;

	//Cues that need sub-graphs
	//-------------------------
	//Clear previous contour
	ReferApp.Cu.Pe.PerimeterVertices.clear();
	ReferApp.Cu.Pe.PerimeterEdges.clear();
	ReferApp.Cu.Pe.Tjunction.clear();
	ReferApp.Cu.Pe.Tedges.clear();

	//Clear previous main axes
	ReferApp.Cu.MA.MainAngles.clear();
	pDoc->DB_Brep_MainAxes->Clear();

	//Clear previous typology
	ReferApp.Cu.Ty.Typology= Undefined;

	//Clear previous faces
	pDoc->DB_DrawingOff->ClearFaces();

	//Clear previous hidden edges
	ReferApp.Cu.Hi.TypesOfEdges.clear();

	//Clear previous labels
	ReferApp.Cu.La.LabelsInVertices.clear();
	ReferApp.Cu.La.LabelsInEdges.clear();

	//Clear previous symmetries
	ReferApp.Cu.Sy.FaceAxesU.clear();
	ReferApp.Cu.Sy.FaceAxesV.clear();
	ReferApp.Cu.Sy.SymPlanes.clear();
	ReferApp.Cu.Sy.SymMerits.clear();

	//Clear previous levels
	ReferApp.DIL.LevelOfVertex.clear();
}


void CCueManager::FindCues(CDB_Model *DDBB)
// Manages the sequential calculation of cues selected by the user

// WARNING!
// Some cues are dependent on others
// Hence, the sequence is important
// Besides, every cue should check whether the information it requests as input has been calculated
{
	//Calculate valences of the vertices
	if (DetectValences){
		CCueValences V;
		V.GetValences(DDBB);
	}

	//Find parallel edges
	if (DetectParallelEdges){
		CCueParallelEdges PE;
		ReferApp.Cu.PE.ParallelEdges= ReferApp.Cu.VP.ParallelEdges= PE.GetParallelEdges(DDBB);
	}

	//Find Vanishing Points
	if (DetectVanishingPoints){
		CCueVanishingPoints VP(DDBB);
		VP.GetVanishingPoints();
	}

	//Find sub-graphs
	if (DetectGraphs){
		CCueSubGraphs SG;
			SG.GetSubGraphs(DDBB);
	}

	if (DetectGraphs){
		//Find perimeter
		if (ReferApp.GeneralParams.DetectPerimeter){
			CCuePerimeter CP;
			CP.GetPerimeter(DDBB);
		}

		//Calculate Labels
		if (DetectLabels){
			CCueLabels LB;
			LB.GetLabels(DDBB);
		}

		//Find Main Axes
		if (DetectMainAxes){
			CCueMainAxes MA;
			MA.GetMainAxes(DDBB);
		}

		//Find Faces
		if (DetectFaces){
			GetFaces(DDBB);
		}

		//Find the Typology of the 2D model (uses faces, if available)
		if (DetectTypology){
			CCueTypology CT;
			ReferApp.Cu.Ty.Typology= CT.GetTypology(DDBB);
		}

		//Detect hidden edges (requires faces)
		if(DetectHiddenEdges){
			//Pointer to the document that contains the databases
			CCueHiddenEdges CHE;
			CHE.GetHiddenEdges(DDBB);
		}

		//Calculate bilateral symmetry
		if(DetectSymmetry){
			CCueSymmetry CS;
			CS.GetBilateralSymmetryPlanes(DDBB);
		}
	}

	//Calculate levels of the vertices (Uses Hidden edges if available)
	if(DetectLevels){
		CCueLevels CL;
		CL.GetLevels(DDBB);
	}
}