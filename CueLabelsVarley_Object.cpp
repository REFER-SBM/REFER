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
#include <cmath>

#include <vector>
#include "DataLabels.h"

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"


VERTEX_INDEX NewVertex (OBJECT *Object)
{
	if (Object->Labelling.VertexCount >= MAX_VERTICES){
		AfxMessageBox("ERROR: TOO MANY VERTICES!\r\n Cannot create new vertex!");
		return NO_VERTEX;
	}

	VERTEX_INDEX NextNumber = Object->Labelling.VertexCount++;

	Object->Labelling.VertexLabel[NextNumber].Shape  = JunctionIsUndefined;
	Object->Labelling.VertexLabel[NextNumber].Vexity = 0;
	Object->IsOnBoundary[NextNumber] = cFALSE;
	Object->Labelling.VertexUnderlyingType[NextNumber] = VFinalIsTrihedral;
	Object->Labelling.VertexLabellingIsFinalised[NextNumber] = cFALSE;
	Object->Vertices[NextNumber].VertexSubgraph  = 0;
	Object->HiddenByEdge[NextNumber] = NO_EDGE;
	Object->Labelling.VECount[NextNumber] = 0;
	Object->Vertices[NextNumber].FaceCount = 0;
	Object->Vertices[NextNumber].OccludesThisFace = NO_FACE;
	for (int n=0; n<MAX_EDGES_PER_VERTEX; ++n)
		Object->Labelling.VENumber[NextNumber][n] = NO_EDGE;
	for (int n=0; n<MAX_FACES_PER_VERTEX; ++n)
		Object->Vertices[NextNumber].Findex[n] = NO_FACE;
	Object->Vertices[NextNumber].IsHidden       = cFALSE;
	Object->Vertices[NextNumber].C2Symmetry     = 0.0;
	Object->Vertices[NextNumber].C3Symmetry     = 0.0;
	Object->Vertices[NextNumber].C4Symmetry     = 0.0;
	Object->Vertices[NextNumber].C5Symmetry     = 0.0;
	Object->Vertices[NextNumber].C6Symmetry     = 0.0;
	Object->Coordinates[NextNumber].x = 0.0;
	Object->Coordinates[NextNumber].y = 0.0;
	Object->Coordinates[NextNumber].z = 0.0;
	Object->Vertices[NextNumber].XYKnown = cFALSE;
	Object->Vertices[NextNumber].DepthKnown = cFALSE;

	return NextNumber;
}


EDGE_INDEX NewEdge (OBJECT *Object, lBOOLEAN IsHidden, VERTEX_INDEX Nfrom, VERTEX_INDEX Nto)
{
	EDGE_INDEX  iE = Object->Labelling.EdgeCount++;

	//Object->Edges[iE].EdgeBundle = 0;
	Object->Edges[iE].EdgeSubgraph = 0;
	Object->Edges[iE].OnSubgraphBoundary = cFALSE;
	//Object->Edges[iE].EquationKnown  = cFALSE;
	Object->Edges[iE].LineDirectionKnown = cFALSE;
	Object->Edges[iE].DrawnLineDirection = 0.0;
	Object->Labelling.EdgeStartVertex[iE] = Nfrom;
	Object->Labelling.EdgeEndVertex[iE]   = Nto;
	Object->Labelling.EdgeForwardFace[iE] = NO_FACE;
	Object->Labelling.EdgeReverseFace[iE] = NO_FACE;
	Object->Edges[iE].IsHidden       = IsHidden;
	//Object->Edges[iE].C2Symmetry     = 0.0;
	//Object->Edges[iE].MirrorSymmetry = 0.0;
	Object->Labelling.EdgeLabelIsFinalised[iE] = cFALSE;
	Object->Labelling.EdgeLabel[iE] = LineIsUnknown;
	Object->Edges[iE].IsPretend      = cFALSE;
	Object->Edges[iE].WasOriginallyOccluding = cFALSE;

	//Object->Edges[iE].LineCurveData.CurveNumber     = -1;
	//Object->Edges[iE].LineCurveData.CurveStartPoint = -1;
	//Object->Edges[iE].LineCurveData.CurveEndPoint   = -1;

	if ((Nfrom != NO_VERTEX) && (Nfrom == Nto)){
		AfxMessageBox("Labelling error:: Create New Edge");
		//exit(BADNEWS);
	}

	//if (Object->ProcessingStatus & STATUS_DEPTH_ESTIMATES_MADE)  MakeEdgeVector(Object,iE);
	return iE;
}


FACE_INDEX NewFace (OBJECT *Object, lBOOLEAN IsHidden)
{
	FACE_INDEX iF = Object->FaceCount++;
	Object->Faces[iF].PlaneNumber     = -1;
	Object->Faces[iF].IsPartialFace   = cFALSE;
	Object->Faces[iF].IsHidden        = cFALSE;
	Object->Faces[iF].IsSimpleConvex  = cMAYBE;
	Object->Faces[iF].NumberOfLoops = 0;
	//Object->Faces[iF].NormalBundle  = 0;
	Object->Faces[iF].NumberOfLoops = 1;
	//InitialiseLoop(&Object->Faces[iF].Loops[0]);
	Object->Faces[iF].IsHidden = IsHidden;
	return iF;
}


