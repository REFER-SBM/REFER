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
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "ReferDoc.h"
#include "Refer.h"

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Maths.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"

#include "CueLabelsVarley_LabellingConstants.h"
#include "CueLabelsVarley_LabellingParameters.h"

#include "CueLabelsVarley_IdentifyJunctionAngles.h"


/* -------------------------------------------------------------------------------- */
// Make the angle always positive


CCueLabelsVarley_IdentifyJunctionAngles::CCueLabelsVarley_IdentifyJunctionAngles(CCueLabelsVarley_LabellingParameters *eLP)
{
	//Labelling parameters
	LP= eLP;
}


CCueLabelsVarley_IdentifyJunctionAngles::~CCueLabelsVarley_IdentifyJunctionAngles()
{
}


DEGREES CCueLabelsVarley_IdentifyJunctionAngles::DirectionFromPoint2ToPoint2 (POINT2Dv *FromPoint, POINT2Dv *ToPoint)
{
   DEGREES Angle = (180.0/M_PI) * atan2(ToPoint->x-FromPoint->x,FromPoint->y-ToPoint->y);
   while (Angle < 0)  Angle += 360.0;
   return Angle;
}


DEGREES CCueLabelsVarley_IdentifyJunctionAngles::OppositeDirection (DEGREES FromDirection)
{
   return (FromDirection >= 180.0) ? FromDirection-180.0 : FromDirection+180.0;
}


