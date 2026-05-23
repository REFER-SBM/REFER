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

//Get faces from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003

#include <stdafx.h>
#include <cstdio>
#include <cmath>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "resource.h"
#include "ReferDoc.h"

#include "DataLabels.h"
#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Maths.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"
#include "CueLabelsVarley_LabellingConstants.h"
#include "CueLabelsVarley_LabellingParameters.h"
#include "CueLabelsVarley_LineToJunctions.h"
#include "CueLabelsVarley_JunctionToLines.h"

#include "CueLabelsVarley_Combinatorially.h"


		MERIT TCLbaLabellingPriorityB     = 0.546;
		MERIT TCLbaLabellingPriorityI     = 0.081;
		MERIT TCLabLabellingPriority      = 0.095;
		MERIT TCLacLabellingPriority      = 0.212;
		MERIT TCLcbLabellingPriority      = 0.432;
		MERIT TCTbaaLabellingPriority     = 0.429;
		MERIT TCTbabLabellingPriority     = 0.158;
		MERIT TCWbcaLabellingPriorityB    = 0.734;
		MERIT TCWbcaLabellingPriorityI    = 0.681;
		MERIT TCWcdcLabellingPriority     = 0.782;
		MERIT TCWdcdLabellingPriority     = 0.435;
		MERIT TCXccccLabellingPriority    = 0.475;
		MERIT TCYabdLabellingPriority     = 0.751;
		MERIT TCYcccLabellingPriority     = 0.995;
		MERIT TCYdddLabellingPriority     = 0.189;


CCueLabelsVarley_Combinatorially::CCueLabelsVarley_Combinatorially(CCueLabelsVarley_LabellingConstants *eLC,
																   CCueLabelsVarley_LabellingParameters *eLP)
{
	//Labelling constants (tuning constants)
	LC= eLC;

	//Labelling parameters
	LP= eLP;
}


CCueLabelsVarley_Combinatorially::~CCueLabelsVarley_Combinatorially()
{
}


void CCueLabelsVarley_Combinatorially::InitialiseLabellingBuffer ()
{
   FavouredLabelling = 0;
   LP->LabellingCount    = 0;
   LP->LabellingCutoff = 0.0;
}


void CCueLabelsVarley_Combinatorially::AddLabellingHypothesis (VERTEX_INDEX Vertex,  TYPEMARK Vexity,  MERIT Merit)
{
   int i;
   for (i=0; i<LP->LabellingHypothesisCount; ++i)
   {
      if ((LP->LabellingHypotheses[i].Vertex == Vertex) && (LP->LabellingHypotheses[i].Vexity == Vexity))
      {
         LP->LabellingHypotheses[i].Merit = Pmul(LP->LabellingHypotheses[i].Merit,Merit);
         return;
      }
   }

   if (LP->LabellingHypothesisCount >= MAX_VERTICES*3)  return;

   i = LP->LabellingHypothesisCount++;
   LP->LabellingHypotheses[i].Vertex = Vertex;
   LP->LabellingHypotheses[i].Vexity = Vexity;
   LP->LabellingHypotheses[i].Merit  = Merit;
}


#define SORTROUTINE __cdecl
int SORTROUTINE LabellingOrder (const void *Labelling1, const void *Labelling2)
{
   LABELLING_HYPOTHESIS *L1 = (LABELLING_HYPOTHESIS *)Labelling1;
   LABELLING_HYPOTHESIS *L2 = (LABELLING_HYPOTHESIS *)Labelling2;
   if (L1->Merit < L2->Merit)  return 1;   /* we want descending order */
   if (L1->Merit > L2->Merit)  return -1;
   return 0;
}