int VertexEdgeIndex (OBJECT *Object, VERTEX_INDEX V, EDGE_INDEX E)
{
	int N = Object->Labelling.VECount[V];
	for (int i=0; i<N; i++)
		if (Object->Labelling.VENumber[V][i] == E)
			return i;
	return -1;
}


void CheckVertexNumber (LABELLING *Labelling, VERTEX_INDEX n, char *Text)
{
	if ((n < 0) || (n >= Labelling->VertexCount)){
		AfxMessageBox("Failure in labelling: vertex out of range");
	}
}


VERTEX_INDEX EdgeOtherEnd (OBJECT *Object, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
{
	if ((Nedge >= 0) && (Nedge < Object->Labelling.EdgeCount))
	{
		VERTEX_INDEX Vs = Object->Labelling.EdgeStartVertex[Nedge];
		VERTEX_INDEX Ve = Object->Labelling.EdgeEndVertex[Nedge];

		if (Vs == Nvert)  return Ve;
		if (Ve == Nvert)  return Vs;
		//printf("EdgeOtherEnd ERROR: Nedge=%d, Vs=%d, Ve=%d, Nvert=%d\n", Nedge, Vs, Ve, Nvert);
		return NO_VERTEX;
	}
	else
	{
		//printf("EdgeOtherEnd ERROR: Nedge=%d\n", Nedge);
		return NO_VERTEX;
	}
}


double DistanceFromJunctionToJunction (OBJECT *Object, VERTEX_INDEX From, VERTEX_INDEX To)
{
	return _hypot(Object->Coordinates[From].x-Object->Coordinates[To].x,
				  Object->Coordinates[From].y-Object->Coordinates[To].y);
}


void ReplaceInterceptsAtSplit (OBJECT *Object, EDGE_INDEX OldEdge, EDGE_INDEX NewEdge, VERTEX_INDEX Start, VERTEX_INDEX AsFarAs)
{
	for (int n=0; n<Object->Labelling.VertexCount; ++n)
	{
		if (Object->HiddenByEdge[n] == OldEdge)
		{
			if (DistanceFromJunctionToJunction(Object,Start,n) < DistanceFromJunctionToJunction(Object,Start,AsFarAs))
			{
				Object->HiddenByEdge[n] = NewEdge;
			}
		}
	}
}


FACE_INDEX FaceContainingEdges (OBJECT *Object, EDGE_INDEX E1, EDGE_INDEX E2)
{
   FACE_INDEX FF1 = Object->Labelling.EdgeForwardFace[E1];
   FACE_INDEX FF2 = Object->Labelling.EdgeForwardFace[E2];
   FACE_INDEX RF2 = Object->Labelling.EdgeReverseFace[E2];
   if ((FF1 == FF2) || (FF1 == RF2))  return FF1;

   FACE_INDEX RF1 = Object->Labelling.EdgeReverseFace[E1];
   if ((RF1 == FF2) || (RF1 == RF2))  return RF1;
   return NO_EDGE;
}


VERTEX_INDEX VertexIndexOnLoop (LOOP *Loop, VERTEX_INDEX Nvert)
{
   if (Loop)
   {
      int n;
      for (n=0; n<Loop->CornerCount; ++n)  if (Loop->Corners[n].Number == Nvert)  return n;
   }
   return NO_VERTEX;
}


EDGE_INDEX EdgeIndexOnLoop (LOOP *Loop, EDGE_INDEX Nedge)
{
   if (Loop)
   {
      int n;
      for (n=0; n<Loop->SideCount; ++n)  if (Loop->Sides[n].Number == Nedge)  return n;
   }
   return NO_EDGE;
}

void RemoveVertexFromLoopRecord (LOOP *Loop, VERTEX_INDEX Nold)
{
   int i;
   int N = Loop->CornerCount;
   lBOOLEAN Move = cFALSE;

   for (i=0; i<N; i++)
   {
      if (Move)  Loop->Corners[i-1] = Loop->Corners[i];
      else
      if (Loop->Corners[i].Number == Nold)  Move = cTRUE;
   }

   if (Move)  --Loop->CornerCount;
}


void ReplaceEdgeInLoopRecord (LOOP *Loop, EDGE_INDEX Nold, EDGE_INDEX Nnew)
{
   int Iv = EdgeIndexOnLoop(Loop,Nold);
   if (Iv != NO_EDGE)  Loop->Sides[Iv].Number = Nnew;
}


void RemoveFaceFromVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Nface)
{
   int i;
   int Ecount = Object->Vertices[Nvert].FaceCount;
   lBOOLEAN Move = cFALSE;

   //printf("Remove face %d from vertex %d\n", Nface, Nvert);

   for (i=0; i<Ecount; i++)
   {
      if (Move)
      {
         Object->Vertices[Nvert].Findex[i-1] = Object->Vertices[Nvert].Findex[i];
      }
      else
      if (Object->Vertices[Nvert].Findex[i] == Nface)  Move = cTRUE;
   }

   if (Move)
   {
      i = --(Object->Vertices[Nvert].FaceCount);
      Object->Vertices[Nvert].Findex[i] = NO_FACE;
   }
}