int CCueLabelsVarley_IdentifyJunctionAngles::CountOfEdgesMeetingVertex (OBJECT *Object, VERTEX_INDEX Nvert)
{
   int Total=0;

   for (int i=0; i<Object->Labelling.EdgeCount; ++i)
   {
      if ((Object->Labelling.EdgeStartVertex[i] == Nvert) || (Object->Labelling.EdgeEndVertex[i] == Nvert)) ++Total;
   }
   return Total;
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::EdgesShareVertex (OBJECT *Object, EDGE_INDEX E0, EDGE_INDEX E1)
{
	VERTEX_INDEX S0 = Object->Labelling.EdgeStartVertex[E0];
	if (S0 == Object->Labelling.EdgeStartVertex[E1])  return cTRUE;
	if (S0 == Object->Labelling.EdgeEndVertex[E1])    return cTRUE;
	S0 = Object->Labelling.EdgeEndVertex[E0];
	if (S0 == Object->Labelling.EdgeStartVertex[E1])  return cTRUE;
	if (S0 == Object->Labelling.EdgeEndVertex[E1])    return cTRUE;
	return cFALSE;
}


lREAL CCueLabelsVarley_IdentifyJunctionAngles::DistanceFromPoint2ToPoint2 (POINT2Dv *FromPoint, POINT2Dv *ToPoint)
{
	return _hypot(ToPoint->x-FromPoint->x,ToPoint->y-FromPoint->y);
}




#define CROSSING_THRESHOLD (50.0)  // Crossing points are all within a threshold distance (10 pixels) of one another 





lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::ThereIsAUseableIntercept (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, EDGE_INDEX G, POINT2Dv *Pcross, VERTEX_INDEX *CrossV)
{
	VERTEX_INDEX Es = Object->Labelling.EdgeStartVertex[E];
	VERTEX_INDEX Et = Object->Labelling.EdgeEndVertex[E];
	VERTEX_INDEX Fs = Object->Labelling.EdgeStartVertex[F];
	VERTEX_INDEX Ft = Object->Labelling.EdgeEndVertex[F];
	VERTEX_INDEX Gs = (G == NO_EDGE) ? NO_VERTEX : Object->Labelling.EdgeStartVertex[G];
	VERTEX_INDEX Gt = (G == NO_EDGE) ? NO_VERTEX : Object->Labelling.EdgeEndVertex[G];

	for (VERTEX_INDEX V = 0;  V < Object->Labelling.VertexCount;  ++V)
	{
		if ((V != Es) && (V != Et) && (V != Fs) && (V != Ft) && (V != Gs) && (V != Gt))
		{
			if ((Object->HiddenByEdge[V] == E) || (Object->HiddenByEdge[V] == F) || (G != NO_EDGE) && ((Object->HiddenByEdge[V] == G)))
			{
				POINT2Dv Pv;
				Pv.x = Object->Coordinates[V].x;
				Pv.y = Object->Coordinates[V].y;
				if (DistanceFromPoint2ToPoint2(Pcross,&Pv) < CROSSING_THRESHOLD)
				{
					*CrossV = V;
					return cTRUE;
				}
			}
		}
	}
	return cFALSE;
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::ThereIsAUseableJunction (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, EDGE_INDEX G, POINT2Dv *Pcross, VERTEX_INDEX *CrossV)
{
   VERTEX_INDEX Es = Object->Labelling.EdgeStartVertex[E];
   VERTEX_INDEX Et = Object->Labelling.EdgeEndVertex[E];
   VERTEX_INDEX Fs = Object->Labelling.EdgeStartVertex[F];
   VERTEX_INDEX Ft = Object->Labelling.EdgeEndVertex[F];
   VERTEX_INDEX Gs = (G == NO_EDGE) ? NO_VERTEX : Object->Labelling.EdgeStartVertex[G];
   VERTEX_INDEX Gt = (G == NO_EDGE) ? NO_VERTEX : Object->Labelling.EdgeEndVertex[G];

   for (VERTEX_INDEX V = 0;  V < Object->Labelling.VertexCount;  ++V)
   {
      if ((V != Es) && (V != Et) && (V != Fs) && (V != Ft) && (V != Gs) && (V != Gt))
      {
         if (Object->HiddenByEdge[V] == NO_EDGE)
         {
            POINT2Dv Pv;
            Pv.x = Object->Coordinates[V].x;
            Pv.y = Object->Coordinates[V].y;
            if (DistanceFromPoint2ToPoint2(Pcross,&Pv) < CROSSING_THRESHOLD)
            {
               *CrossV = V;
               return cTRUE;
            }
         }
      }
   }
   return cFALSE;
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::Intercept (IMPLICIT_LINE2 *FirstLine, IMPLICIT_LINE2 *SecondLine, POINT2Dv *P)
{
	lREAL A = FirstLine->Normal.x;
	lREAL B = FirstLine->Normal.y;
	lREAL C = FirstLine->Distance;
	lREAL I = SecondLine->Normal.x;
	lREAL J = SecondLine->Normal.y;
	lREAL K = SecondLine->Distance;
	lREAL D = A*J - B*I;
	if (std::fabs(D) < 0.00000001)  return cFALSE;   /* lines are parallel or colinear */

	P->x = (B*K-C*J)/D;
	P->y = (C*I-A*K)/D;
	return cTRUE;
}


double CCueLabelsVarley_IdentifyJunctionAngles::DistanceFromJunctionToPoint2 (OBJECT *Object, VERTEX_INDEX From, POINT2Dv *To)
{
	return _hypot(Object->Coordinates[From].x-To->x,Object->Coordinates[From].y-To->y);
}

/* -------------------------------------------------------------------------------- */
/* This doesn't properly take account of boundary conditions                        */

lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::TwoLinesCrossAtPoint (OBJECT *Object, EDGE_INDEX Line0, EDGE_INDEX Line1, POINT2Dv *P)
{
   if (!Intercept(&Object->Edges[Line0].LineEquation,&Object->Edges[Line1].LineEquation,P))
   {
      /* lines are parallel or collinear */
      return cFALSE;
   }

   //printf("Line %d equation is %s\n", Line0, Line2String(&Object->Edges[Line0]->LineEquation));
   //printf("Line %d equation is %s\n", Line1, Line2String(&Object->Edges[Line1]->LineEquation));
   //printf("Crossing position of lines %d and %d is %s\n", Line0, Line1, Vector2DString(P));

   VERTEX_INDEX Vs0 = Object->Labelling.EdgeStartVertex[Line0];
   VERTEX_INDEX Ve0 = Object->Labelling.EdgeEndVertex[Line0];

   lREAL Length0 = DistanceFromJunctionToJunction(Object,Vs0,Ve0);

   lREAL VSP0 = DistanceFromJunctionToPoint2(Object,Vs0,P);
   if (VSP0 >= Length0)  return cFALSE;

   lREAL VEP0 = DistanceFromJunctionToPoint2(Object,Ve0,P);
   if (VEP0 >= Length0)  return cFALSE;

   VERTEX_INDEX Vs1 = Object->Labelling.EdgeStartVertex[Line1];
   VERTEX_INDEX Ve1 = Object->Labelling.EdgeEndVertex[Line1];

   lREAL Length1 = DistanceFromJunctionToJunction(Object,Vs1,Ve1);

   lREAL VSP1 = DistanceFromJunctionToPoint2(Object,Vs1,P);
   if (VSP1 >= Length1)  return cFALSE;

   lREAL VEP1 = DistanceFromJunctionToPoint2(Object,Ve1,P);
   if (VEP1 >= Length1)  return cFALSE;

   //printf("Lines %d and %d cross\n",Line0,Line1);
   //printf("Line %d length %f, s=%f, t=%f\n",Line0,Length0,VSP0,VEP0);
   //printf("Line %d length %f, s=%f, t=%f\n",Line1,Length1,VSP1,VEP1);

   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::OneEdgeInterceptsTheOther (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F)
{
	if (Object->HiddenByEdge[Object->Labelling.EdgeStartVertex[E]] == F)  return cTRUE;
	if (Object->HiddenByEdge[Object->Labelling.EdgeEndVertex[E]] == F)  return cTRUE;
	if (Object->HiddenByEdge[Object->Labelling.EdgeStartVertex[F]] == E)  return cTRUE;
	if (Object->HiddenByEdge[Object->Labelling.EdgeEndVertex[F]] == E)  return cTRUE;
	return cFALSE;
}


void CCueLabelsVarley_IdentifyJunctionAngles::CreateCrossVertexFromIntercept (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, VERTEX_INDEX CrossV)
{
   //printf("Create Cross Vertex from Intercept %s\n", VertexString(Object,&Object->Labelling,CrossV));
   //printf("%s\n",EdgeString(Object,&Object->Labelling,E));
   //printf("%s\n",EdgeString(Object,&Object->Labelling,F));
   VERTEX_INDEX Et = Object->Labelling.EdgeEndVertex[E];
   VERTEX_INDEX Ft = Object->Labelling.EdgeEndVertex[F];

   EDGE_INDEX E2 = NewEdge(Object,VISIBLE,CrossV,Et);
   ReplaceEdgeInVertexRecord(Object,Et,E,E2);

   EDGE_INDEX F2 = NewEdge(Object,VISIBLE,CrossV,Ft);
   ReplaceEdgeInVertexRecord(Object,Ft,F,F2);

   Object->Labelling.EdgeEndVertex[E] = CrossV;
   Object->Labelling.EdgeEndVertex[F] = CrossV;
   StoreEdgeInVertexRecord(Object,CrossV,E);
   StoreEdgeInVertexRecord(Object,CrossV,E2);
   StoreEdgeInVertexRecord(Object,CrossV,F);
   StoreEdgeInVertexRecord(Object,CrossV,F2);

   ReplaceInterceptsAtSplit(Object,E,E2,Et,CrossV);
   ReplaceInterceptsAtSplit(Object,F,F2,Ft,CrossV);

   IdentifyAnglesAtNormalJunction(Object,CrossV);
   //printf("%s\n",VertexString(Object,&Object->Labelling,CrossV));
   // TBD: note that this is a weirdo junction
   LP->KJunctionsKnownToBePresent = cTRUE;
   //printf("%s\n",VertexString(Object,&Object->Labelling,Et));
   //printf("%s\n",VertexString(Object,&Object->Labelling,Ft));
}


void CCueLabelsVarley_IdentifyJunctionAngles::CreateTripleCrossVertexFromIntercept (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, EDGE_INDEX G, VERTEX_INDEX CrossV)
{
   //printf("Create Triple Cross Vertex from Intercept %s\n", VertexString(Object,&Object->Labelling,CrossV));
   VERTEX_INDEX Et = Object->Labelling.EdgeEndVertex[E];
   VERTEX_INDEX Ft = Object->Labelling.EdgeEndVertex[F];
   VERTEX_INDEX Gt = Object->Labelling.EdgeEndVertex[G];

   EDGE_INDEX E2 = NewEdge(Object,VISIBLE,CrossV,Et);
   ReplaceEdgeInVertexRecord(Object,Et,E,E2);

   EDGE_INDEX F2 = NewEdge(Object,VISIBLE,CrossV,Ft);
   ReplaceEdgeInVertexRecord(Object,Ft,F,F2);

   EDGE_INDEX G2 = NewEdge(Object,VISIBLE,CrossV,Gt);
   ReplaceEdgeInVertexRecord(Object,Gt,G,G2);

   Object->Labelling.EdgeEndVertex[E] = CrossV;
   Object->Labelling.EdgeEndVertex[F] = CrossV;
   Object->Labelling.EdgeEndVertex[G] = CrossV;
   StoreEdgeInVertexRecord(Object,CrossV,E);
   StoreEdgeInVertexRecord(Object,CrossV,E2);
   StoreEdgeInVertexRecord(Object,CrossV,F);
   StoreEdgeInVertexRecord(Object,CrossV,F2);
   StoreEdgeInVertexRecord(Object,CrossV,G);
   StoreEdgeInVertexRecord(Object,CrossV,G2);

   ReplaceInterceptsAtSplit(Object,E,E2,Et,CrossV);
   ReplaceInterceptsAtSplit(Object,F,F2,Ft,CrossV);
   ReplaceInterceptsAtSplit(Object,G,G2,Gt,CrossV);

   IdentifyAnglesAtNormalJunction(Object,CrossV);
   //printf("%s\n",VertexString(Object,&Object->Labelling,CrossV));
   // TBD: note that this is a weirdo junction
   LP->KJunctionsKnownToBePresent = cTRUE;
}


void CCueLabelsVarley_IdentifyJunctionAngles::CreateCrossVertexFromJunction (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, VERTEX_INDEX CrossV)
{
   //printf("Create Cross from Junction %s\n", VertexString(Object,&Object->Labelling,CrossV));
   VERTEX_INDEX Et = Object->Labelling.EdgeEndVertex[E];
   VERTEX_INDEX Ft = Object->Labelling.EdgeEndVertex[F];

   EDGE_INDEX E2 = NewEdge(Object,VISIBLE,CrossV,Et);
   ReplaceEdgeInVertexRecord(Object,Et,E,E2);

   EDGE_INDEX F2 = NewEdge(Object,VISIBLE,CrossV,Ft);
   ReplaceEdgeInVertexRecord(Object,Ft,F,F2);

   Object->Labelling.EdgeEndVertex[E] = CrossV;
   Object->Labelling.EdgeEndVertex[F] = CrossV;
   StoreEdgeInVertexRecord(Object,CrossV,E);
   StoreEdgeInVertexRecord(Object,CrossV,E2);
   StoreEdgeInVertexRecord(Object,CrossV,F);
   StoreEdgeInVertexRecord(Object,CrossV,F2);

   ReplaceInterceptsAtSplit(Object,E,E2,Et,CrossV);
   ReplaceInterceptsAtSplit(Object,F,F2,Ft,CrossV);

   IdentifyAnglesAtNormalJunction(Object,CrossV);
   //printf("%s\n",VertexString(Object,&Object->Labelling,CrossV));
   // TBD: note that this is a weirdo junction
   LP->KJunctionsKnownToBePresent = cTRUE;
}


void CCueLabelsVarley_IdentifyJunctionAngles::CreateTripleCrossVertexFromJunction (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, EDGE_INDEX G, VERTEX_INDEX CrossV)
{
   //printf("Create Triple Cross from Junction %s\n", VertexString(Object,&Object->Labelling,CrossV));
   VERTEX_INDEX Et = Object->Labelling.EdgeEndVertex[E];
   VERTEX_INDEX Ft = Object->Labelling.EdgeEndVertex[F];
   VERTEX_INDEX Gt = Object->Labelling.EdgeEndVertex[G];

   EDGE_INDEX E2 = NewEdge(Object,VISIBLE,CrossV,Et);
   ReplaceEdgeInVertexRecord(Object,Et,E,E2);

   EDGE_INDEX F2 = NewEdge(Object,VISIBLE,CrossV,Ft);
   ReplaceEdgeInVertexRecord(Object,Ft,F,F2);

   EDGE_INDEX G2 = NewEdge(Object,VISIBLE,CrossV,Gt);
   ReplaceEdgeInVertexRecord(Object,Gt,G,G2);

   Object->Labelling.EdgeEndVertex[E] = CrossV;
   Object->Labelling.EdgeEndVertex[F] = CrossV;
   Object->Labelling.EdgeEndVertex[G] = CrossV;
   StoreEdgeInVertexRecord(Object,CrossV,E);
   StoreEdgeInVertexRecord(Object,CrossV,E2);
   StoreEdgeInVertexRecord(Object,CrossV,F);
   StoreEdgeInVertexRecord(Object,CrossV,F2);
   StoreEdgeInVertexRecord(Object,CrossV,G);
   StoreEdgeInVertexRecord(Object,CrossV,G2);

   ReplaceInterceptsAtSplit(Object,E,E2,Et,CrossV);
   ReplaceInterceptsAtSplit(Object,F,F2,Ft,CrossV);
   ReplaceInterceptsAtSplit(Object,G,G2,Gt,CrossV);

   IdentifyAnglesAtNormalJunction(Object,CrossV);
   //printf("%s\n",VertexString(Object,&Object->Labelling,CrossV));
   // TBD: note that this is a weirdo junction
   LP->KJunctionsKnownToBePresent = cTRUE;
}


void CCueLabelsVarley_IdentifyJunctionAngles::CreateCrossVertexFromNew (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F, POINT2Dv *Pcross)
{
   //printf("Create Cross Vertex from New\n");
   VERTEX_INDEX CrossV = NewVertex(Object);
   Object->Coordinates[CrossV].x = Pcross->x;
   Object->Coordinates[CrossV].y = Pcross->y;

   VERTEX_INDEX Et = Object->Labelling.EdgeEndVertex[E];
   VERTEX_INDEX Ft = Object->Labelling.EdgeEndVertex[F];

   EDGE_INDEX E2 = NewEdge(Object,VISIBLE,CrossV,Et);
   ReplaceEdgeInVertexRecord(Object,Et,E,E2);

   EDGE_INDEX F2 = NewEdge(Object,VISIBLE,CrossV,Ft);
   ReplaceEdgeInVertexRecord(Object,Ft,F,F2);

   Object->Labelling.EdgeEndVertex[E] = CrossV;
   Object->Labelling.EdgeEndVertex[F] = CrossV;
   StoreEdgeInVertexRecord(Object,CrossV,E);
   StoreEdgeInVertexRecord(Object,CrossV,E2);
   StoreEdgeInVertexRecord(Object,CrossV,F);
   StoreEdgeInVertexRecord(Object,CrossV,F2);

   ReplaceInterceptsAtSplit(Object,E,E2,Et,CrossV);
   ReplaceInterceptsAtSplit(Object,F,F2,Ft,CrossV);

   RearrangeEdgesAt4Junction(Object,CrossV);
   //printf("%s\n",VertexString(Object,&Object->Labelling,CrossV));
   // TBD: note that this is a weirdo junction
   LP->KJunctionsKnownToBePresent = cTRUE;
}


void CCueLabelsVarley_IdentifyJunctionAngles::CreateXJunctionAtPointOfIntersection (OBJECT *Object, EDGE_INDEX Ei, EDGE_INDEX Ej, EDGE_INDEX Ek, POINT2Dv *Pij, POINT2Dv *Pik, POINT2Dv *Pjk)
{
   VERTEX_INDEX Vbi = Object->Labelling.EdgeEndVertex[Ei];
   VERTEX_INDEX Vbj = Object->Labelling.EdgeEndVertex[Ej];
   VERTEX_INDEX Vbk = Object->Labelling.EdgeEndVertex[Ek];

   //printf("Lines %d,%d,%d cross - replacing by X6 junction!\n", Ei, Ej, Ek);

   VERTEX_INDEX X6 = NewVertex(Object);
   Object->Labelling.VertexLabel[X6].Shape  = JunctionIsZ6;
   Object->Labelling.VertexLabel[X6].Vexity = Xcdcdcd|Xdcdcdc;

   Object->Coordinates[X6].x = (Pij->x + Pik->x + Pjk->x) / 3.0;
   Object->Coordinates[X6].y = (Pij->y + Pik->y + Pjk->y) / 3.0;
   Object->Vertices[X6].XYKnown = cTRUE;

   EDGE_INDEX Xi = NewEdge(Object,VISIBLE,X6,Vbi);
   StoreEdgeInVertexRecord(Object,X6,Xi);
   ReplaceEdgeInVertexRecord(Object,Vbi,Ei,Xi);
   ReplaceVertexInEdgeRecord(Object,Ei,Vbi,X6);

   EDGE_INDEX Xj = NewEdge(Object,VISIBLE,X6,Vbj);
   StoreEdgeInVertexRecord(Object,X6,Xj);
   ReplaceEdgeInVertexRecord(Object,Vbj,Ej,Xj);
   ReplaceVertexInEdgeRecord(Object,Ej,Vbj,X6);

   EDGE_INDEX Xk = NewEdge(Object,VISIBLE,X6,Vbk);
   StoreEdgeInVertexRecord(Object,X6,Xk);
   ReplaceEdgeInVertexRecord(Object,Vbk,Ek,Xk);
   ReplaceVertexInEdgeRecord(Object,Ek,Vbk,X6);

   StoreEdgeInVertexRecord(Object,X6,Ei);
   StoreEdgeInVertexRecord(Object,X6,Ej);
   StoreEdgeInVertexRecord(Object,X6,Ek);

   RearrangeEdgesAtMultiJunction(Object,X6);
   //PrintVerticesAndEdges(Object,&Object->Labelling);
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::Turn2CrossingLinesIntoXJunction (OBJECT *Object)
{
	
	for (EDGE_INDEX i=0; i<Object->Labelling.EdgeCount; ++i){

		for (EDGE_INDEX j=i+1; j<Object->Labelling.EdgeCount; ++j){
			POINT2Dv Pij;

			//Raquel: Habría que añadir una nueva condición, porque las dos aristas que se cortan
			//han de pertencer al mismo subgrafo

			if ((!EdgesShareVertex(Object,i,j))
				&& (!OneEdgeInterceptsTheOther(Object,i,j))
				&& (TwoLinesCrossAtPoint(Object,i,j,&Pij))){

				VERTEX_INDEX CrossV;
				if (ThereIsAUseableJunction(Object,i,j,NO_EDGE,&Pij,&CrossV)){
					//printf("Lines %d,%d cross near %d!\n", i, j, CrossV);
					CreateCrossVertexFromJunction(Object,i,j,CrossV);
				}
				else if (ThereIsAUseableIntercept(Object,i,j,NO_EDGE,&Pij,&CrossV)){
					//printf("Lines %d,%d cross near %d!\n", i, j, CrossV);
					CreateCrossVertexFromIntercept(Object,i,j,CrossV);
				}
				else{
					//printf("Lines %d,%d cross!\n", i, j);
					CreateCrossVertexFromNew(Object,i,j,&Pij);
				}
				return cTRUE;
			}
		}
	}

	return cFALSE;
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::Turn3CrossingLinesIntoXJunction (OBJECT *Object)
{
	for (EDGE_INDEX i=0; i<Object->Labelling.EdgeCount; ++i)
	{
		for (EDGE_INDEX j=i+1; j<Object->Labelling.EdgeCount; ++j)
		{
			POINT2Dv Pij, Pik, Pjk;

			if ((!EdgesShareVertex(Object,i,j))
				&& (!OneEdgeInterceptsTheOther(Object,i,j))
				&& TwoLinesCrossAtPoint(Object,i,j,&Pij))
			{
				//printf("Lines %d,%d cross!\n", i, j);

				for (EDGE_INDEX k=j+1; k<Object->Labelling.EdgeCount; ++k)
				{
					if ((!EdgesShareVertex(Object,i,k))
						&& (!EdgesShareVertex(Object,j,k))
						&& (!OneEdgeInterceptsTheOther(Object,i,k))  // required for extended-K
						&& (!OneEdgeInterceptsTheOther(Object,j,k))  // required for extended-K
						&& TwoLinesCrossAtPoint(Object,i,k,&Pik)
						&& TwoLinesCrossAtPoint(Object,j,k,&Pjk))
					{
						if ((DistanceFromPoint2ToPoint2(&Pij,&Pik) < CROSSING_THRESHOLD) &&
							(DistanceFromPoint2ToPoint2(&Pij,&Pjk) < CROSSING_THRESHOLD) &&
							(DistanceFromPoint2ToPoint2(&Pik,&Pjk) < CROSSING_THRESHOLD))
						{
							// With the introduction of 7- and 8-way junctions, we need
							// to check whether a triple-cross is near an existing junction or intercept
							VERTEX_INDEX CrossV;

							if (ThereIsAUseableJunction(Object,i,j,k,&Pij,&CrossV))
							{
								//printf("Lines %d,%d,%d cross near %d!\n", i, j, k, CrossV);
								CreateTripleCrossVertexFromJunction(Object,i,j,k,CrossV);
							}
							else
							if (ThereIsAUseableIntercept(Object,i,j,k,&Pij,&CrossV))
							{
								//printf("Lines %d,%d,%d cross near %d!\n", i, j, k, CrossV);
								CreateTripleCrossVertexFromIntercept(Object,i,j,k,CrossV);
							}
							else
							{
								//printf("Lines %d,%d,%d cross!\n", i, j, k);
								CreateXJunctionAtPointOfIntersection(Object,i,j,k,&Pij,&Pik,&Pjk);
							}
						}
						else
						{
							CString sMensaje, sMensajeAux;
							sMensaje+="Labelling error: Lines ";
							sMensajeAux.Format("%i", i);
							sMensaje+=sMensajeAux;
							sMensaje+=", ";
							sMensajeAux.Format("%i", j);
							sMensaje+=sMensajeAux;
							sMensaje+=", ";
							sMensajeAux.Format("%i", k);
							sMensaje+=sMensajeAux;
							sMensaje+="cross too far apart!";
							AfxMessageBox(sMensaje);

							AfxMessageBox("Lines %d,%d,%d cross too far apart!\n");
							AfxMessageBox("Can't do anything sensible with this, so giving up!");
							//exit(BADNEWS);
						}
					}
				}
			}
		}
	}

	return cFALSE;
}









static lBOOLEAN DirectionsAreFixed = cFALSE;

// NOTE that directions are fixed by calling CalculateAllLineDirections
// Directions should not be fixed while alternative labellings exist
// Directions should be fixed before starting bundling

/* -------------------------------------------------------------------------------- */

void CCueLabelsVarley_IdentifyJunctionAngles::SetLineDirection (OBJECT *Object, EDGE_INDEX Nedge, lBOOLEAN IsKnown, DEGREES Direction)
{
   Object->Edges[Nedge].LineDirectionKnown = IsKnown;
   Object->Edges[Nedge].DrawnLineDirection = Direction;
}


DEGREES CCueLabelsVarley_IdentifyJunctionAngles::CalculateLineDirection (OBJECT *Object, LABELLING *Labelling, EDGE_INDEX Nedge)
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


DEGREES CCueLabelsVarley_IdentifyJunctionAngles::GetLineDirection (OBJECT *Object, EDGE_INDEX Nedge)
{
   if (Object->Edges[Nedge].LineDirectionKnown)  return Object->Edges[Nedge].DrawnLineDirection;
   return CalculateLineDirection(Object,&Object->Labelling,Nedge);
}


void CCueLabelsVarley_IdentifyJunctionAngles::CalculateAllLineDirections (OBJECT *Object)
{
   for (EDGE_INDEX e=0; e<Object->Labelling.EdgeCount; ++e)
   {
	  SetLineDirection(Object,e,cTRUE,CalculateLineDirection(Object,&Object->Labelling,e));
   }

   DirectionsAreFixed = cTRUE;
}


DEGREES CCueLabelsVarley_IdentifyJunctionAngles::DirectionAlongLineFromJunction (OBJECT *Object, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
{
   DEGREES Direction;

   if (DirectionsAreFixed)
   {
	  Direction = GetLineDirection(Object,Nedge);
   }
   else
   {
	  Direction = CalculateLineDirection(Object,&Object->Labelling,Nedge);
   }

   if (Object->Labelling.EdgeStartVertex[Nedge] == Nvert)  return Direction;
   if (Object->Labelling.EdgeEndVertex[Nedge] == Nvert)  return OppositeDirection(Direction);
   return 0.0;
}


////void CCueLabelsVarley_IdentifyJunctionAngles::CheckVertexNumber (LABELLING *Labelling, VERTEX_INDEX n, char *Text)
////{
////   if ((n < 0) || (n >= Labelling->VertexCount))
////   {
////      printf("ERROR: Vertex %d/%d in Check Vertex Number %s!", n, Labelling->VertexCount, Text);
////      exit(666);
////   }
////}


void CCueLabelsVarley_IdentifyJunctionAngles::ConvertT3JunctionToT4 (OBJECT *Object, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Intercepts,Definition;
   VERTEX_INDEX Vb,Vc;
   DEGREES  DirectionA,DirectionX;

   EDGE_INDEX EdgeTLeft,EdgeTRight,EdgeTDown;

   CheckVertexNumber(&Object->Labelling,Nvert,"ConvertT3JunctionToT4");

   Intercepts = Object->HiddenByEdge[Nvert];
   Definition = Object->Labelling.VENumber[Nvert][0];

   Vb = Object->Labelling.EdgeEndVertex[Intercepts];
   Vc = EdgeOtherEnd(Object,Definition,Nvert);

   EdgeTLeft  = Intercepts;

   EdgeTRight = NewEdge(Object,VISIBLE,Vb,Nvert);
   ReplaceEdgeInVertexRecord(Object,Vb,Intercepts,EdgeTRight);
   StoreEdgeInVertexRecord(Object,Nvert,EdgeTRight);

   EdgeTDown  = Definition;

   DirectionX = DirectionAlongLineFromJunction(Object,Definition,Nvert);

   //printf("Do T3->T4 %d %d\n", Nvert, Intercepts);
   //printf(" ... splitting Def=%d, Int=%d into T[%d,%d,%d]\n", Definition, Intercepts, EdgeTLeft, EdgeTRight, EdgeTDown);

   DirectionA = CalculateLineDirection(Object,&Object->Labelling,Intercepts);
   Object->Labelling.EdgeEndVertex[Intercepts] = Nvert;

   Object->Labelling.EdgeLabel[EdgeTRight] = LineIsUnknown;
   ReplaceInterceptsAtSplit(Object,Intercepts,EdgeTRight,Vb,Nvert);

   if (clockwise(DirectionX,DirectionA))
   {
      Object->Labelling.VENumber[Nvert][0] = EdgeTRight;
      Object->Labelling.VENumber[Nvert][1] = EdgeTLeft;
   }
   else
   {
      Object->Labelling.VENumber[Nvert][0] = EdgeTLeft;
      Object->Labelling.VENumber[Nvert][1] = EdgeTRight;
   }

   Object->Labelling.VENumber[Nvert][2] = EdgeTDown;
   Object->Labelling.VECount[Nvert] = 3;
   Object->HiddenByEdge[Nvert] = NO_EDGE;

   /* any vertices which previously intercepted "Intercepts"       */
   /* might now intercept one or the other of "Intercepts" or "EdgeTRight" */
   /* depending on which side of the "Defined" line they are       */
   for (int n=0; n<Object->Labelling.VertexCount; ++n)
   {
      if (Object->HiddenByEdge[n] == Intercepts)
      {
         DEGREES DirectionD = DirectionFromJunctionToJunction(Object,Vc,Nvert);
         Object->HiddenByEdge[n] = Object->Labelling.VENumber[Nvert][ (clockwise(DirectionFromJunctionToJunction(Object,Vc,n),DirectionD)) ? 0 : 1 ];
      }
   }

   Object->Labelling.VertexLabel[Nvert].Shape = JunctionIsT4;
   ////printf("%s\n",VertexString(Object,&Object->Labelling,Nvert));
}


//void CCueLabelsVarley_IdentifyJunctionAngles::ReturnT3JunctionsToT4 (OBJECT *Object)
//{
//   for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)  if (Object->Labelling.VertexLabel[n].Shape == JunctionIsT)
//   {
//      ConvertT3JunctionToT4(Object,n);
//   }
//}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::AllInOneHemisphere (OBJECT *Object, VERTEX_INDEX Nvert)
{
   DEGREES Direction1 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][0],Nvert);
   DEGREES Direction2 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][1],Nvert);
   DEGREES Direction3 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][2],Nvert);

   lBOOLEAN Left12 = clockwise(Direction2,Direction1);

   lBOOLEAN W = (Left12 == clockwise(Direction3,Direction1)) || (Left12 == clockwise(Direction2,Direction3));

   //printf("Directions %f %f %f => %c\n", Direction1, Direction2, Direction3, W?'W':'Y');
   return W;
}

/* ================================================================================ */

void CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtTJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   ConvertT3JunctionToT4(Object,Nvert);
   //printf("T %s\n", VertexString(Object,Object->Labelling,Nvert));
}

