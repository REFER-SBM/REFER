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

#include <stdafx.h>  //CString
#include <cmath>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"

#include "DataLabels.h"

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Maths.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"
#include "CueLabelsVarley_LabellingConstants.h"
#include "CueLabelsVarley_LabellingParameters.h"
#include "CueLabelsVarley_LineToJunctions.h"

#include "CueLabelsVarley_LineEquations.h"

#include "CueLabelsVarley_Lamban.h"


	static lREAL Icoord[MAX_VERTICES];
	static lREAL Jcoord[MAX_VERTICES];
	static lREAL Kcoord[MAX_VERTICES];

	static lREAL Icoocc[MAX_VERTICES];
	static lREAL Jcoocc[MAX_VERTICES];
	static lREAL Kcoocc[MAX_VERTICES];


	static VERTEX_INDEX Nvert;
	static lREAL Xcoord[MAX_VERTICES];
	static lREAL Ycoord[MAX_VERTICES];
	static lREAL tScaling,tRotAxisLat,tRotAxisLong,tRotAngle;
	// Also, if using deductive reasoning, we want to know the bounding box of the object
	static IMPLICIT_PLANE MinusILimit;
	static IMPLICIT_PLANE MinusJLimit;
	static IMPLICIT_PLANE MinusKLimit;


	

CCueLabelsVarley_Lamban::CCueLabelsVarley_Lamban(CCueLabelsVarley_LabellingConstants *eLC,
												 CCueLabelsVarley_LabellingParameters *eLP)
{
	LC= eLC;
	LP= eLP;

	int MaxCandidates  = 0;
	int CandidateCount = 0;

	// a bigger figure of tcMp makes the system more fussy
	// about parallel lines really being parallel
	tcMp = 0.4560;
	tcMd = 0.0412;

	pAnyTwoLinesAreParallel = 0.050;
	pOppositeSidesOfQuadrilateralAreParallel = 0.350;
	pContinueDespiteOneFailure = 0.600;


	LambanAxisOptions = LambanAxesFrequentIJMiddleK;

	UseDeductionsInLamban = cFALSE;

	plusI = TEN_O_CLOCK; minusI = FOUR_O_CLOCK;
	plusJ = TWO_O_CLOCK; minusJ = EIGHT_O_CLOCK;
	plusK = SIX_O_CLOCK; minusK = TWELVE_O_CLOCK;

	tcOk = 0.5;

	tcOe = 1.0000;
	tcOy = 0.6250;
	tcOz = 0.9375;

	CollinearLineCount = 0;

	FoMIndeterminate = 0.1;

	DirectionsAreFixed = cFALSE;

	tcOp = 0.0000;

	OldStyle = cFALSE;

	VexityInversion = cFALSE;
}


CCueLabelsVarley_Lamban::~CCueLabelsVarley_Lamban()
{
}


void CCueLabelsVarley_Lamban::LearnThatEdgesAreParallel (EDGE_INDEX A, EDGE_INDEX B)
{
   //printf("Learn that edges %d and %d are parallel\n", A, B);
   if (A != B)  Knowledge[trindex(A,B)].Yes = cTRUE;
}


void CCueLabelsVarley_Lamban::LearnThatEdgesAreNotParallel (EDGE_INDEX A, EDGE_INDEX B)
{
   //printf("Learn that edges %d and %d are not parallel\n", A, B);
   if (A != B)  Knowledge[trindex(A,B)].No = cTRUE;
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromLJunction (OBJECT *Object, VERTEX_INDEX V)
{
   // under no circumstances can the two lines at an L-junction be parallel
   LearnThatEdgesAreNotParallel(Object->Labelling.VENumber[V][0], Object->Labelling.VENumber[V][1]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromWJunction (OBJECT *Object, VERTEX_INDEX V)
{
   // under no circumstances can any pair of two lines at a W-junction be parallel
   LearnThatEdgesAreNotParallel(Object->Labelling.VENumber[V][0], Object->Labelling.VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Object->Labelling.VENumber[V][0], Object->Labelling.VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Object->Labelling.VENumber[V][1], Object->Labelling.VENumber[V][2]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromYJunction (OBJECT *Object, VERTEX_INDEX V)
{
   // under no circumstances can any pair of two lines at a Y-junction be parallel
   LearnThatEdgesAreNotParallel(Object->Labelling.VENumber[V][0], Object->Labelling.VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Object->Labelling.VENumber[V][0], Object->Labelling.VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Object->Labelling.VENumber[V][1], Object->Labelling.VENumber[V][2]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromTJunction (OBJECT *Object, VERTEX_INDEX V)
{
   // TBD: we get here for old-style bundling,
   //      and obviously the occluded edge can't be parallel to the occluding edge
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromT4Junction (LABELLING *Labelling, VERTEX_INDEX V)
{
   // Edges 0 and 1 are inevitably parallel (they started off as the same edge)
   // Edge 2 cannot possibly be parallel to either edge 0 or edge 1
   LearnThatEdgesAreParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][2]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromMJunction (LABELLING *Labelling, VERTEX_INDEX V)
{
   // under no circumstances can any pair of two lines at an M-junction be parallel
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][3]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromX4Junction (LABELLING *Labelling, VERTEX_INDEX V)
{
   // under no circumstances can any pair of two lines at an X-junction be parallel
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][3]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromKJunction (LABELLING *Labelling, VERTEX_INDEX V)
{
   // Edges 0 and 1 are inevitably parallel (they started off as the same edge)
   // Edges 2 and 3 cannot possibly be parallel to either edge 0 or edge 1 or one another
   LearnThatEdgesAreParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][3]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromZJunction (LABELLING *Labelling, VERTEX_INDEX V)
{
   // Edges 0 and 2 are inevitably parallel (they started off as the same edge)
   // Edges 1 and 3 cannot possibly be parallel to either edge 0 or edge 2 or one another
   LearnThatEdgesAreParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][3]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromX5Junction (LABELLING *Labelling, VERTEX_INDEX V)
{
   // under no circumstances can any pair of two lines at an X-junction be parallel
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][3], Labelling->VENumber[V][4]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromX6Junction (LABELLING *Labelling, VERTEX_INDEX V)
{
   // X6 junctions are tricky in that opposite lines might, but need not, be parallel

   // under no circumstances can any other pair of two lines at an X-junction be parallel
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][3], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][5]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][5]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][3], Labelling->VENumber[V][5]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][4], Labelling->VENumber[V][5]);
}


void CCueLabelsVarley_Lamban::DetermineKnowledgeFromZ6Junction (LABELLING *Labelling, VERTEX_INDEX V)
{
   // In Z6 junctions the junction was created from three crossing
   // lines, so opposite lines must inevitably be parallel;
   LearnThatEdgesAreParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][3]);
   LearnThatEdgesAreParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][4]);
   LearnThatEdgesAreParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][5]);

   // under no circumstances can any other pair of two lines at an X-junction be parallel
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][1]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][2]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][3]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][2], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][3], Labelling->VENumber[V][4]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][0], Labelling->VENumber[V][5]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][1], Labelling->VENumber[V][5]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][3], Labelling->VENumber[V][5]);
   LearnThatEdgesAreNotParallel(Labelling->VENumber[V][4], Labelling->VENumber[V][5]);
}


void CCueLabelsVarley_Lamban::InitialiseParallelKnowledge (OBJECT *Object)
{
   int KnowledgeSize = trisize(Object->Labelling.EdgeCount);

   for (int i=0; i<KnowledgeSize; ++i)  Knowledge[i].Yes = Knowledge[i].No = cFALSE;

   for (int i=0; i<Object->Labelling.VertexCount; ++i)
   {
      switch (Object->Labelling.VertexLabel[i].Shape)
      {
         case JunctionIsK:
            DetermineKnowledgeFromKJunction(&Object->Labelling,i);
         break;

         case JunctionIsL:
            DetermineKnowledgeFromLJunction(Object,i);
         break;

         case JunctionIsM:
            DetermineKnowledgeFromMJunction(&Object->Labelling,i);
         break;

         case JunctionIsT:
            DetermineKnowledgeFromTJunction(Object,i);
         break;

         case JunctionIsT4:
            DetermineKnowledgeFromT4Junction(&Object->Labelling,i);
         break;

         case JunctionIsW:
            DetermineKnowledgeFromWJunction(Object,i);
         break;

         case JunctionIsX:
            DetermineKnowledgeFromX4Junction(&Object->Labelling,i);
         break;

         case JunctionIsY:
            DetermineKnowledgeFromYJunction(Object,i);
         break;

         case JunctionIsZ:
            DetermineKnowledgeFromZJunction(&Object->Labelling,i);
         break;

         case JunctionIs5way:
            DetermineKnowledgeFromX5Junction(&Object->Labelling,i);
         break;

         case JunctionIs6way:
            DetermineKnowledgeFromX6Junction(&Object->Labelling,i);
         break;

         case JunctionIsZ6:
            DetermineKnowledgeFromZ6Junction(&Object->Labelling,i);
         break;
      }
   }
}


lBOOLEAN CCueLabelsVarley_Lamban::EdgesKnownToBeParallel (EDGE_INDEX A, EDGE_INDEX B)
{
   return (A == B) ? cTRUE : Knowledge[trindex(A,B)].Yes;
}


lBOOLEAN CCueLabelsVarley_Lamban::EdgesKnownNotToBeParallel (EDGE_INDEX A, EDGE_INDEX B)
{
   return (A == B) ? cFALSE : Knowledge[trindex(A,B)].No;
}


BUNDLE CCueLabelsVarley_Lamban::LowestUnusedBundleNumber (OBJECT *Object)
{
   BUNDLE n = 1;

   for (;;)
   {
      int i;
      lBOOLEAN Used = cFALSE;
      for (i=0; i<Object->Labelling.EdgeCount; ++i)  if (Object->Edges[i].EdgeBundle == n)
      {
         Used = cTRUE;
      }
      if (!Used)
      {
         if (n > Object->BundleCount)  Object->BundleCount = n;
         return n;
      }
      ++n;
   }
}


void CCueLabelsVarley_Lamban::GroupEdgesTogether (OBJECT *Object, EDGE_INDEX E, EDGE_INDEX F)
{
   BUNDLE Eb = Object->Edges[E].EdgeBundle;
   BUNDLE Fb = Object->Edges[F].EdgeBundle;
   EDGE_INDEX i,N;

   N = Object->Labelling.EdgeCount;
   //printf("Grouping Edges %d and %d together\n", E, F);

   if (Eb && Fb)
   {
      if (Eb != Fb)
      {
         BUNDLE Lb = (Eb < Fb) ? Eb : Fb;
         BUNDLE Hb = (Eb < Fb) ? Fb : Eb;
         for (i=0; i<N; ++i)  if (Object->Edges[i].EdgeBundle == Hb)  Object->Edges[i].EdgeBundle = Lb;
      }
   }
   else
   if (Eb)  Object->Edges[F].EdgeBundle = Eb;
   else
   if (Fb)  Object->Edges[E].EdgeBundle = Fb;
   else
   {
      Object->Edges[E].EdgeBundle = Object->Edges[F].EdgeBundle = LowestUnusedBundleNumber(Object);
   }

   for (i=0; i<N; ++i)
   {
      if (EdgesKnownToBeParallel(E,i))  LearnThatEdgesAreParallel(F,i);
      if (EdgesKnownToBeParallel(F,i))  LearnThatEdgesAreParallel(E,i);
      if (EdgesKnownNotToBeParallel(E,i))  LearnThatEdgesAreNotParallel(F,i);
      if (EdgesKnownNotToBeParallel(F,i))  LearnThatEdgesAreNotParallel(E,i);
   }
}


lBOOLEAN CCueLabelsVarley_Lamban::AreOppositeSidesOfQuadrilateral (OBJECT *Object, EDGE_INDEX A, EDGE_INDEX B)
{
   for (FACE_INDEX f=0; f<Object->FaceCount; ++f)
   {
      for (int n=0; n<Object->Faces[n].NumberOfLoops; ++n)
      {
         LOOP *Loop = &Object->Faces[f].Loops[n];
         if (Loop->SideCount == 4)
         {
            int iA = EdgeIndexOnLoop(Loop,A);
            int iB = EdgeIndexOnLoop(Loop,B);
			if ((iA != NO_EDGE) && (iB != NO_EDGE) && (std::abs(iA-iB) == 2))  return cTRUE;
         }
      }
   }

   return cFALSE;
}


MERIT CCueLabelsVarley_Lamban::FoMAnglesParallel2D (DEGREES A, DEGREES B)
{
   return pow(std::fabs(cos((A-B)*M_PI/180.0)),(tcMp*100.0));
}


MERIT CCueLabelsVarley_Lamban::FoMParallel2D (VECTOR2D *V1, VECTOR2D *V2)
{
   lREAL DotProduct = std::fabs(V1->x*V2->x + V1->y*V2->y);
   return pow(DotProduct,(tcMp*100.0));
}


MERIT CCueLabelsVarley_Lamban::FoMLinesParallel (OBJECT *Object, EDGE_INDEX A, EDGE_INDEX B)
{
   return FoMParallel2D(&Object->Edges[A].LineEquation.Normal,&Object->Edges[B].LineEquation.Normal);
}




#define SORTROUTINE __cdecl
int SORTROUTINE DescendingOrder (const void *A, const void *B)
{
   CANDIDATE *Ca = (CANDIDATE *)A;
   CANDIDATE *Cb = (CANDIDATE *)B;
   if (Ca->M < Cb->M)  return 1;   /* we want descending order */
   if (Ca->M > Cb->M)  return -1;
   return 0;
}


