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

#include <vector>
#include "DataLabels.h"

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"
#include "CueLabelsVarley_LabellingConstants.h"

#include "CueLabelsVarley_LabelLines.h"
#include "CueLabelsVarley_LabelJunctions.h"

#include "CueLabelsVarley_JunctionToLines.h"


CCueLabelsVarley_JunctionToLines::CCueLabelsVarley_JunctionToLines (CCueLabelsVarley_LabellingConstants *eLC)
{
	//Labelling constants (tuning constants)
	LC= eLC;
}


CCueLabelsVarley_JunctionToLines::~CCueLabelsVarley_JunctionToLines()
{
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromWJunction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np = Labelling->VENumber[Nvert][0];
   EDGE_INDEX Nq = Labelling->VENumber[Nvert][1];
   EDGE_INDEX Nr = Labelling->VENumber[Nvert][2];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Wbca:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wbca,0,0,0);
         Labelling->Merit *= LC->tcWbca;
         break;

      case Wabc:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseWLabel(Labelling,Nvert,Wabc,0,0,0);
		 Labelling->Merit *= LC->tcWabc;
         break;

      case Wabd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseWLabel(Labelling,Nvert,Wabd,0,0,0);
		 Labelling->Merit *= LC->tcWabd;
         break;

      case Wacc:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseWLabel(Labelling,Nvert,Wacc,0,0,0);
		 Labelling->Merit *= LC->tcWacc;
         break;

      case Wacd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseWLabel(Labelling,Nvert,Wacd,0,0,0);
		 Labelling->Merit *= LC->tcWacd;
         break;

      case Wadc:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseWLabel(Labelling,Nvert,Wadc,0,0,0);
		 Labelling->Merit *= LC->tcWadc;
         break;

      case Wbaa:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wbaa,0,0,0);
		 Labelling->Merit *= LC->tcWbaa;
         break;

      case Wbba:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wbba,0,0,0);
		 Labelling->Merit *= LC->tcWbaa;
         break;

      case Wbcd:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseWLabel(Labelling,Nvert,Wbcd,0,0,0);
		 Labelling->Merit *= LC->tcWbcd;
         break;

      case Wbda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wbda,0,0,0);
		 Labelling->Merit *= LC->tcWbda;
         break;

      case Wbdc:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseWLabel(Labelling,Nvert,Wbdc,0,0,0);
		 Labelling->Merit *= LC->tcWbdc;
         break;

      case Wbdd:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseWLabel(Labelling,Nvert,Wbdd,0,0,0);
		 Labelling->Merit *= LC->tcWbdd;
         break;

      case Wcab:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wcab,0,0,0);
		 Labelling->Merit *= LC->tcWabc;
         break;

      case Wcac:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseWLabel(Labelling,Nvert,Wcac,0,0,0);
		 Labelling->Merit *= LC->tcWcac;
         break;

      case Wcbc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseWLabel(Labelling,Nvert,Wcbc,0,0,0);
		 Labelling->Merit *= LC->tcWcac;
         break;

      case Wcbd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseWLabel(Labelling,Nvert,Wcbd,0,0,0);
		 Labelling->Merit *= LC->tcWcbd;
         break;

      case Wccb:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wccb,0,0,0);
		 Labelling->Merit *= LC->tcWacc;
         break;

      case Wcda:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wcda,0,0,0);
		 Labelling->Merit *= LC->tcWbdc;
         break;

      case Wcdb:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wcdb,0,0,0);
		 Labelling->Merit *= LC->tcWadc;
         break;

      case Wcdc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseWLabel(Labelling,Nvert,Wcdc,0,0,0);
		 Labelling->Merit *= LC->tcWcdc;
         break;

      case Wdab:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wdab,0,0,0);
		 Labelling->Merit *= LC->tcWabd;
         break;

      case Wdac:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseWLabel(Labelling,Nvert,Wdac,0,0,0);
		 Labelling->Merit *= LC->tcWcbd;
         break;

      case Wdad:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseWLabel(Labelling,Nvert,Wdad,0,0,0);
		 Labelling->Merit *= LC->tcWdad;
         break;

      case Wdbd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseWLabel(Labelling,Nvert,Wdbd,0,0,0);
		 Labelling->Merit *= LC->tcWdad;
         break;

      case Wdca:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wdca,0,0,0);
		 Labelling->Merit *= LC->tcWbcd;
         break;

      case Wdcb:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wdcb,0,0,0);
		 Labelling->Merit *= LC->tcWacd;
         break;

      case Wdcd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseWLabel(Labelling,Nvert,Wdcd,0,0,0);
		 Labelling->Merit *= LC->tcWdcd;
         break;

      case Wdda:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseWLabel(Labelling,Nvert,Wdda,0,0,0);
		 Labelling->Merit *= LC->tcWbdd;
         break;

      case Wadc|Wbdc:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         break;

      case Wbca|Wbda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         break;

      case Wbca|Wdca:
         ChangeLine(Labelling,Np,LineIsConcave|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         break;

      case Wbca|Wbcd:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave|LineIsA(Labelling,Nr,Nvert));
         break;

      case Wbca|Wdcd:
         ChangeLine(Labelling,Np,LineIsConcave|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave|LineIsA(Labelling,Nr,Nvert));
         break;

      case Wbcd|Wbdd:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         break;

      case Wbda|Wbdc:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsA(Labelling,Nr,Nvert));
         break;

      case Wbda|Wbdd:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave|LineIsA(Labelling,Nr,Nvert));
         break;

      case Wbda|Wcda:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         break;

      case Wbda|Wdda:
         ChangeLine(Labelling,Np,LineIsConcave|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         break;

      case Wcda|Wcdb:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsArrow|LineIsBackArrow);
         break;

      case Wdca|Wdcb:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsArrow|LineIsBackArrow);
         break;

      case Wdca|Wdda:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         break;

      case Wbca|Wcdc|Wdcd:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave|LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave|LineIsA(Labelling,Nr,Nvert));
         break;

      default:
         //printf("EEK! Don't know what to do with W junction! (%x)\n", Labelling->VertexLabel[Nvert].Vexity);
         break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromLJunction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np = Labelling->VENumber[Nvert][0];
   EDGE_INDEX Nq = Labelling->VENumber[Nvert][1];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Lba:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         FinaliseLLabel(Labelling,Nvert,Lba,0,0);
         Labelling->Merit *= LC->tcLba;
      break;

      case Lac:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         FinaliseLLabel(Labelling,Nvert,Lac,0,0);
		 Labelling->Merit *= LC->tcLac;
      break;

      case Lab:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         FinaliseLLabel(Labelling,Nvert,Lab,0,0);
		 Labelling->Merit *= LC->tcLab;
      break;

      case Lcb:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         FinaliseLLabel(Labelling,Nvert,Lcb,0,0);
		 Labelling->Merit *= LC->tcLac;
      break;

      case Lbd:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         FinaliseLLabel(Labelling,Nvert,Lbd,0,0);
		 Labelling->Merit *= LC->tcLbd;
      break;

      case Lda:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         FinaliseLLabel(Labelling,Nvert,Lda,0,0);
		 Labelling->Merit *= LC->tcLbd;
      break;

      case Laa:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         FinaliseLLabel(Labelling,Nvert,Laa,0,0);
		 Labelling->Merit *= LC->tcLaa;
      break;

      case Lbb:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         FinaliseLLabel(Labelling,Nvert,Lbb,0,0);
		 Labelling->Merit *= LC->tcLaa;
      break;

      case Lba|Lda:
         ChangeLine(Labelling,Np,LineIsConcave|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
      break;

      case Lba|Lbd:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave|LineIsA(Labelling,Nq,Nvert));
      break;

      case Lab|Lac:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsB(Labelling,Nq,Nvert));
      break;

      case Lab|Lcb:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
      break;

      case Lac|Lbd:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
      break;

      case Lcb|Lda:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsArrow|LineIsBackArrow);
      break;

      case Lba|Laa:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
      break;

      case Lba|Lbb:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsArrow|LineIsBackArrow);
      break;

      case Lab|Laa:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsArrow|LineIsBackArrow);
      break;

      case Lab|Lbb:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow);
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
      break;

      case Lab|Lba|Lbd|Lda:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsArrow|LineIsBackArrow|LineIsConcave);
      break;

      case Lba|Lab|Lac|Lbd|Lda:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsArrow|LineIsBackArrow|LineIsConvex|LineIsConcave);
      break;

      case Lba|Lab|Lcb|Lbd|Lda:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow|LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsArrow|LineIsBackArrow|LineIsConcave);
      break;

      case Lba|Lab|Lac|Lcb|Lda:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow|LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsArrow|LineIsBackArrow|LineIsConvex);
      break;

      case Ltrihedral:
         /* nothing can be deduced from this */ ;
      break;

      default:
         //printf("EEK! Don't know what to do with L junction! (%x)\n",Labelling->VertexLabel[Nvert].Vexity);
      break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromYJunction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np,Nq,Nr;

   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFromYJunction");
   Np = Labelling->VENumber[Nvert][0];
   Nq = Labelling->VENumber[Nvert][1];
   Nr = Labelling->VENumber[Nvert][2];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Ytrihedral:
         /* nothing can be deduced from this */;
      break;

      case Yaab:
	     ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
	     ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
	     ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
	     FinaliseYLabel(Labelling,Nvert,Yaab,0,0,0);
		 Labelling->Merit *= LC->tcYaab;
         break;

      case Yaba:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsB(Labelling,Nq,Nvert));
	     FinaliseYLabel(Labelling,Nvert,Yaab,0,0,0);
		 Labelling->Merit *= LC->tcYaab;
         break;

      case Yabb:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
	     FinaliseYLabel(Labelling,Nvert,Yabb,0,0,0);
		 Labelling->Merit *= LC->tcYaab;
         break;

      case Yabc:
	     ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
	     ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
	     ChangeLine(Labelling,Nr,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Yabc,0,0,0);
		 Labelling->Merit *= LC->tcYabc;
         break;

      case Yabd:
	     ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
	     ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
	     ChangeLine(Labelling,Nr,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yabd,0,0,0);
		 Labelling->Merit *= LC->tcYabd;
         break;

      case Yacc:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Yacc,0,0,0);
		 Labelling->Merit *= LC->tcYacc;
         break;

      case Yacd:
	     ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
	     ChangeLine(Labelling,Nq,LineIsConvex);
	     ChangeLine(Labelling,Nr,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yacd,0,0,0);
		 Labelling->Merit *= LC->tcYacd;
         break;

      case Yadd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yadd,0,0,0);
		 Labelling->Merit *= LC->tcYadd;
         break;

      case Ybaa:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsB(Labelling,Np,Nvert));
	     FinaliseYLabel(Labelling,Nvert,Yaab,0,0,0);
		 Labelling->Merit *= LC->tcYaab;
         break;

      case Ybab:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsB(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsB(Labelling,Np,Nvert));
	     FinaliseYLabel(Labelling,Nvert,Yabb,0,0,0);
		 Labelling->Merit *= LC->tcYaab;
         break;

      case Ybba:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsB(Labelling,Nq,Nvert));
	     FinaliseYLabel(Labelling,Nvert,Yabb,0,0,0);
		 Labelling->Merit *= LC->tcYaab;
         break;

      case Ybca:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Yabc,0,0,0);
		 Labelling->Merit *= LC->tcYabc;
         break;

      case Ybcc:
	     ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
	     ChangeLine(Labelling,Nq,LineIsConvex);
	     ChangeLine(Labelling,Nr,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Ybcc,0,0,0);
		 Labelling->Merit *= LC->tcYacc;
         break;

      case Ybda:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yabd,0,0,0);
		 Labelling->Merit *= LC->tcYabd;
         break;

      case Ybdc:
	     ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
	     ChangeLine(Labelling,Nq,LineIsConcave);
	     ChangeLine(Labelling,Nr,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Ybdc,0,0,0);
		 Labelling->Merit *= LC->tcYacd;
         break;

      case Ybdd:
	     ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
	     ChangeLine(Labelling,Nq,LineIsConcave);
	     ChangeLine(Labelling,Nr,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Ybdd,0,0,0);
		 Labelling->Merit *= LC->tcYadd;
         break;

      case Ycab:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsB(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Yabc,0,0,0);
		 Labelling->Merit *= LC->tcYabc;
         break;

      case Ycac:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsConvex);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Yacc,0,0,0);
		 Labelling->Merit *= LC->tcYacc;
         break;

      case Ycbc:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsConvex);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Ybcc,0,0,0);
		 Labelling->Merit *= LC->tcYacc;
         break;

      case Ycbd:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsConcave);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Ybdc,0,0,0);
		 Labelling->Merit *= LC->tcYacd;
         break;

      case Ycca:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsConvex);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Yacc,0,0,0);
		 Labelling->Merit *= LC->tcYacc;
         break;

      case Yccb:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsB(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsConvex);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Ybcc,0,0,0);
		 Labelling->Merit *= LC->tcYacc;
         break;

      case Yccc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseYLabel(Labelling,Nvert,Yccc,0,0,0);
		 Labelling->Merit *= LC->tcYccc;
      break;

      case Ycda:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsConvex);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yacd,0,0,0);
		 Labelling->Merit *= LC->tcYacd;
         break;

      case Ydab:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsB(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yabd,0,0,0);
		 Labelling->Merit *= LC->tcYabd;
         break;

      case Ydac:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsConvex);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yacd,0,0,0);
		 Labelling->Merit *= LC->tcYacd;
         break;

      case Ydad:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsConcave);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yadd,0,0,0);
		 Labelling->Merit *= LC->tcYadd;
         break;

      case Ydbd:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Nr,LineIsConcave);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Np,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Ybdd,0,0,0);
		 Labelling->Merit *= LC->tcYadd;
         break;

      case Ydcb:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsB(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsConcave);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsConvex);
	     FinaliseYLabel(Labelling,Nvert,Ybdc,0,0,0);
		 Labelling->Merit *= LC->tcYacd;
         break;

      case Ydda:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsConcave);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Yadd,0,0,0);
		 Labelling->Merit *= LC->tcYadd;
         break;

      case Yddb:
         ChangeLine(Labelling,Labelling->VENumber[Nvert][0] = Nr,LineIsB(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Labelling->VENumber[Nvert][1] = Np,LineIsConcave);
         ChangeLine(Labelling,Labelling->VENumber[Nvert][2] = Nq,LineIsConcave);
	     FinaliseYLabel(Labelling,Nvert,Ybdd,0,0,0);
		 Labelling->Merit *= LC->tcYadd;
         break;

      case Yddd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseYLabel(Labelling,Nvert,Yddd,0,0,0);
		 Labelling->Merit *= LC->tcYddd;
         break;

      case Yccc|Yddd:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         break;

      case Ydab|Yddd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave|LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave|LineIsB(Labelling,Nr,Nvert));
         break;

      case Yabc|Yabd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         break;

      case Ybca|Ybda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         break;

      case Ycab|Ydab:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         break;

      case Ybdd|Ybda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave|LineIsA(Labelling,Nr,Nvert));
         break;

      case Yddb|Ydab:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave|LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         break;

      case Yacc|Ybcc:
         ChangeLine(Labelling,Np,LineIsArrow|LineIsBackArrow);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         break;

      case Ycac|Ycbc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsArrow|LineIsBackArrow);
         ChangeLine(Labelling,Nr,LineIsConvex);
         break;

      case Ycca|Yccb:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsArrow|LineIsBackArrow);
         break;

      case Yacc|Yacd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         break;

      case Ycca|Ycda:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         break;

      case Ybcc|Ybca:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsA(Labelling,Nr,Nvert));
         break;

      case Yccb|Ycab:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         break;

      case Ybcc|Ybdc:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         break;

      case Ycbc|Ycbd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         break;

      case Yaba|Yabb:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsArrow|LineIsBackArrow);
         break;

      default:
         /* don't propagate anything yet */
         //printf("EEK! Don't know what to do with Y junction! (%x)\n", Labelling->VertexLabel[Nvert].Vexity);
         break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromTJunction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   /* nothing much can be inferred from T-junctions */

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Tbaa:
         FinaliseTLabel(Labelling,Nvert,Tbab,0,0,0);
		 Labelling->Merit *= LC->tcTbaa;
      break;

      case Tbab:
         FinaliseTLabel(Labelling,Nvert,Tbab,0,0,0);
		 Labelling->Merit *= LC->tcTbaa;
      break;

      case Tbac:
         FinaliseTLabel(Labelling,Nvert,Tbac,0,0,0);
		 Labelling->Merit *= LC->tcTbac;
      break;

      case Tbad:
         FinaliseTLabel(Labelling,Nvert,Tbad,0,0,0);
		 Labelling->Merit *= LC->tcTbad;
      break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromT4Junction (OBJECT *Object, LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np = Labelling->VENumber[Nvert][0];
   EDGE_INDEX Nq = Labelling->VENumber[Nvert][1];
   EDGE_INDEX Nr = Labelling->VENumber[Nvert][2];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Tbaa|Tbab|Tbac|Tbad|Tbda|Tbdc|Tdab|Tdac:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave|LineIsA(Labelling,Nq,Nvert));
      break;

      case Tbaa|Tbab|Tbac|Tbad:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
      break;

      case Tbaa:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tbaa,0,0,0);
		 Labelling->Merit *= LC->tcTbaa;
      break;

      case Tbab:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tbab,0,0,0);
		 Labelling->Merit *= LC->tcTbaa;
      break;

      case Tbac:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseTLabel(Labelling,Nvert,Tbac,0,0,0);
		 Labelling->Merit *= LC->tcTbac;
      break;

      case Tbad:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         FinaliseTLabel(Labelling,Nvert,Tbad,0,0,0);
		 Labelling->Merit *= LC->tcTbad;
      break;

      case Tbda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tbda,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTbda;
      break;

      case Tbdc:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseTLabel(Labelling,Nvert,Tbdc,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTbdc;
      break;

      case Tdab:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tdab,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTbda;
      break;

      case Tdac:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseTLabel(Labelling,Nvert,Tdac,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTbdc;
      break;

      case Taba:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Taba,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTaba;
      break;

      case Tabb:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tabb,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTaba;
      break;

      case Tbca:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tbca,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTbca;
      break;

      case Tbcc:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseTLabel(Labelling,Nvert,Tbcc,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTbcc;
      break;

      case Tcab:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tcab,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTbca;
      break;

      case Tcac:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         FinaliseTLabel(Labelling,Nvert,Tcac,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTbcc;
      break;

      case Tcca:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tcca,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTcca;
      break;

      case Tccb:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tccb,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTcca;
      break;

      case Tcda:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tcda,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTcda;
      break;

      case Tdcb:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tdcb,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTcda;
      break;

      case Tdda:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tdda,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTdda;
      break;

      case Tddb:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         FinaliseTLabel(Labelling,Nvert,Tdda,0,0,0);
		 Object->Vertices[Nvert].IsHidden = cFALSE;
		 Labelling->Merit *= LC->tcTdda;
      break;

      case Tbda|Tbdc:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsA(Labelling,Nr,Nvert));
      break;

      case Tdab|Tdac:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsB(Labelling,Nr,Nvert));
      break;

      case Tbac|Tbad:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
      break;

      case Tbac|Tbdc:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave|LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
      break;

      case Tbac|Tcac:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
      break;

      case Tbaa|Tbda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave|LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
      break;

      case Tbaa|Tbca:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
      break;

      case Tbab|Tdab:
         ChangeLine(Labelling,Np,LineIsConcave|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
      break;

      case Tbab|Tcab:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
      break;

      case Tcca|Tccb:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsArrow|LineIsBackArrow);
      break;

      case Tcca|Tcda:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
      break;

      case Tdda|Tddb:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsArrow|LineIsBackArrow);
      break;

      case Tbaa|Tbab|Tbac:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsArrow|LineIsBackArrow|LineIsConvex);
      break;

      case Tbaa|Tbab|Tbad:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsArrow|LineIsBackArrow|LineIsConcave);
      break;

      default:
         //printf("EEK! Don't know what to do with T4 junction! (%x)\n", Labelling->VertexLabel[Nvert].Vexity);
      break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromZ4Junction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np,Nq,Nr,Ns;

   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFromXJunction");

   Np = Labelling->VENumber[Nvert][0];
   Nq = Labelling->VENumber[Nvert][1];
   Nr = Labelling->VENumber[Nvert][2];
   Ns = Labelling->VENumber[Nvert][3];
   ChangeLine(Labelling,Np,LineIsConvex);
   ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
   ChangeLine(Labelling,Nr,LineIsConcave);
   ChangeLine(Labelling,Ns,LineIsA(Labelling,Ns,Nvert));

   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromXJunction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np,Nq,Nr,Ns;

   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFromXJunction");

   Np = Labelling->VENumber[Nvert][0];
   Nq = Labelling->VENumber[Nvert][1];
   Nr = Labelling->VENumber[Nvert][2];
   Ns = Labelling->VENumber[Nvert][3];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Xcccc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseXLabel(Labelling,Nvert,Xcccc,0,0,0,0);
		 Labelling->Merit *= LC->tcXcccc;
      break;

      case Xcccd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConcave);
         FinaliseXLabel(Labelling,Nvert,Xcccd,0,0,0,0);
		 Labelling->Merit *= LC->tcXcccd;
      break;

      case Xccdc:
         Labelling->VertexLabel[Nvert].Vexity = Xcccd;
         Labelling->VENumber[Nvert][0] = Ns;
         Labelling->VENumber[Nvert][1] = Np;
         Labelling->VENumber[Nvert][2] = Nq;
         Labelling->VENumber[Nvert][3] = Nr;
      break;

      case Xcdcc:
         Labelling->VertexLabel[Nvert].Vexity = Xcccd;
         Labelling->VENumber[Nvert][0] = Nr;
         Labelling->VENumber[Nvert][1] = Ns;
         Labelling->VENumber[Nvert][2] = Np;
         Labelling->VENumber[Nvert][3] = Nq;
      break;

      case Xdccc:
         Labelling->VertexLabel[Nvert].Vexity = Xcccd;
         Labelling->VENumber[Nvert][0] = Nq;
         Labelling->VENumber[Nvert][1] = Nr;
         Labelling->VENumber[Nvert][2] = Ns;
         Labelling->VENumber[Nvert][3] = Np;
      break;

      case Xcdcd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseXLabel(Labelling,Nvert,Xcdcd,0,0,0,0);
		 Labelling->Merit *= LC->tcXcdcd;
      break;

      case Xdcdc:
         Labelling->VertexLabel[Nvert].Vexity = Xcdcd;
         Labelling->VENumber[Nvert][0] = Nq;
         Labelling->VENumber[Nvert][1] = Nr;
         Labelling->VENumber[Nvert][2] = Ns;
         Labelling->VENumber[Nvert][3] = Np;
      break;

      case Xcddd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
         FinaliseXLabel(Labelling,Nvert,Xcddd,0,0,0,0);
		 Labelling->Merit *= LC->tcXcddd;
      break;

      case Xdcdd:
         Labelling->VertexLabel[Nvert].Vexity = Xcddd;
         Labelling->VENumber[Nvert][0] = Nq;
         Labelling->VENumber[Nvert][1] = Nr;
         Labelling->VENumber[Nvert][2] = Ns;
         Labelling->VENumber[Nvert][3] = Np;
      break;

      case Xddcd:
         Labelling->VertexLabel[Nvert].Vexity = Xcddd;
         Labelling->VENumber[Nvert][0] = Nr;
         Labelling->VENumber[Nvert][1] = Ns;
         Labelling->VENumber[Nvert][2] = Np;
         Labelling->VENumber[Nvert][3] = Nq;
      break;

      case Xdddc:
         Labelling->VertexLabel[Nvert].Vexity = Xcddd;
         Labelling->VENumber[Nvert][0] = Ns;
         Labelling->VENumber[Nvert][1] = Np;
         Labelling->VENumber[Nvert][2] = Nq;
         Labelling->VENumber[Nvert][3] = Nr;
      break;

      case Xdddd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
         FinaliseXLabel(Labelling,Nvert,Xdddd,0,0,0,0);
		 Labelling->Merit *= LC->tcXdddd;
      break;

      case Xabcd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConcave);
         FinaliseXLabel(Labelling,Nvert,Xabcd,0,0,0,0);
		 Labelling->Merit *= LC->tcXabcd;
      break;

      case Xbcda:
         Labelling->VertexLabel[Nvert].Vexity = Xabcd;
         Labelling->VENumber[Nvert][0] = Ns;
         Labelling->VENumber[Nvert][1] = Np;
         Labelling->VENumber[Nvert][2] = Nq;
         Labelling->VENumber[Nvert][3] = Nr;
      break;

      case Xcdab:
         Labelling->VertexLabel[Nvert].Vexity = Xabcd;
         Labelling->VENumber[Nvert][0] = Nr;
         Labelling->VENumber[Nvert][1] = Ns;
         Labelling->VENumber[Nvert][2] = Np;
         Labelling->VENumber[Nvert][3] = Nq;
      break;

      case Xdabc:
         Labelling->VertexLabel[Nvert].Vexity = Xabcd;
         Labelling->VENumber[Nvert][0] = Nq;
         Labelling->VENumber[Nvert][1] = Nr;
         Labelling->VENumber[Nvert][2] = Ns;
         Labelling->VENumber[Nvert][3] = Np;
      break;

      case Xabdc:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex);
         FinaliseXLabel(Labelling,Nvert,Xabdc,0,0,0,0);
		 Labelling->Merit *= LC->tcXabcd;
      break;

      case Xbdca:
         Labelling->VertexLabel[Nvert].Vexity = Xabdc;
         Labelling->VENumber[Nvert][0] = Ns;
         Labelling->VENumber[Nvert][1] = Np;
         Labelling->VENumber[Nvert][2] = Nq;
         Labelling->VENumber[Nvert][3] = Nr;
      break;

      case Xdcab:
         Labelling->VertexLabel[Nvert].Vexity = Xabdc;
         Labelling->VENumber[Nvert][0] = Nr;
         Labelling->VENumber[Nvert][1] = Ns;
         Labelling->VENumber[Nvert][2] = Np;
         Labelling->VENumber[Nvert][3] = Nq;
      break;

      case Xcabd:
         Labelling->VertexLabel[Nvert].Vexity = Xabdc;
         Labelling->VENumber[Nvert][0] = Nq;
         Labelling->VENumber[Nvert][1] = Nr;
         Labelling->VENumber[Nvert][2] = Ns;
         Labelling->VENumber[Nvert][3] = Np;
      break;

      case Xabdd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
         FinaliseXLabel(Labelling,Nvert,Xabdd,0,0,0,0);
		 Labelling->Merit *= LC->tcXabdd;
      break;

      case Xbdda:
         Labelling->VertexLabel[Nvert].Vexity = Xabdd;
         Labelling->VENumber[Nvert][0] = Ns;
         Labelling->VENumber[Nvert][1] = Np;
         Labelling->VENumber[Nvert][2] = Nq;
         Labelling->VENumber[Nvert][3] = Nr;
      break;

      case Xddab:
         Labelling->VertexLabel[Nvert].Vexity = Xabdd;
         Labelling->VENumber[Nvert][0] = Nr;
         Labelling->VENumber[Nvert][1] = Ns;
         Labelling->VENumber[Nvert][2] = Np;
         Labelling->VENumber[Nvert][3] = Nq;
      break;

      case Xdabd:
         Labelling->VertexLabel[Nvert].Vexity = Xabdd;
         Labelling->VENumber[Nvert][0] = Nq;
         Labelling->VENumber[Nvert][1] = Nr;
         Labelling->VENumber[Nvert][2] = Ns;
         Labelling->VENumber[Nvert][3] = Np;
      break;

      case Xcccc|Xcccd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
      break;

      case Xcccc|Xccdc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex);
      break;

      case Xcccc|Xcdcc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConvex);
      break;

      case Xcccc|Xdccc:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConvex);
      break;

      case Xcccd|Xcdcd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConcave);
      break;

      case Xcdcc|Xcdcd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
      break;

      case Xccdc|Xdcdc:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex);
      break;

      case Xdccc|Xdcdc:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex);
      break;

      case Xcdcd|Xddcd:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConcave);
      break;

      case Xdcdc|Xdcdd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
      break;

      case Xcdcd|Xcddd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
      break;

      case Xdcdc|Xdddc:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex);
      break;

      case Xdddd|Xcddd:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
      break;

      case Xdddd|Xdcdd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
      break;

      case Xdddd|Xddcd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
      break;

      case Xdddd|Xdddc:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
      break;

      case Xabcd|Xabdd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
      break;

      case Xbcda|Xbdda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsA(Labelling,Ns,Nvert));
      break;

      case Xcdab|Xddab:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsA(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Ns, LineIsB(Labelling,Ns,Nvert));
      break;

      case Xdabc|Xdabd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsA(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsB(Labelling,Nr,Nvert));
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
      break;

      case Xbcda|Xdcdd:
         ChangeLine(Labelling,Np,LineIsConcave|LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave|LineIsA(Labelling,Ns,Nvert));
      break;

      case Xdcdd|Xcccd:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConcave);
      break;

      case Xcddd|Xccdc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
      break;

      case Xdddc|Xcdcc:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex);
      break;

      case Xddcd|Xdccc:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
      break;

      default:
      //printf("EEK! Don't know what to do with X junction! (%x)\n", Labelling->VertexLabel[Nvert].Vexity);
      break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromMJunction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np,Nq,Nr,Ns;

   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFromMJunction");
   Np = Labelling->VENumber[Nvert][0];
   Nq = Labelling->VENumber[Nvert][1];
   Nr = Labelling->VENumber[Nvert][2];
   Ns = Labelling->VENumber[Nvert][3];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Mccdc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseMLabel(Labelling,Nvert,Mccdc,0,0,0,0);
		 Labelling->Merit *= LC->tcMccdc;
      break;

      case Mcdcc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseMLabel(Labelling,Nvert,Mcdcc,0,0,0,0);
		 Labelling->Merit *= LC->tcMccdc;
      break;

      case Mcdcd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseMLabel(Labelling,Nvert,Mcdcd,0,0,0,0);
		 Labelling->Merit *= LC->tcMcdcd;
      break;

      case Mcddc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseMLabel(Labelling,Nvert,Mcddc,0,0,0,0);
		 Labelling->Merit *= LC->tcMcddc;
      break;

      case Mdccd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseMLabel(Labelling,Nvert,Mdccd,0,0,0,0);
		 Labelling->Merit *= LC->tcMdccd;
      break;

      case Mdcdc:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseMLabel(Labelling,Nvert,Mdcdc,0,0,0,0);
		 Labelling->Merit *= LC->tcMcdcd;
      break;

      case Mdcdd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseMLabel(Labelling,Nvert,Mdcdd,0,0,0,0);
		 Labelling->Merit *= LC->tcMdcdd;
      break;

      case Mddcd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseMLabel(Labelling,Nvert,Mddcd,0,0,0,0);
		 Labelling->Merit *= LC->tcMdcdd;
      break;

      case Mbcca:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsA(Labelling,Ns,Nvert));
         FinaliseMLabel(Labelling,Nvert,Mbcca,0,0,0,0);
		 Labelling->Merit *= LC->tcMbcca;
      break;

      case Mbcda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsA(Labelling,Ns,Nvert));
         FinaliseMLabel(Labelling,Nvert,Mbcda,0,0,0,0);
		 Labelling->Merit *= LC->tcMbcda;
      break;

      case Mbdca:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsA(Labelling,Ns,Nvert));
         FinaliseMLabel(Labelling,Nvert,Mbdca,0,0,0,0);
		 Labelling->Merit *= LC->tcMbcda;
      break;

      case Mbcca|Mbcda:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsA(Labelling,Ns,Nvert));
      break;

      case Mbcca|Mbdca:
         ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsA(Labelling,Ns,Nvert));
      break;

      case Mccdc|Mdcdc:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
      break;

      case Mccdc|Mcddc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
      break;

      case Mddcd|Mcdcd:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
      break;

      case Mddcd|Mdcdd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConcave);
      break;

      case Mall:
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
      break;

      default:
         //printf("EEK! Don't know what to do with M junction! (%x)\n",Labelling->VertexLabel[Nvert].Vexity);
      break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromKJunction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np = Labelling->VENumber[Nvert][0];
   EDGE_INDEX Nq = Labelling->VENumber[Nvert][1];
   EDGE_INDEX Nr = Labelling->VENumber[Nvert][2];
   EDGE_INDEX Ns = Labelling->VENumber[Nvert][3];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Kcccd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseKLabel(Labelling,Nvert,Kcccd,0,0,0,0);
		 Labelling->Merit *= LC->tcKcccd;
      break;

      case Kccdc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseKLabel(Labelling,Nvert,Kccdc,0,0,0,0);
		 Labelling->Merit *= LC->tcKcccd;
      break;

      case Kcdcd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseKLabel(Labelling,Nvert,Kcdcd,0,0,0,0);
		 Labelling->Merit *= LC->tcKcdcd;
      break;

      case Kdcdc:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseKLabel(Labelling,Nvert,Kdcdc,0,0,0,0);
		 Labelling->Merit *= LC->tcKcdcd;
      break;

      case Kddcd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseKLabel(Labelling,Nvert,Kddcd,0,0,0,0);
		 Labelling->Merit *= LC->tcKddcd;
      break;

      case Kdddc:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseKLabel(Labelling,Nvert,Kdddc,0,0,0,0);
		 Labelling->Merit *= LC->tcKddcd;
      break;

      case Kabcd:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         FinaliseKLabel(Labelling,Nvert,Kabcd,0,0,0,0);
		 Labelling->Merit *= LC->tcKabcd;
      break;

      case Kabdc:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
         FinaliseKLabel(Labelling,Nvert,Kabdc,0,0,0,0);
		 Labelling->Merit *= LC->tcKabcd;
      break;

      case Kcdcd|Kcccd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
      break;

      case Kcdcd|Kddcd:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
      break;

      case Kccdc|Kdcdc:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
      break;

      case Kdddc|Kddcd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex|LineIsConcave);
      break;

      case Kdddc|Kdcdc:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex|LineIsConcave);
      break;

      case Kabcd|Kabdc:
         ChangeLine(Labelling,Np,LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex|LineIsConcave);
      break;

      case Kabdc|Kdddc:
         ChangeLine(Labelling,Np,LineIsConcave|LineIsA(Labelling,Np,Nvert));
         ChangeLine(Labelling,Nq,LineIsConcave|LineIsB(Labelling,Nq,Nvert));
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex);
      break;

      default:
         //printf("EEK! Don't know what to do with K junction! (%x)\n", Labelling->VertexLabel[Nvert].Vexity);
      break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFromXbcccaJunction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np = Labelling->VENumber[Nvert][0];
   EDGE_INDEX Nq = Labelling->VENumber[Nvert][1];
   EDGE_INDEX Nr = Labelling->VENumber[Nvert][2];
   EDGE_INDEX Ns = Labelling->VENumber[Nvert][3];
   EDGE_INDEX Nt = Labelling->VENumber[Nvert][4];

   ChangeLine(Labelling,Np,LineIsB(Labelling,Np,Nvert));
   ChangeLine(Labelling,Nq,LineIsConvex);
   ChangeLine(Labelling,Nr,LineIsConvex);
   ChangeLine(Labelling,Ns,LineIsConvex);
   ChangeLine(Labelling,Nt,LineIsA(Labelling,Nt,Nvert));
   Labelling->VertexLabel[Nvert].Vexity = Xccccc;
   Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs5ccccc;
   Labelling->Merit *= LC->tcXccccc;
   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFrom5Junction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np,Nq,Nr,Ns,Nt;

   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFrom5Junction");

   Np = Labelling->VENumber[Nvert][0];
   Nq = Labelling->VENumber[Nvert][1];
   Nr = Labelling->VENumber[Nvert][2];
   Ns = Labelling->VENumber[Nvert][3];
   Nt = Labelling->VENumber[Nvert][4];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Xccccc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConvex);
         ChangeLine(Labelling,Nt,LineIsConvex);
         Labelling->VertexLabel[Nvert].Vexity = Xccccc;
		 Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs5ccccc;
		 Labelling->Merit *= LC->tcXccccc;
		 Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;
      break;

      case Xddddd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConcave);
         ChangeLine(Labelling,Nt,LineIsConcave);
         Labelling->VertexLabel[Nvert].Vexity = Xddddd;
		 Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs5ddddd;
		 Labelling->Merit *= LC->tcXddddd;
		 Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;
      break;

      case Xccccc|Xddddd:
         ChangeLine(Labelling,Np,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nt,LineIsConvex|LineIsConcave);
      break;

      case Xbccca:
         PropagateConstraintsFromXbcccaJunction(Labelling,Nvert);
      break;

      case Xcccab:
         Labelling->VertexLabel[Nvert].Vexity = Xbccca;
         Labelling->VENumber[Nvert][0] = Nt;
         Labelling->VENumber[Nvert][1] = Np;
         Labelling->VENumber[Nvert][2] = Nq;
         Labelling->VENumber[Nvert][3] = Nr;
         Labelling->VENumber[Nvert][4] = Ns;
         PropagateConstraintsFromXbcccaJunction(Labelling,Nvert);
      break;

      case Xccabc:
         Labelling->VertexLabel[Nvert].Vexity = Xbccca;
         Labelling->VENumber[Nvert][0] = Ns;
         Labelling->VENumber[Nvert][1] = Nt;
         Labelling->VENumber[Nvert][2] = Np;
         Labelling->VENumber[Nvert][3] = Nq;
         Labelling->VENumber[Nvert][4] = Nr;
         PropagateConstraintsFromXbcccaJunction(Labelling,Nvert);
      break;

      case Xcabcc:
         Labelling->VertexLabel[Nvert].Vexity = Xbccca;
         Labelling->VENumber[Nvert][0] = Nr;
         Labelling->VENumber[Nvert][1] = Ns;
         Labelling->VENumber[Nvert][2] = Nt;
         Labelling->VENumber[Nvert][3] = Np;
         Labelling->VENumber[Nvert][4] = Nq;
         PropagateConstraintsFromXbcccaJunction(Labelling,Nvert);
      break;

      case Xabccc:
         Labelling->VertexLabel[Nvert].Vexity = Xbccca;
         Labelling->VENumber[Nvert][0] = Nq;
         Labelling->VENumber[Nvert][1] = Nr;
         Labelling->VENumber[Nvert][2] = Ns;
         Labelling->VENumber[Nvert][3] = Nt;
         Labelling->VENumber[Nvert][4] = Np;
         PropagateConstraintsFromXbcccaJunction(Labelling,Nvert);
      break;

      default:
         //printf("EEK! Don't know what to do with 5 junction! (%x)\n", Labelling->VertexLabel[Nvert].Vexity);
         break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateConstraintsFrom6Junction (LABELLING *Labelling, VERTEX_INDEX Nvert)
{
   EDGE_INDEX Np,Nq,Nr,Ns,Nt,Nu;

   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFrom6Junction");

   Np = Labelling->VENumber[Nvert][0];
   Nq = Labelling->VENumber[Nvert][1];
   Nr = Labelling->VENumber[Nvert][2];
   Ns = Labelling->VENumber[Nvert][3];
   Nt = Labelling->VENumber[Nvert][4];
   Nu = Labelling->VENumber[Nvert][5];

   switch (Labelling->VertexLabel[Nvert].Vexity)
   {
      case Xcccccc:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConvex);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConvex);
         ChangeLine(Labelling,Nt,LineIsConvex);
         ChangeLine(Labelling,Nu,LineIsConvex);
         Labelling->VertexLabel[Nvert].Vexity = Xcccccc;
		 Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs6cccccc;
		 Labelling->Merit *= LC->tcXcccccc;
		 Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;
      break;

      case Xcdcdcd:
         ChangeLine(Labelling,Np,LineIsConvex);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex);
         ChangeLine(Labelling,Ns,LineIsConcave);
         ChangeLine(Labelling,Nt,LineIsConvex);
         ChangeLine(Labelling,Nu,LineIsConcave);
         Labelling->VertexLabel[Nvert].Vexity = Xcdcdcd;
		 Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs6cdcdcd;
		 Labelling->Merit *= LC->tcXcdcdcd;
		 Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;
      break;

      case Xdcdcdc:
         Labelling->VENumber[Nvert][0] = Nq;
         Labelling->VENumber[Nvert][1] = Nr;
         Labelling->VENumber[Nvert][2] = Ns;
         Labelling->VENumber[Nvert][3] = Nt;
         Labelling->VENumber[Nvert][4] = Nu;
         Labelling->VENumber[Nvert][5] = Np;
         Labelling->VertexLabel[Nvert].Vexity = Xcdcdcd;
      break;

      case Xdddddd:
         ChangeLine(Labelling,Np,LineIsConcave);
         ChangeLine(Labelling,Nq,LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns,LineIsConcave);
         ChangeLine(Labelling,Nt,LineIsConcave);
         ChangeLine(Labelling,Nu,LineIsConcave);
         Labelling->VertexLabel[Nvert].Vexity = Xdddddd;
		 Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs6dddddd;
		 Labelling->Merit *= LC->tcXdddddd;
		 Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;
      break;

      case Xcdcdcd|Xdcdcdc:
         ChangeLine(Labelling,Np, LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nq, LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nt,LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nu, LineIsConvex|LineIsConcave);
      break;

      case Xdddddd|Xdcdcdc:
         ChangeLine(Labelling,Np, LineIsConcave);
         ChangeLine(Labelling,Nq, LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nr,LineIsConcave);
         ChangeLine(Labelling,Ns, LineIsConvex|LineIsConcave);
         ChangeLine(Labelling,Nt,LineIsConcave);
         ChangeLine(Labelling,Nu, LineIsConvex|LineIsConcave);
      break;

      default:
         //printf("EEK! Don't know what to do with 6 junction! (%x)\n", Labelling->VertexLabel[Nvert].Vexity);
         break;
   }
}