void RenumberEdge (OBJECT *Object, EDGE_INDEX Nold, EDGE_INDEX Nnew)
{
   if (Nold != Nnew)
   {
      int n;

      //printf("Renumbering edge %d as %d\n", Nold, Nnew);

      VERTEX_INDEX Nvs = Object->Labelling.EdgeStartVertex[Nold];
      VERTEX_INDEX Nve = Object->Labelling.EdgeEndVertex[Nold];

      //printf("Updating vertices %d and %d\n", Nvs, Nve);
      if ((Nvs >= 0) && (Nvs < Object->Labelling.VertexCount))
      {
         ReplaceEdgeInVertexRecord(Object,Nvs,Nold,Nnew);
      }
      if ((Nve >= 0) && (Nve < Object->Labelling.VertexCount)) ReplaceEdgeInVertexRecord(Object,Nve,Nold,Nnew);

      for (VERTEX_INDEX v=0; v<Object->Labelling.VertexCount; ++v)
      {
         if (Object->HiddenByEdge[v] == Nold)  Object->HiddenByEdge[v] = Nnew;
      }

      for (n=0; n<Object->FaceCount; ++n)
      {
         LOOP *Loop = &Object->Faces[n].Loops[0];
         EDGE_INDEX Iv = EdgeIndexOnLoop(Loop,Nold);
         if (Iv != NO_EDGE)  Loop->Sides[Iv].Number = Nnew;
      }

      EDGE_INDEX Iv = EdgeIndexOnLoop(&Object->Background,Nold);
      if (Iv != NO_EDGE)  Object->Background.Sides[Iv].Number = Nnew;

      for (n=0; n<Object->MirrorCount; ++n)
      {
         MIRROR *M = &Object->Mirrors[n];
         if (!M->AIsVertex && (M->ANumber == Nold))  M->ANumber = Nnew;
         if (!M->BIsVertex && (M->BNumber == Nold))  M->BNumber = Nnew;
      }

      Object->Labelling.EdgeLabel[Nnew] = Object->Labelling.EdgeLabel[Nold];

      Object->Labelling.EdgeStartVertex[Nnew] = Object->Labelling.EdgeStartVertex[Nold];
      Object->Labelling.EdgeEndVertex[Nnew]   = Object->Labelling.EdgeEndVertex[Nold];

      Object->Labelling.EdgeForwardFace[Nnew] = Object->Labelling.EdgeForwardFace[Nold];
      Object->Labelling.EdgeReverseFace[Nnew] = Object->Labelling.EdgeReverseFace[Nold];

      // renumber edge in features
      for (int i=0;  i<Object->FeatureCount;  ++i)
      {
         FEATURE *F = &Object->Features[i];
         if (F->El == Nold)  F->El = Nnew;
         if (F->Et == Nold)  F->Et = Nnew;
         if (F->Ew == Nold)  F->Ew = Nnew;
         if (F->Ey == Nold)  F->Ey = Nnew;
      }

      /* TBD also in cofacial configurations! */
   }
}


void RemoveEdgeFromVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nold)
{
   lBOOLEAN Move = cFALSE;

   //printf("Remove edge %d from vertex %d\n", Nold, Nvert);

   CheckVertexNumber(&Object->Labelling,Nvert,"RemoveEdgeFromVertexRecord");
   int Ecount = Object->Labelling.VECount[Nvert];

   for (int i=0; i<Ecount; i++)
   {
      if (Move)  Object->Labelling.VENumber[Nvert][i-1] = Object->Labelling.VENumber[Nvert][i];
      else
      if (Object->Labelling.VENumber[Nvert][i] == Nold)  Move = cTRUE;
   }

   if (Move)
   {
      --Ecount;
      Object->Labelling.VENumber[Nvert][Ecount] = NO_EDGE;
      Object->Labelling.VECount[Nvert] = Ecount;
   }
}


void RemoveEdgeFromFaceRecord (OBJECT *Object, FACE_INDEX Nface, EDGE_INDEX Old)
{
   LOOP *Loop = (Nface >= 0) ? &Object->Faces[Nface].Loops[0] : &Object->Background;
   int N = Loop->SideCount;
   lBOOLEAN Move = cFALSE;

   for (int i=0; i<N; i++)
   {
      if (Move)  Loop->Sides[i-1] = Loop->Sides[i];
      else
      if (Loop->Sides[i].Number == Old)  Move = cTRUE;
   }

   if (Move)  --Loop->SideCount;
}


void DeleteEdge (OBJECT *Object, EDGE_INDEX Nedge)
{
   RemoveEdgeFromFaceRecord(Object,Object->Labelling.EdgeForwardFace[Nedge],Nedge);
   RemoveEdgeFromFaceRecord(Object,Object->Labelling.EdgeReverseFace[Nedge],Nedge);

   RemoveEdgeFromVertexRecord(Object,Object->Labelling.EdgeStartVertex[Nedge],Nedge);
   RemoveEdgeFromVertexRecord(Object,Object->Labelling.EdgeEndVertex[Nedge],Nedge);

   //printf("Deleting edge %d/%d\n", Nedge, Object->Labelling.EdgeCount);
   if (Nedge != (Object->Labelling.EdgeCount-1))
   {
      //printf(" ... renumbering edge %d as %d\n", Object->Labelling.EdgeCount-1, Nedge);
      RenumberEdge(Object,Object->Labelling.EdgeCount-1,Nedge);
      Object->Edges[Nedge] = Object->Edges[Object->Labelling.EdgeCount-1];
   }
   --Object->Labelling.EdgeCount;
}