void CCueLabelsVarley_Combinatorially::GenerateLabellingHypothesisTable (OBJECT *Object)
{
   for (VERTEX_INDEX n=0; n<Object->Labelling.VertexCount; ++n)
   {
      switch (Object->Labelling.VertexLabel[n].Shape)
      {
         case JunctionIsL:
            if (Object->IsOnBoundary[n])
            {
               AddLabellingHypothesis(n,Lba,TCLbaLabellingPriorityB);
               AddLabellingHypothesis(n,Lab,TCLabLabellingPriority);
            }
            else
            {
               AddLabellingHypothesis(n,Lba,TCLbaLabellingPriorityI);
               AddLabellingHypothesis(n,Lcb,TCLcbLabellingPriority);
               AddLabellingHypothesis(n,Lac,TCLacLabellingPriority);
            }
         break;

         case JunctionIsW:
            if (Object->IsOnBoundary[n])
            {
               AddLabellingHypothesis(n,Wbca,TCWbcaLabellingPriorityB);
            }
            else
            {
               AddLabellingHypothesis(n,Wcdc,TCWcdcLabellingPriority);
               AddLabellingHypothesis(n,Wdcd,TCWdcdLabellingPriority);
               AddLabellingHypothesis(n,Wbca,TCWbcaLabellingPriorityI);
            }
         break;

         case JunctionIsY:
            if (Object->IsOnBoundary[n])
            {
               AddLabellingHypothesis(n,Yabd,TCYabdLabellingPriority);
               AddLabellingHypothesis(n,Ybda,TCYabdLabellingPriority);
               AddLabellingHypothesis(n,Ydab,TCYabdLabellingPriority);
            }
            else
            {
               AddLabellingHypothesis(n,Yccc,TCYcccLabellingPriority);
               AddLabellingHypothesis(n,Yddd,TCYdddLabellingPriority);
            }
         break;

         case JunctionIsT4:
            AddLabellingHypothesis(n,Tbaa,TCTbaaLabellingPriority);
            AddLabellingHypothesis(n,Tbab,TCTbabLabellingPriority);
         break;

         case JunctionIsX:
            if (Object->IsOnBoundary[n])
            {
            }
            else
            {
               AddLabellingHypothesis(n,Xcccc,TCXccccLabellingPriority);
            }
         break;

         case JunctionIs5way:
            if (Object->IsOnBoundary[n])
            {
            }
            else
            {
               AddLabellingHypothesis(n,Xccccc,0.6276);
            }
         break;
      }
   }

   for (int i=0;  i<Object->CofacialCount;  ++i)
   {
      COFACIAL_CONFIGURATION *C = &Object->CofacialConfigurations[i];
      //printf("** Cofac feature, inner=%d(%s), outer=%d(%s)\n", C->InnerVertex,TypeToString(C->ExpectedInnerType),C->OuterVertex,TypeToString(C->ExpectedOuterType));
      AddLabellingHypothesis(C->InnerVertex,C->ExpectedInnerType->Vexity,C->Merit);
      AddLabellingHypothesis(C->OuterVertex,C->ExpectedOuterType->Vexity,C->Merit);
   }

   for (int i=0;  i<Object->FeatureCount;  ++i)
   {
      FEATURE *F = &Object->Features[i];

      switch (F->FeatureType)
      {
         case FEATURE_WY:  /* bundling only */   break;

         case FEATURE_UNDERSLOT:
            //printf("** Underslot feature, L=%d, W=%d\n", F->L, F->W);
            AddLabellingHypothesis(F->W,Wbca,F->Merit);
            AddLabellingHypothesis(F->L,Lba,F->Merit);
         break;

         case FEATURE_VALLEY:
            //printf("** Valley feature, L=%d, W=%d\n", F->L, F->W);
            AddLabellingHypothesis(F->W,Wbca,F->Merit);
            AddLabellingHypothesis(F->L,Lba,F->Merit);
         break;

         default:
            printf("TBD: feature in labelling hypothesis merit?\n");
         break;
      }
   }

   qsort (&LP->LabellingHypotheses[0], LP->LabellingHypothesisCount, sizeof(LABELLING_HYPOTHESIS), LabellingOrder);

   //for (n=0; n<LabellingHypothesisCount; ++n)
   //{
   //   LABELLING_HYPOTHESIS *L = &LabellingHypotheses[n];
   //   printf("V%d is %8x, merit %f\n", L->Vertex, L->Vexity, L->Merit);
   //}
}