void CCueLabelsVarley_Lamban::CreateParallelLineGroups (OBJECT *Object)
{
   int i,j;
   int ActualCount    = 0;

   MERIT CumulativeMerit;
   MaxCandidates = trisize(Object->Labelling.EdgeCount);
   CandidateCount = 0;

   //printf("Create Parallel Line Groups\n");
	CCueLabelsVarley_LineEquations LE;
	LE.GetUnknownLineEquations(Object);  // shouldn't be necessary

   for (i=0; i<Object->Labelling.EdgeCount; ++i)
   {
      for (j=i+1; j<Object->Labelling.EdgeCount; ++j)
      {
         if (!EdgesKnownNotToBeParallel(i,j))
         {
            if (EdgesKnownToBeParallel(i,j))
            {
               GroupEdgesTogether(Object,i,j);
            }
            else if (CandidateCount < MaxCandidates)
            {
               Candidates[CandidateCount].E = i;
               Candidates[CandidateCount].F = j;
               Candidates[CandidateCount].M = FoMLinesParallel(Object,i,j);
               //printf("Candidate E=%d,F=%d,M=%f\n",Candidates[CandidateCount].E,Candidates[CandidateCount].F,Candidates[CandidateCount].M);
               ++CandidateCount;
            }
         }
      }
   }

   qsort (Candidates, CandidateCount, sizeof(CANDIDATE), DescendingOrder);

   i = 0;
   CumulativeMerit = 1.0;

   for (;;)
   {
      MERIT Prior;
      //printf("Candidate E=%d,F=%d,M=%f\n",Candidates[i].E,Candidates[i].F,Candidates[i].M);
      if (EdgesKnownNotToBeParallel(Candidates[i].E,Candidates[i].F))  break;

      Prior = AreOppositeSidesOfQuadrilateral(Object,Candidates[i].E,Candidates[i].F) ? pOppositeSidesOfQuadrilateralAreParallel : pAnyTwoLinesAreParallel;
      if (Pmul(Prior,Candidates[i].M) > THRESHOLD)
      {
         GroupEdgesTogether(Object,Candidates[i].E,Candidates[i].F);
         ++ActualCount;
      }
      else
      {
         CumulativeMerit *= pContinueDespiteOneFailure;
      }
      if (CumulativeMerit < THRESHOLD)  break;
      ++i;
      if (i >= CandidateCount)  break;
   }

   // There might be lines which aren't parallel to anything
   for (i=0; i<Object->Labelling.EdgeCount; ++i)  if (!Object->Edges[i].EdgeBundle)
   {
      Object->Edges[i].EdgeBundle = LowestUnusedBundleNumber(Object);
   }

   //printf("Created Parallel Line Groups: ActualCount=%d, CandidateCount = %d\n", ActualCount, CandidateCount);
   //OverallActualCount += ActualCount;
   //OverallCandidateCount += CandidateCount;
   //printf("Created Parallel Line Groups: OverallActualCount=%d, OverallCandidateCount = %d\n", OverallActualCount, OverallCandidateCount);

   //for (i=0; i<Object->Labelling.EdgeCount; ++i)
   //{
   //   printf("Edge %d, angle %f, allocated to bundle %d\n", i, CalculateLineDirection(Object,Object->Labelling,i), Object->Edges[i]->EdgeBundle);
   //}
}


int CCueLabelsVarley_Lamban::EdgesInBundle (OBJECT *Object, int Bundle)
{
   int C=0;
   for (EDGE_INDEX i=0; i<Object->Labelling.EdgeCount; ++i)  if (Object->Edges[i].EdgeBundle == Bundle) ++C;
   return C;
}


void CCueLabelsVarley_Lamban::SortBundleNumbers (OBJECT *Object)
{
   int BundleNumbers[MAX_BUNDLES];
   int BundleCounts[MAX_BUNDLES];
   int Renumbering[MAX_BUNDLES];
   int Zeros = 0;  // Lamban Labelling can produce zero-sized bundles
   int i,j;

   for (i=1; i<=Object->BundleCount; ++i)
   {
      BundleNumbers[i] = i;
      BundleCounts[i]  = EdgesInBundle(Object,i);
      if (!BundleCounts[i])  ++Zeros;
   }

   /* bubble sort! */
   for (i=1; i<=Object->BundleCount; ++i)
   {
      for (j=Object->BundleCount; j>=i+1; --j)
      {
         if (BundleCounts[j-1] < BundleCounts[j])
         {
            int c = BundleCounts[j-1];
            int n = BundleNumbers[j-1];
            BundleCounts[j-1] = BundleCounts[j];
            BundleNumbers[j-1] = BundleNumbers[j];
            BundleCounts[j] = c;
            BundleNumbers[j] = n;
         }
      }
   }

   for (i=1; i<=Object->BundleCount; ++i)  Renumbering[BundleNumbers[i]] = i;

   for (i=0; i<Object->Labelling.EdgeCount; ++i)
   {
      BUNDLE B = Object->Edges[i].EdgeBundle;
      if (B)  Object->Edges[i].EdgeBundle = Renumbering[B];
   }

   Object->BundleCount -= Zeros;
}


void CCueLabelsVarley_Lamban::GetBundleAngles (OBJECT *Object)
{
   POINT2Dv P[MAX_EDGES];
   int n;

   for (n=1; n<=Object->BundleCount; ++n)  P[n].x = P[n].y = 0.0;

   for (n=0; n<Object->Labelling.EdgeCount; ++n)
   {
      int   B = Object->Edges[n].EdgeBundle;
      double         L = LineLength(Object,n);
      DEGREES        D;
      RADIANS        R;
      double        dX,dY;
      D = GetLineDirection(Object,n);
      R = (D > 180.0 ? D-180.0 : D)*M_PI/180.0;
      dX = sin(R);
      dY = cos(R);

      if (std::fabs(dX) > std::fabs(dY))  {  if (dX < 0.0)  { dX = -dX;  dY = -dY; } }
      else                      {  if (dY < 0.0)  { dX = -dX;  dY = -dY; } }

      P[B].x += dX*L;
      P[B].y -= dY*L;
   }

   for (n=1; n<=Object->BundleCount; ++n)
   {
      Object->BundleAngles[n] = (180.0/M_PI) * atan2(P[n].x,-P[n].y);
   }
}


lINTEGER CCueLabelsVarley_Lamban::HighestIndex4 (lREAL A, lREAL B, lREAL C, lREAL D)
{
   if ((A >= B) && (A >= C) && (A >= D))  return 0;
   if ((B >= A) && (B >= C) && (B >= D))  return 1;
   if ((C >= A) && (C >= B) && (C >= D))  return 2;
   return 3;
}


void CCueLabelsVarley_Lamban::ChooseIJKAxes (OBJECT *Object, BUNDLE *I, BUNDLE *J, BUNDLE *K)
{
   DEGREES I1 = danglecross(300.0,Object->BundleAngles[1]);
   DEGREES I2 = danglecross(300.0,Object->BundleAngles[2]);
   DEGREES I3 = danglecross(300.0,Object->BundleAngles[3]);
   DEGREES J1 = danglecross( 60.0,Object->BundleAngles[1]);
   DEGREES J2 = danglecross( 60.0,Object->BundleAngles[2]);
   DEGREES J3 = danglecross( 60.0,Object->BundleAngles[3]);
   DEGREES K1 = danglecross(180.0,Object->BundleAngles[1]);
   DEGREES K2 = danglecross(180.0,Object->BundleAngles[2]);
   DEGREES K3 = danglecross(180.0,Object->BundleAngles[3]);
   DEGREES IJK = I1+J2+K3;
   DEGREES IKJ = I1+K2+J3;
   DEGREES JIK = J1+I2+K3;
   DEGREES JKI = J1+K2+I3;
   DEGREES KIJ = K1+I2+J3;
   DEGREES KJI = K1+J2+I3;
   if (IJK < dmin5(IKJ,JIK,JKI,KIJ,KJI))  { *I = 1;  *J = 2;  *K = 3; }
   else
   if (IKJ < dmin4(JIK,JKI,KIJ,KJI))  { *I = 1;  *J = 3;  *K = 2; }
   else
   switch(HighestIndex4(-JIK,-JKI,-KIJ,-KJI))
   {
      case 0:  { *I = 2;  *J = 1;  *K = 3; }  break;
      case 1:  { *I = 3;  *J = 1;  *K = 2; }  break;
      case 2:  { *I = 2;  *J = 3;  *K = 1; }  break;
      case 3:  { *I = 3;  *J = 2;  *K = 1; }  break;
   }
}


void CCueLabelsVarley_Lamban::ChooseIJAxes (OBJECT *Object, BUNDLE *I, BUNDLE *J)
{
   DEGREES I1 = danglecross(300.0,Object->BundleAngles[1]);
   DEGREES I2 = danglecross(300.0,Object->BundleAngles[2]);
   DEGREES I3 = danglecross(300.0,Object->BundleAngles[3]);
   DEGREES J1 = danglecross( 60.0,Object->BundleAngles[1]);
   DEGREES J2 = danglecross( 60.0,Object->BundleAngles[2]);
   DEGREES J3 = danglecross( 60.0,Object->BundleAngles[3]);
   DEGREES K1 = danglecross(180.0,Object->BundleAngles[1]);
   DEGREES K2 = danglecross(180.0,Object->BundleAngles[2]);
   DEGREES K3 = danglecross(180.0,Object->BundleAngles[3]);
   DEGREES IJK = I1+J2+K3;
   DEGREES IKJ = I1+K2+J3;
   DEGREES JIK = J1+I2+K3;
   DEGREES JKI = J1+K2+I3;
   DEGREES KIJ = K1+I2+J3;
   DEGREES KJI = K1+J2+I3;
   if (IJK < dmin5(IKJ,JIK,JKI,KIJ,KJI))  { *I = 1;  *J = 2; }
   else
   if (IKJ < dmin4(JIK,JKI,KIJ,KJI))  { *I = 1;  *J = 3; }
   else
   switch(HighestIndex4(-JIK,-JKI,-KIJ,-KJI))
   {
      case 0:  { *I = 2;  *J = 1; }  break;
      case 1:  { *I = 3;  *J = 1; }  break;
      case 2:  { *I = 2;  *J = 3; }  break;
      case 3:  { *I = 3;  *J = 2; }  break;
   }
}


lREAL CCueLabelsVarley_Lamban::LengthOfBundle (OBJECT *Object, int Bundle)
{
   lREAL L=0;
   for (EDGE_INDEX i=0; i<Object->Labelling.EdgeCount; ++i)  if (Object->Edges[i].EdgeBundle == Bundle)  L += LineLength(Object,i);
   return L;
}


void CCueLabelsVarley_Lamban::SortBundleNumbersByLength (OBJECT *Object)
{
   int BundleNumbers[MAX_BUNDLES];
   lREAL BundleLengths[MAX_BUNDLES];
   int Renumbering[MAX_BUNDLES];
   int Zeros = 0;  // Lamban Labelling can produce zero-sized bundles
   int i,j;

   for (i=1; i<=Object->BundleCount; ++i)
   {
      BundleNumbers[i] = i;
      BundleLengths[i] = LengthOfBundle(Object,i);
      if (!BundleLengths[i])  ++Zeros;
   }

   /* bubble sort! */
   for (i=1; i<=Object->BundleCount; ++i)
   {
      for (j=Object->BundleCount; j>=i+1; --j)
      {
         if (BundleLengths[j-1] < BundleLengths[j])
         {
            lREAL c = BundleLengths[j-1];
            int n = BundleNumbers[j-1];
            BundleLengths[j-1] = BundleLengths[j];
            BundleNumbers[j-1] = BundleNumbers[j];
            BundleLengths[j] = c;
            BundleNumbers[j] = n;
         }
      }
   }

   for (i=1; i<=Object->BundleCount; ++i)  Renumbering[BundleNumbers[i]] = i;

   for (i=0; i<Object->Labelling.EdgeCount; ++i)
   {
      BUNDLE B = Object->Edges[i].EdgeBundle;
      if (B)  Object->Edges[i].EdgeBundle = Renumbering[B];
   }

   Object->BundleCount -= Zeros;
}


DEGREES CCueLabelsVarley_Lamban::OppositeDirection (DEGREES FromDirection)
{
   return (FromDirection >= 180.0) ? FromDirection-180.0 : FromDirection+180.0;
}


DEGREES CCueLabelsVarley_Lamban::NormaliseDirection (DEGREES FromDirection)
{
   while (FromDirection >= 360.0)  FromDirection -= 360.0;
   while (FromDirection < 0.0)  FromDirection += 360.0;
   return FromDirection;
}


BUNDLE CCueLabelsVarley_Lamban::ChooseBisectingBundle (OBJECT *Object, BUNDLE Ibundle, BUNDLE Jbundle)
{
   DEGREES Bisector   = midangle(Object->BundleAngles[Jbundle],Object->BundleAngles[Ibundle]);
   BUNDLE  Chosen     = 0;
   DEGREES Difference = 180.0;

   if (danglecross(Bisector,180.0) > 45.0)  Bisector = anglesum(Bisector,90.0);
   //printf("Bisector is %f\n",Bisector);

   for (BUNDLE B = 1; B <= Object->BundleCount; ++B)  if ((B != Ibundle) && (B != Jbundle))
   {
      DEGREES Bdifference = danglecross(Bisector,Object->BundleAngles[B]);
      //printf("Bdifference for bundle %d angle %f is %f\n",B,Object->BundleAngles[B],Bdifference);
      if (Bdifference < Difference)
      {
         Difference = Bdifference;
         Chosen     = B;
      }
   }

   return Chosen;
}


lBOOLEAN CCueLabelsVarley_Lamban::FaceUsesAllBundles (OBJECT *Object, FACE_INDEX Nface)
{
   int FaceMask = 0;
   int AllMask  = 0;

   for (int n=1; n<=Object->BundleCount; ++n)  AllMask |= (1 << n);

   for (int i = 0; i < Object->Faces[Nface].NumberOfLoops; ++i)
   {
      LOOP *Loop = &Object->Faces[Nface].Loops[i];
      for (int j=0; j < Loop->SideCount; ++j)
      {
         FaceMask |= (1 << Object->Edges[Loop->Sides[j].Number].EdgeBundle);
         if (FaceMask == AllMask)  return cTRUE;
      }
   }

   return cFALSE;
}


lBOOLEAN CCueLabelsVarley_Lamban::LoopUsesBundle (OBJECT *Object, LOOP *Loop, int Bundle)
{
   for (int i=0; i<Loop->SideCount; ++i)  if (!Loop->Sides[i].OccludesFace)
   {
      if (Object->Edges[EdgeOnLoop(Loop,i)].EdgeBundle == Bundle)  return cTRUE;
   }
   return cFALSE;
}


lINTEGER CCueLabelsVarley_Lamban::nuts (lINTEGER NotThis, lINTEGER NorThis, lINTEGER First,
										lINTEGER Second, lINTEGER Third, lINTEGER Default)
{
   if ((NotThis == First) && (NorThis == Second)) return Third;
   if ((NotThis == Second) && (NorThis == First)) return Third;
   if ((NotThis == First) && (NorThis == Third))  return Second;
   if ((NotThis == Third) && (NorThis == First))  return Second;
   if ((NotThis == Second) && (NorThis == Third)) return First;
   if ((NotThis == Third) && (NorThis == Second)) return First;
   return Default;
}


EDGE_INDEX CCueLabelsVarley_Lamban::EdgeLeavingLoopAtVertex (OBJECT *Object, LOOP *Loop, VERTEX_INDEX Vertex)
{
   if (Loop && (Vertex >= 0) && (Vertex < Object->Labelling.VertexCount))
   {
      int Ecount = Object->Labelling.VECount[Vertex];

      for (int n=0; n<Ecount; ++n)
      {
         EDGE_INDEX E = Object->Labelling.VENumber[Vertex][n];
         if (EdgeLeavesLoop(Object,Loop,E))  return E;
      }
   }

   return NO_EDGE;
}