lBOOLEAN NoVerticesRemainOnFace (FACE *Face)
{
   LOOP *L = &Face->Loops[0];
   int N = L->CornerCount;
   for (int i=0; i<N; ++i)  if (!L->Corners[i].OccludesFace)
   {
      return cFALSE;
   }
   return cTRUE;
}


void DeleteFace (OBJECT *Object, FACE_INDEX Nface)
{
   int n;
   /* sledgehammer approach - cleverer approaches are possible but harder to debug */
   //printf("Delete face %d\n", Nface);
   for (n=0; n<Object->Labelling.VertexCount; ++n)  RemoveFaceFromVertexRecord(Object,n,Nface);

   for (n=0; n<Object->Labelling.EdgeCount; ++n)  RemoveFaceFromEdgeRecord(Object,n,Nface);

   if (Nface != (Object->FaceCount-1))
   {
      RenumberFace(Object,Object->FaceCount-1,Nface);
      Object->Faces[Nface] = Object->Faces[Object->FaceCount-1];
   }

   --Object->FaceCount;
}


void RemoveFaceFromEdgeRecord (OBJECT *Object, EDGE_INDEX Nedge, FACE_INDEX F)
{
   if (Object->Labelling.EdgeForwardFace[Nedge] == F)  Object->Labelling.EdgeForwardFace[Nedge] = NO_FACE;
   if (Object->Labelling.EdgeReverseFace[Nedge] == F)  Object->Labelling.EdgeReverseFace[Nedge] = NO_FACE;
}


void ReplaceFaceInVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, FACE_INDEX Old, FACE_INDEX New)
{
   int C = Object->Vertices[Nvert].FaceCount;
   for (int i=0; i<C; ++i)
   {
      if (Object->Vertices[Nvert].Findex[i] == Old)  Object->Vertices[Nvert].Findex[i] = New;
   }
}


void ReplaceFaceInEdgeRecord (OBJECT *Object, EDGE_INDEX Nedge, FACE_INDEX Old, FACE_INDEX New)
{
   if (Object->Labelling.EdgeForwardFace[Nedge] == Old)  Object->Labelling.EdgeForwardFace[Nedge] = New;
   if (Object->Labelling.EdgeReverseFace[Nedge] == Old)  Object->Labelling.EdgeReverseFace[Nedge] = New;
}


void RenumberFace (OBJECT *Object, FACE_INDEX Nold, FACE_INDEX Nnew)
{
   if (Nold != Nnew)
   {
      int n;

      //printf("Renumbering face %d as %d\n", Nold, Nnew);

      for (n=0; n<Object->Labelling.VertexCount; ++n)  ReplaceFaceInVertexRecord(Object,n,Nold,Nnew);
      for (n=0; n<Object->Labelling.EdgeCount; ++n)  ReplaceFaceInEdgeRecord(Object,n,Nold,Nnew);

      for (n=0; n<Object->MirrorCount; ++n)
      {
         MIRROR *M = &Object->Mirrors[n];
         if (M->Face == Nold)  M->Face = Nnew;
      }

      for (n=0; n<Object->ChainCount; ++n)
      {
         CHAINED_MIRRORS *Chain = Object->Chains[n];
         if (Chain)
         {
            for (int j=0; j<Chain->FaceCount; ++j)  if (Chain->Faces[j] == Nold)  Chain->Faces[j] = Nnew;
         }
         else
         {
            CString sMensaje, sMensajeAux;
            sMensaje= "Labelling error: Mirror Chain ";
            sMensajeAux.Format("%d", n);
            sMensaje+=sMensajeAux;
            sMensaje+="has disappeared!!";
            AfxMessageBox(sMensaje);
            //exit(BADNEWS);
         }
      }
   }
}


void ReplaceEdgeInVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nold, EDGE_INDEX Nnew)
{
   CheckVertexNumber(&Object->Labelling,Nvert,"ReplaceEdgeInVertexRecord");

   int Ecount = Object->Labelling.VECount[Nvert];

   for (int i=0; i<Ecount; ++i)
   {
      if (Object->Labelling.VENumber[Nvert][i] == Nold)  Object->Labelling.VENumber[Nvert][i] = Nnew;
   }
   if (Object->HiddenByEdge[Nvert] == Nold)  Object->HiddenByEdge[Nvert] = Nnew;
}


lBOOLEAN AnyTypemarkChanges(bool Set, lBOOLEAN NewATC)
{
	static lBOOLEAN ATC= cFALSE;
	if (Set)
		ATC= NewATC;

	return ATC;
}


