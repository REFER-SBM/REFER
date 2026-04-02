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

//Get labels from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003

#include <stdafx.h>
#include <cstdio>

#include <cassert>
#include <cmath>

//-------used to import/export data from/to the external database-----------
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
#include "DB_CSG.h"
#include "resource.h"
#include "ReferDoc.h"
#include "Refer.h"
//--------------------------------------------------------------------------

#include "DataLabels.h"
#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"
#include "CueLabelsVarley_LabellingConstants.h"
#include "CueLabelsVarley_LabellingParameters.h"
#include "CueLabelsVarley_LineEquations.h"

#include "CueLabelsVarley_IdentifyJunctionAngles.h"
#include "CueLabelsVarley_InitialiseJunctionLabels.h"

#include "CueLabelsVarley_LineToJunctions.h"
#include "CueLabelsVarley_JunctionToLines.h"

#include "CueLabelsVarley_Combinatorially.h"
#include "CueLabelsVarley_Relaxation.h"
#include "CueLabelsVarley_Lamban.h"
#include "CueLabelsVarley.h"


CCueLabelsVarley::CCueLabelsVarley()
{
	//Labelling constants (tuning constants)
	LC= new CCueLabelsVarley_LabellingConstants;

	//Labelling parameters
	LP= new CCueLabelsVarley_LabellingParameters;

	//Local database
	Object= new OBJECT;

	//Reset vertices in Object
	Object->Labelling.VertexCount = 0;
	for (int n=0; n<MAX_VERTICES; ++n) 
		Object->Labelling.VECount[n] = 0;

	for (VERTEX_INDEX i=0; i<MAX_VERTICES; ++i)
	{
		Object->Labelling.VertexLabel[i].Shape  = JunctionIsUndefined;
		Object->Labelling.VertexLabel[i].Vexity = 0;
		Object->Labelling.VertexUnderlyingType[i] = VFinalIsUnknown;
		Object->Labelling.VertexLabellingIsFinalised[i] = cFALSE;
	}

	//Reset edges in Object
	//for (int n=0; n<MAX_EDGES; ++n){
	//	Object->Labelling.EdgeLabelIsFinalised[n] = cFALSE;
	//}

	Object->Labelling.EdgeCount = 0;
	//for (EDGE_INDEX i=0; i<MAX_EDGES; ++i){
	//	Object->Labelling.EdgeLabel[i] = LineIsUnknown;
	//}
	for (int i=0; i<MAX_EDGES; ++i)
		Object->Labelling.EdgeStartVertex[i] = Object->Labelling.EdgeEndVertex[i] = NO_VERTEX;

	for (int i=0; i<MAX_EDGES; ++i)
		Object->Labelling.EdgeForwardFace[i] = Object->Labelling.EdgeReverseFace[i] = NO_FACE;

	Object->FaceCount = 0;

}


CCueLabelsVarley::~CCueLabelsVarley()
{
	//Destroy labelling constants (tuning constants)
	if(LC){
		delete LC;
		LC= NULL;
	}

	////Labelling parameters
	if(LP){
		delete LP;
		LP= NULL;
	}

	////Local database
	if(Object){
		delete Object;
		Object= NULL;
	}
}


void CCueLabelsVarley::DetermineLimits (OBJECT *Object)
{
   int n;

   Object->LeftX    = Object->RightX   = Object->Coordinates[0].x;
   Object->TopY     = Object->BottomY  = Object->Coordinates[0].y;
   Object->FrontZ   = Object->BackZ    = Object->Coordinates[0].z;

   for (n=1; n<Object->Labelling.VertexCount; ++n)
   {
      POINT3D P = Object->Coordinates[n];

      if (P.x < Object->LeftX)    Object->LeftX   = P.x;
      if (P.x > Object->RightX)   Object->RightX  = P.x;
      if (P.y < Object->TopY)     Object->TopY    = P.y;
      if (P.y > Object->BottomY)  Object->BottomY = P.y;
      if (P.z < Object->FrontZ)   Object->FrontZ  = P.z;
      if (P.z > Object->BackZ)    Object->BackZ   = P.z;
   }

   Object->MaxBound = max(Object->RightX-Object->LeftX, max(Object->BottomY-Object->TopY,Object->BackZ-Object->FrontZ));
   Object->ViewScale = Object->MaxBound * 1.1;

   Object->MidX = (Object->LeftX+Object->RightX)/2;
   Object->MidY = (Object->TopY+Object->BottomY)/2;
}


void CCueLabelsVarley::CreateEdgeJoiningTable (OBJECT *Object)
{
   for (VERTEX_INDEX a = 0;  a < MAX_VERTICES;  ++a)
   {
      for (VERTEX_INDEX b = 0;  b < MAX_VERTICES;  ++b)
      {
         Object->EdgeExistsTable[a][b] = cFALSE;
      }
   }

   for (EDGE_INDEX e = 0;  e < Object->Labelling.EdgeCount; ++e)
   {
      VERTEX_INDEX S = Object->Labelling.EdgeStartVertex[e];
      VERTEX_INDEX T = Object->Labelling.EdgeEndVertex[e];
      Object->EdgeExistsTable[S][T] = Object->EdgeExistsTable[T][S] = cTRUE;
   }

   for (VERTEX_INDEX v = 0;  v < Object->Labelling.VertexCount; ++v)
   {
      EDGE_INDEX e = Object->HiddenByEdge[v];
      if (e != NO_EDGE)
      {
         VERTEX_INDEX S = Object->Labelling.EdgeStartVertex[e];
         VERTEX_INDEX T = Object->Labelling.EdgeEndVertex[e];
         Object->EdgeExistsTable[S][v] = Object->EdgeExistsTable[v][S] = cTRUE;
         Object->EdgeExistsTable[T][v] = Object->EdgeExistsTable[v][T] = cTRUE;
      }
   }
}


void CCueLabelsVarley::Input_Varley(CDB_Model *DDBB)
// Copies information from external DataBase
// into the local database of this FindingLabels algorithm