lBOOLEAN CCueLabelsVarley_Lamban::IdentifyLambanAxes (OBJECT *Object)
{
   if (LP->LambanAxisOptions != LambanAxesFixedIJK)
   {
      InitialiseParallelKnowledge(Object);
      CreateParallelLineGroups(Object);

      switch(LP->LambanAxisOptions)
      {
         case LambanAxesFrequentIJK:
         case LambanAxesFrequentIJOptK:
            SortBundleNumbers(Object);  // Sort Bundles by Frequency
            GetBundleAngles(Object);
            ChooseIJKAxes(Object,&Ibundle,&Jbundle,&Kbundle);
         break;

         case LambanAxesFrequentIJMiddleK:
            SortBundleNumbers(Object);  // Sort Bundles by Frequency
            GetBundleAngles(Object);
            ChooseIJAxes(Object,&Ibundle,&Jbundle);
         break;

         case LambanAxesLongestIJK:
            SortBundleNumbersByLength(Object);
            GetBundleAngles(Object);
            ChooseIJKAxes(Object,&Ibundle,&Jbundle,&Kbundle);
         break;

         case LambanAxesLongestIJFixedK:
            SortBundleNumbersByLength(Object);
            GetBundleAngles(Object);
            ChooseIJAxes(Object,&Ibundle,&Jbundle);
         break;
      }

      plusI = minusI = Object->BundleAngles[Ibundle];
      if (dangleclockMaths(TEN_O_CLOCK,plusI) > 90.0)  plusI = OppositeDirection(plusI);  else minusI = OppositeDirection(minusI);

      plusJ = minusJ = Object->BundleAngles[Jbundle];
      if (dangleclockMaths(TWO_O_CLOCK,plusJ) > 90.0)  plusJ = OppositeDirection(plusJ);  else minusJ = OppositeDirection(minusJ);

      if ((LP->LambanAxisOptions & LambanAxesWhatK) == LambanAxesFreeK)
      {
         plusK = minusK = Object->BundleAngles[Kbundle];
         if (dangleclockMaths(SIX_O_CLOCK,plusK) > 90.0)  plusK = OppositeDirection(plusK);  else minusK = OppositeDirection(minusK);
      }
      else
      if ((LP->LambanAxisOptions & LambanAxesWhatK) == LambanAxesOptK)
      {
         plusK = NormaliseDirection(Object->BundleAngles[Kbundle]);
         if (dangleclockMaths(SIX_O_CLOCK,plusK) > 90.0)  plusK = NormaliseDirection(OppositeDirection(plusK));
         plusK = (1.0-tcOk)*180.0 + tcOk*plusK;
         minusK = OppositeDirection(plusK);
         //printf("PlusK %f, MinusK %f\n",plusK,minusK);
      }
      else
      if ((LP->LambanAxisOptions & LambanAxesWhatK) == LambanAxesMiddleK)
      {
         Kbundle = ChooseBisectingBundle(Object,Ibundle,Jbundle);
         plusK = NormaliseDirection(Object->BundleAngles[Kbundle]);
         if (dangleclockMaths(SIX_O_CLOCK,plusK) > 90.0)  plusK = NormaliseDirection(OppositeDirection(plusK));
         plusK = (1.0-tcOk)*180.0 + tcOk*plusK;
         minusK = OppositeDirection(plusK);
         //printf("MiddleK PlusK %f, MinusK %f\n",plusK,minusK);
      }
   }
   //printf("PlusI = %f, PlusJ = %f, PlusK = %f\n", plusI, plusJ, plusK);

   if (UseDeductionsInLamban)
   {
      // Any face containing lines in all bundles is not a full face
      for (FACE_INDEX n=0; n<Object->FaceCount; ++n)  FullFace[n] = !FaceUsesAllBundles(Object,n);

      // TBD: we ought to be able to do some reasoning from cofacial configurations

      for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)
      {
         if (Object->Labelling.VertexLabel[n].Shape == JunctionIsT4)
         {
            // A face including the tail of a T-junction is pretty certainly
            // not a full face
            EDGE_INDEX E = Object->Labelling.VENumber[n][2];
            FACE_INDEX A = Object->Labelling.EdgeForwardFace[E];
            FACE_INDEX B = Object->Labelling.EdgeReverseFace[E];
            //printf ("T4 %d %d\n", A, B);
            if (A >= 0)  FullFace[A] = cFALSE;
            if (B >= 0)  FullFace[B] = cFALSE;
         }
      }

      for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)
      {
         if (Object->Labelling.VertexLabel[n].Shape == JunctionIsL)
         {
            // An L-junction always occludes something: any L-junction with two
            // faces (rather than face and background) occludes one or other of
            // the faces, so unless we have any prior knowledge, to be safe,
            // assume neither is a full face
            FACE_INDEX A = Object->Vertices[n].Findex[0];
            FACE_INDEX B = Object->Vertices[n].Findex[1];
            if ((A >= 0) && (B >= 0))
            {
               //printf ("L %d %d\n", A, B);

               //If we already know, from prior reasoning, that one
               //or other face is not a full face, we might assume
               //that this L-junction has already been dealt with

               //On the other hand, if we don't, there's no way of
               //telling, so assume that neither is a full face
               if (FullFace[A] && FullFace[B])
               {
                  FullFace[A] = cFALSE;
                  FullFace[B] = cFALSE;
               }
            }
         }
      }

      for (FACE_INDEX n=0; n<Object->FaceCount; ++n)  if (FullFace[n])
      {
         LOOP *Loop = &Object->Faces[n].Loops[0];
         lBOOLEAN BI = LoopUsesBundle(Object,Loop,Ibundle);
         lBOOLEAN BJ = LoopUsesBundle(Object,Loop,Jbundle);
         lBOOLEAN BK = LoopUsesBundle(Object,Loop,Kbundle);

         if (BI && BJ)
         {
            //printf("Face %d uses BI and BJ, so normal is in K-axis\n",n);
            Object->Faces[n].NormalBundle = Kbundle;
            FaceBundleMerit[n] = 1.0;
         }
         else
         if (BI && BK)
         {
            //printf("Face %d uses BI and BK, so normal is in J-axis\n",n);
            Object->Faces[n].NormalBundle = Jbundle;
            FaceBundleMerit[n] = 1.0;
         }
         else
         if (BJ && BK)
         {
            //printf("Face %d uses BJ and BK, so normal is in I-axis\n",n);
            Object->Faces[n].NormalBundle = Ibundle;
            FaceBundleMerit[n] = 1.0;
         }
      }

      for (FACE_INDEX n=0; n<Object->FaceCount; ++n)  if (!Object->Faces[n].NormalBundle)
      {
         double Iweight,Jweight,Kweight;
         int p,q;
         LOOP *Loop = &Object->Faces[n].Loops[0];

         FullFace[n] = cFALSE;
         Iweight = Jweight = Kweight = 0.0;

         for (p=0; p<Loop->SideCount; ++p)  for (q=p+1; q<Loop->SideCount; ++q)
         {
            BUNDLE B = (BUNDLE)nuts(Object->Edges[Loop->Sides[p].Number].EdgeBundle,Object->Edges[Loop->Sides[q].Number].EdgeBundle,Ibundle,Jbundle,Kbundle,0);
            if (B == Ibundle)  Iweight += 1.0;
            else
            if (B == Jbundle)  Jweight += 1.0;
            else
            if (B == Kbundle)  Kweight += 1.0;
         }

         for (p=0; p<Loop->CornerCount; ++p)
         {
            VERTEX_INDEX V = Loop->Corners[n].Number;
            VERTEX_LABEL Shape = Object->Labelling.VertexLabel[V].Shape;

            if ((Shape == JunctionIsW) || (Shape == JunctionIsY))
            {
               BUNDLE B = Object->Edges[EdgeLeavingLoopAtVertex(Object,Loop,V)].EdgeBundle;
               if (B == Ibundle)  Iweight += 1.0;
               else
               if (B == Jbundle)  Jweight += 1.0;
               else
               if (B == Kbundle)  Kweight += 1.0;
            }
         }

         //printf (" ... weights I=%f, J=%f, K=%f\n", Iweight, Jweight, Kweight);

         double Sum = dmax2(Iweight+Jweight+Kweight,1);
         Iweight /= Sum;
         Jweight /= Sum;
         Kweight /= Sum;

         // If the face shares an edge with a face which is definitely
         // normal to a bundle, this face can't be normal to the same bundle
         for (p=0; p<Loop->SideCount; ++p)
         {
            FACE_INDEX Fo = EdgeOtherFace(Object,Loop->Sides[p].Number,n);
            if ((Fo >= 0) && FullFace[Fo])
            {
               if (Object->Faces[Fo].NormalBundle == Ibundle)  Iweight = 0.0;
               else
               if (Object->Faces[Fo].NormalBundle == Jbundle)  Jweight = 0.0;
               else
               if (Object->Faces[Fo].NormalBundle == Kbundle)  Kweight = 0.0;
            }
         }

         switch (HighestIndex3(Iweight,Jweight,Kweight))
         {
            case 0:
               Object->Faces[n].NormalBundle = Ibundle;
               FaceBundleMerit[n] = Iweight-dmax2(Jweight,Kweight);
            break;

            case 1:
               Object->Faces[n].NormalBundle = Jbundle;
               FaceBundleMerit[n] = Jweight-dmax2(Iweight,Kweight);
            break;

            case 2:
               Object->Faces[n].NormalBundle = Kbundle;
               FaceBundleMerit[n] = Kweight-dmax2(Iweight,Jweight);
            break;
         }
      }
      //for (int n=0; n<Object->FaceCount; ++n)  printf ("Face %d normal bundle %d merit %f\n", n, Object->Faces[n]->NormalBundle, FaceBundleMerit[n]);
   }

   return cTRUE;
}


lINTEGER CCueLabelsVarley_Lamban::CountAndRecordTJunctions (LABELLING *Labelling)
{
   int Count = 0;

   for (VERTEX_INDEX v=0; v<Labelling->VertexCount; ++v)
   {
      Ttranslation[v] = (Labelling->VertexLabel[v].Shape == JunctionIsT4) ? Count++ : NO_VERTEX;
   }
   return Count;
}


int CCueLabelsVarley_Lamban::CountCofacialPairs (OBJECT *Object, LABELLING *Labelling)
{
   int Count = 0;

   for (FACE_INDEX F=0; F<Object->FaceCount; ++F)  if (FaceBundleMerit[F] > FoMIndeterminate)
   {
      lBOOLEAN Use = cFALSE;
      if (Object->Faces[F].NormalBundle == Ibundle)  Use = cTRUE;
      else
      if (Object->Faces[F].NormalBundle == Jbundle)  Use = cTRUE;
      else
      if (Object->Faces[F].NormalBundle == Kbundle)  Use = cTRUE;

      if (Use)
      {
         for (int L=0; L<Object->Faces[F].NumberOfLoops; ++L)
         {
            int q = Object->Faces[F].Loops[L].CornerCount;
            Count += q*(q+1)/2;
         }
      }
   }

   return Count;
}


int CCueLabelsVarley_Lamban::CountCollinearLines (OBJECT *Object, LABELLING *Labelling)
{
   CollinearLineCount = 0;
   for (EDGE_INDEX e=0; e<Labelling->EdgeCount; ++e)
   {
      for (EDGE_INDEX f=e+1; f<Labelling->EdgeCount; ++f)
      {
         if ((CollinearLineMerit[CollinearLineCount] = FoMLinesCollinear(Object,e,f)) > 0.5)
         {
            //printf("Lines %d and %d might be collinear, merit %f\n",e,f,CollinearLineMerit[CollinearLineCount]);
            CollinearLineE[CollinearLineCount] = e;
            CollinearLineF[CollinearLineCount] = f;
            ++CollinearLineCount;
            if (CollinearLineCount >= MAX_COLLINEAR)  return MAX_COLLINEAR;
         }
      }
   }
   return CollinearLineCount;
}

MERIT CCueLabelsVarley_Lamban::FoMDistanceZero (double D)
{
   // in principle, this can't be greater than 1 unless the distance is negative,
   // but just to be sure ...
   return dmin2(pow((1.0+tcMd),-D),1.0);
}


MERIT CCueLabelsVarley_Lamban::FoMLinesCollinear (OBJECT *Object, EDGE_INDEX Eab, EDGE_INDEX Ecd)
{
   VERTEX_INDEX Va = Object->Labelling.EdgeStartVertex[Eab];
   VERTEX_INDEX Vb = Object->Labelling.EdgeEndVertex[Eab];
   VERTEX_INDEX Vc = Object->Labelling.EdgeStartVertex[Ecd];
   VERTEX_INDEX Vd = Object->Labelling.EdgeEndVertex[Ecd];
   POINT2Dv Pa,Pb,Pc,Pd;
   Pa.x = Object->Coordinates[Va].x;
   Pa.y = Object->Coordinates[Va].y;
   Pb.x = Object->Coordinates[Vb].x;
   Pb.y = Object->Coordinates[Vb].y;
   Pc.x = Object->Coordinates[Vc].x;
   Pc.y = Object->Coordinates[Vc].y;
   Pd.x = Object->Coordinates[Vd].x;
   Pd.y = Object->Coordinates[Vd].y;
   MERIT Mll = FoMLinesParallel(Object,Eab,Ecd);
   MERIT Ma  = FoMDistanceZero(DistanceFromPoint2ToLine2(&Object->Edges[Ecd].LineEquation,&Pa));
   MERIT Mb  = FoMDistanceZero(DistanceFromPoint2ToLine2(&Object->Edges[Ecd].LineEquation,&Pb));
   MERIT Mc  = FoMDistanceZero(DistanceFromPoint2ToLine2(&Object->Edges[Eab].LineEquation,&Pc));
   MERIT Md  = FoMDistanceZero(DistanceFromPoint2ToLine2(&Object->Edges[Eab].LineEquation,&Pd));
   return Mll*Ma*Mb*Mc*Md;
}


OrthoStruct *CCueLabelsVarley_Lamban::InitialiseEquations (lINTEGER MaxEquations, lINTEGER Variables)
{
   lINTEGER n;

   if (MaxEquations < 1)
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: Something wrong here: MaxEquations = ";
      sMensajeAux.Format("%d", MaxEquations);
      sMensaje+=sMensajeAux;
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }

   //printf ("Initialise Equations, %d equations, %d unknowns\n", MaxEquations, Variables);

   OrthoStruct *Set = (OrthoStruct *)malloc(sizeof(OrthoStruct));
   /*
   if (!Set)
   {
      AfxMessageBox("Can't allocate memory for equation set");
      //exit(BADNEWS);
   }
   */
   Set->CurrentEquationCount = 0;
   Set->MaximumEquations     = MaxEquations;
   Set->NumberOfUnknowns     = Variables;

   Set->EquationCoefficients = (lREAL *)malloc(sizeof(lREAL)*MaxEquations*Variables);
   /*
   if (!Set->EquationCoefficients)
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: Can't allocate memory for ";
      sMensajeAux.Format("%d", MaxEquations*Variables);
      sMensaje+=sMensajeAux;
      sMensaje+=" equation coefficients";
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }
   */
#ifdef DEBUG_EQUATIONS
   printf ("... equation coefficients located at %x\n", (lINTEGER)Set->EquationCoefficients);
#endif

   Set->EquationValues = (lREAL *)malloc(sizeof(lREAL)*MaxEquations);

   /*
   if (!Set->EquationValues)
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: Can't allocate memory for ";
      sMensajeAux.Format("%d", MaxEquations);
      sMensaje+=sMensajeAux;
      sMensaje+=" equation values";
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }
   */

   Set->UnknownValues  = NULL;

   Set->CoefficientSet = (lBOOLEAN *)malloc(sizeof(lBOOLEAN)*Variables);
   /*
   if (!Set->CoefficientSet)
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: Can't allocate memory for ";
      sMensajeAux.Format("%d", Variables);
      sMensaje+=sMensajeAux;
      sMensaje+=" variable flags";
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }
   */

   for (n=0; n<Variables; ++n)  Set->CoefficientSet[n] = cFALSE;

   return Set;
}


