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

#include "CueLabelsVarley_LabelLines.h"
#include "CueLabelsVarley_LabelJunctions.h"


void FinaliseWLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label)
{
   EDGE_INDEX E0 = Labelling->VENumber[Nvert][0];
   EDGE_INDEX E1 = Labelling->VENumber[Nvert][1];
   EDGE_INDEX E2 = Labelling->VENumber[Nvert][2];

   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;

   switch (Labelling->VertexLabel[Nvert].Vexity = Vexity)
   {
      case Wbca:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIs3ccc|VFinalIsXcccc|VFinalIsXcccd|VFinalIs5ccccc|VFinalIs6cccccc);
      break;

      case Wcdc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs3ccd;
      break;

      case Wdcd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs3cdd;
      break;

      case Wabc:
      case Wacc:
      case Wcab:
      case Wcac:
      case Wcbc:
      case Wccb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMccdc;
      break;

      case Wabd:
      case Wdab:
      case Wacd:
      case Wdcb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIsMcdcd|VFinalIsKcdcd);
      break;

      case Wbdc:
      case Wcbd:
      case Wcda:
      case Wdac:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMcdcd;
      break;

      case Wadc:
      case Wcdb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMcddc;
      break;

      case Wbcd:
      case Wdca:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMdccd;
      break;

      case Wbdd:
      case Wdad:
      case Wdbd:
      case Wdda:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMdcdd;
      break;

      case Wbaa:
      case Wbba:
      case Wbda:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXcccd;
      break;

      default:
         printf("** Don't know how to finalise W label for vexity %x\n", Vexity);
      break;
   }

   switch(Edge1Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,E0,Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,E0,Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,E0);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,E0);  break;
   }

   switch(Edge2Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,E1,Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,E1,Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,E1);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,E1);  break;
   }

   switch(Edge3Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,E2,Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,E2,Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,E2);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,E2);  break;
   }
}


void FinaliseLLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label)
{
   EDGE_INDEX E0 = Labelling->VENumber[Nvert][0];
   EDGE_INDEX E1 = Labelling->VENumber[Nvert][1];

   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;

   switch (Labelling->VertexLabel[Nvert].Vexity = Vexity)
   {
      case Lba:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIs3ccc|VFinalIsXcccc|VFinalIsMdccd|VFinalIs5ccccc|VFinalIs6cccccc);
      break;

      case Lab:
      case Lac:
      case Lcb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIs3ccd|VFinalIsMccdc|VFinalIsMcddc);
      break;

      case Lbd:
      case Lda:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIs3cdd|VFinalIsMdccd|VFinalIsMcdcd|VFinalIsMdcdd|VFinalIsKcdcd);
      break;

      case Laa:
      case Lbb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIsMcdcd|VFinalIsKcdcd);
      break;

      default:
         printf("** Don't know how to finalise L label for vexity %x\n", Vexity);
      break;
   }

   switch(Edge1Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,E0,Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,E0,Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,E0);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,E0);  break;
   }

   switch(Edge2Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,E1,Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,E1,Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,E1);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,E1);  break;
   }
}


void FinaliseYLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label)
{
   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;

   switch (Labelling->VertexLabel[Nvert].Vexity = Vexity)
   {
      case Yccc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs3ccc;
      break;

      case Yddd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIs3ddd;
      break;

      case Yabd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIs3ccd|VFinalIsXcccd|VFinalIsXcdcd|VFinalIsMccdc|VFinalIsMcdcd|VFinalIsKcccd|VFinalIsKcdcd);
      break;

      case Yabc:
      case Yacc:
      case Ybcc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXcccd;
      break;

      case Yacd:
      case Ybdc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIsXcdcd|VFinalIsMcdcd|VFinalIsKcdcd);
      break;

      case Yadd:
      case Ybdd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIsXcddd|VFinalIsMdcdd|VFinalIsKddcd);
      break;

      case Yaab:
      case Yabb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMccdc;
      break;

      default:
         printf("** Don't know how to finalise Y label for vexity %x\n", Vexity);
      break;
   }

   switch(Edge1Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][0]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][0]);  break;
   }

   switch(Edge2Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][1]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][1]);  break;
   }

   switch(Edge3Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][2]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][2]);  break;
   }
}


void FinaliseTLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label)
{
   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;

   switch (Labelling->VertexLabel[Nvert].Vexity = Vexity)
   {
      case Tbaa:
      case Tbab:
      case Tbac:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIsUnknown-VFinalIsConcave);
      break;

      case Tbad:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & (VFinalIsUnknown-VFinalIsConvex);
      break;

      case Taba:
      case Tabb:
      case Tbca:
      case Tcab:
      case Tbcc:
      case Tcac:
      case Tcca:
      case Tccb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsKcccd;
      break;

      case Tbda:
      case Tbdc:
      case Tdab:
      case Tdac:
      case Tcda:
      case Tdcb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsKcdcd;
      break;

      case Tdda:
      case Tddb:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsKddcd;
      break;

      default:
         printf("** Don't know how to finalise T label for vexity %x\n", Vexity);
      break;
   }

   switch(Edge1Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][0]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][0]);  break;
   }

   switch(Edge2Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][1]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][1]);  break;
   }

   switch(Edge3Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][2]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][2]);  break;
   }
}


void FinaliseXLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label, TYPEMARK Edge4Label)
{
   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;

   switch (Labelling->VertexLabel[Nvert].Vexity = Vexity)
   {
      case Xcccc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXcccc;
      break;

      case Xcccd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXcccd;
      break;

      case Xcdcd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXcdcd;
      break;

      case Xcddd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXcddd;
      break;

      case Xdddd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXdddd;
      break;

      case Xabcd:
      case Xabdc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMccdc;
      break;

      case Xabdd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMcddc;
      break;

      default:
         printf("** Don't know how to finalise X label for vexity %x\n", Vexity);
      break;
   }

   switch(Edge1Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][0]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][0]);  break;
   }

   switch(Edge2Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][1]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][1]);  break;
   }

   switch(Edge3Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][2]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][2]);  break;
   }

   switch(Edge4Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][3],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][3],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][3]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][3]);  break;
   }
}


void FinaliseMLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label, TYPEMARK Edge4Label)
{
   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;

   switch (Labelling->VertexLabel[Nvert].Vexity = Vexity)
   {
      case Mccdc:
      case Mcdcc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMccdc;
      break;

      case Mcddc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMcddc;
      break;

      case Mcdcd:
      case Mdcdc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMcdcd;
      break;

      case Mdccd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMdccd;
      break;

      case Mdcdd:
      case Mddcd:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsMdcdd;
      break;

      case Mbcca:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXcccc;
      break;

      case Mbcda:
      case Mbdca:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsXcccd;
      break;

      default:
         printf("** Don't know how to finalise M label for vexity %x\n", Vexity);
      break;
   }

   switch(Edge1Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][0]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][0]);  break;
   }

   switch(Edge2Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][1]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][1]);  break;
   }

   switch(Edge3Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][2]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][2]);  break;
   }

   switch(Edge4Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][3],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][3],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][3]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][3]);  break;
   }
}


void FinaliseKLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label, TYPEMARK Edge4Label)
{
   Labelling->VertexLabellingIsFinalised[Nvert] = cTRUE;

   switch (Labelling->VertexLabel[Nvert].Vexity = Vexity)
   {
      case Kabcd:
      case Kabdc:
      case Kcccd:
      case Kccdc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsKcccd;
      break;

      case Kcdcd:
      case Kdcdc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsKcdcd;
      break;

      case Kddcd:
      case Kdddc:
         Labelling->VertexUnderlyingType[Nvert] = Labelling->FinalVertexTypes & VFinalIsKddcd;
      break;

      default:
         printf("** Don't know how to finalise K label for vexity %x\n", Vexity);
      break;
   }

   switch(Edge1Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][0],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][0]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][0]);  break;
   }

   switch(Edge2Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][1],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][1]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][1]);  break;
   }

   switch(Edge3Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][2],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][2]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][2]);  break;
   }

   switch(Edge4Label)
   {
      case LineIsArrow:      LabelLineAsRelativeArrow(Labelling,Labelling->VENumber[Nvert][3],Nvert);  break;
      case LineIsBackArrow:  LabelLineAsRelativeBackArrow(Labelling,Labelling->VENumber[Nvert][3],Nvert);  break;
      case LineIsConvex:     LabelLineAsConvex(Labelling,Labelling->VENumber[Nvert][3]);  break;
      case LineIsConcave:    LabelLineAsConcave(Labelling,Labelling->VENumber[Nvert][3]);  break;
   }
}