// The intput required by the algorithms is:
//		VertexCount= number of vertex in the wireframe
//		VertexX[i]= X coordinate of the i-th vertex
//		VertexY[i]= Y coordinate of the i-th vertex
//		VertexZ[i]= Z coordinate of the i-th vertex
//		(VertexZ onlyr required for the Inflated approach)
//		EdgeCount= number of edges in the wireframe
//		EdgeU[i]= Head vertex defining the i-th edge
//		EdgeV[i]= Tail vertex defining the i-th edge

// This function should be adapted to different external databases
// This function should be called before calculating labels by Get_Labels_Varley
{

	//Copy vertices from global to local database
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		CVertex Vertice= DDBB->GetVertex(i);

		VERTEX_INDEX ii = NewVertex(Object);
		//Object->Labelling.VertexLabel[ii].Shape  = JunctionIsUndefined;
		//Object->Labelling.VertexLabel[ii].Vexity = 0;

		//Object->Labelling.VENumber[Vertex][0] = NO_EDGE;
		//Object->Labelling.VECount[Vertex] = 0;
		//Object->HiddenByEdge[Vertex] = NO_EDGE;

		Object->Coordinates[ii].x= Vertice.x;
		Object->Coordinates[ii].y= Vertice.y;
		Object->Coordinates[ii].z= Vertice.z;
		Object->Vertices[ii].XYKnown = cTRUE;
	}

	//Copy edges from global to local database
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge Arista= DDBB->GetEdge(i);

		VERTEX_INDEX v0= (VERTEX_INDEX)Arista.Head;
		VERTEX_INDEX v1= (VERTEX_INDEX)Arista.Tail;
		EDGE_INDEX Line= NewEdge(Object,VISIBLE,v0,v1);
		StoreEdgeInVertexRecord(Object,v0,Line);
		StoreEdgeInVertexRecord(Object,v1,Line);
	}

	//Calculate derived data
	DetermineLimits(Object);
	CCueLabelsVarley_LineEquations LE;
	LE.GetAllLineEquations(Object);
	CreateEdgeJoiningTable(Object);

	//Copy subgraph information
	Object->SubgraphCount= (GRAPHLOOP)ReferApp.Cu.SG.NumSubgraphs;

	//Copy contour from global to local database
	for(long NumGraph= 0; NumGraph<ReferApp.Cu.SG.NumSubgraphs; NumGraph++){
		for (long i=0; i<(long)ReferApp.Cu.Pe.PerimeterVertices[NumGraph].size(); i++){
			Object->IsOnBoundary[ReferApp.Cu.Pe.PerimeterVertices[NumGraph][i]] = cTRUE;
		}

		long NumVertex= 0;
		for (long i=0; i<(long)ReferApp.Cu.Pe.PerimeterEdges[NumGraph].size(); i++){
			long NumEdge= ReferApp.Cu.Pe.PerimeterEdges[NumGraph][i];
			Object->Edges[NumEdge].OnSubgraphBoundary = cTRUE;

			long First= ReferApp.Cu.Pe.PerimeterVertices[NumGraph][NumVertex];
			NumVertex++;
			long Second= ReferApp.Cu.Pe.PerimeterVertices[NumGraph][NumVertex];

			CEdge Edge= DDBB->GetEdge(NumEdge);
			if (Edge.Head== Second && Edge.Tail== First){
				Object->Labelling.EdgeReverseFace[NumEdge]= BACKGROUND_FACE;
			}
			else{
				Object->Labelling.EdgeForwardFace[NumEdge]= BACKGROUND_FACE;
			}
		}
	}

	//FindAndStoreEdgeInLoop

	//Copy faces from global to local database
	//for (long i=0; i<DDBB->GetSizeFaces(); i++){
	//	CFace Cara= DDBB->GetFace(i);

	//	lBOOLEAN IsHidden= cTRUE;
	//	FACE_INDEX Face= NewFace(Object, IsHidden);
	//}

	//MakeEdgesCorrectForFace(Object,Nface,cTRUE);
	//MakeFaceEquation(Object,Nface);
}


void CCueLabelsVarley::SelectApproach(short int &LabellingProcedure,
									  char &WhichLabellingCatalogue,
									  int &RelaxationIterations,
									  bool &UseJunctionStatisticsForRelaxationLabelling,
									  bool &UseShapePairStatisticsForRelaxationLabelling,
									  bool &UseMaxInRelaxationLabelling)

//LABELLING_COMBIN_ONLY
// Use Kanatani's combinatorial labelling only

//LABELLING_RELAXN_COMBIN
// Use a defined number of iterations of relaxation labelling;
// if that didn't converge, use combinatorial labelling to
// finish off whatever is left.
// There are various options for the input probabilities to
// relaxation labelling.
// Note that (as implied) relaxation labelling on its own may
// not converge.

//LABELLING_LAMBAN_RELAXN_COMBIN
// Use geometric labelling to determine the initial probability
// values for relaxation labelling.
// Otherwise, as previous method.
// Note that geometric labelling on its own isn't worth bothering
// with.

//LABELLING_COMBIN_LAMBAN_RELAXN
// Start with equal probabilities for all labels.
// Run combinatorial labelling on the assumption that the drawing
// is trihedral: if this results in a unique labelling, bump up
// the probabilities for these labels.
// If that didn't work, try again assuming that the drawing is
// extended trihedral.
// Run geometric labelling, and bump up the probabilities for the
// predicted line labels.
// Run relaxation labelling using the probabilties so determined.
// And finally, if that didn't converge, go back to combinatorial
// labelling to finish off whatever is left.

