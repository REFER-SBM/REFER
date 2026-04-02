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
#include <cmath>
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "CueManager.h"
#include "DetectionManager.h"
#include "InflationAxonometric.h"
#include "DirectInflation.h"
//#include "DialogProgress.h"
//#include "OptimizationBasedInflation.h"

#include "CueMainAxes.h"

#include "DB_CSG.h"
#include "DataCSG.h"

#include "InOutCSG.h"

#include "CSG_Maths.h"
#include "CSG_Deconstruct.h"
#include "CSG_Modeling.h"


CCSG_Modeling::CCSG_Modeling()
{
}


CCSG_Modeling::~CCSG_Modeling()
{
}


long CCSG_Modeling::CalculeOrigin(CDB_Model *DDBB, long SubGraph)
//Calculates the origin for the current SubGraph

//The lower and most left vertex of the perimeter of the subgraph is selected
//(T vertices of the perimeter are not checked)
{
	//Calculate the threshold based on the length of the longest edge
	double maxLength= 0;
	for(long k=1; k<(long)DDBB->GetSizeEdges(); k++)	{
		if((IsValueInVector(k, ReferApp.Cu.Pe.PerimeterEdges[SubGraph]) &&
		   (DDBB->GetLength2D(k) > maxLength)))
			maxLength= DDBB->GetLength2D(k);
	}
	double threshold= 0.05 * maxLength;

	//Calculate the origin
	long iOrigin= ReferApp.Cu.Pe.PerimeterVertices[SubGraph][0];

	//Check all the vertices of the perimeter
	for(long i=1; i<(long)ReferApp.Cu.Pe.PerimeterVertices[SubGraph].size(); i++){
		long periVerI= ReferApp.Cu.Pe.PerimeterVertices[SubGraph][i];
		if(periVerI < 0)
			continue;

		CVertex Vertexi= DDBB->GetVertex(periVerI);
		CVertex Origin= DDBB->GetVertex(iOrigin);

		if((std::fabs(Origin.x - Vertexi.x) < threshold) &&
			(Vertexi.y < Origin.y)){
			iOrigin= ReferApp.Cu.Pe.PerimeterVertices[SubGraph][i];
		}
		else if(Origin.x - Vertexi.x > threshold){ 
			iOrigin= ReferApp.Cu.Pe.PerimeterVertices[SubGraph][i];
		}
	}

	return iOrigin;
}


void CCSG_Modeling::PreCalculeOrigins(CDB_Model *DDBB, 
									  std::vector<long> &Origins)
//Selects the vertices of each subgraph 
//that will be used as Origins of the Local Coordinate Systems

//The current heuristic criterion is:
//	- The vertex of each subgraph placed at the lower and most left 
//	  location in the input drawing is selected

//	- If only a subraph exists, its central normalo ertex is selected

//Future improvements should consider symmetry planes

//Origins related to subgraphs (Steps and Pockets) 
//are later recalculated when container faces are detected
{
	Origins.clear();

	//Each subgraph will have a different origin
	if(ReferApp.Cu.SG.NumSubgraphs > 1){
		//WARNING: loop from 1 because 
		//when the drawing contains multiple subrgraphs, 
		//the first of the saved perimeters 
		//corresponds to the global perimeter of the whole drawing,
		//(without considering each of the SGs independently)
		for(long i= 1; i<=(long)ReferApp.Cu.SG.NumSubgraphs; i++){
			Origins.push_back(CalculeOrigin(DDBB, i));
		}
	}
	else{
		CInflationAxonometric AI;
		Origins.push_back(AI.GetCentralNormalonVertex(DDBB));
		//Origins.push_back(CalculateOrigin(DDBB, 0));
	}
}


void CCSG_Modeling::InitDatumGraph(CDB_Model *DDBB, CDB_Model *DB_DatumGraph)
//The database of datums for the CSG model is initially populated
//with the vertices and edges of the input 3D model

//Faces of the 2D model are NOT included in the datum graph

//We need the datum graph of the model to include
//the corners of the legs of the decomposed oblique edges, 
//thus being able to calculate distances to the origin
{
	DDBB->CopyVerticesTo(DB_DatumGraph);
	DDBB->CopyEdgesTo(DB_DatumGraph);
}


void CCSG_Modeling::AddMidPointToDatumGraph(CDB_Model *DDBB, 
											CDB_Model *DB_DatumGraph,
											long corner,
											CVertex &midV,  //Return the new vertex
											long &midVert)   //Return its location