/* ================================================================================ */
/* Junctions with only two edges are, by definition, L-junctions                    */
/* Ensure that the edge on the left is the defining one                             */

void CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtLJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   DEGREES DirectionAlongP,DirectionAlongQ;

   CheckVertexNumber(&Object->Labelling,Nvert,"RearrangeEdgesAtLJunction");

   EDGE_INDEX Np = Object->Labelling.VENumber[Nvert][0];
   EDGE_INDEX Nq = Object->Labelling.VENumber[Nvert][1];
   //printf("Rearrange Edges at L Junction %d (%d,%d)\n", Nvert, Np, Nq);

   DirectionAlongP = DirectionAlongLineFromJunction(Object,Np,Nvert);
   DirectionAlongQ = DirectionAlongLineFromJunction(Object,Nq,Nvert);

   //printf("P => %6.2f, Q => %6.2f\n", DirectionAlongP, DirectionAlongQ);

   if (clockwise(DirectionAlongP,DirectionAlongQ))
   {
      //printf("L junction: edges already correct\n");
      /* that's OK */
   }
   else
   {
      //printf("L junction: swapping %d and %d\n", Np, Nq);
      Object->Labelling.VENumber[Nvert][0] = Nq;
      Object->Labelling.VENumber[Nvert][1] = Np;
   }

   Object->Labelling.VertexLabel[Nvert].Shape = JunctionIsL;
   //printf("L %s\n", VertexString(Object,Object->Labelling,Nvert));

   Object->Vertices[Nvert].IsHidden = cFALSE;  // L-junctions are never occluding
}