//LABELLING_COMBIN_RELAXN
// Start with equal probabilities for all labels.
// Run combinatorial labelling on the assumption that the drawing
// is trihedral: if this results in a unique labelling, bump up
// the probabilities for these labels.
// If that didn't work, try again assuming that the drawing is
// extended trihedral.
// Run relaxation labelling using the probabilties so determined.
// And finally, if that didn't converge, go back to combinatorial
// labelling to finish off whatever is left.
// ================================================================================
{

	//LabellingProcedure = ReferApp.Cu.La.LabelsApproach;
	//WhichLabellingCatalogue = ReferApp.Cu.La.VertexCatalogue;

	int val= 4;
		//val= 1; //"SI Full Catalogue";
		//val= 2; //"SI XTrihedral Catalogue"
		//val= 3; //"SI Trihedral Catalogue"
		//val= 4; //"Rel 30 Iterations"
		//val= 5; //"Rel 100 Iterations"
		//val= 6; //"Rel 300 Iterations"
		//val= 7; //"Rel 300 Iterations (H)"
		//val= 10; //"Lamban (Fixed) + Rel 300"
		//val= 13; //"Lamban (Fixed)(H) + Rel 300"
		//val= 11; //"Lamban (Adjust) + Rel 300"
		//val= 12; //"Lamban (Adjust,Deduce) + Rel 300"
		//val= 20; //"Combin + (Lamban Fixed) + Rel 300"
		//val= 21; //"Combin + (Lamban Adjust) + Rel 300"
		//val= 22; //"Combin + (Lamban Adjust,Deduce) + Rel 300"
		//val= 30; //"Trihedral Only"
		//val= 31; //"Trihedral + KZ"
		//val= 32; //"Trihedral + MX"
		//val= 40; //"Normal"
		//val= 42; //"Unified"
		//val= 43; //"Unified+Binary"
		//val= 51; //"Lamban Fixed IJK"
		//val= 52; //"Lamban Frequent IJK"
		//val= 53; //"Lamban Frequent IJ Fixed K"
		//val= 54; //"Lamban Longest IJK"
		//val= 55; //"Lamban Longest IJ Fixed K"
		//val= 56; //"Lamban Frequent IJ Opt K"
		//val= 57; //"Lamban Frequent IJ Middle K"

   switch (val)
   {
      case 1:
         LabellingProcedure = LABELLING_COMBIN_ONLY;
         WhichLabellingCatalogue = LABELLING_HEXAHEDRAL;
      break;

      case 2:
         LabellingProcedure = LABELLING_COMBIN_ONLY;
         WhichLabellingCatalogue = LABELLING_NTAA;
      break;

      case 3:
         LabellingProcedure = LABELLING_COMBIN_ONLY;
         WhichLabellingCatalogue = LABELLING_TRIHEDRAL;
      break;

      case 4:
         LabellingProcedure = LABELLING_RELAXN_COMBIN;
         RelaxationIterations = 30;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;

         WhichLabellingCatalogue = LABELLING_HEXAHEDRAL;
      break;

      case 5:
         LabellingProcedure = LABELLING_RELAXN_COMBIN;
         RelaxationIterations = 100;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;

         WhichLabellingCatalogue = LABELLING_HEXAHEDRAL;
      break;

      case 6:
         LabellingProcedure = LABELLING_RELAXN_COMBIN;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
         UseMaxInRelaxationLabelling = false;

         WhichLabellingCatalogue = LABELLING_HEXAHEDRAL;
      break;

      case 7:
         LabellingProcedure = LABELLING_RELAXN_COMBIN;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
         UseMaxInRelaxationLabelling = true;

         WhichLabellingCatalogue = LABELLING_HEXAHEDRAL;
      break;

      case 10:
         LabellingProcedure = LABELLING_LAMBAN_RELAXN_COMBIN;
         LP->LambanAxisOptions = LambanAxesFixedIJK;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
         UseMaxInRelaxationLabelling = false;
      break;

      case 13:
         LabellingProcedure = LABELLING_LAMBAN_RELAXN_COMBIN;
         LP->LambanAxisOptions = LambanAxesFixedIJK;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
         UseMaxInRelaxationLabelling = true;
      break;

      case 11:
         LabellingProcedure = LABELLING_LAMBAN_RELAXN_COMBIN;
         LP->LambanAxisOptions = LambanAxesFrequentIJOptK;
         LP->UseDeductionsInLamban = false;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
      break;

      case 12:
         LabellingProcedure = LABELLING_LAMBAN_RELAXN_COMBIN;
         LP->LambanAxisOptions = LambanAxesFrequentIJOptK;
         LP->UseDeductionsInLamban = true;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
      break;

      case 20:
         LabellingProcedure = LABELLING_COMBIN_LAMBAN_RELAXN;
         LP->LambanAxisOptions = LambanAxesFixedIJK;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
      break;

      case 21:
         LabellingProcedure = LABELLING_COMBIN_LAMBAN_RELAXN;
         LP->LambanAxisOptions = LambanAxesFrequentIJOptK;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
      break;

      case 22:
         LabellingProcedure = LABELLING_COMBIN_LAMBAN_RELAXN;
         LP->LambanAxisOptions = LambanAxesFrequentIJOptK;
         LP->UseDeductionsInLamban = true;
         RelaxationIterations = 300;
         UseJunctionStatisticsForRelaxationLabelling = false;
         UseShapePairStatisticsForRelaxationLabelling = false;
      break;

      case 30:
         ////SketchedObject->Labelling.FinalVertexTypes = VFinalIsTrihedral;
      break;

      case 31:
         ////SketchedObject->Labelling.FinalVertexTypes = VFinalIsTrihedral|VFinalIsKany|VFinalIsZany;
      break;

      case 32:
         ////SketchedObject->Labelling.FinalVertexTypes = VFinalIsTrihedral|VFinalIsMany|VFinalIsXany;
      break;

      case 40:
         UnifiedFrontalGeometry= cFALSE;
      break;

      case 42:
         RelaxationIterations = 600;
         UnifiedFrontalGeometry= cTRUE;
         UseBinaryFrontalGeometry(true, cFALSE);
         WhichLabellingCatalogue = LABELLING_HEXAHEDRAL;
      break;

      case 43:
         RelaxationIterations = 600;
         UnifiedFrontalGeometry= cTRUE;
         UseBinaryFrontalGeometry(true, cTRUE);
         WhichLabellingCatalogue = LABELLING_HEXAHEDRAL;
      break;

      case 51:
         LP->LambanAxisOptions = LambanAxesFixedIJK;
      break;

      case 52:
         LP->LambanAxisOptions = LambanAxesFrequentIJK;
      break;

      case 53:
         LP->LambanAxisOptions = LambanAxesFrequentIJFixedK;
      break;

      case 54:
         LP->LambanAxisOptions = LambanAxesLongestIJK;
      break;

      case 55:
         LP->LambanAxisOptions = LambanAxesLongestIJFixedK;
      break;

      case 56:
         LP->LambanAxisOptions = LambanAxesFrequentIJOptK;
      break;

      case 57:
         LP->LambanAxisOptions = LambanAxesFrequentIJMiddleK;
      break;

      default:;
   }
}