//Adds the midpoint of edge "corner" as a new vertex
//Splits the edge in two
{
	CEdge Edg= DDBB->GetEdge(corner);
	CVertex Tail= DDBB->GetVertex(Edg.Tail);
	CVertex Head= DDBB->GetVertex(Edg.Head);

	midV.x= (Tail.x + Head.x)/2;
	midV.y= (Tail.y + Head.y)/2;

	//We add the midpoit as a new vertex to DB_DatumGraph (if not yet included)
	midVert= DB_DatumGraph->GetVertexNum(midV);
	if(midVert == -1){
		DB_DatumGraph->AddVertex(midV);
		midVert= DB_DatumGraph->GetSizeVertices()-1;
	}

	//Split the edge in two sections,
	//A new Edge, plus the original edge with altered enpoint
	CEdge NewEdge;
	NewEdge.Head= Edg.Head;
	NewEdge.Tail= midVert;
	long pos_NewEdg= DB_DatumGraph->GetEdgeByVerticesNums(NewEdge.Head, NewEdge.Tail);
	if(pos_NewEdg == -1){ 
		DB_DatumGraph->AddEdge(NewEdge);
		Edg.Head= midVert;
		DB_DatumGraph->SetEdge(corner, Edg);
	}
}


void CCSG_Modeling::SaveSymPlanesAsDatum(CDB_Model *DDBB_Deconstruct,
										 CDB_Model *DB_DatumGraph,
										 CDB_CSG *DB_CSG_Model)
//Save symmetry planes as datum planes, provided that:
//	- Plane is "orthogonal" (Perpendicular to any main axis)
//	- Merit is over a threshold

//The symmetry planes of all the subgraphs are included

//Updates DB_DatumGraph as well: 
//introducing new vertices at intersections 
//between the symmetry planes and the edges, 
//and splitting the edges accordingly

//Requires cue Symmetry to be calculated in advance
{
	std::vector<double> MainAngles= ReferApp.Cu.MA.MainAngles;

	std::vector<double> ReductionCoefficients;
	ReductionCoefficients.push_back(1/ReferApp.Cu.MA.CoefE[0]);
	ReductionCoefficients.push_back(1/ReferApp.Cu.MA.CoefE[1]);
	ReductionCoefficients.push_back(1/ReferApp.Cu.MA.CoefE[2]);

	//Loop over all the subgraphs
	for(long i=0; i< ReferApp.Cu.SG.NumSubgraphs; i++){

		//Loop over all the symmetry planes in the current subgraph
		for(long j=0; j< (long) ReferApp.Cu.Sy.SymPlanes[i].size(); j++){

			if(ReferApp.Cu.Sy.SymMerits[i][j] < ReferApp.CSG.CSG_ThresholdDMerit)
				continue;   //Ignore symmetry planes below a Threshold merit!!

			//Calculate parameters of the datum plane
			//(Update DB_DatumGraph meanwhile)
			//---------------------------------------
			CDatumPlane DatPlane;

			//Include the corners of the symmetry plane
			CVertex Vert0(0, 0, 0);
			CVertex VertH(0, 0, 0);
			long numVertices= ReferApp.Cu.Sy.SymPlanes[i][j].size()-1; //The first vertex is repeated in the Sym list
			for(long k=0; k<numVertices; k++){
				long corner= ReferApp.Cu.Sy.SymPlanes[i][j][k];
				CVertex VertT;
				if(corner >= 0){   //The corner is in a vertex
					VertT= DDBB_Deconstruct->GetVertex(corner);
					DatPlane.AddPlaneVertex(corner);
				}
				else{   //The corner is the midpoint of an edge
					//Add the corner to the DB_DatumGraph (and split the edge)
					corner= (-corner)-1;  //Renumber the edge, as it was saved as a negative number
					long midVert;
					AddMidPointToDatumGraph(DDBB_Deconstruct, DB_DatumGraph, corner, VertT, midVert);
					DatPlane.AddPlaneVertex(midVert);
				}

				//Continuation
				if(k == 0){
					Vert0= VertT;
				}
				else{
					//Include the edges of the datum plane
					//(it is assumed that vertices are already sequenced)
					CEdge Edge(DB_DatumGraph->GetVertexNum(VertH), DB_DatumGraph->GetVertexNum(VertT));
					if(DB_DatumGraph->GetEdgeNum(Edge) == -1){
						DB_DatumGraph->AddEdge(Edge);
					}
				}

				VertH= VertT;
			}

			//Add closing edge
			CEdge Edge(DB_DatumGraph->GetVertexNum(VertH), DB_DatumGraph->GetVertexNum(Vert0));
			if(DB_DatumGraph->GetEdgeNum(Edge) == -1){
				DB_DatumGraph->AddEdge(Edge);
			}

			DatPlane.SetIDDPlane(DB_CSG_Model->DatumPlanesSize()); //Label with consecutive natural numbers

			//Orientation of the datum plane
			std::vector<long> DatumPlaneVertices;
			for(long k= 0; k<(long)DatPlane.PlaneVerticesSize(); k++){
				DatumPlaneVertices.push_back(DatPlane.GetPlaneVertex(k));
			}

			CPlaneOrientation orient= PlaneOrientation(DB_DatumGraph, 
													   DB_CSG_Model, 
													   DatumPlaneVertices, 
													   MainAngles);
			if(orient != XY && orient != XZ && orient != YZ)
				continue;   //Ignore no-orthogonal symmetry planes !!

			DatPlane.SetOrientation(orient);
			DatPlane.SetSubGraph(i);
			if(ReferApp.CSG.CSG_UseMultipleOrigins){
				DatPlane.SetOrigin(i);   //Use this assigement to calculate distances to local origins
			}
			else{
				DatPlane.SetOrigin(0);   //Use this assigement to calculate distances to global origin
			}

			DatPlane.SetIsSymmetry(1);

			//Add datum plane to the database
			//-------------------------------
			DB_CSG_Model->AddDatumPlane(DatPlane);
		}
	}
}