/* ================================================================================ */
/* In non-trihedral sketches, we have to watch out for K-junctions, which appear    */
/* to be places where two lines create a double-T-junction by intersecting a third  */
/* line at the same junction. These are illegal in trihedral sketches as they       */
/* contradict the general-position assumption.                                      */
/* We must also distinguish X4 junctions (which can be trihedral) from K-junctions, */
/* which are implicitly non-trihedral.                                              */
/* -------------------------------------------------------------------------------- */

lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtKJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   EDGE_INDEX E1,E2,E3,E4;
   DEGREES Dv1,Dv3,Dv4;

   CheckVertexNumber(&Object->Labelling,Nvert,"RearrangeEdgesAtKJunction");
   E1 = Object->HiddenByEdge[Nvert];
   E3 = Object->Labelling.VENumber[Nvert][0];
   E4 = Object->Labelling.VENumber[Nvert][1];

   VERTEX_INDEX V1 = Object->Labelling.EdgeStartVertex[E1];
   VERTEX_INDEX V2 = Object->Labelling.EdgeEndVertex[E1];

   Dv1 = DirectionFromJunctionToJunction(Object,Nvert,V1);
   Dv3 = DirectionAlongLineFromJunction(Object,E3,Nvert);
   Dv4 = DirectionAlongLineFromJunction(Object,E4,Nvert);

   if (clockwise(Dv1,Dv3))
   {
      Object->Labelling.EdgeStartVertex[E1] = V2;
      Object->Labelling.EdgeEndVertex[E1]   = Nvert;

      E2 = NewEdge(Object,VISIBLE,V1,Nvert);
      ReplaceEdgeInVertexRecord(Object,V1,E1,E2);
      StoreEdgeInVertexRecord(Object,Nvert,E2);
      ReplaceInterceptsAtSplit(Object,E1,E2,V1,Nvert);
   }
   else
   {
      Object->Labelling.EdgeStartVertex[E1] = V1;
      Object->Labelling.EdgeEndVertex[E1] = Nvert;

      E2 = NewEdge(Object,VISIBLE,V2,Nvert);
      ReplaceEdgeInVertexRecord(Object,V2,E1,E2);
      StoreEdgeInVertexRecord(Object,Nvert,E2);
      ReplaceInterceptsAtSplit(Object,E1,E2,V2,Nvert);
   }

   Object->Labelling.VENumber[Nvert][0] = E1;
   Object->Labelling.VENumber[Nvert][1] = E2;

   if (clockwise(Dv3,Dv4))
   {
      Object->Labelling.VENumber[Nvert][2] = E3;
      Object->Labelling.VENumber[Nvert][3] = E4;
   }
   else
   {
      Object->Labelling.VENumber[Nvert][2] = E4;
      Object->Labelling.VENumber[Nvert][3] = E3;
   }

   Object->Labelling.VECount[Nvert] = 4;
   Object->Labelling.VertexLabel[Nvert].Shape = JunctionIsK;

   Object->Vertices[Nvert].IsHidden = cFALSE;  // K-junctions are never occluding
   Object->HiddenByEdge[Nvert] = NO_EDGE;

   LP->KJunctionsKnownToBePresent = cTRUE;

   //printf("K %s\n", VertexString(Object,&Object->Labelling,Nvert));
   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtXJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Ed;
   VERTEX_INDEX Vis,Vie,Vc,Vd;
   DEGREES Dvc,Dvb,Dva;

   CheckVertexNumber(&Object->Labelling,Nvert,"RearrangeEdgesAtXJunction");

   EDGE_INDEX Ea = Object->Labelling.VENumber[Nvert][1];
   EDGE_INDEX Eb = Object->Labelling.VENumber[Nvert][0];
   EDGE_INDEX Ec = Object->HiddenByEdge[Nvert];
   Vis = Object->Labelling.EdgeStartVertex[Ec];
   Vie = Object->Labelling.EdgeEndVertex[Ec];

   Dvc = DirectionFromJunctionToJunction(Object,Nvert,Vis);
   Dvb = DirectionAlongLineFromJunction(Object,Eb,Nvert);
   Dva = DirectionAlongLineFromJunction(Object,Ea,Nvert);

   if (clockwise(Dva,Dvb))
      /* that's the way it should be */ ;
   else
   {
      EDGE_INDEX Etemp;
      DEGREES          Dtemp;
      Etemp = Eb;   Eb  = Ea;   Ea  = Etemp;
      Dtemp = Dvb;  Dvb = Dva;  Dva = Dtemp;
   }

   if (clockwise(Dva,Dvc))  { Vc = Vis;  Vd = Vie; }  else  { Vc = Vie;  Vd = Vis;  Dvc = OppositeDirection(Dvc); }

   Object->Labelling.EdgeStartVertex[Ec] = Vc;
   Object->Labelling.EdgeEndVertex[Ec] = Nvert;

   Ed = NewEdge(Object,VISIBLE,Vd,Nvert);
   ReplaceEdgeInVertexRecord(Object,Vd,Ec,Ed);
   StoreEdgeInVertexRecord(Object,Nvert,Ed);

   /* It's not a good idea to set the edge types here,  */
   /* just in case the edges get split up unpredictably */

   Object->Labelling.VENumber[Nvert][0] = Ec;
   Object->Labelling.VENumber[Nvert][1] = Eb;
   Object->Labelling.VENumber[Nvert][2] = Ed;
   Object->Labelling.VENumber[Nvert][3] = Ea;
   Object->Labelling.VECount[Nvert] = 4;
   Object->Labelling.VertexLabel[Nvert].Shape  = JunctionIsZ;
   Object->Labelling.VertexLabel[Nvert].Vexity = Zcbda;
   Object->Labelling.VertexUnderlyingType[Nvert] = VFinalIs6cdcdcd;

   Object->HiddenByEdge[Nvert] = NO_EDGE;

   //printf("Z4 %s\n", VertexString(Object,&Object->Labelling,Nvert));
   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtKorXJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   DEGREES  Dvc = DirectionFromJunctionToJunction(Object,Nvert,Object->Labelling.EdgeStartVertex[Object->HiddenByEdge[Nvert]]);
   DEGREES  Dvb = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][0],Nvert);
   DEGREES  Dva = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][1],Nvert);

   //printf("Rearrange Edges at K or X Junction\n");

   if (clockwise(Dvc,Dvb) == clockwise(Dvc,Dva))
   {
      return RearrangeEdgesAtKJunction(Object,Nvert);
   }
   else
   {
      return RearrangeEdgesAtXJunction(Object,Nvert);
   }
}