void CCueLabelsVarley::InitialiseLabellingBlackboard (OBJECT *Object,
													  short int LabellingProcedure,
													  int RelaxationIterations,
													  bool UseJunctionStatisticsForRelaxationLabelling,
													  bool UseShapePairStatisticsForRelaxationLabelling,
													  bool UseMaxInRelaxationLabelling)
{
	//For each vertex, mark that it does not have a final type
	for (int n=0; n<Object->Labelling.VertexCount; ++n)
		Object->Labelling.VertexLabellingIsFinalised[n] = cFALSE;

	//For each edge, mark that the edge type has not been propagated
	for (int n=0; n<Object->Labelling.EdgeCount; ++n)
		Object->Labelling.EdgeLabelIsFinalised[n] = cFALSE;

	//For each edge on the object boundary,
	//set the edge type to arrow or backarrow as appropriate

	//Set extreme junctions on boundary
	CCueLabelsVarley_LineToJunctions LtJ;
	for (int n=0; n<Object->Labelling.EdgeCount; ++n)
	{
		if (Object->Labelling.EdgeForwardFace[n] == BACKGROUND_FACE)
		{
			LabelEdgeAsBackArrow(Object,n);
			LtJ.PropagateConstraintsFromArrow(&Object->Labelling,n);
		}
		else
		if (Object->Labelling.EdgeReverseFace[n] == BACKGROUND_FACE)
		{
			LabelEdgeAsArrow(Object,n);
			LtJ.PropagateConstraintsFromArrow(&Object->Labelling,n);
		}
	}

	//Do one iteration of combinatorial labelling to propagate the really obvious labels to edges
	CCueLabelsVarley_JunctionToLines JtL(LC);
	JtL.PropagateJunctionConstraintsToLines(Object,&Object->Labelling);

	//This is done by the calling program (RibaldIO or RibaldCLI)       (Get_Labels_Varley, in this implementation)
	//but it needs to be redone if we use a second labelling attempt
	LC->SetLabellingConstants(LabellingProcedure,
							 UseJunctionStatisticsForRelaxationLabelling,
							 UseShapePairStatisticsForRelaxationLabelling,
							 RelaxationIterations,
							 UseMaxInRelaxationLabelling,
							 LP->LambanAxisOptions,
							 LP->UseDeductionsInLamban);
}


void CCueLabelsVarley::CreateLabellingSubset (LABELLING *Output, LABELLING *Input, char LabellingType)
{
	*Output = *Input;

	CCueLabelsVarley_InitialiseJunctionLabels IJL;
	IJL.SetAllJunctionsToType(Output,LabellingType);
}