lBOOLEAN CCueLabelsVarley_Combinatorially::AmbiguousType (TYPEMARK Mark)
{
   while (Mark)
   {
      if (Mark == 1)  return cFALSE;
      if (Mark & 1)   return cTRUE;
      Mark >>= 1;
   }
   return cTRUE;
}


lBOOLEAN CCueLabelsVarley_Combinatorially::SketchLabellingIsAmbiguous (LABELLING *Labelling)
{
   int n;
   for (n=0; n<Labelling->VertexCount; ++n)  if (AmbiguousType(Labelling->VertexLabel[n].Vexity))  return cTRUE;
   for (n=0; n<Labelling->EdgeCount; ++n)  if (AmbiguousType(Labelling->EdgeLabel[n]))  return cTRUE;
   return cFALSE;
}


lINTEGER BitsSet (lINTEGER Value)
{
   lINTEGER B = 0;

   while (Value)
   {
      if (Value & 1)  ++B;
      Value >>= 1;
   }

   return B;
}


MERIT CCueLabelsVarley_Combinatorially::SimplicityLabellingMerit (LABELLING *Labelling, int VertexCount)
{
   int n;
   int   Funnies = 0;
   TYPEMARK SimpleJunctions = 0;
   TYPEMARK ComplexJunctions[MAX_VERTICES];
   int   ComplexJunctionCount = 0;
   double D;
   //printf("Simplicity Labelling Merit? (%d vertices)\n", VertexCount);

   for (n=0; n<VertexCount; ++n)
   {
      TYPEMARK Vexity = Labelling->VertexUnderlyingType[n];
      if (BitsSet(Vexity) == 1)
         SimpleJunctions |= Vexity;
      else
         ComplexJunctions[ComplexJunctionCount++] = Vexity;
   }

   /* Count the number of complex junctions which can't be */
   /* interpreted in terms of the known simple junctions */
   for (n=0; n<ComplexJunctionCount; ++n)
   {
      if ((ComplexJunctions[n] & SimpleJunctions) == 0)  ++Funnies;
   }
   //printf("Simplicity Labelling Merit: %d complex junctions, %d funnies\n", ComplexJunctionCount, Funnies);

   /* in practice, the number of bits set is in the range 1..13, and Funnies is always zero */
   D = pow(Funnies + BitsSet(SimpleJunctions),-LP->TCMinimiseUnderlyingTypes);
   //printf("Simplicity Labelling Merit is %f\n", D);

   return D;
}


int CCueLabelsVarley_Combinatorially::SubgraphsWithOccludingBoundaries (OBJECT *Object)
{
   lBOOLEAN Occluding[MAX_GRAPHLOOPS];
   int n;
   int Count = 0;

   for (n=1; n<=Object->SubgraphCount; ++n)  Occluding[n] = cTRUE;

   for (n=0; n<Object->Labelling.EdgeCount; ++n)
   {
      if (Object->Edges[n].OnSubgraphBoundary)
      {
         if (Object->Labelling.EdgeLabel[n] != LineIsArrow)
			 Occluding[Object->Edges[n].EdgeSubgraph] = cFALSE;
      }

      //printf("** SUBGRAPHS: Edge %d, On Boundary %d, %s, Subgraph %d Occluding %d\n",
      //        n,
      //        Object->Edges[n]->OnSubgraphBoundary,
      //        LineTypeToString(Object->Labelling.EdgeLabel[n]),
      //        Object->Edges[n]->EdgeSubgraph,
      //        Occluding[Object->Edges[n]->EdgeSubgraph]);
   }

   for (n=1; n<=Object->SubgraphCount; ++n)  if (Occluding[n])  ++Count;

   //printf("** %d SUBGRAPHS, %d SEPARATE GRAPHS **\n", Object->SubgraphCount, Count);

   return Count;
}