void CCueLabelsVarley_JunctionToLines::PropagateJunctionConstraintsToLines (OBJECT *Object, LABELLING *Labelling)
//Finalise Vertex Labelling
//Mostly straightforward. Also multiplies the labelling merit by the merit
//for this particular junction label.
//Set Final Vertex Types To Match
//For each vertex:
//	If it has three edges, limit the final types to trihedral types
//	If it has four edges, limit the final types to four-edge types
//	If it has five edges, limit the final types to five-edge types
//	If it has six edges, limit the final types to six-edge types
//	Count the numbers of known convex and concave edges at the vertex
//	Limit the final types to those with at least as many convex edges
//	Limit the final types to those with at least as many concave edges
{
   for (int n=0; n<Labelling->VertexCount; ++n)
   {
      if (!Labelling->VertexLabellingIsFinalised[n])
      {
         JUNCTION_TYPEMARK *Jlabel;

         Jlabel = &Labelling->VertexLabel[n];

         switch (Jlabel->Shape)
         {
            case JunctionIsW:
               PropagateConstraintsFromWJunction(Labelling,n);
            break;

            case JunctionIsL:
               PropagateConstraintsFromLJunction(Labelling,n);
            break;

            case JunctionIsY:
               PropagateConstraintsFromYJunction(Labelling,n);
            break;

            case JunctionIsT:
               PropagateConstraintsFromTJunction(Labelling,n);
            break;

            case JunctionIsT4:
               PropagateConstraintsFromT4Junction(Object,Labelling,n);
            break;

            case JunctionIsX:
               PropagateConstraintsFromXJunction(Labelling,n);
            break;

            case JunctionIsZ:
               PropagateConstraintsFromZ4Junction(Labelling,n);
            break;

            case JunctionIsM:
               PropagateConstraintsFromMJunction(Labelling,n);
            break;

            case JunctionIsK:
               PropagateConstraintsFromKJunction(Labelling,n);
            break;

            case JunctionIs5way:
               PropagateConstraintsFrom5Junction(Labelling,n);
            break;

            case JunctionIs6way:
            case JunctionIsZ6:
               PropagateConstraintsFrom6Junction(Labelling,n);
            break;
         }
      }
   }
}