void CCueLabelsVarley::AddLabellingToProbabilities (OBJECT *Object, LABELLING *Labelling)
{
   for (int n=0; n<Labelling->VertexCount; ++n)
   {
	  switch(Labelling->VertexLabel[n].Shape)
	  {
         case JunctionIsL:
         {
			MERIT *LP = Object->Vprobabilities[n];
            switch (Labelling->VertexLabel[n].Vexity)
            {
			   case Lba:  LP[Lbamerit] += 1.0;  break;
			   case Lab:  LP[Labmerit] += 1.0;  break;
			   case Lac:  LP[Lacmerit] += 1.0;  break;
			   case Lcb:  LP[Lcbmerit] += 1.0;  break;
			   case Lbd:  LP[Lbdmerit] += 1.0;  break;
			   case Lda:  LP[Ldamerit] += 1.0;  break;
			   default: break;
			}
         }
         break;

         case JunctionIsT:
         {
			MERIT *TP = Object->Vprobabilities[n];
            switch (Labelling->VertexLabel[n].Vexity)
            {
			   case Tbaa:  TP[Tbaamerit] += 1.0;  break;
			   case Tbab:  TP[Tbabmerit] += 1.0;  break;
			   case Tbac:  TP[Tbacmerit] += 1.0;  break;
			   case Tbad:  TP[Tbadmerit] += 1.0;  break;
			   default: break;
			}
         }
         break;

         case JunctionIsT4:
         {
			MERIT *TP = Object->Vprobabilities[n];
            switch (Labelling->VertexLabel[n].Vexity)
            {
			   case Tbaa:  TP[Tbaamerit] += 1.0;  break;
			   case Tbab:  TP[Tbabmerit] += 1.0;  break;
			   case Tbac:  TP[Tbacmerit] += 1.0;  break;
			   case Tbad:  TP[Tbadmerit] += 1.0;  break;
			   case Tbda:  TP[Tbdamerit] += 1.0;  break;
			   case Tbdc:  TP[Tbdcmerit] += 1.0;  break;
			   case Tdab:  TP[Tdabmerit] += 1.0;  break;
			   case Tdac:  TP[Tdacmerit] += 1.0;  break;
			   default: break;
			}
         }
         break;

         case JunctionIsW:
         {
			WPROBABILITIES *WP = (WPROBABILITIES *)(Object->Vprobabilities[n]);
            switch (Labelling->VertexLabel[n].Vexity)
            {
			   case Wbca:  WP->Wbcamerit += 1.0;  break;
			   case Wcdc:  WP->Wcdcmerit += 1.0;  break;
			   case Wdcd:  WP->Wdcdmerit += 1.0;  break;
			   default: break;
			}
         }
         break;

         case JunctionIsY:
         {
			YPROBABILITIES *YP = (YPROBABILITIES *)(Object->Vprobabilities[n]);
            switch (Labelling->VertexLabel[n].Vexity)
            {
			   case Yabd: // Yabd is tricky
			      YP->Yabdmerit += 0.333;
			      YP->Ybdamerit += 0.333;
			      YP->Ydabmerit += 0.333;
			   break;

			   case Yccc:  YP->Ycccmerit += 1.0;  break;
			   case Yddd:  YP->Ydddmerit += 1.0;  break;
			   default: break;
			}
         }
         break;
	  }
   }

   for (int n=0; n<Labelling->EdgeCount; ++n)
   {
	  switch(Labelling->EdgeLabel[n])
	  {
         case LineIsConvex:
            Object->Eprobabilities[n].Cmerit += 1.0;
         break;

         case LineIsConcave:
            Object->Eprobabilities[n].Dmerit += 1.0;
         break;

         case LineIsConvex|LineIsConcave:
            Object->Eprobabilities[n].Cmerit += 0.25;
            Object->Eprobabilities[n].Dmerit += 0.25;
         break;

         case LineIsArrow|LineIsBackArrow:
            Object->Eprobabilities[n].Amerit += 0.25;
            Object->Eprobabilities[n].Bmerit += 0.25;
         break;

         case LineIsArrow:
            // Minor problem area as the line direction may have been reversed
            if (Labelling->EdgeStartVertex[n] == Object->Labelling.EdgeStartVertex[n])
               Object->Eprobabilities[n].Amerit += 1.0;
            else
               Object->Eprobabilities[n].Bmerit += 1.0;
         break;

         case LineIsArrow|LineIsConcave:
            // Minor problem area as the line direction may have been reversed
            if (Labelling->EdgeStartVertex[n] == Object->Labelling.EdgeStartVertex[n])
               Object->Eprobabilities[n].Amerit += 0.25;
            else
               Object->Eprobabilities[n].Bmerit += 0.25;
            Object->Eprobabilities[n].Dmerit += 0.25;
         break;

         case LineIsBackArrow|LineIsConcave:
            // Minor problem area as the line direction may have been reversed
            if (Labelling->EdgeStartVertex[n] == Object->Labelling.EdgeStartVertex[n])
               Object->Eprobabilities[n].Bmerit += 0.25;
            else
               Object->Eprobabilities[n].Amerit += 0.25;
            Object->Eprobabilities[n].Dmerit += 0.25;
         break;

         case LineIsUnknown:
            // Don't do anything
         break;

         default:
            printf("TBD: what to do with edge type %x\n", Labelling->EdgeLabel[n]);
         break;
	  }
   }
}


void CCueLabelsVarley::AddCombinatorialResultsToRelaxation (OBJECT *Object)
{
	LABELLING TemporaryLabelling;

	CCueLabelsVarley_Combinatorially LLC(LC, LP);

	CreateLabellingSubset(&TemporaryLabelling,&Object->Labelling,LABELLING_TRIHEDRAL);
	if (LLC.LabelLinesUsingKanatani(Object,&TemporaryLabelling))
	{
		AddLabellingToProbabilities(Object,&TemporaryLabelling);
	}
	else
	{
		CreateLabellingSubset(&TemporaryLabelling,&Object->Labelling,LABELLING_NTAA);
		if (LLC.LabelLinesUsingKanatani(Object,&TemporaryLabelling))
		{
			AddLabellingToProbabilities(Object,&TemporaryLabelling);
		}
	}
}


bool CCueLabelsVarley::LabelSketch (OBJECT *Object,
									short int LabellingProcedure,
									int RelaxationIterations,
									bool UseJunctionStatisticsForRelaxationLabelling,
									bool UseShapePairStatisticsForRelaxationLabelling,
									bool UseMaxInRelaxationLabelling)
// Label Lines and Faces