lBOOLEAN CCueLabelsVarley_Combinatorially::VertexLabelsIdentical (JUNCTION_TYPEMARK *A, JUNCTION_TYPEMARK *B)
{
   return (A->Shape == B->Shape) && (A->Vexity == B->Vexity);
}


MERIT CCueLabelsVarley_Combinatorially::CofacialConfigurationLabellingMerit (LABELLING *Labelling, COFACIAL_CONFIGURATION *C)
{
   if (C)
   {
      VERTEX_INDEX Vo = C->OuterVertex;
      VERTEX_INDEX Vi = C->InnerVertex;
      MERIT M;

      M = (VertexLabelsIdentical(&Labelling->VertexLabel[Vo],C->ExpectedOuterType) &&
           VertexLabelsIdentical(&Labelling->VertexLabel[Vi],C->ExpectedInnerType))
             ? 1.0 : pow(1.0-C->Merit,LP->TCLabellingFromBadCofacial);
      //printf(" ... merit for cofacial configuration is %f\n", M);
      return M;
   }
   else
   {
      printf("OOPS: bad cofacial configuration (is it really there?)\n");
      return 1.0;
   }
}


double CCueLabelsVarley_Combinatorially::EdgeLabellingMerit (LABELLING *Labelling, int EdgeCount)
{
   int n = 0;
   int Arrows = 0;
   double D;

   for (n=0; n<EdgeCount; ++n)  if (Labelling->EdgeLabel[n] == LineIsArrow)  ++Arrows;

   //printf("Edge Labelling Merit? %d arrows out of %d edges\n", Arrows, EdgeCount);
   D = pow(1.0-((float)Arrows)/((float)EdgeCount),LP->TCOccludingNonBoundaryLines);

   //printf("Edge Labelling Merit is %f\n", D);

   return D;
}


void CCueLabelsVarley_Combinatorially::GetLabellingMerit (OBJECT *Object, LABELLING *Labelling, int VertexCount, int EdgeCount)
{
   MERIT CCMerit = 1.0;

   if ((Object->SubgraphCount > 1) && (SubgraphsWithOccludingBoundaries(Object) > 1))
   {
      Labelling->Merit = 0.0;
      return;
   }

   for (int i=0;  i<Object->CofacialCount;  ++i)
   {
      COFACIAL_CONFIGURATION *C = &Object->CofacialConfigurations[i];
      CCMerit *= CofacialConfigurationLabellingMerit(Labelling,C);
   }
   //printf("Cofacial Configuration Merit is %f\n", CCMerit);
   //printf("Edge Labelling Merit is %f\n", EdgeLabellingMerit(Labelling,EdgeCount));
   //printf("Simplicity Labelling Merit is %f\n", SimplicityLabellingMerit(Labelling,VertexCount));
   Labelling->Merit *= EdgeLabellingMerit(Labelling,EdgeCount) * SimplicityLabellingMerit(Labelling,VertexCount) * CCMerit;

   //printf("... merit for labelling, %d vertices, is %6.4f\n", VertexCount, Labelling->Merit);
}


lBOOLEAN CCueLabelsVarley_Combinatorially::InterpretAmbiguousJunctionType (int VertexCount, LABELLING *One, LABELLING *Two)
{
   for (int i=0; i<LP->LabellingHypothesisCount; ++i)
   {
      VERTEX_INDEX V = LP->LabellingHypotheses[i].Vertex;
      TYPEMARK Want  = LP->LabellingHypotheses[i].Vexity;
      TYPEMARK Got   = One->VertexLabel[V].Vexity;

      if ((Want != Got) && ((Want & Got) != 0))
      {
         One->VertexLabel[V].Vexity = Want;
         One->VertexLabellingIsFinalised[V] = cFALSE;

         Two->VertexLabel[V].Vexity = Got & ~Want;
         Two->VertexLabellingIsFinalised[V] = cFALSE;

         //printf("Alternative labellings: one(%8x), two(%8x)\n", Want, Got & ~Want);
         return cTRUE;
      }
   }

   return cFALSE;
}