void CCueLabelsVarley_Lamban::GetPerkinsDepthRatios (DEGREES A0, DEGREES A1, DEGREES A2, lREAL *R0, lREAL *R1, lREAL *R2)
{
   lREAL tanABtanAC;
   lREAL tanA01 = tan(danglecwiseMaths(A0,A1) * M_PI/180.0);
   lREAL tanA12 = tan(danglecwiseMaths(A1,A2) * M_PI/180.0);
   lREAL tanA20 = tan(danglecwiseMaths(A2,A0) * M_PI/180.0);

   tanABtanAC = tanA01*tanA20;
   *R0 = (tanABtanAC > 1.0) ? dmin2(100.0,1.0/sqrt(tanABtanAC-1)) : 0.0;

   tanABtanAC = tanA12*tanA01;
   *R1 = (tanABtanAC > 1.0) ? dmin2(100.0,1.0/sqrt(tanABtanAC-1)) : 0.0;

   tanABtanAC = tanA20*tanA12;
   *R2 = (tanABtanAC > 1.0) ? dmin2(100.0,1.0/sqrt(tanABtanAC-1)) : 0.0;
}


lINTEGER CCueLabelsVarley_Lamban::NextEquation (OrthoStruct *Set)
{
   lINTEGER E,n;
   E = Set->CurrentEquationCount++;
   if (E >= Set->MaximumEquations)
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: Maximum Number of Equations Exceeded!! (only space for ";
      sMensajeAux.Format("%d", Set->MaximumEquations);
      sMensaje+=sMensajeAux;
      sMensaje+=")";
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }
   for (n=0; n<Set->NumberOfUnknowns; ++n)  Set->EquationCoefficients[E*Set->NumberOfUnknowns+n] = 0.0;   Set->EquationValues[E] = 0.0;
   //printf ("... next equation from set is %d\n", E);
   return E;
}


void CCueLabelsVarley_Lamban::SetCoefficient (OrthoStruct *Set, lINTEGER Equation, lINTEGER Variable, lREAL Coefficient)
{
   if (Equation >= Set->MaximumEquations)
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: SetCoefficient equation ";
      sMensajeAux.Format("%d", Equation);
      sMensaje+=sMensajeAux;
      sMensaje+="out of range 0..";
      sMensajeAux.Format("%d", Set->MaximumEquations-1);
      sMensaje+=sMensajeAux;
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }

   if (Variable >= Set->NumberOfUnknowns)
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: SetCoefficient variable ";
      sMensajeAux.Format("%d", Variable);
      sMensaje+=sMensajeAux;
      sMensaje+="out of range 0..";
      sMensajeAux.Format("%d", Set->NumberOfUnknowns-1);
      sMensaje+=sMensajeAux;
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }

#ifdef DEBUG_EQUATIONS
   printf ("... setting coefficient %d of equation %d to %f\n", Variable, Equation, Coefficient);
   printf ("... equation coefficients located at %x\n", (lINTEGER)Set->EquationCoefficients);
   printf ("... index is %d\n", (Equation*Set->NumberOfUnknowns)+Variable);
#endif
   Set->EquationCoefficients[(Equation*Set->NumberOfUnknowns)+Variable] += Coefficient;
   Set->CoefficientSet[Variable] = cTRUE;
}


void CCueLabelsVarley_Lamban::SetRHSValue (OrthoStruct *Set, lINTEGER Equation, lREAL Coefficient)
{
   if (Equation >= Set->MaximumEquations)
   {
      CString sMensaje, sMensajeAux;
      sMensaje= "Labelling error: SetRHSValue equation ";
      sMensajeAux.Format("%d", Equation);
      sMensaje+=sMensajeAux;
      sMensaje+="out of range 0..";
      sMensajeAux.Format("%d", Set->MaximumEquations-1);
      sMensaje+=sMensajeAux;
      AfxMessageBox(sMensaje);
      //exit(BADNEWS);
   }

   //printf ("... setting RHS of equation %d to %f\n", Equation, Coefficient);
   Set->EquationValues[Equation] = Coefficient;
}



lBOOLEAN CCueLabelsVarley_Lamban::SplitLengthIntoComponents (lREAL L, DEGREES Langle, lREAL *A, DEGREES Aangle, lREAL *B, DEGREES Bangle)
// Assuming my geometry is correct:                                                 */
// This splits a length L along direction Langle into two lengths A and B along     */
// directions Aangle and Bangle such that the vector sum of the two components is   */
// the original length.                                                             */
{
   lREAL divisor = sin((Aangle-Bangle)*M_PI/180.0);
   if (std::fabs(divisor) < 0.000001)  return cFALSE;

   *A = L*sin((Bangle-Langle)*M_PI/180.0)/(-divisor);
   *B = L*sin((Aangle-Langle)*M_PI/180.0)/divisor;
   return cTRUE;
}


int CCueLabelsVarley_Lamban::VariableIndex (LABELLING *Labelling, VERTEX_INDEX v, EDGE_INDEX e)
{
   if (Labelling->VertexLabel[v].Shape == JunctionIsT4)
   {
      if (e != Labelling->VENumber[v][2])  return Ttranslation[v]+Labelling->VertexCount;
   }
   return v;
}


lREAL CCueLabelsVarley_Lamban::Squarea (POINT2Dv *A, POINT2Dv *B, POINT2Dv *C)
{
   return ScalarCross(A->x,B->x,C->x,A->y,B->y,C->y);
}


void CCueLabelsVarley_Lamban::AddDisjointSubgraphEquations (OBJECT *Object, LABELLING *Labelling, OrthoStruct *IEquations, OrthoStruct *JEquations, OrthoStruct *KEquations)
{
   lBOOLEAN ThisVertexIsFixed[MAX_VERTICES];

   for (VERTEX_INDEX V=0; V<Labelling->VertexCount; ++V)  ThisVertexIsFixed[V] = cFALSE;

   for (int i=0;  i<Object->CofacialCount;  ++i)
   {
      COFACIAL_CONFIGURATION *CC = &Object->CofacialConfigurations[i];
      VERTEX_INDEX A = CC->InnerVertex;

	  if ((A >= 0) && (A < Labelling->VertexCount) && !ThisVertexIsFixed[A])
	  {
	     VERTEX_INDEX B = CC->OuterVertex;
         if ((B >= 0) && (B < Labelling->VertexCount))
         {
	        VERTEX_INDEX C = CC->OuterNextInLoop;
            if ((C >= 0) && (C < Labelling->VertexCount))
            {
	           VERTEX_INDEX D = CC->OuterPrevInLoop;
	           POINT2Dv P,R,S,T;
	           double  Z;

               if ((D >= 0) && (D < Labelling->VertexCount))
               {
   	              //if (DebugUFG)  fprintf(DebugUFG,"Joining Disjoint Subgraphs, A=%d, B=%d, C=%d, D=%d\n", A, B, C, D);
	              /* work out a and b from the x and y coordinates of the vertices */
	              P.x = Object->Coordinates[B].x;
	              P.y = Object->Coordinates[B].y;
	              R.x = Object->Coordinates[C].x;
	              R.y = Object->Coordinates[C].y;
	              S.x = Object->Coordinates[D].x;
	              S.y = Object->Coordinates[D].y;
	              T.x = Object->Coordinates[A].x;
	              T.y = Object->Coordinates[A].y;
	              Z = Squarea(&P,&R,&S);
	              CC->Xcoeff = Squarea(&P,&T,&S)/Z;
	              CC->Ycoeff = Squarea(&P,&R,&T)/Z;

	              //printf ("CoCo, A=%d, B=%d, C=%d, D=%d\n", A, B, C, D);

	              lINTEGER Eqn = NextEquation(IEquations);
	              SetCoefficient(IEquations,Eqn,A,1.0);
	              SetCoefficient(IEquations,Eqn,B,CC->Xcoeff+CC->Ycoeff-1.0);
	              SetCoefficient(IEquations,Eqn,C,-CC->Xcoeff);
	              SetCoefficient(IEquations,Eqn,D,-CC->Ycoeff);
	              SetRHSValue(IEquations,Eqn,0);

	              Eqn = NextEquation(JEquations);
	              SetCoefficient(JEquations,Eqn,A,1.0);
	              SetCoefficient(JEquations,Eqn,B,CC->Xcoeff+CC->Ycoeff-1.0);
	              SetCoefficient(JEquations,Eqn,C,-CC->Xcoeff);
	              SetCoefficient(JEquations,Eqn,D,-CC->Ycoeff);
	              SetRHSValue(JEquations,Eqn,0);

	              Eqn = NextEquation(KEquations);
	              SetCoefficient(KEquations,Eqn,A,1.0);
	              SetCoefficient(KEquations,Eqn,B,CC->Xcoeff+CC->Ycoeff-1.0);
	              SetCoefficient(KEquations,Eqn,C,-CC->Xcoeff);
	              SetCoefficient(KEquations,Eqn,D,-CC->Ycoeff);
	              SetRHSValue(KEquations,Eqn,0);

                  ThisVertexIsFixed[A] = cTRUE;
               }
            }
         }
      }
   }

   for (int c=0; c<CollinearLineCount; ++c)
   {
      EDGE_INDEX E = CollinearLineE[c];
      EDGE_INDEX F = CollinearLineF[c];
      MERIT M = CollinearLineMerit[c];
      VERTEX_INDEX Es = Labelling->EdgeStartVertex[E];
      VERTEX_INDEX Et = Labelling->EdgeEndVertex[E];
      VERTEX_INDEX Fs = Labelling->EdgeStartVertex[F];
      VERTEX_INDEX Ft = Labelling->EdgeEndVertex[F];
      lREAL Le = DistanceFromJunctionToJunction(Object,Es,Et);
      lREAL Lf = DistanceFromJunctionToJunction(Object,Fs,Ft);
      if (dangleclockMaths(DirectionFromJunctionToJunction(Object,Es,Et),DirectionFromJunctionToJunction(Object,Fs,Ft)) > 90.0)
      {
         Ft = Labelling->EdgeStartVertex[F];
         Fs = Labelling->EdgeEndVertex[F];
      }

      lINTEGER Eqn = NextEquation(IEquations);
      SetCoefficient(IEquations,Eqn,Es,M*Lf);
      SetCoefficient(IEquations,Eqn,Et,-M*Lf);
      SetCoefficient(IEquations,Eqn,Fs,-M*Le);
      SetCoefficient(IEquations,Eqn,Ft,M*Le);
      SetRHSValue(IEquations,Eqn,0.0);

      Eqn = NextEquation(JEquations);
      SetCoefficient(JEquations,Eqn,Es,M*Lf);
      SetCoefficient(JEquations,Eqn,Et,-M*Lf);
      SetCoefficient(JEquations,Eqn,Fs,-M*Le);
      SetCoefficient(JEquations,Eqn,Ft,M*Le);
      SetRHSValue(JEquations,Eqn,0.0);

      Eqn = NextEquation(KEquations);
      SetCoefficient(KEquations,Eqn,Es,M*Lf);
      SetCoefficient(KEquations,Eqn,Et,-M*Lf);
      SetCoefficient(KEquations,Eqn,Fs,-M*Le);
      SetCoefficient(KEquations,Eqn,Ft,M*Le);
      SetRHSValue(KEquations,Eqn,0.0);
   }
}


lREAL CCueLabelsVarley_Lamban::RowProduct (lREAL Ra[], lREAL Rb[], lINTEGER RowSize)
{
   lREAL S = 0.0;
   for (lINTEGER J=0; J<RowSize; ++J)  S += Ra[J] * Rb[J];
   return S;
}


lREAL CCueLabelsVarley_Lamban::ColumnProduct (lREAL Ma[], lREAL Rb[], lINTEGER ColumnSize, lINTEGER RowSize)
{
   lREAL S = 0.0;
   for (lINTEGER J=0; J<ColumnSize; ++J)  S += Ma[J*RowSize] * Rb[J];
   return S;
}