//RRM notwithstanding, normalising the object to centre it around (0,0) doesn't help much
//if at all, and makes PostScript throw a wobbly when we come to print it. Don't do it.
{
	InitialiseLabellingBlackboard(Object,
								  LabellingProcedure,
								  RelaxationIterations,
								  UseJunctionStatisticsForRelaxationLabelling,
								  UseShapePairStatisticsForRelaxationLabelling,
								  UseMaxInRelaxationLabelling);

	CCueLabelsVarley_Combinatorially LLC(LC, LP);
	CCueLabelsVarley_Relaxation LLR(LC, LP);
	CCueLabelsVarley_Lamban Lam(LC, LP);

	switch(LabellingProcedure)
	{
		case LABELLING_COMBIN_ONLY:
			if (LLC.LabelLinesCombinatorially(Object))  return cTRUE;
		break;

		case LABELLING_RELAXN_COMBIN:
			LLR.InitialiseRelaxationEdgeLabels(Object);
			LLR.InitialiseRelaxationVertexLabels(Object);
			LLR.UpdateRelaxationProbabilitiesFromFeatures(Object);
			LLR.RenormaliseVertexProbabilities(Object);
			LLR.RenormaliseEdgeProbabilities(Object);
			if (LLR.LabelLinesUsingRelaxation(Object,
											  RelaxationIterations,
											  UseJunctionStatisticsForRelaxationLabelling,
											  UseShapePairStatisticsForRelaxationLabelling,
											  UseMaxInRelaxationLabelling))  return cTRUE;

			if (LLC.LabelLinesCombinatorially(Object))  return cTRUE;
		break;

		case LABELLING_LAMBAN_RELAXN_COMBIN:
			LLR.InitialiseRelaxationEdgeLabels(Object);
			LLR.InitialiseRelaxationVertexLabels(Object);
			Lam.LabelLinesUsingLamban(Object);
			LLR.UpdateRelaxationProbabilitiesFromFeatures(Object);
			LLR.RenormaliseVertexProbabilities(Object);
			LLR.RenormaliseEdgeProbabilities(Object);
			if (LLR.LabelLinesUsingRelaxation(Object,
											  RelaxationIterations,
											  UseJunctionStatisticsForRelaxationLabelling,
											  UseShapePairStatisticsForRelaxationLabelling,
											  UseMaxInRelaxationLabelling))  return cTRUE;

			if (LLC.LabelLinesCombinatorially(Object))  return cTRUE;
		break;

		case LABELLING_COMBIN_LAMBAN_RELAXN:
			LLR.InitialiseRelaxationEdgeLabels(Object);
			LLR.InitialiseRelaxationVertexLabels(Object);
			Lam.LabelLinesUsingLamban(Object);
			AddCombinatorialResultsToRelaxation(Object);
			LLR.UpdateRelaxationProbabilitiesFromFeatures(Object);
			LLR.RenormaliseVertexProbabilities(Object);
			LLR.RenormaliseEdgeProbabilities(Object);
			if (LLR.LabelLinesUsingRelaxation(Object,
											  RelaxationIterations,
											  UseJunctionStatisticsForRelaxationLabelling,
											  UseShapePairStatisticsForRelaxationLabelling,
											  UseMaxInRelaxationLabelling))  return cTRUE;
			if (LLC.LabelLinesCombinatorially(Object))  return cTRUE;
		break;

		case LABELLING_COMBIN_RELAXN:
			LLR.InitialiseRelaxationEdgeLabels(Object);
			LLR.InitialiseRelaxationVertexLabels(Object);
			AddCombinatorialResultsToRelaxation(Object);
			LLR.UpdateRelaxationProbabilitiesFromFeatures(Object);
			LLR.RenormaliseVertexProbabilities(Object);
			LLR.RenormaliseEdgeProbabilities(Object);
			if (LLR.LabelLinesUsingRelaxation(Object,
											  RelaxationIterations,
											  UseJunctionStatisticsForRelaxationLabelling,
											  UseShapePairStatisticsForRelaxationLabelling,
											  UseMaxInRelaxationLabelling))  return cTRUE;
			if (LLC.LabelLinesCombinatorially(Object))  return cTRUE;
		break;
	}
	return false;
}


void CCueLabelsVarley::ConvertT4JunctionToOccluding (OBJECT *Object, VERTEX_INDEX Nvert)
{
   //COFACIAL_CONFIGURATION *C;

   EDGE_INDEX A = Object->Labelling.VENumber[Nvert][0];
   EDGE_INDEX B = Object->Labelling.VENumber[Nvert][1];
   FACE_INDEX OccludingFace = FaceContainingEdges(Object,A,B);

   CheckVertexNumber(&Object->Labelling,Nvert,"ConvertT4JunctionToOccluding");

   VERTEX_INDEX Va = EdgeOtherEnd(Object,A,Nvert);
   VERTEX_INDEX Vb = EdgeOtherEnd(Object,B,Nvert);

   //Return to trihedral
   ReplaceEdgeInVertexRecord(Object,Vb,B,A);
   ReplaceVertexInEdgeRecord(Object,A,Nvert,Vb);
   Object->Vertices[Nvert].IsHidden = cTRUE;
   Object->HiddenByEdge[Nvert] = A;
   Object->Labelling.VENumber[Nvert][0] = Object->Labelling.VENumber[Nvert][2];
   Object->Labelling.VENumber[Nvert][1] = NO_EDGE;
   Object->Labelling.VENumber[Nvert][2] = NO_EDGE;
   Object->Labelling.VECount[Nvert] = 1;
   Object->Labelling.VertexLabel[Nvert].Shape = JunctionIsT;

   if (OccludingFace >= 0) // it might be the background!
   {
      RemoveVertexFromLoopRecord(&Object->Faces[OccludingFace].Loops[0],Nvert);
      RemoveFaceFromVertexRecord(Object,Nvert,OccludingFace);
   }

   /* before deleting the edge, get the number right for any T-junctions occluded by it */
   for (int n=0; n<Object->Labelling.VertexCount; ++n)
   {
      if (Object->HiddenByEdge[n] == B)  Object->HiddenByEdge[n] = A;
   }

   /* and make sure that cofacial configurations don't list the T-junction as 'next around' */
   for (int i=0;  i<Object->CofacialCount;  ++i)
   {
      COFACIAL_CONFIGURATION *C = &Object->CofacialConfigurations[i];
      if (C->InnerNextInLoop == Nvert)  C->InnerNextInLoop = (C->InnerVertex == Va) ? Vb : Va;
      if (C->InnerPrevInLoop == Nvert)  C->InnerPrevInLoop = (C->InnerVertex == Va) ? Vb : Va;
      if (C->OuterNextInLoop == Nvert)  C->OuterNextInLoop = (C->OuterVertex == Va) ? Vb : Va;
      if (C->OuterPrevInLoop == Nvert)  C->OuterPrevInLoop = (C->OuterVertex == Va) ? Vb : Va;
   }

   /* before deleting the edge, get the number right for any other faces occluded by it */
   /* note that all faces (not just the ones around the T-junction) have to be tested */
   for (int n=0; n<Object->FaceCount; ++n)
   {
      if (n != OccludingFace)
      {
         LOOP *Loop = &Object->Faces[n].Loops[0];
         int   Iv = VertexIndexOnLoop(Loop,Nvert);
         if (Iv != NO_VERTEX)  Loop->Corners[Iv].OccludesFace = cTRUE;
         ReplaceEdgeInLoopRecord(Loop,B,A);
      }
   }

   //printf("Deleting Edge %d\n",B);
   DeleteEdge(Object,B);  /* removes edge from occluding face record implicitly */

   //PrintFace(debugReturnT,Object,OccludingFace);
   //printf("Return to trihedral: %s\n", VertexString(Object,&Object->Labelling,Nvert));
   //printf(" ... occluding edge: %s\n", EdgeString(Object,&Object->Labelling,A));
}