lBOOLEAN UseBinaryFrontalGeometry(bool Set, lBOOLEAN NewUBFG)
{
	static lBOOLEAN UBFG= cFALSE;
	if (Set)
		UBFG= NewUBFG;

	return UBFG;
}



void ChangeLine (LABELLING *Labelling, EDGE_INDEX Nedge, TYPEMARK Mask)
{
   TYPEMARK OldValue = Labelling->EdgeLabel[Nedge];
   TYPEMARK NewValue = OldValue & Mask;

   //if (!NewValue)
   //{
      //CString sMensaje, sMensajeAux;
      //sMensaje= "Labelling error: Change Edge ";
      //sMensajeAux.Format("%d", Nedge);
      //sMensaje+=sMensajeAux;
      //sMensaje+="Old Value ";
      //sMensajeAux.Format("%x", OldValue);
      //sMensaje+=sMensajeAux;
      //sMensaje+=" Mask ";
      //sMensajeAux.Format("%x", Mask);
      //sMensaje+=sMensajeAux;
      //sMensaje+=", no new value!";
      //AfxMessageBox(sMensaje);
   //   exit(999);
   //}

   Labelling->EdgeLabel[Nedge] = NewValue;
   if (NewValue != OldValue)  AnyTypemarkChanges(true, cTRUE);
}


void ChangeVertex (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Mask)
{
   TYPEMARK OldValue = Labelling->VertexLabel[Nvert].Vexity;
   TYPEMARK NewValue = OldValue & Mask;

   //if (!NewValue)
   //{
      //CString sMensaje, sMensajeAux;
      //sMensaje= "Labelling error: Change Vertex ";
      //sMensajeAux.Format("%d", Nvert);
      //sMensaje+=sMensajeAux;
      //sMensaje+=" Old Value ";
      //sMensajeAux.Format("%x", OldValue);
      //sMensaje+=sMensajeAux;
      //sMensaje+=" Mask ";
      //sMensajeAux.Format("%x", Mask);
      //sMensaje+=sMensajeAux;
      //sMensaje+=", no new value!";
      //AfxMessageBox(sMensaje);
   //   exit(999);
   //}

   Labelling->VertexLabel[Nvert].Vexity = NewValue;
   if (NewValue != OldValue)  AnyTypemarkChanges(true, cTRUE);
}


TYPEMARK LineIsA (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Start)
{
	return Labelling->EdgeStartVertex[Nedge] == Start ? LineIsBackArrow : LineIsArrow;
}


TYPEMARK LineIsB (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Start)
{
	return Labelling->EdgeStartVertex[Nedge] == Start ? LineIsArrow : LineIsBackArrow;
}


void LabelEdgeAsArrow (OBJECT *Object, EDGE_INDEX Nedge)
{
   if (!Object->Labelling.EdgeLabelIsFinalised[Nedge])
   {
      Object->Labelling.EdgeLabel[Nedge] = LineIsArrow;
      Object->Eprobabilities[Nedge].Amerit = 1.0;
      Object->Eprobabilities[Nedge].Bmerit = 0.0;
      Object->Eprobabilities[Nedge].Cmerit = 0.0;
      Object->Eprobabilities[Nedge].Dmerit = 0.0;
      Object->Labelling.EdgeLabelIsFinalised[Nedge] = cTRUE;
      if (UseBinaryFrontalGeometry(false, cFALSE))
      {
         SetFaceAndEdgeCoplanar(Object,Object->Labelling.EdgeForwardFace[Nedge],Nedge);
         SetFaceAndEdgeNotCoplanar(Object,Object->Labelling.EdgeReverseFace[Nedge],Nedge);
      }
   }
}


void LabelEdgeAsBackArrow (OBJECT *Object, EDGE_INDEX Nedge)
{
   if (!Object->Labelling.EdgeLabelIsFinalised[Nedge])
   {
      // Reverse Vertices For Edge
      VERTEX_INDEX Vtemp = Object->Labelling.EdgeEndVertex[Nedge];
      Object->Labelling.EdgeEndVertex[Nedge] = Object->Labelling.EdgeStartVertex[Nedge];
      Object->Labelling.EdgeStartVertex[Nedge] = Vtemp;

      // Reverse Faces For Edge
      FACE_INDEX Ftemp = Object->Labelling.EdgeForwardFace[Nedge];
      Object->Labelling.EdgeForwardFace[Nedge] = Object->Labelling.EdgeReverseFace[Nedge];
      Object->Labelling.EdgeReverseFace[Nedge] = Ftemp;

      LabelEdgeAsArrow(Object,Nedge);
   }
}


void LabelEdgeAsNotArrow (OBJECT *Object, EDGE_INDEX Nedge)
{
   //printf("Label Edge %d as Not Arrow\n", Nedge);

   if (!Object->Labelling.EdgeLabelIsFinalised[Nedge])
   {
      MERIT Total = Object->Eprobabilities[Nedge].Bmerit + Object->Eprobabilities[Nedge].Cmerit + Object->Eprobabilities[Nedge].Dmerit;
      if (Total > 0.0)
      {
         Object->Eprobabilities[Nedge].Amerit = 0.0;
         Object->Eprobabilities[Nedge].Bmerit /= Total;
         Object->Eprobabilities[Nedge].Cmerit /= Total;
         Object->Eprobabilities[Nedge].Dmerit /= Total;
      }
   }
}