lBOOLEAN CCueLabelsVarley_Lamban::Ortholin2 (lREAL A[], lINTEGER Nrows, lINTEGER Ncols, lREAL B[], lREAL EPS, lREAL X[], lINTEGER MaxIterations)
{
   lINTEGER ColIndex;
   lREAL *RowVector,*ColVector,*Uspace,*Triangle;

   RowVector = (lREAL *)malloc(Nrows * sizeof(lREAL));
   ColVector = (lREAL *)malloc(Ncols * sizeof(lREAL));
   Uspace    = (lREAL *)malloc(Nrows * Ncols * sizeof(lREAL));
   Triangle  = (lREAL *)malloc((Ncols*(Ncols+1)/2) * sizeof(lREAL));

   for (ColIndex=0; ColIndex<Nrows*Ncols; ColIndex++)  Uspace[ColIndex] = A[ColIndex];

    lINTEGER L = 0;

   for (ColIndex=0; ColIndex<Ncols; ColIndex++)
   {
      lREAL S = 0.0;

      for (lINTEGER RowIndex=0; RowIndex<Nrows; RowIndex++)
      {
         lREAL T = RowVector[RowIndex] = Uspace[RowIndex*Ncols+ColIndex];
         S += T*T;
      }
      Triangle[L++] = S;

      X[ColIndex] = RowProduct(RowVector,B,Nrows);
      for (lINTEGER H=ColIndex+1; H<Ncols; ++H)
      {
         lREAL T = (Triangle[L++] = ColumnProduct(&Uspace[H],RowVector,Nrows,Ncols)) / S;
         for (lINTEGER r=0; r<Nrows; ++r)  Uspace[r*Ncols+H] -= RowVector[r]*T;
      }
   }

   lINTEGER MaxL = L;

   /* backsubstitution */
   for (ColIndex=Ncols-1; ColIndex>=0; ColIndex--)
   {
      X[ColIndex] = (X[ColIndex] - RowProduct(&Triangle[L],&X[ColIndex+1],Ncols-ColIndex-1))/Triangle[L-1];
      L -= (Ncols+1)-ColIndex;
   } /* backsubstitition */

   /* iterative improvement on the solution */
   lREAL S0 = 0;

   lREAL EPS2 = EPS*EPS;

   for (lINTEGER Iteration=0; Iteration<MaxIterations; ++Iteration)  /* iteration */
   {
      lREAL S1 = 0;
      lREAL S2 = 0;
      L = MaxL;

      /* residual vector */
      for (lINTEGER RowIndex=0; RowIndex<Nrows; RowIndex++)  RowVector[RowIndex] = B[RowIndex]-RowProduct(&A[RowIndex*Ncols],X,Ncols);
      /* vector on the right side */
      for (lINTEGER c=0; c<Ncols; c++)  ColVector[c] = ColumnProduct(&Uspace[c],RowVector,Nrows,Ncols);

      /* backsubstitution */
      for (lINTEGER ColIndex=Ncols-1; ColIndex>=0; ColIndex--)
      {
         lREAL T = X[ColIndex];
         lREAL S = ColVector[ColIndex] = (ColVector[ColIndex] - RowProduct(&Triangle[L],&ColVector[ColIndex+1],Ncols-(ColIndex+1)))/Triangle[L-1];
         S1 += S*S;
         S2 += T*T;
         X[ColIndex] += S;
         L -= (Ncols+1)-ColIndex;
      } /* backsubstitution */

      if (S1 >= S2/4)  return cFALSE;
      if ((S1 >= S2*EPS2) || ((S0 == 0) && (S1 <= S0 * 0.01)))  S0 = S1;  else break;
   } /* iterative improvement */

   free(RowVector);
   free(ColVector);
   free(Uspace);
   free(Triangle);

   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_Lamban::SolveEquations (OrthoStruct *Set)
{
   //printf ("Solve %d equations for %d variables\n",Set->CurrentEquationCount,Set->NumberOfUnknowns);

   if (Set->CurrentEquationCount < Set->NumberOfUnknowns)
   {
      //printf ("Insufficient equations to solve for variables\n");
      return cFALSE;
   }

   for (lINTEGER n=0; n<Set->NumberOfUnknowns; ++n)  if (!Set->CoefficientSet[n])
   {
      //printf ("No equation contains coefficient %d\n", n);
      return cFALSE;
   }

   Set->UnknownValues = (lREAL *)malloc(sizeof(lREAL)*Set->NumberOfUnknowns);

   /* set the maximum number of improvement iterations to twice the number of unknowns - that should be plenty */
   Ortholin2 (Set->EquationCoefficients,Set->CurrentEquationCount,Set->NumberOfUnknowns,Set->EquationValues,0.0001,Set->UnknownValues,Set->NumberOfUnknowns*2);

   for (lINTEGER n=0; n<Set->NumberOfUnknowns; ++n)
   {
      if (!isfinite(Set->UnknownValues[n]))
      {
         //printf ("Result for variable %d is not finite\n", n);
         return cFALSE;
      }
   }

   return cTRUE;
}


void CCueLabelsVarley_Lamban::DeleteEquations (OrthoStruct *Set)
{
   if (Set->EquationCoefficients)  free(Set->EquationCoefficients);
   if (Set->EquationValues)  free(Set->EquationValues);
   if (Set->UnknownValues)   free(Set->UnknownValues);
   if (Set->CoefficientSet)  free(Set->CoefficientSet);
   free(Set);
}


lREAL CCueLabelsVarley_Lamban::VariableValue (OrthoStruct *Set, lINTEGER VariableNumber)
{
   return Set->UnknownValues[VariableNumber];
}


int CCueLabelsVarley_Lamban::Tvertex (LABELLING *Labelling, int n)
{
   for (int v=0; v<Labelling->VertexCount; ++v)
   {
      if (Ttranslation[v]+Labelling->VertexCount == n)  return v;
   }
   return -1;
}


lBOOLEAN CCueLabelsVarley_Lamban::SolveLambanEquations (OBJECT *Object)
{
   int n;
   lREAL devOy = 3.2*tcOy;
   lREAL devOz = 3.2*tcOz;
   //printf("Oy=%f, Oz=%f\n",tcOy,tcOz);

   lINTEGER TCount = CountAndRecordTJunctions(&Object->Labelling);

   lINTEGER VariableCount = Object->Labelling.VertexCount + TCount;

   /* There are three sets of equations, I, J and K */
   /* Each line contributes one equation to each set */
   /* There is an extra equation in each set to fix a reference vertex */
   /* There is one variable in each set for each vertex */

   lINTEGER EquationCount = Object->Labelling.EdgeCount+TCount+1;

   /* Additionally, if there are multiple subgraphs, we need extra equations */
   /* to tie them together: use the cofacial configurations for these        */

   if (Object->SubgraphCount > 1)
   {
      EquationCount += Object->CofacialCount;
      EquationCount += CountCollinearLines(Object,&Object->Labelling);
   }

   /* Additionally, if we are using deductive reasoning, we need lots and */
   /* lots of extra equations to allow for all this reasoning             */

   if (UseDeductionsInLamban)
   {
      EquationCount += CountCofacialPairs(Object,&Object->Labelling);
   }

   OrthoStruct *IEquations = InitialiseEquations(EquationCount,VariableCount);
   OrthoStruct *JEquations = InitialiseEquations(EquationCount,VariableCount);
   OrthoStruct *KEquations = InitialiseEquations(EquationCount,VariableCount);

   Iscaling = Jscaling = Kscaling = 1.0;

   if (LambanAxisOptions != LambanAxesFixedIJK)
   {
      lREAL Icubic,Jcubic,Kcubic,divisor;
      GetPerkinsDepthRatios(plusI,plusJ,plusK,&Icubic,&Jcubic,&Kcubic);
      divisor = (Icubic+Jcubic+Kcubic)/3.0;  // should probably be geometric mean instead
      if (divisor > 0.0)
      {
         Iscaling = ((Icubic/divisor)*tcOe)+(1.0-tcOe);
         Jscaling = ((Jcubic/divisor)*tcOe)+(1.0-tcOe);
         Kscaling = ((Kcubic/divisor)*tcOe)+(1.0-tcOe);
      }
   }

   /* Fix the I, J and K coordinates for a reference vertex */
   lINTEGER Eqn = NextEquation(IEquations);
   SetCoefficient(IEquations,Eqn,0,1.0);
   SetRHSValue(IEquations,Eqn,0.0);

   Eqn = NextEquation(JEquations);
   SetCoefficient(JEquations,Eqn,0,1.0);
   SetRHSValue(JEquations,Eqn,0.0);

   Eqn = NextEquation(KEquations);
   SetCoefficient(KEquations,Eqn,0,1.0);
   SetRHSValue(KEquations,Eqn,0.0);

   // To ensure that the equations solve, add a very-low-weight equation
   // for each T-junction putting its proper coordinates and its occluding
   // coordinates in the same place.

   for (n=0; n<Object->Labelling.VertexCount; ++n)  if (Ttranslation[n] >= 0)
   {
       int t = Ttranslation[n]+Object->Labelling.VertexCount;

      Eqn = NextEquation(IEquations);
      SetCoefficient(IEquations,Eqn,n,0.001);
      SetCoefficient(IEquations,Eqn,t,-0.001);
      SetRHSValue(IEquations,Eqn,0.0);

      Eqn = NextEquation(JEquations);
      SetCoefficient(JEquations,Eqn,n,0.001);
      SetCoefficient(JEquations,Eqn,t,-0.001);
      SetRHSValue(JEquations,Eqn,0.0);

      Eqn = NextEquation(KEquations);
      SetCoefficient(KEquations,Eqn,n,0.001);
      SetCoefficient(KEquations,Eqn,t,-0.001);
      SetRHSValue(KEquations,Eqn,0.0);
   }

   for (n=0; n<Object->Labelling.EdgeCount; ++n)
   {
	  lREAL deltaI, deltaJ, deltaK;
	  lREAL weightI, weightJ, weightK;

	  DEGREES Angle = CalculateLineDirection(Object,&Object->Labelling,n);
	  lREAL length = LineLength(Object,n);
	  //printf("Edge %d, Angle %f, Length %f\n",n,Angle,length);

      if (dangleclockMaths(Angle,plusI) < 0.5)
      {
         deltaI = length;
         deltaJ = 0.0;
         deltaK = 0.0;
         weightI = weightJ = weightK = 1.0;
	  }
	  else if (dangleclockMaths(Angle,minusI) < 0.5)
      {
         deltaI = -length;
         deltaJ = 0.0;
         deltaK = 0.0;
         weightI = weightJ = weightK = 1.0;
	  }
	  else if (dangleclockMaths(Angle,plusJ) < 0.5)
      {
         deltaI = 0.0;
         deltaJ = length;
         deltaK = 0.0;
         weightI = weightJ = weightK = 1.0;
	  }
	  else if (dangleclockMaths(Angle,minusJ) < 0.5)
      {
         deltaI = 0.0;
         deltaJ = -length;
         deltaK = 0.0;
         weightI = weightJ = weightK = 1.0;
	  }
	  else if (dangleclockMaths(Angle,plusK) < 0.5)
      {
         deltaI = 0.0;
         deltaJ = 0.0;
         deltaK = length;
         weightI = weightJ = weightK = 1.0;
	  }
	  else if (dangleclockMaths(Angle,minusK) < 0.5)
      {
         deltaI = 0.0;
         deltaJ = 0.0;
         deltaK = -length;
         weightI = weightJ = weightK = 1.0;
	  }
	  else if (clockwise(plusI,Angle) && clockwise(Angle,minusK))
	  {
		 SplitLengthIntoComponents(length,Angle,&deltaI,plusI,&deltaK,minusK);
		 deltaK = -deltaK;
		 deltaJ = 0.0;

         if (clockwise(Angle,midangle(plusI,minusK)))
         {
            lREAL diff = anglediff(plusI,Angle);
            weightI = 1.0-diff*devOy;
            weightJ = weightK = 1.0-diff*devOz;
         }
         else
         {
            lREAL diff = anglediff(Angle,minusK);
            weightK = 1.0-diff*devOy;
            weightI = weightJ = 1.0-diff*devOz;
         }
	  }
	  else if (clockwise(minusK,Angle) && clockwise(Angle,plusJ))
	  {
		 SplitLengthIntoComponents(length,Angle,&deltaK,minusK,&deltaJ,plusJ);
		 deltaK = -deltaK;
		 deltaI = 0.0;

         if (clockwise(Angle,midangle(minusK,plusJ)))
         {
            lREAL diff = anglediff(minusK,Angle);
            weightK = 1.0-diff*devOy;
            weightI = weightJ = 1.0-diff*devOz;
         }
         else
         {
            lREAL diff = anglediff(Angle,plusJ);
            weightJ = 1.0-diff*devOy;
            weightI = weightK = 1.0-diff*devOz;
         }
	  }
	  else if (clockwise(plusJ,Angle) && clockwise(Angle,minusI))
	  {
		 SplitLengthIntoComponents(length,Angle,&deltaJ,plusJ,&deltaI,minusI);
		 deltaI = -deltaI;
		 deltaK = 0.0;

         if (clockwise(Angle,midangle(plusJ,minusI)))
         {
            lREAL diff = anglediff(plusJ,Angle);
            weightJ = 1.0-diff*devOy;
            weightI = weightK = 1.0-diff*devOz;
         }
         else
         {
            lREAL diff = anglediff(Angle,minusI);
            weightI = 1.0-diff*devOy;
            weightJ = weightK = 1.0-diff*devOz;
         }
	  }
	  else if (clockwise(minusI,Angle) && clockwise(Angle,plusK))
	  {
		 SplitLengthIntoComponents(length,Angle,&deltaI,minusI,&deltaK,plusK);
		 deltaI = -deltaI;
		 deltaJ = 0.0;

         if (clockwise(Angle,midangle(minusI,plusK)))
         {
            lREAL diff = anglediff(minusI,Angle);
            weightI = 1.0-diff*devOy;
            weightJ = weightK = 1.0-diff*devOz;
         }
         else
         {
            lREAL diff = anglediff(Angle,plusK);
            weightK = 1.0-diff*devOy;
            weightI = weightJ = 1.0-diff*devOz;
         }
	  }
	  else if (clockwise(plusK,Angle) && clockwise(Angle,minusJ))
	  {
		 SplitLengthIntoComponents(length,Angle,&deltaK,plusK,&deltaJ,minusJ);
		 deltaJ = -deltaJ;
		 deltaI = 0.0;

         if (clockwise(Angle,midangle(plusK,minusJ)))
         {
            lREAL diff = anglediff(plusK,Angle);
            weightK = 1.0-diff*devOy;
            weightI = weightJ = 1.0-diff*devOz;
         }
         else
         {
            lREAL diff = anglediff(Angle,minusJ);
            weightJ = 1.0-diff*devOy;
            weightI = weightK = 1.0-diff*devOz;
         }
	  }
	  else if (clockwise(minusJ,Angle) && clockwise(Angle,plusI))
	  {
		 SplitLengthIntoComponents(length,Angle,&deltaJ,minusJ,&deltaI,plusI);
		 deltaJ = -deltaJ;
		 deltaK = 0.0;

         if (clockwise(Angle,midangle(minusJ,plusI)))
         {
            lREAL diff = anglediff(minusJ,Angle);
            weightJ = 1.0-diff*devOy;
            weightI = weightK = 1.0-diff*devOz;
         }
         else
         {
            lREAL diff = anglediff(Angle,plusI);
            weightI = 1.0-diff*devOy;
            weightJ = weightK = 1.0-diff*devOz;
         }
	  }
	  else
	  {
         CString sMensaje, sMensajeAux;
         sMensaje= "Labelling error: What to do with dangleclock ";
         sMensajeAux.Format("%f", Angle);
         sMensaje+=sMensajeAux;
         AfxMessageBox(sMensaje);
         //exit(999);
	  }
	  //printf("DeltaI=%f(%f), DeltaJ=%f(%f), DeltaK=%f(%f)\n",deltaI,weightI,deltaJ,weightJ,deltaK,weightK);

	  int S = VariableIndex(&Object->Labelling,Object->Labelling.EdgeStartVertex[n],n);
	  int T = VariableIndex(&Object->Labelling,Object->Labelling.EdgeEndVertex[n],n);

      //printf ("V:%d:%d, ang=%f, dI=%f, dJ=%f, dK=%f, wI=%f, wJ=%f, wK=%f\n", S, T, Angle, deltaI, deltaJ, deltaK, weightI, weightJ, weightK);
      if (weightI > 0.0)
      {
	     Eqn = NextEquation(IEquations);
	     SetCoefficient(IEquations,Eqn,S,-weightI);
	     SetCoefficient(IEquations,Eqn,T,weightI);
	     SetRHSValue(IEquations,Eqn,deltaI*Iscaling*weightI);
	  }

      if (weightJ > 0.0)
      {
  	     Eqn = NextEquation(JEquations);
	     SetCoefficient(JEquations,Eqn,S,-weightJ);
	     SetCoefficient(JEquations,Eqn,T,weightJ);
	     SetRHSValue(JEquations,Eqn,deltaJ*Jscaling*weightJ);
	  }

      if (weightK > 0.0)
      {
	     Eqn = NextEquation(KEquations);
	     SetCoefficient(KEquations,Eqn,S,-weightK);
	     SetCoefficient(KEquations,Eqn,T,weightK);
	     SetRHSValue(KEquations,Eqn,deltaK*Kscaling*weightK);
	  }
   }

   //printf ("%d Subgraphs, %d Cofacial Configurations\n", Object->SubgraphCount, Object->CofacialCount);
   if (Object->SubgraphCount > 1)  AddDisjointSubgraphEquations(Object,&Object->Labelling,IEquations,JEquations,KEquations);

   if (UseDeductionsInLamban)
   {
      // There will be extra equations from the deductive reasoning
      for ( FACE_INDEX F=0; F<Object->FaceCount; ++F)  if (FaceBundleMerit[F] > FoMIndeterminate)
      {
         OrthoStruct *Equations = NULL;
         if (Object->Faces[F].NormalBundle == Ibundle)
         {
            //printf("Face bundle I, merit %f\n",FaceBundleMerit[F]);
            Equations = IEquations;
         }
         else
         if (Object->Faces[F].NormalBundle == Jbundle)
         {
            //printf("Face bundle J, merit %f\n",FaceBundleMerit[F]);
            Equations = JEquations;
         }
         else
         if (Object->Faces[F].NormalBundle == Kbundle)
         {
            //printf("Face bundle K, merit %f\n",FaceBundleMerit[F]);
            Equations = KEquations;
         }

         if (Equations)
         {
            for ( int L=0; L<Object->Faces[F].NumberOfLoops; ++L)
            {
                LOOP *Loop = &Object->Faces[F].Loops[L];
                int K = Loop->CornerCount;
               for ( int p=0; p<K; ++p)  for ( int q=p+1; q<K; ++q)
               {
                  Eqn = NextEquation(Equations);
                  SetCoefficient(Equations,Eqn,Loop->Corners[p].Number,FaceBundleMerit[F]);
                  SetCoefficient(Equations,Eqn,Loop->Corners[q].Number,-FaceBundleMerit[F]);
                  SetRHSValue(Equations,Eqn,0);
               }
            }
         }
      }
   }

   //printf ("Ready to solve equations\n");
   lBOOLEAN isOK = SolveEquations(IEquations) && SolveEquations(JEquations) && SolveEquations(KEquations);
   //printf ("Back from trying to solve equations\n");

   if (isOK)
   {
      for (n=0; n<Object->Labelling.VertexCount; ++n)
      {
		 Icoord[n] = VariableValue(IEquations,n);
		 Jcoord[n] = VariableValue(JEquations,n);
		 Kcoord[n] = VariableValue(KEquations,n);
		 //printf ("V%d: I=%f, J=%f, K=%f\n", n, Icoord[n], Jcoord[n], Kcoord[n]);
	  }

      for (n=Object->Labelling.VertexCount; n<VariableCount; ++n)
      {
          int t = Tvertex(&Object->Labelling,n);
		 Icoocc[t] = VariableValue(IEquations,n);
		 Jcoocc[t] = VariableValue(JEquations,n);
		 Kcoocc[t] = VariableValue(KEquations,n);
		 //printf ("Vt%d: I=%f, J=%f, K=%f\n", t, Icoocc[t], Jcoocc[t], Kcoocc[t]);
      }
   }
   else
   {
      AfxMessageBox("Labelling error: Lamban Equations won't solve!");
      //exit(999);
   }

   DeleteEquations(IEquations);
   DeleteEquations(JEquations);
   DeleteEquations(KEquations);
   return isOK;
}


void RotatePointAboutAxis (POINT3D *Point,  VECTOR3D *Axis,  DEGREES Angle)
{
   lREAL sinAngle = sin(Angle*M_PI/180.0);
   lREAL cosAngle = cos(Angle*M_PI/180.0);
   lREAL verAngle = 1-cosAngle;
   lREAL x = Point->x;
   lREAL y = Point->y;
   lREAL z = Point->z;
   lREAL p = Axis->x;
   lREAL q = Axis->y;
   lREAL r = Axis->z;
   lREAL pp = p*p;
   lREAL qq = q*q;
   lREAL rr = r*r;
   lREAL pqv = p*q*verAngle;
   lREAL prv = p*r*verAngle;
   lREAL qrv = q*r*verAngle;
   Point->x = x*(pp+(qq+rr)*cosAngle) + y*(pqv-r*sinAngle) + z*(prv+q*sinAngle);
   Point->y = x*(pqv+r*sinAngle) + y*(qq+(pp+rr)*cosAngle) + z*(qrv-p*sinAngle);
   Point->z = x*(prv-q*sinAngle) + y*(qrv+p*sinAngle) + z*(rr+(pp+qq)*cosAngle);
}


lREAL DeltaDifference (lREAL Variables[])
{
   lREAL total = 0.0;
   POINT3D P;
   VECTOR3D Axis;
   int n;
   lREAL dX,dY;

   Axis.x = cos(Variables[1]*M_PI/180.0) * sin(Variables[2]*M_PI/180.0);
   Axis.y = cos(Variables[1]*M_PI/180.0) * cos(Variables[2]*M_PI/180.0);
   Axis.z = sin(Variables[1]*M_PI/180.0);

   for (n=0; n<Nvert; ++n)
   {
      P.x = Icoord[n];
      P.y = Jcoord[n];
      P.z = Kcoord[n];
      RotatePointAboutAxis(&P,&Axis,Variables[3]);
      dX = (Variables[0]*P.x) - (Xcoord[n]-Xcoord[0]);
      dY = (Variables[0]*P.y) - (Ycoord[n]-Ycoord[0]);
      total += dX*dX+dY*dY;
   }

   //printf ("V: %f, %f, %f, %f => total %f\n", Variables[0], Variables[1], Variables[2], Variables[3], total);

   return total;
}


lREAL CCueLabelsVarley_Lamban::VariableSumOverAllVectors (lREAL VariableSpace[], lINTEGER Variable)
{
    lINTEGER index;
   lREAL sum = 0.0;
   for (index=0; index<CountOfVariableVectors; ++index)  sum += VariableSpace[index*CountOfVariables+Variable];
   return sum;
}


void CCueLabelsVarley_Lamban::CreateTrialVariablesByReflecting (lREAL VariableSlot[], lREAL VariableTotals[], lREAL TryTheseVariables[])
// It's possible that amoeba can get stuck in a loop if this is an exact relection
// So use slightly less than a full reflection instead
{
    lINTEGER j;
   for (j=0; j<CountOfVariables; j++)
   {
      lREAL T = VariableTotals[j];
      lREAL V = VariableSlot[j];
      TryTheseVariables[j] = (2.0*(T-V)/CountOfVariables) - V;
   }
}


void CCueLabelsVarley_Lamban::CreateTrialVariablesByReflectingAndExpanding (lREAL VariableSlot[], lREAL VariableTotals[], lREAL TryTheseVariables[])
{
    lINTEGER j;
   for (j=0; j<CountOfVariables; j++)
   {
      lREAL T = VariableTotals[j];
      lREAL V = VariableSlot[j];
      TryTheseVariables[j] = (4.0*(T-V)/CountOfVariables) - 3*V;
   }
}


void CCueLabelsVarley_Lamban::CreateTrialVariablesByContraction (lREAL VariableSlot[], lREAL VariableTotals[], lREAL TryTheseVariables[])
{
    lINTEGER j;
   for (j=0; j<CountOfVariables; j++)
   {
      lREAL V = VariableSlot[j];
      TryTheseVariables[j] = (0.5/CountOfVariables)*(VariableTotals[j]-V) + 0.5*V;
   }
}


void CCueLabelsVarley_Lamban::UpdateVariableTotals (lREAL VariableSlot[], lREAL VariableTotals[], lREAL TryTheseVariables[])
{
    lINTEGER j;

   for (j=0; j<CountOfVariables; j++)
   {
      VariableTotals[j] += TryTheseVariables[j]-VariableSlot[j];
      VariableSlot[j] = TryTheseVariables[j];
   }
}


void CCueLabelsVarley_Lamban::ContractAboutBestPoint(lREAL VariableSpace[], lREAL VariableTotals[], lREAL ResultVector[], lINTEGER BestSoFar, lREAL (*ObjectiveFunction)(lREAL []))
{
    lINTEGER index;
   for (index=0; index<CountOfVariableVectors; ++index)  if (index != BestSoFar)
   {
       lINTEGER j;
      for (j=0; j<CountOfVariables; j++)
         VariableSpace[index*CountOfVariables+j]=VariableTotals[j]=0.5*(VariableSpace[index*CountOfVariables+j]+VariableSpace[BestSoFar*CountOfVariables+j]);
      ResultVector[index] = ObjectiveFunction(VariableTotals);
   }
   for (index=0; index<CountOfVariables; ++index)  VariableTotals[index] = VariableSumOverAllVectors(VariableSpace,index);
}


lREAL CCueLabelsVarley_Lamban::DownhillOptimise (lINTEGER NumVars, lREAL StartValues[], lREAL DeltaValues[], lREAL (*ObjectiveFunction)(lREAL []), lINTEGER MaxIterations, lREAL Tolerance)
{
    lINTEGER index,jndex,BestSoFar;
   lINTEGER IterationCount;
   lREAL Result;
   lREAL *ReflectionVariables,*ReflectAndExpandVariables,*ContractionVariables;
   lREAL *Workspace;
   lREAL *VariableTotals;
   lREAL *VariableSpace;
   lREAL *ResultVector;

   CountOfVariables = NumVars;
   CountOfVariableVectors = NumVars+1;

#define VT_SIZE (CountOfVariables)
#define VS_SIZE (CountOfVariables*CountOfVariableVectors)
#define RV_SIZE (CountOfVariableVectors)
#define RF_SIZE (CountOfVariables)
#define RX_SIZE (CountOfVariables)
#define CV_SIZE (CountOfVariables)

   Workspace = (lREAL *)malloc(sizeof(lREAL)*(VT_SIZE+VS_SIZE+RV_SIZE+RF_SIZE+RX_SIZE+CV_SIZE));
   VariableTotals = Workspace;
   VariableSpace  = VariableTotals+VT_SIZE;
   ResultVector   = VariableSpace+VS_SIZE;
   ReflectionVariables = ResultVector+RV_SIZE;
   ReflectAndExpandVariables = ReflectionVariables+RF_SIZE;
   ContractionVariables = ReflectAndExpandVariables+RX_SIZE;

   for (index=0; index<CountOfVariableVectors; index++)  for (jndex=0; jndex<CountOfVariables; ++jndex)  VariableSpace[index*CountOfVariables+jndex] = StartValues[jndex];
   for (index=0; index<CountOfVariables; index++)  VariableSpace[index*CountOfVariables+index] = DeltaValues[index];
   for (index=0; index<CountOfVariableVectors; index++)  ResultVector[index] = ObjectiveFunction(&VariableSpace[index*CountOfVariables]);

   for (index=0; index<CountOfVariables; ++index)  VariableTotals[index] = VariableSumOverAllVectors(VariableSpace,index);

   for (IterationCount = 0;  IterationCount <= MaxIterations; )
   {
      lREAL ReflectionResult, ReflectAndExpandResult, ContractionResult;
       lINTEGER WorstSoFar,SecondWorst;

      if (ResultVector[0] > ResultVector[1])
      {
         BestSoFar   = 1;
         SecondWorst = 1;
         WorstSoFar  = 0;
      }
      else
      {
         BestSoFar   = 0;
         SecondWorst = 0;
         WorstSoFar  = 1;
      }

      for (index=2; index<CountOfVariableVectors; ++index)
      {
         if (ResultVector[index] <= ResultVector[BestSoFar]) BestSoFar = index;
         else
         {
            if (ResultVector[index] > ResultVector[WorstSoFar])  { SecondWorst = WorstSoFar;  WorstSoFar = index; }
            else if (ResultVector[index] > ResultVector[SecondWorst])  SecondWorst = index;
         }
      }

      if ((ResultVector[BestSoFar] < Tolerance) || (std::fabs(ResultVector[BestSoFar]-ResultVector[WorstSoFar]) < Tolerance))
      {
         break;
         /* if we needed the variables rather than what they represent, they're in VariableSpace[BestSoFar] */
      }

      /* begin a new iteration */
      //printf("Reflect, Worst = %d\n", WorstSoFar);
      CreateTrialVariablesByReflecting(&VariableSpace[WorstSoFar*CountOfVariables],VariableTotals,ReflectionVariables);
      ReflectionResult = ObjectiveFunction(ReflectionVariables);

      if (ReflectionResult <= ResultVector[BestSoFar])
      {
         IterationCount += 2;
         //printf("Reflect&Expand\n");
         CreateTrialVariablesByReflectingAndExpanding(&VariableSpace[WorstSoFar*CountOfVariables],VariableTotals,ReflectAndExpandVariables);
         ReflectAndExpandResult = ObjectiveFunction(ReflectAndExpandVariables);

         if (ReflectAndExpandResult < ReflectionResult)
         {
			ResultVector[WorstSoFar] = ReflectAndExpandResult;
            UpdateVariableTotals(&VariableSpace[WorstSoFar*CountOfVariables],VariableTotals,ReflectAndExpandVariables);
	     }
         else
         {
			ResultVector[WorstSoFar] = ReflectionResult;
            UpdateVariableTotals(&VariableSpace[WorstSoFar*CountOfVariables],VariableTotals,ReflectionVariables);
	     }
      }
      else if (ReflectionResult >= ResultVector[SecondWorst])
      {
         IterationCount += 2;
         //printf("Contract\n");
         CreateTrialVariablesByContraction(&VariableSpace[WorstSoFar*CountOfVariables],VariableTotals,ContractionVariables);
         ContractionResult = ObjectiveFunction(ContractionVariables);

         if ((ContractionResult >= ResultVector[WorstSoFar]) && (ReflectionResult >= ResultVector[WorstSoFar]))
         {
            //printf("Contract About Best Point\n");
            ContractAboutBestPoint(VariableSpace,VariableTotals,ResultVector,BestSoFar,ObjectiveFunction);
            IterationCount += CountOfVariables;
         }
         else
         {
            if (ContractionResult < ReflectionResult)
            {
			   ResultVector[WorstSoFar] = ContractionResult;
               UpdateVariableTotals(&VariableSpace[WorstSoFar*CountOfVariables],VariableTotals,ContractionVariables);
		    }
            else
            {
			   ResultVector[WorstSoFar] = ReflectionResult;
               UpdateVariableTotals(&VariableSpace[WorstSoFar*CountOfVariables],VariableTotals,ReflectionVariables);
		    }
         }
      }
      else
      {
         IterationCount ++;
         ResultVector[WorstSoFar] = ReflectionResult;
         UpdateVariableTotals(&VariableSpace[WorstSoFar*CountOfVariables],VariableTotals,ReflectionVariables);
      }

      Result = ResultVector[BestSoFar];
      //printf ("Storing result %f in slot %d\n", Result, BestSoFar);
   }

   // Best So Far might have changed during the last iteration
   for (index=0; index<CountOfVariableVectors; ++index)
   {
      if (ResultVector[index] <= ResultVector[BestSoFar]) BestSoFar = index;
   }

   for (index=0; index<CountOfVariables; index++)  DeltaValues[index] = VariableSpace[BestSoFar*CountOfVariables+index];

   free((void *)Workspace);

   return Result;
}


lBOOLEAN CCueLabelsVarley_Lamban::DetermineZCoordinates (OBJECT *Object)
// This uses the IJK coordinates to determine Z-coordinates for each vertex
// Run it as an optimisation over four variables:
//   0 = scaling
//   1 = rotation axis latitude
//   2 = rotation axis longitude
//   3 = rotation angle
{
   int n;
   lREAL StartValues[4],DeltaValues[4];
   VECTOR3D Axis;

   Nvert = Object->Labelling.VertexCount;

   for (n=0; n<Nvert; ++n)
   {
      Xcoord[n] = Object->Coordinates[n].x;
      Ycoord[n] = Object->Coordinates[n].y;
   }

   StartValues[0] =   1.0;  DeltaValues[0] =   1.5;
   StartValues[1] = -60.0;  DeltaValues[1] = -45.0;
   StartValues[2] = 150.0;  DeltaValues[2] = 165.0;
   StartValues[3] = 135.0;  DeltaValues[3] = 150.0;

   DownhillOptimise(4,StartValues,DeltaValues,DeltaDifference,6000,0.000001);
   tScaling     = DeltaValues[0];
   tRotAxisLat  = DeltaValues[1];
   tRotAxisLong = DeltaValues[2];
   tRotAngle    = DeltaValues[3];
   //printf ("Scaling %f, RotAxisLat %f, RotAxisLong %f, RotAngle %f\n", tScaling, tRotAxisLat, tRotAxisLong, tRotAngle);

   Axis.x = cos(tRotAxisLat*M_PI/180.0) * sin(tRotAxisLong*M_PI/180.0);
   Axis.y = cos(tRotAxisLat*M_PI/180.0) * cos(tRotAxisLong*M_PI/180.0);
   Axis.z = sin(tRotAxisLat*M_PI/180.0);

   for (n=0; n<Nvert; ++n)
   {
      POINT3D P;
      P.x = Icoord[n];
      P.y = Jcoord[n];
      P.z = Kcoord[n];
      RotatePointAboutAxis(&P,&Axis,tRotAngle);
      Object->Coordinates[n].z = 1000.0 + P.z * tScaling;
      Object->Vertices[n].DepthKnown = cTRUE;
      //printf ("Vertex %d, depth %f\n", n, z);

      if (Object->Labelling.VertexLabel[n].Shape == JunctionIsT4)
      {
         P.x = Icoocc[n];
         P.y = Jcoocc[n];
         P.z = Kcoocc[n];
         RotatePointAboutAxis(&P,&Axis,tRotAngle);
         Object->Vertices[n].OccludingZ = 1000.0 + P.z * tScaling;
      }
   }

   if (UseDeductionsInLamban)
   {
      POINT3D P0,Pi,Pj,Pk;

      P0.x = Pi.x = Icoord[0];
      P0.y = Pj.y = Jcoord[0];
      P0.z = Pk.z = Kcoord[0];

      for (n=1; n<Nvert; ++n)
      {
         if (Icoord[n] < P0.x)  P0.x = Icoord[n];
         if (Icoord[n] > Pi.x)  Pi.x = Icoord[n];
         if (Jcoord[n] < P0.y)  P0.y = Jcoord[n];
         if (Jcoord[n] > Pj.y)  Pj.y = Jcoord[n];
         if (Kcoord[n] < P0.z)  P0.z = Kcoord[n];
         if (Kcoord[n] > Pk.z)  Pk.z = Kcoord[n];
      }
      Pj.x = Pk.x = P0.x;
      Pi.y = Pk.y = P0.y;
      Pi.z = Pj.z = P0.z;
      RotatePointAboutAxis(&P0,&Axis,tRotAngle);
      RotatePointAboutAxis(&Pi,&Axis,tRotAngle);
      RotatePointAboutAxis(&Pj,&Axis,tRotAngle);
      RotatePointAboutAxis(&Pk,&Axis,tRotAngle);
      FindImplicitPlane(&MinusILimit,&P0,&Pj,&Pk);
      FindImplicitPlane(&MinusJLimit,&P0,&Pi,&Pk);
      FindImplicitPlane(&MinusKLimit,&P0,&Pi,&Pj);
   }

   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_Lamban::BoundaryDepthInverted (OBJECT *Object)
{
   lREAL zSum = 0.0;
    EDGE_INDEX C = Object->Labelling.EdgeCount;

   for ( EDGE_INDEX e=0; e<C; ++e)
   {
       VERTEX_INDEX S = Object->Labelling.EdgeStartVertex[e];
	   VERTEX_INDEX T = Object->Labelling.EdgeEndVertex[e];

      if (Object->IsOnBoundary[S] && !Object->IsOnBoundary[T])
      {
		 zSum += Object->Coordinates[S].z - Object->Coordinates[T].z;
	  }
	  else
      if (Object->IsOnBoundary[T] && !Object->IsOnBoundary[S])
      {
		 zSum += Object->Coordinates[T].z - Object->Coordinates[S].z;
	  }
   }
   return zSum < 0.0;
}


void CCueLabelsVarley_Lamban::InvertVexity (OBJECT *Object)
{
    VERTEX_INDEX C = Object->Labelling.VertexCount;

   for ( VERTEX_INDEX v=0; v<C; ++v)
   {
      Object->Coordinates[v].z = 1000.0-Object->Coordinates[v].z;
   }
}


lBOOLEAN CCueLabelsVarley_Lamban::GenerateLambanZCoordinates (OBJECT *Object)
{
   if (!IdentifyLambanAxes(Object))  return cFALSE;
   if (!SolveLambanEquations(Object))  return cFALSE;
   if (!DetermineZCoordinates(Object))  return cFALSE;

   VexityInversion = BoundaryDepthInverted(Object);
   if (VexityInversion)  InvertVexity(Object);  // there is the strong chance that z-coordinates need inverting
   return cTRUE;
}


VERTEX_INDEX CCueLabelsVarley_Lamban::VertexOnLoop (LOOP *Loop, VERTEX_INDEX n)
{
   while (n < 0)  n += Loop->CornerCount;
   return Loop->Corners[n%Loop->CornerCount].Number;
}


lREAL CCueLabelsVarley_Lamban::ChooseAppropriateZCoordinate (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Nvert)
{
   // if it's a T- or T4-junction, we want the proper z-coordinate for
   // the T-tail, but the occlusion coordinate for the other face

   if (Object->Labelling.VertexLabel[Nvert].Shape == JunctionIsT4)
   {
       EDGE_INDEX Ttail = Object->Labelling.VENumber[Nvert][2];
      if ((Object->Labelling.EdgeForwardFace[Ttail] != Nface) && (Object->Labelling.EdgeReverseFace[Ttail] != Nface))  return Object->Vertices[Nvert].OccludingZ;
   }

   if (Object->Labelling.VertexLabel[Nvert].Shape == JunctionIsT)
   {
       EDGE_INDEX Ttail = Object->Labelling.VENumber[Nvert][0];
      if ((Object->Labelling.EdgeForwardFace[Ttail] != Nface) && (Object->Labelling.EdgeReverseFace[Ttail] != Nface))  return Object->Vertices[Nvert].OccludingZ;
   }

   // default
   return Object->Coordinates[Nvert].z;
}


lBOOLEAN CCueLabelsVarley_Lamban::Cramer3 (lREAL augmatrix[], lREAL result[])
// Solve a 3x3 linear system using Cramer's rule
// The augmatrix is a 3x4 matrix containing the coefficients:
      //[ a0  a1  a2 ] [ r0 ] = [  a3 ]
      //[ a4  a5  a6 ] [ r1 ] = [  a7 ]
      //[ a8  a9 a10 ] [ r2 ] = [ a11 ]

// the coding is deliberately inefficient to maximise clarity
{
   lREAL det0,det1,det2,det3;
   lREAL a = augmatrix[0];
   lREAL b = augmatrix[1];
   lREAL c = augmatrix[2];
   lREAL x = augmatrix[3];
   lREAL d = augmatrix[4];
   lREAL e = augmatrix[5];
   lREAL f = augmatrix[6];
   lREAL y = augmatrix[7];
   lREAL g = augmatrix[8];
   lREAL h = augmatrix[9];
   lREAL i = augmatrix[10];

   lREAL eifh = e*i-f*h;
   lREAL difg = d*i-f*g;
   lREAL dheg = d*h-e*g;

   det0 = a*eifh-b*difg+c*dheg;
   if (det0 == 0.0)  return cFALSE;

   det1 = x*eifh-b*(y*i-f*augmatrix[11])+c*(y*h-e*augmatrix[11]);
   det2 = a*(y*i-f*augmatrix[11])-x*difg+c*(d*augmatrix[11]-y*g);
   det3 = a*(e*augmatrix[11]-y*h)-b*(d*augmatrix[11]-y*g)+x*dheg;

   result[0] = det1/det0;
   result[1] = det2/det0;
   result[2] = det3/det0;
   return cTRUE;
}


void CCueLabelsVarley_Lamban::NormaliseImplicitPlane (IMPLICIT_PLANE *Plane)
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


void CCueLabelsVarley_Lamban::NegateVector (VECTOR3D *Vector)
{
   Vector->x = -Vector->x;
   Vector->y = -Vector->y;
   Vector->z = -Vector->z;
}


void CCueLabelsVarley_Lamban::MakeFaceVisible (IMPLICIT_PLANE *Plane)
{
   if (Plane->Normal.z < 0.0) {  NegateVector(&Plane->Normal);  Plane->Distance = -Plane->Distance; }
}


void CCueLabelsVarley_Lamban::SetFacePlane (OBJECT *Object, FACE_INDEX Nface, IMPLICIT_PLANE *FacePlane)
{
    int Plane = Object->Faces[Nface].PlaneNumber;
   if (Plane < 0)
   {
      Plane = Object->Faces[Nface].PlaneNumber = Object->PlaneCount++;
   }

   Object->Planes[Plane].PlaneEquation3D = *FacePlane;
   Object->Planes[Plane].IsEquationKnown = 3;
}


lBOOLEAN CCueLabelsVarley_Lamban::MakeFaceEquation3 (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Vertices[])
{
   int n;
   int D = 0;
   double A[12];
   double R[3];
   IMPLICIT_PLANE Equation;

   for (n=0; n<3; ++n)
   {
      A[D*4]   = Object->Coordinates[n].x;
      A[D*4+1] = Object->Coordinates[n].y;
      A[D*4+2] = ChooseAppropriateZCoordinate(Object,Nface,n);
      A[D*4+3] = -1.0;
      ++D;
   }

   if (!Cramer3(A,R))
   {
      //printf("ERROR: MakeFaceEquation3 cannot use Cramer's Rule, as determinant is zero!\n");
      return cFALSE;
   }

   Equation.Normal.x = R[0];
   Equation.Normal.y = R[1];
   Equation.Normal.z = R[2];
   //printf("3-vertex face normal is %s\n", Vector3DString(&Equation.Normal));
   Equation.Distance = 1.0;
   NormaliseImplicitPlane(&Equation);
   MakeFaceVisible(&Equation);
   // technically, invert the vector if the face is hidden,
   // but as this routine is now used for Lamban labelling, it won't be

   SetFacePlane(Object,Nface,&Equation);

   return cTRUE;
}


void CCueLabelsVarley_Lamban::AllocateWorkspace (lINTEGER VertexCount, lREAL *A, lREAL *B)
{
   if (VertexCount > MAX_VERTICES)
   {
      printf ("PLANEFIT: Increase MAX_VERTICES to %d\n", VertexCount);
      exit(EXIT_FAILURE);
   }

   if (!A)
   {
      A = (lREAL *)malloc(sizeof(lREAL)*MAX_VERTICES*3);
      B = (lREAL *)malloc(sizeof(lREAL)*MAX_VERTICES);
   }
}


lBOOLEAN CCueLabelsVarley_Lamban::GetPlaneFittingWeightedVertices (lINTEGER VertexCount, POINT3D Vertices[], lREAL Weights[], IMPLICIT_PLANE *Plane)
{
    lINTEGER i;
   lREAL   F[3];
   lBOOLEAN  R;

   lREAL  *A = NULL;
   lREAL  *B = NULL;

   AllocateWorkspace(VertexCount, A, B);

   for (i=0; i<VertexCount; i++)
   {
      A[i*3+0] = Vertices[i].x*Weights[i];
      A[i*3+1] = Vertices[i].y*Weights[i];
      A[i*3+2] = Weights[i];
      B[i]     = -Vertices[i].z*Weights[i];
   }

   R = Ortholin2(A,VertexCount,3,B,0.0001,F,1);

   Plane->Normal.x = F[0];
   Plane->Normal.y = F[1];
   Plane->Normal.z = 1.0;
   Plane->Distance = F[2];

   return R;
}


void CCueLabelsVarley_Lamban::MakeFaceEquationW (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Vertices[], int N, lREAL Weighting[])
{
   POINT3D Points[MAX_VERTICES];
   IMPLICIT_PLANE Equation;

   for ( int i=0; i<N; ++i)
   {
      Points[i].x = Object->Coordinates[Vertices[i]].x;
	  Points[i].y = Object->Coordinates[Vertices[i]].y;
	  Points[i].z = ChooseAppropriateZCoordinate(Object,Nface,Vertices[i]);
   }

   GetPlaneFittingWeightedVertices(N,Points,Weighting,&Equation);
   NormaliseImplicitPlane(&Equation);
   MakeFaceVisible(&Equation);
   // technically, invert the vector if the face is hidden,
   // but as this routine is now used for Lamban labelling, it won't be

   SetFacePlane(Object,Nface,&Equation);
}


void CCueLabelsVarley_Lamban::MakeFaceEquationN (OBJECT *Object, FACE_INDEX Nface, VERTEX_INDEX Vertices[], int N)
{
   double Weighting[MAX_VERTICES];

   for ( int i=0; i<N; ++i)
   {
      //Old-style, used for topology
      //Weighting[i] = V->IsHidden?0.01:(VertexLabelsIdentical(&Object->Labelling.VertexLabel[Vertices[i]],&LabelLcb))?0.1:1.0;

      //New-style, used for Lamban labelling
      VERTEX_LABEL Shape = Object->Labelling.VertexLabel[Vertices[i]].Shape;
      Weighting[i] = ((Shape == JunctionIsT) || (Shape == JunctionIsT4)) ? LC->tcSv : 1.0;
   }

   MakeFaceEquationW(Object,Nface,Vertices,N,Weighting);
}


lBOOLEAN CCueLabelsVarley_Lamban::MakeFaceEquationUsingVertexDepths (OBJECT *Object, FACE_INDEX Nface)
// Second fallback for general face equations, but (more importantly) now the
// main way of getting face equations while doing Lamban labelling
{
    LOOP *Loop = &Object->Faces[Nface].Loops[0];

   int Count = 0;
   VERTEX_INDEX Vertices[MAX_VERTICES];

   for ( int n=0; n<Loop->SideCount; ++n)
   {
      VERTEX_INDEX V = VertexOnLoop(Loop,n);
      if (Object->Vertices[V].DepthKnown)  Vertices[Count++] = V;
   }

   //printf("Make Face %d Equation using %d Vertex Depths\n", Nface, Count);
   if (Count < 3)  return cFALSE;

   if ((Count == 3) && MakeFaceEquation3(Object,Nface,Vertices))  return cTRUE;

   // default case
   MakeFaceEquationN(Object,Nface,Vertices,Count);
   return cTRUE;
}


void CCueLabelsVarley_Lamban::SetLineDirection (OBJECT *Object, EDGE_INDEX Nedge, lBOOLEAN IsKnown, DEGREES Direction)
{
   Object->Edges[Nedge].LineDirectionKnown = IsKnown;
   Object->Edges[Nedge].DrawnLineDirection = Direction;
}


DEGREES CCueLabelsVarley_Lamban::GetLineDirection (OBJECT *Object, EDGE_INDEX Nedge)
{
   if (Object->Edges[Nedge].LineDirectionKnown)  return Object->Edges[Nedge].DrawnLineDirection;
   return CalculateLineDirection(Object,&Object->Labelling,Nedge);
}


DEGREES CCueLabelsVarley_Lamban::CalculateLineDirection (OBJECT *Object, LABELLING *Labelling, EDGE_INDEX Nedge)
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


void CCueLabelsVarley_Lamban::CalculateAllLineDirections (OBJECT *Object)
{
   for ( EDGE_INDEX e=0; e<Object->Labelling.EdgeCount; ++e)
   {
	  SetLineDirection(Object,e,cTRUE,CalculateLineDirection(Object,&Object->Labelling,e));
   }

   DirectionsAreFixed = cTRUE;
}


DEGREES CCueLabelsVarley_Lamban::DirectionAlongLineFromJunction (OBJECT *Object, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
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


lBOOLEAN CCueLabelsVarley_Lamban::MakeFaceNormalUsingCubicCorners (OBJECT *Object, FACE_INDEX Nface, VECTOR3D *Normal)
// An alternative way of getting face equations while doing Lamban labelling
{
   lBOOLEAN NormalSet = cFALSE;
    LOOP *Loop = &Object->Faces[Nface].Loops[0];

   Normal->x = Normal->y = Normal->z = 0.0;

   for ( int n=0; n<Loop->CornerCount; ++n)
   {
      lBOOLEAN MayBeCubicCorner = cFALSE;
      VERTEX_INDEX V = VertexOnLoop(Loop,n);

      switch (Object->Labelling.VertexLabel[V].Shape)
      {
         case JunctionIsW:
         {
            DEGREES A0 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[V][0],V);
            DEGREES A1 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[V][1],V);
            DEGREES A2 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[V][2],V);
            if ((dangleclockMaths(A0,A1) < 90.0) && (dangleclockMaths(A1,A2) < 90.0) && (dangleclockMaths(A0,A2) > 90.0))  MayBeCubicCorner = cTRUE;
         }
         break;

         case JunctionIsY:
         {
            DEGREES A0 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[V][0],V);
            DEGREES A1 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[V][1],V);
            DEGREES A2 = DirectionAlongLineFromJunction(Object,Object->Labelling.VENumber[V][2],V);
            if ((dangleclockMaths(A0,A1) > 90.0) && (dangleclockMaths(A1,A2) > 90.0) && (dangleclockMaths(A2,A0) > 90.0))  MayBeCubicCorner = cTRUE;
         }
         break;
      }

      if (MayBeCubicCorner)
      {
         EDGE_INDEX E0 = Object->Labelling.VENumber[V][0];
         EDGE_INDEX E1 = Object->Labelling.VENumber[V][1];
         EDGE_INDEX E2 = Object->Labelling.VENumber[V][2];
         EDGE_INDEX Ea = EdgeOnLoop(Loop,n);
         EDGE_INDEX Eb = EdgeOnLoop(Loop,n+1);
         EDGE_INDEX En = (EDGE_INDEX)uniq5(E0,E1,E2,Ea,Eb);

         if (En >= 0)
         {
            // N.B. GetVectorForEdge doesn't work yet as we don't have proper depth
            VECTOR3D EdgeVector;
            GetVectorFromPoint3ToPoint3(&EdgeVector,
                                        &Object->Coordinates[V],
                                        &Object->Coordinates[EdgeOtherEnd(Object,En,V)]);
            if (EdgeVector.z > 0.0)  NegateVector(&EdgeVector);
            //printf("Vertex %d may be a cubic corner, add edge %d, %s\n",V, En, Vector3DString(&EdgeVector));
            AddVector(Normal,&EdgeVector);
            NormalSet = cTRUE;
         }
      }
   }

   if (NormalSet)  NormaliseVector(Normal);
   return NormalSet;
}