lBOOLEAN CCueLabelsVarley_Combinatorially::GenerateAlternativeInterpretations (LABELLING *One, LABELLING *Two)
{
   // Try to use intelligence in picking the most plausible junction labels first
   if (InterpretAmbiguousJunctionType(One->VertexCount,One,Two))
   {
      return cTRUE;
   }

   // That didn't work, so use an unintelligent default mechanism for choosing between interpretations
   for (int n=0; n<One->EdgeCount; ++n)
   {
      TYPEMARK TypeEn = One->EdgeLabel[n];

      if (AmbiguousType(TypeEn))
      {
         TYPEMARK T1, T2;
         InterpretAmbiguousEdgeType(TypeEn,&T2,&T1);
         One->EdgeLabel[n] = T1;
         Two->EdgeLabel[n] = T2;
         Two->EdgeLabelIsFinalised[n] = cFALSE;
         One->EdgeLabelIsFinalised[n] = cFALSE;
         return cTRUE;
      }
   }

   //Nothing Ambiguous
   return cFALSE;
}


void CCueLabelsVarley_Combinatorially::InterpretAmbiguousEdgeType (TYPEMARK T, TYPEMARK *T1, TYPEMARK *T2)
{
   if (T == LineIsUnknown)  {  *T1 = LineIsConvex|LineIsConcave;  *T2 = LineIsArrow|LineIsBackArrow; }
   else
   if (T & LineIsConvex)    {  *T1 = LineIsConvex;  *T2 = T & ~LineIsConvex; }
   else
   if (T & LineIsConcave)   {  *T1 = LineIsConcave; *T2 = T & ~LineIsConcave; }
   else
   if (T & LineIsArrow)     {  *T1 = LineIsArrow;   *T2 = T & ~LineIsArrow;   }
   else
   {
      AfxMessageBox("Labelling error: OOPS!! Can't interpret ambiguous edge type");
      //exit(BADNEWS);
   }
}


void CCueLabelsVarley_Combinatorially::StoreLabelling (LABELLING *Labelling)
{
   int InsertA,InsertB,InsertHere,j;

   //printf("Add Labelling to Tree\n");
   //printf(" ... merit of latest labelling is %f\n", Labelling->Merit);
   //printf(" ... %d labellings already in tree\n", LabellingCount);
   //if (LabellingCount)  printf(" ... best labelling merit is %f\n", LabellingBuffer[0]->Merit);
   //if (LabellingCount > 1)  printf(" ... worst labelling merit stored is %f\n", LabellingBuffer[LabellingCount-1]->Merit);

   if (!LP->LabellingCount)
   {
      LP->LabellingBuffer[LP->LabellingCount++] = *Labelling;
      LP->LabellingCutoff = 2.0*Labelling->Merit - 1.0;
      return;
   }

   if (Labelling->Merit < LP->LabellingBuffer[LP->LabellingCount-1].Merit)
   {
      if (LP->LabellingCount < MAX_STORED_LABELLINGS)
      {
         LP->LabellingBuffer[LP->LabellingCount++] = *Labelling;
      }
      return;
   }

   InsertA = 0;
   InsertB = LP->LabellingCount-1;

   while (InsertA < InsertB)
   {
      int InsertM = (InsertA+InsertB)/2;

      if (Labelling->Merit > LP->LabellingBuffer[InsertM].Merit)
         InsertB = InsertM;
      else
         InsertA = InsertM+1;
   }
   InsertHere = InsertA;

   if (LP->LabellingCount >= MAX_STORED_LABELLINGS)  --LP->LabellingCount;

   //printf("+++ Storing in slot %d/%d +++\n", InsertHere, LabellingCount+1);

   if (InsertHere == 0)  LP->LabellingCutoff = 2.0*Labelling->Merit - 1.0;
   for (j=LP->LabellingCount-1; j>=InsertHere; --j)  LP->LabellingBuffer[j+1] = LP->LabellingBuffer[j];
   LP->LabellingBuffer[InsertHere] = *Labelling;
   ++LP->LabellingCount;
}