void LabelEdgeAsNotBackArrow (OBJECT *Object, EDGE_INDEX Nedge)
{
   if (!Object->Labelling.EdgeLabelIsFinalised[Nedge])
   {
      MERIT Total = Object->Eprobabilities[Nedge].Amerit + Object->Eprobabilities[Nedge].Cmerit + Object->Eprobabilities[Nedge].Dmerit;
      if (Total > 0.0)
      {
         Object->Eprobabilities[Nedge].Amerit /= Total;
         Object->Eprobabilities[Nedge].Bmerit = 0.0;
         Object->Eprobabilities[Nedge].Cmerit /= Total;
         Object->Eprobabilities[Nedge].Dmerit /= Total;
      }
   }
}


void SetFaceAndVertexCoplanar (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Nvert)
{
   if (Nface < 0)  return;  // The background isn't coplanar to anything

   if (!FVCoplanarPropagated[Nface][Nvert])
   {
      FVCoplanarPropagated[Nface][Nvert]   = cTRUE;
      PFaceAndVertexCoplanar[Nface][Nvert] = 1.0;
      FaceAndVertexCoplanar[Nface][Nvert]  = cYES;

      for (int i = 0; i < Object->Labelling.VECount[Nvert]; ++i)
      {
         EDGE_INDEX Ei = Object->Labelling.VENumber[Nvert][i];
         VERTEX_INDEX Overt = EdgeOtherEnd(Object,Ei,Nvert);
         if (FaceAndVertexCoplanar[Nface][Overt] == cYES)  SetFaceAndEdgeCoplanar(Object,Nface,Ei);
      }
   }
}


void SetFaceAndEdgeCoplanar (OBJECT *Object, FACE_INDEX Nface, EDGE_INDEX Nedge)
{
   if (Nface < 0)  return;  // The background isn't coplanar to anything

   if (!FECoplanarPropagated[Nface][Nedge])
   {
      FECoplanarPropagated[Nface][Nedge] = cTRUE;
      FaceAndEdgeCoplanar[Nface][Nedge]  = cYES;

      if (UseBinaryFrontalGeometry(false, cFALSE))
      {
         if (Nface == Object->Labelling.EdgeForwardFace[Nedge])  LabelEdgeAsNotBackArrow(Object,Nedge);
         else
         if (Nface == Object->Labelling.EdgeReverseFace[Nedge])  LabelEdgeAsNotArrow(Object,Nedge);
      }

      SetFaceAndVertexCoplanar(Object,Nface,Object->Labelling.EdgeStartVertex[Nedge]);
      SetFaceAndVertexCoplanar(Object,Nface,Object->Labelling.EdgeEndVertex[Nedge]);
   }
}


void SetFaceAndVertexNotCoplanar (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Nvert)
{
   if (Nface < 0)  return;  // The background isn't coplanar to anything

   if (!FVCoplanarPropagated[Nface][Nvert])
   {
      FVCoplanarPropagated[Nface][Nvert]   = cTRUE;
      PFaceAndVertexCoplanar[Nface][Nvert] = 0.0;
      FaceAndVertexCoplanar[Nface][Nvert]  = cNO;

      for (int i = 0; i < Object->Labelling.VECount[Nvert]; ++i)
      {
         SetFaceAndEdgeNotCoplanar(Object,Nface,Object->Labelling.VENumber[Nvert][i]);
      }
   }
}


void SetFaceAndEdgeNotCoplanar (OBJECT *Object, FACE_INDEX Nface, EDGE_INDEX Nedge)
{
   if (Nface < 0)  return;  // The background isn't coplanar to anything

   if (!FECoplanarPropagated[Nface][Nedge])
   {
      FECoplanarPropagated[Nface][Nedge] = cTRUE;
      FaceAndEdgeCoplanar[Nface][Nedge]  = cNO;

      if (UseBinaryFrontalGeometry(false, cFALSE))
      {
         if (Nface == Object->Labelling.EdgeForwardFace[Nedge])  LabelEdgeAsBackArrow(Object,Nedge);
         else
         if (Nface == Object->Labelling.EdgeReverseFace[Nedge])  LabelEdgeAsArrow(Object,Nedge);
      }

      VERTEX_INDEX S = Object->Labelling.EdgeStartVertex[Nedge];
      VERTEX_INDEX T = Object->Labelling.EdgeEndVertex[Nedge];
      if (FaceAndVertexCoplanar[Nface][S] == cYES)  SetFaceAndVertexNotCoplanar(Object,Nface,T);
      else
      if (FaceAndVertexCoplanar[Nface][T] == cYES)  SetFaceAndVertexNotCoplanar(Object,Nface,S);
   }
}