void CCueLabelsVarley_Lamban::MakeFaceDistanceGivenNormal (OBJECT *Object, FACE_INDEX Nface, VECTOR3D *Normal)
{
   int            Count = 0;
   IMPLICIT_PLANE Plane;
   lREAL          TotalDistance = 0.0;
    LOOP *Loop = &Object->Faces[Nface].Loops[0];

   Plane.Normal = *Normal;

   for ( int n=0; n<Loop->CornerCount; ++n)
   {
      TotalDistance += -DotProduct(&Plane.Normal,&Object->Coordinates[VertexOnLoop(Loop,n)]);
      ++Count;
   }

   Plane.Distance = TotalDistance/Count;
   SetFacePlane(Object,Nface,&Plane);
}


lBOOLEAN CCueLabelsVarley_Lamban::MakeFaceEquationForLambanLabelling (OBJECT *Object, FACE_INDEX Nface)
// A selective way of getting face equations while doing Lamban labelling
{
   VECTOR3D CCNormal;

   if (!MakeFaceEquationUsingVertexDepths(Object,Nface))  return cFALSE;

   if (MakeFaceNormalUsingCubicCorners(Object,Nface,&CCNormal))
   {
      //printf("Face %d, CC normal %s\n",Nface,Vector3DString(&CCNormal));
      VECTOR3D VDNormal;
      GetVFaceNormal(Object,Nface,&VDNormal);

      VECTOR3D OverallNormal;
      //printf("Face %d, VD normal %s\n",Nface,Vector3DString(&VDNormal));
      OverallNormal.x = CCNormal.x*(1.0-LC->tcSu) + VDNormal.x*LC->tcSu;
      OverallNormal.y = CCNormal.y*(1.0-LC->tcSu) + VDNormal.y*LC->tcSu;
      OverallNormal.z = CCNormal.z*(1.0-LC->tcSu) + VDNormal.z*LC->tcSu;
      NormaliseVector(&OverallNormal);
      MakeFaceDistanceGivenNormal(Object,Nface,&OverallNormal);
   }
   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_Lamban::FindRegionPlanes (OBJECT *Object, LABELLING *Labelling)
// This finds the best-fit plane for each region
{
   for ( int n=0; n<Object->FaceCount; ++n)
   {
      Object->Faces[n].IsHidden = cFALSE;
      MakeFaceEquationForLambanLabelling(Object,n);
      //printf("Region %d, Plane %s\n",n,PlaneString(&Object->Faces[n]->FaceEquation3D));
   }

   return cTRUE;
}


MERIT CCueLabelsVarley_Lamban::FoMParallel3D (VECTOR3D *V1, VECTOR3D *V2)
{
   VECTOR3D N1 = *V1;
   VECTOR3D N2 = *V2;
   NormaliseVector(&N1);
   NormaliseVector(&N2);
   return pow(std::fabs(DotProduct(&N1,&N2)),(tcMp*100.0));
}


MERIT CCueLabelsVarley_Lamban::FoMPerpendicular3D (VECTOR3D *V1, VECTOR3D *V2)
{
   VECTOR3D V3, V4;

   GetCrossProduct (&V3, V1, V2);
   if (VectorMagnitude(&V3) < 0.0001)  return 0.0;  /* they're parallel */

   GetCrossProduct (&V4, &V3, V1);
   return FoMParallel3D (&V4, V2);
}



lBOOLEAN CCueLabelsVarley_Lamban::LabelUsingPlanes (OBJECT *Object)
{
   lBOOLEAN allOK = cTRUE;

   //printf("Label %d Edges Using Planes\n", Object->Labelling.EdgeCount);
   //PrintVerticesAndEdges(Object,Labelling);

   for ( EDGE_INDEX e=0; e<Object->Labelling.EdgeCount; ++e)
   {
       FACE_INDEX f = Object->Labelling.EdgeForwardFace[e];
       FACE_INDEX r = Object->Labelling.EdgeReverseFace[e];

      //printf("Labelling Edge %d: faces are %d and %d\n", e, f, r);

      if ((f >= 0) && (f < Object->FaceCount) && (r >= 0) && (r < Object->FaceCount))
      {
		 DEGREES Angle;
		 DEGREES Df,Dr;
		 lREAL Distance;
		 POINT3D Pe;
		 MERIT OopF,OopR,Oop;

		 GetEdgeCentre(Object,e,&Pe);
		 IMPLICIT_PLANE Fplane,Gplane;
		 GetFacePlane(Object,f,&Fplane);
		 GetFacePlane(Object,r,&Gplane);
		 //printf("Forward face equation: %s\n", PlaneString(&Fplane));
		 //printf("Reverse face equation: %s\n", PlaneString(&Gplane));

         //The plane normal z-components must be positive (and non-zero) as the plane is visible
		 lREAL zf = -(Fplane.Normal.x*Pe.x+Fplane.Normal.y*Pe.y+Fplane.Distance)/Fplane.Normal.z;
		 lREAL zg = -(Gplane.Normal.x*Pe.x+Gplane.Normal.y*Pe.y+Gplane.Distance)/Gplane.Normal.z;
		 Distance = zg-zf;
		 //printf("E%d, ze=%f, zf=%f, zg=%f, D=%f\n", e, Pe.z, zf, zg, Distance);
         //printf("Labelling Edge %d: distance between faces is %f\n", e, Distance);

         if (Distance > 0)
         {
			if (Object->Labelling.EdgeLabel[e] & LineIsArrow)  Object->Eprobabilities[e].Amerit += (Distance/LC->tcOo);
		 }
		 else
		 {
			if (Object->Labelling.EdgeLabel[e] & LineIsBackArrow)  Object->Eprobabilities[e].Bmerit += (-Distance/LC->tcOo);
		 }

         // if the edge is way out of the plane of a face, it must occlude that face
         MakeEdgeVector(Object,e);
         OopF = FoMPerpendicular3D(&Object->Edges[e].EdgeVector3D,&Fplane.Normal) * tcOp;
         OopR = FoMPerpendicular3D(&Object->Edges[e].EdgeVector3D,&Gplane.Normal) * tcOp;
         Oop  = Pmul(OopF,OopR);
         if (Object->Labelling.EdgeLabel[e] & LineIsArrow)  Object->Eprobabilities[e].Amerit += OopR;
         if (Object->Labelling.EdgeLabel[e] & LineIsBackArrow)  Object->Eprobabilities[e].Amerit += OopF;

         if (OldStyle)
         {
            // For some projections, the face normal vector comes straight out
            // of the screen, and the angle calculation blows up as a result
            if (((Fplane.Normal.x != 0.0) || (Fplane.Normal.y != 0.0))
             && ((Gplane.Normal.x != 0.0) || (Gplane.Normal.y != 0.0)))
            {
               Angle = CalculateLineDirection(Object,&Object->Labelling,e);
		       Df = atan2(Fplane.Normal.x,-Fplane.Normal.y)*180.0/M_PI;
		       Dr = atan2(Gplane.Normal.x,-Gplane.Normal.y)*180.0/M_PI;
		       //printf("Line angle %f, Df=%f, Dr=%f\n",Angle,Df,Dr);

               if (clockwise(Df,Angle) && clockwise(Angle,Dr))
               {
                  //printf ("Edge %d: I think this line is convex\n", e);
                  // TBD: work on whether the line is convex or occluding
                  if (Object->Labelling.EdgeLabel[e] & LineIsConvex)  Object->Eprobabilities[e].Cmerit += LC->tcOq * (1.0-Oop);
		       }
		       else
		       if (clockwise(Dr,Angle) && clockwise(Angle,Df))
		       {
			   //printf ("Edge %d: I think this line is concave\n", e);
  			   if (Object->Labelling.EdgeLabel[e] & LineIsConcave)  Object->Eprobabilities[e].Dmerit += LC->tcOq * (1.0-Oop);
		       }
		       else
               if (clockwise(Df,Angle) && clockwise(Dr,Angle))
               {
                  //printf ("Edge %d: I think this line is arrow\n", e);
                  if (Object->Labelling.EdgeLabel[e] & LineIsArrow)  Object->Eprobabilities[e].Amerit += LC->tcOq * (1.0-OopF);
		       }
		       else
               if (clockwise(Angle,Df) && clockwise(Angle,Dr))
               {
                  //printf ("Edge %d: I think this line is backarrow\n", e);
                  if (Object->Labelling.EdgeLabel[e] & LineIsBackArrow)  Object->Eprobabilities[e].Bmerit += LC->tcOq * (1.0-OopR);
		       }
		       else
		       {
			      // nothing to contribute
		       }
		    }
         }
         else // N.B. the new way doesn't detect occluding lines
         {
            VECTOR3D Xproduct;
            GetCrossProduct(&Xproduct,&Fplane.Normal,&Gplane.Normal);
            NormaliseVector(&Xproduct);
            lREAL Dot = DotProduct(&Xproduct,&Object->Edges[e].EdgeVector3D);
            //printf("Edge %d, dot product %f\n",e,Dot);
            //Dot product is +ve for concave lines, -ve for convex lines,
            // and should reach a maximum of 1

            if (Dot >= 0.0)
            {
               if (Object->Labelling.EdgeLabel[e] & LineIsConcave)  Object->Eprobabilities[e].Dmerit += Dot * LC->tcOq * (1.0-Oop);
            }
            else
            {
               if (Object->Labelling.EdgeLabel[e] & LineIsConvex)  Object->Eprobabilities[e].Cmerit += (-Dot) * LC->tcOq * (1.0-Oop);
            }
         }
	  }
	  else if ((f >= 0) && (f < Object->FaceCount))
	  {
		 /* Label it as Arrow. */
		 //printf ("Edge %d: One face, so must be arrow\n", e);
		 if (Object->Labelling.EdgeLabel[e] & LineIsArrow)  Object->Eprobabilities[e].Amerit += LC->tcOq;
      }
	  else if ((r >= 0) && (r < Object->FaceCount))
	  {
		 /* Label it as BackArrow. */
		 //printf ("Edge %d: One face, so must be backarrow\n", e);
		 if (Object->Labelling.EdgeLabel[e] & LineIsBackArrow)  Object->Eprobabilities[e].Bmerit += LC->tcOq;
	  }
	  else
	  {
         //CString sMensaje, sMensajeAux;
         //sMensaje= "Labelling error: Neither face exists for edge ";
         //sMensajeAux.Format("%d", e);
         //sMensaje+=sMensajeAux;
         //AfxMessageBox(sMensaje);
		 //exit(999);
	  }

      //printf("Labelled Edge %d\n", e);
   }
   //PrintVerticesAndEdges(Object,&Object->Labelling);

   return cFALSE;
}



lBOOLEAN CCueLabelsVarley_Lamban::LabelLinesUsingLamban (OBJECT *Object)
{
	if (!GenerateLambanZCoordinates(Object))  return cFALSE;

	if (!FindRegionPlanes(Object,&Object->Labelling))  return cFALSE;

	if (!LabelUsingPlanes(Object))  return cFALSE;

	CCueLabelsVarley_LineToJunctions LtJ;
	LtJ.PropagateLineConstraintsToJunctions(&Object->Labelling);

	return cTRUE;
}