void CCSG_Modeling::InflateFeatures(CReferDoc* pDoc)
//Inflate the core, the nodes and the features of the CSG model
{
	//Loop over all the branches
	long numBranches= pDoc->DB_CSG_Model->TreeBranchesSize();
	for(long i= numBranches-1; i>=0; i--){    //Select i>0 to avoid inflating core!!!
		//Loop over all the features in the current branch
		//(from contained to containers)

		long numFeatures= (long)pDoc->DB_CSG_Model->TreeBranchSize(i);
		for (long j= numFeatures-1; j>=0; j--){    //Select j>0 to avoid inflating nodes!!!

			CFeature ijFeature= pDoc->DB_CSG_Model->GetFeature(i, j);
				//Recover geometry of the feature
				CDB_Model F2D= ijFeature.GetFeature2D();
					//Skip empty features
					if(F2D.GetSizeVertices() == 0)
						continue;

					//Update cues
					CCueManager C;  //Uses interactively activated detection flags
					//CCueManager C(true, //Set_DetectValences          //Instantiate with individual activation
					//				true, //Set_DetectParallelEdges
					//				true, //Set_DetectVanishingPoints
					//				true, //Set_DetectGraphs
					//				true, //Set_DetectPerimeter
					//				true, //Set_DetectLabels
					//				true, //Set_DetectMainAxes
					//				true, //Set_DetectFaces
					//				true, //Set_DetectTypology
					//				true, //Set_DetectHiddenEdges
					//				true, //Set_DetectSymmetry
					//				true);//Set_DetectLevels

					//(This modifies the database where cues and features are stored)
					C.FindCues(&F2D);

					//Direct inflation
					if(ReferApp.CSG.CSG_Inflation_Direct){
						CDirectInflation DI;
						DI.GetDirectInflation(&F2D);
					}

					//Optimization-based inflation
					if(ReferApp.CSG.CSG_Inflation_Optimization){
						AfxMessageBox ("WARNING: Optimization-based inflation inactive while deconstructing CSG model");
					//	if (ReferApp.In.iOptAlgorithm != NO_OPTIMIZATION){
					//		COptimizationInflation OI;
					//		OI.OptimizationBasedInflation(pDoc); //Optimization cycle
					//	}
					}

					//Transform to main axes
					CDB_Model F2D_Main= F2D;
					//*pDoc->DB_Brep_MainAxes= *pDoc->DB_Brep_Model;
					//CCueMainAxes MainAxes;
					//MainAxes.TransformToMainAxes(&F2D_Main, true);

				ijFeature.SetFeature2D(F2D_Main);

			pDoc->DB_CSG_Model->SetFeature(i, j, ijFeature);
		}
	}
}


void CCSG_Modeling::GetCSG(CReferDoc* pDoc)
//CSG-based modeling approach:
//	The algorithm detects and analyzes features of 2D drawings in isolation, 
//	and defines the CSG feature tree by the parent–child relationships between features, 
//	and combines this information to obtain a complete and consistent CSG feature tree
//	that can be transferred to a 3D modeler

//The main flow is as follows:
//	- Recursively deconstruct the line-drawing to extract the embeeded features
//	- Sequentially add features to get the model tree