lBOOLEAN StoreEdgeInVertexRecord (OBJECT *Object, VERTEX_INDEX Nvert, EDGE_INDEX Nedge)
{
   CheckVertexNumber(&Object->Labelling,Nvert,"StoreEdgeInVertexRecord");

   for (int n=0; n<MAX_EDGES_PER_VERTEX; ++n)
   {
      int Ecount = Object->Labelling.VECount[Nvert];
      if (Object->Labelling.VENumber[Nvert][n] == Nedge)  return cFALSE;
      if ((n == Ecount) || (Object->Labelling.VENumber[Nvert][n] == NO_EDGE))
      {
         Object->Labelling.VENumber[Nvert][n] = Nedge;
         ++(Object->Labelling.VECount[Nvert]);
         return cTRUE;
      }
   }

   return cFALSE;
}



void ReplaceVertexInEdgeRecord (OBJECT *Object, EDGE_INDEX Nedge, VERTEX_INDEX Nold, VERTEX_INDEX Nnew)
{
   if (Object->Labelling.EdgeStartVertex[Nedge] == Nold)  Object->Labelling.EdgeStartVertex[Nedge] = Nnew;
   if (Object->Labelling.EdgeEndVertex[Nedge] == Nold)  Object->Labelling.EdgeEndVertex[Nedge] = Nnew;
}


lBOOLEAN VertexLabelsIdentical (JUNCTION_TYPEMARK *A, JUNCTION_TYPEMARK *B)
{
   return (A->Shape == B->Shape) && (A->Vexity == B->Vexity);
}


double LineLength (OBJECT *Object, EDGE_INDEX Nedge)
{
   return DistanceFromJunctionToJunction(Object,Object->Labelling.EdgeStartVertex[Nedge],Object->Labelling.EdgeEndVertex[Nedge]);
}


DEGREES DirectionFromPoint2ToPoint2 (POINT2Dv *FromPoint, POINT2Dv *ToPoint)
{
   DEGREES Angle = (180.0/M_PI) * atan2(ToPoint->x-FromPoint->x,FromPoint->y-ToPoint->y);
   while (Angle < 0)  Angle += 360.0;
   return Angle;
}


DEGREES CalculateLineDirection (OBJECT *Object, LABELLING *Labelling, EDGE_INDEX Nedge)
{
   if ((Nedge >= 0) && (Nedge < Labelling->EdgeCount))
   {
      VERTEX_INDEX S = Labelling->EdgeStartVertex[Nedge];
      VERTEX_INDEX T = Labelling->EdgeEndVertex[Nedge];

      POINT2Dv Ps,Pe;
      Ps.x = Object->Coordinates[S].x;
      Ps.y = Object->Coordinates[S].y;
      Pe.x = Object->Coordinates[T].x;
      Pe.y = Object->Coordinates[T].y;
      return DirectionFromPoint2ToPoint2(&Ps,&Pe);
   }

   return 0.0;
}


DEGREES GetLineDirection (OBJECT *Object, EDGE_INDEX Nedge)
{
   if (Object->Edges[Nedge].LineDirectionKnown)  return Object->Edges[Nedge].DrawnLineDirection;
   return CalculateLineDirection(Object,&Object->Labelling,Nedge);
}


EDGE_INDEX EdgeOnLoop (LOOP *Loop, EDGE_INDEX n)
{
   while (n < 0)  n += Loop->SideCount;
   return Loop->Sides[n%Loop->SideCount].Number;
}


lBOOLEAN VertexIsInLoop (LOOP *Loop, VERTEX_INDEX Nvert)
{
   int n;
   for (n=0; n<Loop->CornerCount; ++n)
   {
      if (Loop->Corners[n].Number == Nvert)  return !Loop->Corners[n].OccludesFace;
   }
   return cFALSE;
}


lBOOLEAN EdgeIsInLoop (LOOP *Loop, EDGE_INDEX Nedge)
{
   if (Loop)
   {
      for (int n=0; n<Loop->SideCount; ++n)
      {
         if (Loop->Sides[n].Number == Nedge) return !Loop->Sides[n].OccludesFace;
      }
   }

   return cFALSE;
}


lBOOLEAN EdgeLeavesLoop (OBJECT *Object, LOOP *Loop, EDGE_INDEX Nedge)
{
   if (EdgeIsInLoop(Loop,Nedge))  return cFALSE;
   return VertexIsInLoop(Loop,Object->Labelling.EdgeStartVertex[Nedge])
       || VertexIsInLoop(Loop,Object->Labelling.EdgeEndVertex[Nedge]);
}


FACE_INDEX EdgeOtherFace (OBJECT *Object, EDGE_INDEX Nedge, FACE_INDEX F)
{
   if ((Nedge >= 0) && (Nedge < Object->Labelling.EdgeCount))
   {
      FACE_INDEX Fface = Object->Labelling.EdgeForwardFace[Nedge];
	  FACE_INDEX Rface = Object->Labelling.EdgeReverseFace[Nedge];

      if (Fface == F)  return Rface;
      if (Rface == F)  return Fface;
   }

   return NO_FACE;
}


lREAL DistanceFromPoint2ToLine2 (IMPLICIT_LINE2 *Line, POINT2Dv *P)
{
   return std::fabs(Line->Normal.x*P->x + Line->Normal.y*P->y + Line->Distance);
}