int CCueLabelsVarley_Combinatorially::ResolveAmbiguousLabellingsAndOutput (OBJECT *Object, LABELLING *Labelling, int Level, int MaxConsidered)
{
   //printf("Resolve Ambiguous Labellings and Output, Merit=%6.4f\n",Labelling->Merit);
   if (!SketchLabellingIsValid(Labelling))  return 0;

   if ((!CountLabellings) && (MaxConsidered <= 0))
   {
      //printf("-- Considered too many labellings, got bored, gave up --\n");
      return 0;
   }

   //printf("-- Output Sketch Tree %d: Valid, Merit=%6.4f, Testing for Ambiguity --\n", Level, Labelling->Merit);

   if (SketchLabellingIsAmbiguous(Labelling))
   {
      LABELLING Alternative;
      int     MainBranchCount;
      int Count = 0;

      Alternative = *Labelling;

      MainBranchCount = max(1,(int)(((float)MaxConsidered)*LP->TCLabellingPreferredBranchSize));

      if (GenerateAlternativeInterpretations(Labelling,&Alternative))
      {
         //printf("... trying favourite at level %d (%d), merit=%6.4f\n",Level,MainBranchCount,Labelling->Merit);

		  if (LabelLinesUsingKanatani(Object,Labelling))    Count += ResolveAmbiguousLabellingsAndOutput (Object, Labelling, Level+1, MainBranchCount);
         //printf("... now trying alternative at level %d (%d), merit=%6.4f\n", Level, MaxConsidered-MainBranchCount,Alternative.Merit);
         if (LabelLinesUsingKanatani(Object,&Alternative))  Count += ResolveAmbiguousLabellingsAndOutput (Object, &Alternative, Level+1, MaxConsidered-MainBranchCount);
      }
      else
      {
         Count = 1;
      }
      //printf(" ... popped back to level %d, count is %d\n", Level, Count);
      return Count;
   }
   else
   {
      //printf("... unambiguous, looking for occlusion in regions\n");
      GetLabellingMerit(Object,Labelling,Object->Labelling.VertexCount,Object->Labelling.EdgeCount);

      //printf("== Labelling is UNAMBIGUOUS at level %d, %d-hedral, merit %f\n", Level, Object->MaxHedralVertex, Object->Labelling.Merit);
      if (!CountLabellings) StoreLabelling(Labelling);
      return 1;
   }
}


//void CCueLabelsVarley_Combinatorially::GetProgramParameter (OBJECT *Object, char *Name)
//{
//	strcpy_s(Name, _countof(InFileName), InFileName);
//}


lBOOLEAN CCueLabelsVarley_Combinatorially::RetrieveFavouredLabelling (OBJECT *Object)
{
   //printf("--- want favoured labelling %d, count is %d ---\n", FavouredLabelling, LabellingCount);

   if (FavouredLabelling < LP->LabellingCount)
   {
      Object->Labelling = LP->LabellingBuffer[FavouredLabelling++];
      return cTRUE;
   }

   return cFALSE;
}


lBOOLEAN CCueLabelsVarley_Combinatorially::SketchLabellingIsValid (LABELLING *Labelling)
{
	int n;

	for (n=0; n<Labelling->VertexCount; ++n)
	{
		if (!Labelling->VertexLabel[n].Vexity)
		{
			//printf("Vertex %d has no labelling!\n",n);
			return cFALSE;
		}
	}

	for (n=0; n<Labelling->EdgeCount; ++n)
	{
		if (!Labelling->EdgeLabel[n])
		{
			//printf("Edge %d has no labelling!\n",n);
			return cFALSE;
		}
	}

	return cTRUE;
}