/* ================================================================================ */
/* W Junctions -                                                                    */
/* Rearrange the three edges so that from left to right they are 0, 1, 2            */

void CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtWJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   DEGREES DirectionP,DirectionQ,DirectionR;

   CheckVertexNumber(&Object->Labelling,Nvert,"RearrangeEdgesAtWJunction");

   EDGE_INDEX P = Object->Labelling.VENumber[Nvert][0];
   EDGE_INDEX Q = Object->Labelling.VENumber[Nvert][1];
   EDGE_INDEX R = Object->Labelling.VENumber[Nvert][2];

   DirectionP = DirectionAlongLineFromJunction(Object,P,Nvert);
   DirectionQ = DirectionAlongLineFromJunction(Object,Q,Nvert);
   DirectionR = DirectionAlongLineFromJunction(Object,R,Nvert);

   if ((clockwise(DirectionR,DirectionP)) && (clockwise(DirectionQ,DirectionP)))
   {
      if (clockwise(DirectionR,DirectionQ))
      {
         Object->Labelling.VENumber[Nvert][0] = R;
         Object->Labelling.VENumber[Nvert][1] = Q;
         Object->Labelling.VENumber[Nvert][2] = P;
      }
      else
      {
         Object->Labelling.VENumber[Nvert][0] = Q;
         Object->Labelling.VENumber[Nvert][1] = R;
         Object->Labelling.VENumber[Nvert][2] = P;
      }
   }
   else if ((clockwise(DirectionR,DirectionQ)) && (clockwise(DirectionP,DirectionQ)))
   {
      if (clockwise(DirectionR,DirectionP))
      {
         Object->Labelling.VENumber[Nvert][0] = R;
         Object->Labelling.VENumber[Nvert][1] = P;
         Object->Labelling.VENumber[Nvert][2] = Q;
      }
      else
      {
         Object->Labelling.VENumber[Nvert][0] = P;
         Object->Labelling.VENumber[Nvert][1] = R;
         Object->Labelling.VENumber[Nvert][2] = Q;
      }
   }
   else
   {
      if (clockwise(DirectionQ,DirectionP))
      {
         Object->Labelling.VENumber[Nvert][0] = Q;
         Object->Labelling.VENumber[Nvert][1] = P;
         Object->Labelling.VENumber[Nvert][2] = R;
      }
      else
      {
         Object->Labelling.VENumber[Nvert][0] = P;
         Object->Labelling.VENumber[Nvert][1] = Q;
         Object->Labelling.VENumber[Nvert][2] = R;
      }
   }

   Object->Labelling.VertexLabel[Nvert].Shape = JunctionIsW;
   //printf("W %s\n", VertexString(Object,&Object->Labelling,Nvert));

   Object->Vertices[Nvert].IsHidden = cFALSE;  // W-junctions are never occluding

   if (LP->DepthCornerOptions == DEPTH_AUTO_SELECT)
   {
      DEGREES D0 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][0],Nvert);
      DEGREES D1 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][1],Nvert);
      DEGREES D2 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][2],Nvert);
      if ((dangleclockMaths(D0,D1) >= 90.0) || (dangleclockMaths(D1,D2) >= 90.0))  LP->DepthCornerOptions = DEPTH_JLP;
   }
}