DEGREES DirectionFromJunctionToJunction (OBJECT *Object, VERTEX_INDEX From, VERTEX_INDEX To)
{
   return (180.0/M_PI) * atan2(Object->Coordinates[To].x-Object->Coordinates[From].x,
                               Object->Coordinates[From].y-Object->Coordinates[To].y);
}

VECTOR3D *GetVectorFromPoint3ToPoint3 (VECTOR3D *V, POINT3D *A, POINT3D *B)
{
   V->x = B->x - A->x;
   V->y = B->y - A->y;
   V->z = B->z - A->z;
   return V;
}


VECTOR3D *AddVector (VECTOR3D *Vector, VECTOR3D *Add)
{
   Vector->x += Add->x;
   Vector->y += Add->y;
   Vector->z += Add->z;
   return Vector;
}


lBOOLEAN NormaliseVector (VECTOR3D *Vector)
{
   double X = Vector->x;
   double Y = Vector->y;
   double Z = Vector->z;
   double D = sqrt(X*X+Y*Y+Z*Z);
   if (D == 0.0)  return cFALSE;
   Vector->x /= D;
   Vector->y /= D;
   Vector->z /= D;
   return cTRUE;
}


int GetVFaceNormal (OBJECT *Object, FACE_INDEX Nface, VECTOR3D *Normal)
{
   int Plane = Object->Faces[Nface].PlaneNumber;
   if (Plane >= 0)
   {
      if (Object->Planes[Plane].IsEquationKnown & 1)
      {
         *Normal = Object->Planes[Plane].PlaneEquation3D.Normal;
         return 1;
      }
   }
   return 0;
}


double DotProduct (VECTOR3D *Vector1, VECTOR3D *Vector2)
{
   return (Vector1->x*Vector2->x) + (Vector1->y*Vector2->y) + (Vector1->z*Vector2->z);
}


VERTEX_INDEX VertexOnLoop (LOOP *Loop, VERTEX_INDEX n)
{
   while (n < 0)  n += Loop->CornerCount;
   return Loop->Corners[n%Loop->CornerCount].Number;
}


void GetEdgeCentre (OBJECT *Object, EDGE_INDEX Nedge, POINT3D *Point)
{
   POINT3D Ps = Object->Coordinates[Object->Labelling.EdgeStartVertex[Nedge]];
   POINT3D Pt = Object->Coordinates[Object->Labelling.EdgeEndVertex[Nedge]];
   Point->x = (Ps.x+Pt.x)/2.0;
   Point->y = (Ps.y+Pt.y)/2.0;
   Point->z = (Ps.z+Pt.z)/2.0;
}


int GetFacePlane (OBJECT *Object, FACE_INDEX Nface, IMPLICIT_PLANE *FacePlane)
{
   int Plane = Object->Faces[Nface].PlaneNumber;
   if (Plane >= 0)
   {
      if (Object->Planes[Plane].IsEquationKnown == 3)
      {
         *FacePlane = Object->Planes[Plane].PlaneEquation3D;
         return 1;
      }
   }
   return 0;
}


void MakeEdgeVector (OBJECT *Object, EDGE_INDEX Nedge)
{
   POINT3D Ps = Object->Coordinates[Object->Labelling.EdgeStartVertex[Nedge]];
   POINT3D Pt = Object->Coordinates[Object->Labelling.EdgeEndVertex[Nedge]];
   GetVectorFromPoint3ToPoint3(&Object->Edges[Nedge].EdgeVector3D,&Ps,&Pt);
   NormaliseVector(&Object->Edges[Nedge].EdgeVector3D);
}


VECTOR3D *GetCrossProduct (VECTOR3D *CrossProduct, VECTOR3D *A, VECTOR3D *B)
{
   CrossProduct->x = (A->y*B->z) - (A->z*B->y);
   CrossProduct->y = (A->z*B->x) - (A->x*B->z);
   CrossProduct->z = (A->x*B->y) - (A->y*B->x);
   return CrossProduct;
}


double VectorMagnitude (VECTOR3D *V)
{
   return sqrt(V->x*V->x + V->y*V->y + V->z*V->z);
}


void NormaliseImplicitPlane (IMPLICIT_PLANE *Plane)
{
   lREAL P = Plane->Normal.x;
   lREAL Q = Plane->Normal.y;
   lREAL R = Plane->Normal.z;
   lREAL C = Plane->Distance;
   lREAL D = sqrt(P*P+Q*Q+R*R);
   if (C < 0.0)  D = -D;
   Plane->Normal.x = P/D;
   Plane->Normal.y = Q/D;
   Plane->Normal.z = R/D;
   Plane->Distance = C/D;
}


void FindImplicitPlane (IMPLICIT_PLANE *Plane, POINT3D *Point1, POINT3D *Point2, POINT3D *Point3)
{
   VECTOR3D V12, V13;
   GetVectorFromPoint3ToPoint3(&V12,Point1,Point2);
   GetVectorFromPoint3ToPoint3(&V13,Point1,Point3);
   GetCrossProduct(&Plane->Normal,&V12,&V13);
   Plane->Distance = - ((Point1->x*Plane->Normal.x) + (Point1->y*Plane->Normal.y) + (Point1->z*Plane->Normal.z));
   NormaliseImplicitPlane(Plane);
}