//The method proposes a set of perceptual criteria 
//to determine the feature that:
//	- is a child of the last level and that, at the same time, 
//	- is as "local" as possible.
//	  (By “local” we mean that it is a feature that is 
//	  the child of a single parent and that 
//	  has no indirect links with other elements)

//By recursively applying this process, 
//the features that remained "masked" by others are successively discovered.

//The recursive process ends when reaching the core piece 
//(a piece of material that can be worked on
//by adding or subtracting features to obtain the finished piece)
//of the represented object.


//Features must have been detected in advance.

//Based on:
//	Plumed R.
//	Sobre la percepción de indicios en bocetos de ingeniería
//	y su aplicación al modelado basado en bocetos.
//	PhD thesis. Universitat Jaume I, 2014

//	Plumed R., Varley P.A.C., Company P. and Martin R.R. (2022)
//	Extracting datums to reconstruct CSG Models from 2D engineering sketches of polyhedral shapes. 
//	Computers & Graphics , Vol. 102, pp. 349-359. DOI: 10.1016/j.cag.2021.10.013.
{
	//Check if target features have been identified in the 2D drawing
	if (ReferApp.Fe.RD.NumRibs == 0 &&
		ReferApp.Fe.RD.NumDividers == 0 &&
		ReferApp.Fe.SP.NumSteps == 0 &&
		ReferApp.Fe.SP.NumPockets == 0 &&
		ReferApp.Fe.RS.NumSlots == 0 &&
		ReferApp.Fe.RS.NumRails == 0){

		CString sMensaje;
		sMensaje= "CSG modeling requires prior detection of features";
		AfxMessageBox (sMensaje);
		CReferDoc::GetDoc()->RefreshWindows();
		return;
	}

	//RECOVER INPUT INFORMATION TO DECONSTRUCT
	//(Must have been calculated in advance)
	//----------------------------------------
	if(ReferApp.Cu.MA.MainAngles.size() != 3){
		CString sMensaje;
		sMensaje= "CSG deconstruction requires prior detection of Main Axes";
		AfxMessageBox (sMensaje);
		CReferDoc::GetDoc()->RefreshWindows();
		return;
	}

	if(ReferApp.Cu.SG.NumSubgraphs == 0){
		CString sMensaje;
		sMensaje= "CSG deconstruction requires prior detection of Subgraphs";
		AfxMessageBox (sMensaje);
		CReferDoc::GetDoc()->RefreshWindows();
		return;
	}

	if(ReferApp.Cu.Pe.PerimeterVertices.size() == 0){
		CString sMensaje;
		sMensaje= "CSG deconstruction requires prior detection of Perimeters";
		AfxMessageBox (sMensaje);
		CReferDoc::GetDoc()->RefreshWindows();
		return;
	}

	//PRE-CALCULATE DATUMS AND DATUM GRAPH
	//------------------------------------
	if(ReferApp.Cu.Sy.SymPlanes.size() == 0){
		CString sMensaje;
		sMensaje= "CSG deconstruction requires prior detection of symmetry planes";
		AfxMessageBox (sMensaje);
		CReferDoc::GetDoc()->RefreshWindows();
		return;
	}

	//Symmetry planes are selected as candidate datums
	//(and datumgraph is updated accordingly)
	SaveSymPlanesAsDatum(&pDoc->DB_Deconstruct[0],
						 pDoc->DB_DatumGraph,
						 pDoc->DB_CSG_Model);

	//Precalcule the "Origin" vertices of each subgraph
	//that will be used as origins of their Local Coordinate Systems
	std::vector<long> Origins;
	PreCalculeOrigins(&pDoc->DB_Deconstruct[0], Origins);
	pDoc->DB_CSG_Model->SetDatumOrigins(Origins);

	//DECONSTRUCT CSG 2D
	//------------------
	CCSG_Deconstruct DEC;
	DEC.SetUp();

	pDoc->DB_CSG_Model->ClearBranch();

	//Deconstructs the line drawing to arise the underlying CSG model
	//by RECURSIVELY removing all features
	//until the basic shape ("CORE") is detected
	bool FeatureFather= false;
	DEC.DeconstructCSG_2D(pDoc, FeatureFather);  //RECURSIVE

	//INFLATE the core, the nodes and the features, if inflation is active
	//--------------------------------------------------------------------
	if(ReferApp.CSG.CSG_Inflation){
		InflateFeatures(pDoc);
	}

	//Restore the database of cues and features of the whole drawing
	//(Because it has been altered during recursion)
	//--------------------------------------------------------------
	CDetectionManager DM;
	DM.DetectCuesAndFeatures(pDoc->DB_DrawingOff);

}