lBOOLEAN CCueLabelsVarley_Combinatorially::LabelLinesUsingKanatani (OBJECT *Object,
																	LABELLING *Labelling)
// A Clowes-Huffman line labeller, basically trihedral with a few extensions
{
	int max = 1000;
	CCueLabelsVarley_LineToJunctions LtJ;
	CCueLabelsVarley_JunctionToLines JtL(LC);

	for (int n=0; n<max; ++n)
	{
		AnyTypemarkChanges(true, cFALSE);

		//CCueLabelsVarley_LineToJunctions LtJ;
		LtJ.PropagateLineConstraintsToJunctions(Labelling);

		//CCueLabelsVarley_JunctionToLines JtL(LC);
		JtL.PropagateJunctionConstraintsToLines(Object,Labelling);

		if (!AnyTypemarkChanges(false, cFALSE))
		{
			return cTRUE;
		}

		if (!SketchLabellingIsValid(Labelling))
		{
			//Interpretation Is Invalid
			return cFALSE;
		}

		if (Labelling->Merit < LP->LabellingCutoff)
		{
			//Interpretation Lacks Merit
			return cFALSE;
		}
	}

	AfxMessageBox("Labelling error: failure in LabelLinesUsingKanatani ");
	//exit(999);
	return cFALSE;
}


lBOOLEAN CCueLabelsVarley_Combinatorially::LabelLinesCombinatorially (OBJECT *Object)
{
	CountLabellings             = cFALSE;
	GenerateLabellingAutoUpdate = cFALSE;
	AbandonOnLabellingFailure   = cFALSE;

	lBOOLEAN OK = cTRUE;
	int Count;

	Object->Labelling.Merit = 1.0;
	InitialiseLabellingBuffer();
	GenerateLabellingHypothesisTable(Object);

	Count = LabelLinesUsingKanatani(Object,&Object->Labelling)
			? ResolveAmbiguousLabellingsAndOutput(Object,&Object->Labelling,0,LP->MaxConsideredLabellings)
			: 0;

	switch (Count)
	{
		case 0:
			//printf("LABELSKETCH%c%d: No valid interpretations of sketch\n", Object->LabellingType, Object->MaxHedralVertex);

			////if (GenerateLabellingAutoUpdate)
			////{
			////	char S[64];
			////	//char C = GetProgramOptionValue(Object,OPTION_LABEL_AUTOUPDATE);
			////	GetProgramParameter(Object,S);
			////	//printf("AUTO /^%s/s/|%c:./|%c:%c\n", S, C, C, '0');
			////}

			//////printf("ERROR: Labelsketch failed to find an interpretation\n");
			////if (AbandonOnLabellingFailure) exit(Object->Labelling.VertexCount+Object->Labelling.EdgeCount);
			////return cFALSE;

		case 1:
			// it might be that multiple labellings were tried, and
			// the only one which was successful was a previous one

			//printf("LABELSKETCH%c%d(%x): Sketch labelled unambiguously\n", Object->LabellingType, Object->MaxHedralVertex, Object->Labelling.FinalVertexTypes);
			if (CountLabellings)  exit(EXIT_SUCCESS);

			OK = RetrieveFavouredLabelling(Object);
			Object->Labelling.Merit = OK ? 1.0 : 0.0;
			return OK;

		default:
			if (CountLabellings)  exit(EXIT_SUCCESS);
			//printf("LABELSKETCH%c%d(%x): Sketch has %d interpretations - choosing best\n", Object->LabellingType, Object->MaxHedralVertex, Object->Labelling.FinalVertexTypes, Count);
			OK = RetrieveFavouredLabelling(Object);
			return OK;
	}
}