void CCueLabelsVarley::ReturnT4JunctionsToT3 (OBJECT *Object)
{
	for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)
	{
		if ((Object->Labelling.VertexLabel[n].Shape == JunctionIsT4)
			&& (Object->Labelling.VertexLabel[n].Vexity & Ttrihedral))
		{
			ConvertT4JunctionToOccluding(Object,n);
		}
	}
}


bool CCueLabelsVarley::TLabellingsAreConflicting (int VertexCount, LABELLING *A, LABELLING *B)
{
   for (int i=0; i<VertexCount; ++i)
   {
      JUNCTION_TYPEMARK *JA = &A->VertexLabel[i];
      JUNCTION_TYPEMARK *JB = &B->VertexLabel[i];

      if ((JA->Shape == JunctionIsT4) && (JB->Shape == JunctionIsT))  return true;
      if ((JA->Shape == JunctionIsT) && (JB->Shape == JunctionIsT4))  return true;

      if ((JA->Shape == JunctionIsT4) && (JB->Shape == JunctionIsT4))
      {
         if (Superset(Ttrihedral,JA->Vexity) && Superset(Tall-Ttrihedral,JB->Vexity))  return true;
         if (Superset(Ttrihedral,JB->Vexity) && Superset(Tall-Ttrihedral,JA->Vexity))  return true;
      }
   }

   return false;
}


void CCueLabelsVarley::RemoveConflictingTLabellings (OBJECT *Object)
{
   for (int n=LP->LabellingCount-1; n>=1; n--)
   {
      if (TLabellingsAreConflicting(Object->Labelling.VertexCount,&LP->LabellingBuffer[0],&LP->LabellingBuffer[n]))
      {
         // delete this labelling by moving all the succeeding ones down one
         for (int i=n; i<LP->LabellingCount-1; i++)  LP->LabellingBuffer[i] = LP->LabellingBuffer[i+1];
         --LP->LabellingCount;
      }
   }
}


////bool CCueLabelsVarley::NoVerticesRemainOnFace (FACE *Face)
////{
////   LOOP *L = &Face->Loops[0];
////   int N = L->CornerCount;
////   for (int i=0; i<N; ++i)  if (!L->Corners[i].OccludesFace)
////   {
////      return false;
////   }
////   return true;
////}


void CCueLabelsVarley::IdentifyVerticesOccludingRegion (OBJECT *Object, FACE_INDEX Nface)
{
   LOOP *L = &Object->Faces[Nface].Loops[0];
   int N = L->CornerCount;

   //PrintVerticesAndEdges(Object,Object->Labelling);

   for (int i=0; i<N; ++i)
   {
      if (Object->Labelling.VertexLabel[L->Corners[i].Number].Shape == JunctionIsT)
      {
         L->Corners[i].OccludesFace = cTRUE;
         Object->Faces[Nface].IsPartialFace = cTRUE;
      }
      else
      if (L->Sides[i].OccludesFace && L->Sides[(i+1)%N].OccludesFace)
      {
         L->Corners[i].OccludesFace = cTRUE;
         Object->Faces[Nface].IsPartialFace = cTRUE;
         RemoveFaceFromVertexRecord(Object,L->Corners[i].Number,Nface);
         Object->Vertices[L->Corners[i].Number].OccludesThisFace = Nface;
      }
   }
}


void CCueLabelsVarley::IdentifyEdgesOccludingRegion (OBJECT *Object, FACE_INDEX Nface)
{
   LOOP *Loop = &Object->Faces[Nface].Loops[0];
   int N = Loop->CornerCount;

   for (int i=0; i<N; ++i)
   {
      EDGE_INDEX Ne = Loop->Sides[i].Number;

      if (Object->Labelling.EdgeLabel[Ne] == LineIsArrow)
      {
         if (Nface != Object->Labelling.EdgeForwardFace[Ne])
         {
            //printf(" ... edge %d occludes face %d, so it is partial\n", Ne, Nface);
            //printf(" ... %s\n", EdgeString(Object,Object->Labelling,Ne));
            Loop->Sides[i].OccludesFace = cTRUE;
            Object->Faces[Nface].IsPartialFace = cTRUE;
         }
      }
   }
}


void CCueLabelsVarley::IdentifyOcclusionInRegions (OBJECT *Object)
{
   for (int i=0; i<Object->FaceCount; ++i)
   {
      IdentifyEdgesOccludingRegion(Object,i);
      IdentifyVerticesOccludingRegion(Object,i);
   }

   for (;;)
   {
      lBOOLEAN DeletedOne = cFALSE;
      for (int i=0; i<Object->FaceCount; ++i)  if (NoVerticesRemainOnFace(&Object->Faces[i]))
      {
         DeletedOne = cTRUE;
         DeleteFace(Object,i);
         break;
      }
      if (!DeletedOne) break;
   }

   //PrintFaces(Object);
   //PrintLoop(&Object->Background);
   //printf("============ Occlusions Identified ===========\n");
}


void CCueLabelsVarley::Output_Varley(CDB_Model *DDBB)
// Copies information from local DataBase of the FindingLabels algorithm
// into the external database (should be adapted to different external databases)

// The output of the algorithms is:
//		FaceCount= number of faces found
//		Face[i].C= number of edges in face i
//		Face[i].E[j]= j-th edge in face i
//		Face[i].V[j]= j-th vertex in face i