/* -------------------------------------------------------------------------------- */
/* Y Junctions
   Keep the Defining Edge (it's arbitrary anyway),
   but label the other two 0 and 1 in clockwise order.
   If the drawing is of an axis-aligned object, all three angles should be obtuse.
   (If they're not, we don't want to use the Perkins depth method.)
*/

void CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtYJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   CheckVertexNumber(&Object->Labelling,Nvert,"RearrangeEdgesAtYJunction");

   EDGE_INDEX o = Object->Labelling.VENumber[Nvert][0];
   EDGE_INDEX p = Object->Labelling.VENumber[Nvert][1];

   if (clockwise(DirectionAlongLineFromJunction(Object,o,Nvert),DirectionAlongLineFromJunction(Object,p,Nvert)))
   {
      /* already set up correctly */
   }
   else
   {
      Object->Labelling.VENumber[Nvert][1] = Object->Labelling.VENumber[Nvert][2];
      Object->Labelling.VENumber[Nvert][2] = p;
   }

   Object->Labelling.VertexLabel[Nvert].Shape = JunctionIsY;
   //printf("Y %s\n", VertexString(Object,&Object->Labelling,Nvert));

   Object->Vertices[Nvert].IsHidden = cFALSE;  // Y-junctions are never occluding

   if (LP->DepthCornerOptions == DEPTH_AUTO_SELECT)
   {
      DEGREES D0 = DirectionAlongLineFromJunction(Object,o,Nvert);
      DEGREES D1 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][1],Nvert);
      DEGREES D2 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[Nvert][2],Nvert);
      if ((dangleclockMaths(D0,D1) <= 90.0) || (dangleclockMaths(D0,D2) <= 90.0) || (dangleclockMaths(D1,D2) <= 90.0))
      {
         LP->DepthCornerOptions = DEPTH_JLP;
      }
   }
}


void CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAt4Junction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Edges[MAX_EDGES_PER_VERTEX];
   DEGREES    Angles[MAX_EDGES_PER_VERTEX];
   int m,n,Ecount;
   int EIndex = 0;

   CheckVertexNumber(&Object->Labelling,Nvert,"RearrangeEdgesAt4Junction");
   Ecount = Object->Labelling.VECount[Nvert];
   //printf("Rearrange edges at 4-junction %s\n", VertexString(Object,&Object->Labelling,Nvert));

   for (n=0; n<Object->Labelling.EdgeCount; ++n)
   {
      if ((Object->Labelling.EdgeStartVertex[n] == Nvert) || (Object->Labelling.EdgeEndVertex[n] == Nvert))
      {
         Angles[EIndex] = DirectionAlongLineFromJunction(Object,n,Nvert);
         if (Angles[EIndex] < 0.0)  Angles[EIndex] += 360.0;
         Edges[EIndex++] = n;
      }
   }

   for (m=0; m<(Ecount-1); m++)  for (n=0; n<((Ecount-1)-m); n++)
   {
      if (Angles[n] > Angles[n+1])
      {
         { DEGREES    TempA = Angles[n];  Angles[n] = Angles[n+1];  Angles[n+1] = TempA; }
         { EDGE_INDEX TempE = Edges[n];   Edges[n]  = Edges[n+1];   Edges[n+1]  = TempE; }
      }
   }

   for (n=0; n<Ecount-1; ++n)  if ((Angles[n+1]-Angles[n]) > 180.0)
   {
      int m;
      for (m=0; m<Ecount; ++m)  Object->Labelling.VENumber[Nvert][m] = Edges[(n+1+m)%Ecount];
      Object->Labelling.VertexLabel[Nvert].Shape  = JunctionIsM;
      Object->Labelling.VertexLabel[Nvert].Vexity = Mall;
      Object->Vertices[Nvert].IsHidden = cFALSE;  // M-junctions are never occluding
      return;
   }

   for (n=0; n<Ecount; ++n)  Object->Labelling.VENumber[Nvert][n] = Edges[n];

   Object->Labelling.VertexLabel[Nvert].Shape = (((Angles[0]+360.0)-Angles[Ecount-1]) > 180.0)?JunctionIsM:JunctionIsX;

   Object->Vertices[Nvert].IsHidden = cFALSE;  // X-junctions and M-junctions are never occluding

   LP->XJunctionsKnownToBePresent = cTRUE;

   //printf("X %s\n", VertexString(Object,Labelling,Nvert));
}


void CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtMultiJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   //printf("RearrangeEdgesAtMultiJunction %s\n",VertexString(Object,Labelling,Nvert));
   EDGE_INDEX Edges[MAX_EDGES_PER_VERTEX];
   DEGREES    Angles[MAX_EDGES_PER_VERTEX];
   lBOOLEAN  AllNInOneHemisphere = cFALSE;

   CheckVertexNumber(&Object->Labelling,Nvert,"RearrangeEdgesAtMultiJunction");

   int Ecount = Object->Labelling.VECount[Nvert];
   int EIndex = 0;

   for (int n=0; n<Ecount; ++n)
   {
      EDGE_INDEX E = Object->Labelling.VENumber[Nvert][n];
      Angles[EIndex] = DirectionAlongLineFromJunction(Object,E,Nvert);
      if (Angles[EIndex] < 0.0)  Angles[EIndex] += 360.0;
      Edges[EIndex++] = E;
   }

   for (int m=0; m<Ecount-1; m++)  for (int n=0; n<(Ecount-1)-m; n++)
   {
      if (Angles[n] > Angles[n+1])
      {
         { DEGREES    TempA = Angles[n];  Angles[n] = Angles[n+1];  Angles[n+1] = TempA; }
         { EDGE_INDEX TempE = Edges[n];   Edges[n]  = Edges[n+1];   Edges[n+1]  = TempE; }
      }
   }

   if (Object->Labelling.VertexLabel[Nvert].Shape == JunctionIsM)
   {
      for (int n=0; n<Ecount-1; ++n)  if ((Angles[n+1]-Angles[n]) > 180.0)  AllNInOneHemisphere = cTRUE;
      if (((Angles[0]+360.0)-Angles[Ecount-1]) > 180.0)  AllNInOneHemisphere = cTRUE;
      Object->Labelling.VertexLabel[Nvert].Shape = (AllNInOneHemisphere)?JunctionIsM:JunctionIsX;
   }

   Object->Vertices[Nvert].IsHidden = cFALSE;  // X-junctions and M-junctions are never occluding

   for (int n=0; n<Ecount; ++n)  Object->Labelling.VENumber[Nvert][n] = Edges[n];
   LP->XJunctionsKnownToBePresent = cTRUE;
   //printf("Rearranged Edges At MultiJunction %d\n",Nvert);
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::RearrangeEdgesAtKMultiJunction (OBJECT *Object, VERTEX_INDEX Nvert)
{
   //printf("K(before) vertex %s\n", VertexString(Object,&Object->Labelling,Nvert));

   CheckVertexNumber(&Object->Labelling,Nvert,"RearrangeEdgesAtKMultiJunction");
   EDGE_INDEX Ex = Object->HiddenByEdge[Nvert];
   VERTEX_INDEX VS = Object->Labelling.EdgeStartVertex[Ex];
   VERTEX_INDEX VT = Object->Labelling.EdgeEndVertex[Ex];

   Object->Labelling.EdgeStartVertex[Ex] = VS;
   Object->Labelling.EdgeEndVertex[Ex] = Nvert;
   StoreEdgeInVertexRecord(Object,Nvert,Ex);

   EDGE_INDEX Ey = NewEdge(Object,VISIBLE,VT,Nvert);
   ReplaceEdgeInVertexRecord(Object,VT,Ex,Ey);
   StoreEdgeInVertexRecord(Object,Nvert,Ey);

   ReplaceInterceptsAtSplit(Object,Ex,Ey,VT,Nvert);

   Object->Labelling.VECount[Nvert] = CountOfEdgesMeetingVertex(Object,Nvert);
   RearrangeEdgesAtMultiJunction(Object,Nvert);
   Object->Labelling.VertexLabel[Nvert].Shape = JunctionIsKplus;
   Object->Vertices[Nvert].IsHidden = cFALSE;  // K+-junctions are never occluding

   //printf("K(after)  vertex %s\n", VertexString(Object,&Object->Labelling,Nvert));

   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::IdentifyAnglesAtNormalJunction (OBJECT *Object, VERTEX_INDEX n)
{
	int jlc = CountOfEdgesMeetingVertex(Object,n);
	CString sMensaje, sMensajeAux;

	switch (jlc)
	{
		case 1:
			sMensaje+="ERROR: junction ";
			sMensajeAux.Format("%i", n);
			sMensaje+=sMensajeAux;
			sMensaje+=" has only one line.\r\n The labeler is unable to handle this situation!";
			sMensaje+=" \r\n If it is a T vertex, try 'Split edges in T vertices' in the '2D Refine Configuration' dialog";
			AfxMessageBox(sMensaje);
			return cFALSE;

		case 2:
			RearrangeEdgesAtLJunction(Object,n);
			return cTRUE;

		case 3:
			if (AllInOneHemisphere(Object,n))
				RearrangeEdgesAtWJunction(Object,n);
			else
				RearrangeEdgesAtYJunction(Object,n);
			return cTRUE;

		case 4:
			RearrangeEdgesAt4Junction(Object,n);
			return cTRUE;

		case 5:
			RearrangeEdgesAtMultiJunction(Object,n);
			Object->Labelling.VertexLabel[n].Shape = JunctionIs5way;
			Object->Vertices[n].IsHidden = cFALSE;  // X-junctions are never occluding
			return cTRUE;

		case 6:
			RearrangeEdgesAtMultiJunction(Object,n);
			Object->Labelling.VertexLabel[n].Shape = JunctionIs6way;
			Object->Vertices[n].IsHidden = cFALSE;  // X-junctions are never occluding
			return cTRUE;

		case 7:
			RearrangeEdgesAtMultiJunction(Object,n);
			Object->Labelling.VertexLabel[n].Shape = JunctionIs7way;
			Object->Vertices[n].IsHidden = cFALSE;  // X-junctions are never occluding
			return cTRUE;

		case 8:
			RearrangeEdgesAtMultiJunction(Object,n);
			Object->Labelling.VertexLabel[n].Shape = JunctionIs8way;
			Object->Vertices[n].IsHidden = cFALSE;  // X-junctions are never occluding
			return cTRUE;

		default:
			sMensaje+="ERROR: too many lines at junction ";
			sMensajeAux.Format("%i", n);
			sMensaje+=sMensajeAux;
			sMensaje+="\r\n The labeler is unable to handle this situation!";
			AfxMessageBox(sMensaje);
			return cFALSE;
	}
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::IdentifyAnglesAtInterceptJunction (OBJECT *Object, VERTEX_INDEX n)
{
	int jlc = CountOfEdgesMeetingVertex(Object,n);
	CString sMensaje, sMensajeAux;

	switch (jlc)
	{
		case 1:
			RearrangeEdgesAtTJunction(Object,n);
			return cTRUE;

		case 2:
			return RearrangeEdgesAtKorXJunction(Object,n);

		case 3:
		case 4:
		case 5:
		case 6:
			return RearrangeEdgesAtKMultiJunction(Object,n);

		default:
			sMensaje+="ERROR: too many lines at junction ";
			sMensajeAux.Format("%i", n);
			sMensaje+=sMensajeAux;
			sMensaje+="\r\n The labeler is unable to handle this situation!";
			AfxMessageBox(sMensaje);
		return cFALSE;
	}
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::IdentifyAnglesAtJunction (OBJECT *Object, VERTEX_INDEX n)
{
	//printf("%s\n", VertexString(Object,&Object->Labelling,n));

	if (Object->HiddenByEdge[n] == NO_EDGE)
	{
		return IdentifyAnglesAtNormalJunction(Object,n);
	}
	else
	{
		return IdentifyAnglesAtInterceptJunction(Object,n);
	}
}


lBOOLEAN CCueLabelsVarley_IdentifyJunctionAngles::IdentifyJunctionAngles (OBJECT *Object)

//This subcomponent labels each junction as L, W, Y , T, M, X, K or Z.

//The junction type can be identified purely by considering the number and
//relative angles of lines meeting at each junction, so requires no information other
//than the vertex-edge graph and the vertex x and y coordinates. This preliminary labelling
//simplifies implementation of region identification and should ideally precede
//it. T-junctions must also be labelled as such before the outline of the object can be
//identified with confidence.

//The required inputs are:
//a list of junctions, with 2D coordinates;
//a list of lines, with the junctions they join;
//a list of intercepts, relating T-junctions and K-junctions to the lines they intercept.

//There are no optional inputs.

//The primary output is a junction type for each junction.

//As a secondary output, for each junction, a list of the lines meeting at this
//junction is produced. This list is ordered clockwise.

{
	// check for input consistency
	for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n){
		EDGE_INDEX Ne0 = Object->Labelling.VENumber[n][0];

		if ((Ne0 >= Object->Labelling.EdgeCount) || (Ne0 < 0)){
			//Vertex has an invalid edge
			return cFALSE;
		}

		if ((Object->Labelling.EdgeStartVertex[Ne0] != n) &&
			(Object->Labelling.EdgeEndVertex[Ne0] != n)){
			//Vertex and edge are inconsistent
			return cFALSE;
		}
	}

	for (EDGE_INDEX n=0; n<Object->Labelling.EdgeCount; ++n){
		if ((Object->Labelling.EdgeStartVertex[n] >= Object->Labelling.VertexCount) ||
			(Object->Labelling.EdgeEndVertex[n] >= Object->Labelling.VertexCount)){
			//Edge has an invalid vertex
			return cFALSE;
		}
	}
	//Input is Consistent

	//Repair ill-defined junctions
	while (Turn3CrossingLinesIntoXJunction(Object)) {}

	while (Turn2CrossingLinesIntoXJunction(Object)) {}

	//Label junctions
	for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)
	if (Object->Labelling.VertexLabel[n].Shape == JunctionIsUndefined)
	{
		if (!IdentifyAnglesAtJunction(Object,n))
			return cFALSE;
	}

	return cTRUE;
}