// This function should be adapted to different external databases
// This function should be called after calculating labels by Get_Labels_Varley
{
	// Copy labels of vertices from local to global database
	ReferApp.Cu.La.LabelsInVertices.clear();
	for (int n=0; n<Object->Labelling.VertexCount; ++n)
	{
		ReferApp.Cu.La.LabelsInVertices.push_back(Object->Labelling.VertexLabel[n].Shape);
	}

	//Copy labels of edges from local to global database
	ReferApp.Cu.La.LabelsInEdges.clear();
	for (int n=0; n<Object->Labelling.EdgeCount; ++n)
	{
		switch (Object->Labelling.EdgeLabel[n]){
			case LineIsConvex:
				ReferApp.Cu.La.LabelsInEdges.push_back(CONVEX);
			break;
			case LineIsConcave:
				ReferApp.Cu.La.LabelsInEdges.push_back(CONCAVE);
			break;
			case LineIsArrow:
			{
				CEdge Arista= DDBB->GetEdge(n);
				VERTEX_INDEX v0= Object->Labelling.EdgeStartVertex[n];
				VERTEX_INDEX v1= Object->Labelling.EdgeEndVertex[n];
				if(v1 == Arista.Tail && v0 == Arista.Head){
					ReferApp.Cu.La.LabelsInEdges.push_back(BOUNDARY);
				}
				else{
					ReferApp.Cu.La.LabelsInEdges.push_back(RBOUNDARY);
				}
			}
			break;
			case LineIsBackArrow:
				ReferApp.Cu.La.LabelsInEdges.push_back(RBOUNDARY);
			break;
			case LineIsUnknown:
				ReferApp.Cu.La.LabelsInEdges.push_back(BLANK);
			break;
			case LineIsUnknown3D:
				ReferApp.Cu.La.LabelsInEdges.push_back(BLANK);
			break;
			//case LineIsVisible:
			//	ReferApp.Cu.La.LabelsInEdges.push_back(BLANK);
			//break;
		}
	}
}


bool CCueLabelsVarley::Get_Labels_Varley(CDB_Model *DDBB)
//Main flow to get labels from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003

// We have three labelling algorithms:
// 1) Kanatani's combinatorial labelling
// 2) Relaxation labelling
// 3) Lamb-Bandopadhay geometric labelling

// There are various ways of using them

{
	//Load input data from global to local database
	//---------------------------------------------
	Input_Varley (DDBB);

      ////IdentifyCandidateFeatures(Object);
      ////if (Object->SubgraphCount > 1)  IdentifyCofacialConfigurations(Object);
      //////printf("====== Identify Regions of Sketch ======\n");
      ////IdentifyRegionsOfSketch(Object);

      ////if (PreviousLabellingCatalogue)  // fiddle for batch runs
      ////{
      ////   WhichLabellingCatalogue = PreviousLabellingCatalogue;
      ////   PreviousLabellingCatalogue = 0;
      ////}

	//Junction type identification
	//----------------------------
	CCueLabelsVarley_IdentifyJunctionAngles IJA(LP);
	if (!IJA.IdentifyJunctionAngles(Object))
	{
		AfxMessageBox("Failure in labelling:\r\n Unable to identify junction angles");
		return cFALSE;
	}

	//ValidatePartialObject(Object);

	//Select labelling approach
	//-------------------------
	short int LabellingProcedure= LABELLING_COMBIN_ONLY;
	char WhichLabellingCatalogue = LABELLING_TRIHEDRAL;

	int RelaxationIterations = 300;
	bool UseJunctionStatisticsForRelaxationLabelling= false;
	bool UseShapePairStatisticsForRelaxationLabelling= false;
	bool UseMaxInRelaxationLabelling= false;

	SelectApproach(LabellingProcedure,
				   WhichLabellingCatalogue,
				   RelaxationIterations,
				   UseJunctionStatisticsForRelaxationLabelling,
				   UseShapePairStatisticsForRelaxationLabelling,
				   UseMaxInRelaxationLabelling);

	//Initiallise labelling parameters
	//--------------------------------
	LC->SetLabellingConstants(LabellingProcedure,
							 UseJunctionStatisticsForRelaxationLabelling,
							 UseShapePairStatisticsForRelaxationLabelling,
							 RelaxationIterations,
							 UseMaxInRelaxationLabelling,
							 LP->LambanAxisOptions,
							 LP->UseDeductionsInLamban);

	CCueLabelsVarley_InitialiseJunctionLabels IJL;
	IJL.InitialiseJunctionLabels(Object, WhichLabellingCatalogue);

	////if (UnifiedFrontalGeometry)
	////{
	////	LabelStatus = CreateFrontalGeometries(Object);

	////	if (LabelStatus)
	////	{
	////		Object->ProcessingStatus |= (STATUS_LINES_LABELLED|STATUS_EDGES_BUNDLED|STATUS_DEPTH_ESTIMATES_MADE);
	////	}

	////	return LabelStatus;
	////}

	//Label edges
	//-----------
	bool LabelStatus = true;

	if (LabelSketch(Object,
					LabellingProcedure,
					RelaxationIterations,
					UseJunctionStatisticsForRelaxationLabelling,
					UseShapePairStatisticsForRelaxationLabelling,
					UseMaxInRelaxationLabelling))
	{
		for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)
		{
			if (!(Object->Labelling.VertexLabellingIsFinalised[n]))
			{
				Object->Labelling.VertexUnderlyingType[n] &= Object->Labelling.FinalVertexTypes;
				Object->Labelling.VertexLabellingIsFinalised[n] = cTRUE;
			}
		}

		// Reverse some of the funnies which were needed to label non-trihedral sketches
		ReturnT4JunctionsToT3(Object);
		RemoveConflictingTLabellings(Object);
		IdentifyOcclusionInRegions(Object);
	}
	else
	{
		// Fiddle: we might want to try again with another catalogue
		//if (WhichLabellingCatalogue != LABELLING_HEXAHEDRAL)
		//{
		//	LP->PreviousLabellingCatalogue = WhichLabellingCatalogue;
		//	WhichLabellingCatalogue = LABELLING_HEXAHEDRAL;
		//	for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)  Object->Labelling.VertexLabel[n].Vexity = 0;
		//		return true;
		//}
		LabelStatus = false;
	}

	//Save labelling information into global database
	//-----------------------------------------------
	Output_Varley (DDBB);


	return LabelStatus;
}

