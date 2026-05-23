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

#include "CueLabelsVarley_LineToJunctions.h"
#include "CueLabelsVarley_LabelLines.h"


CCueLabelsVarley_LineToJunctions::CCueLabelsVarley_LineToJunctions ()
{
}


CCueLabelsVarley_LineToJunctions::~CCueLabelsVarley_LineToJunctions()
{
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromArrow (LABELLING *Labelling, EDGE_INDEX Nedge)
{
   VERTEX_INDEX S = Labelling->EdgeStartVertex[Nedge];
   VERTEX_INDEX E = Labelling->EdgeEndVertex[Nedge];

   CheckVertexNumber(Labelling,S,"PropagateConstraintsFromArrow S");
   CheckVertexNumber(Labelling,E,"PropagateConstraintsFromArrow E");

   switch (Labelling->VertexLabel[S].Shape)
   {
      case JunctionIsY:
         if (Labelling->VENumber[S][0] == Nedge)  ChangeVertex(Labelling,S,Ybxx);
         else
         if (Labelling->VENumber[S][1] == Nedge)  ChangeVertex(Labelling,S,Yxbx);
         else
         if (Labelling->VENumber[S][2] == Nedge)  ChangeVertex(Labelling,S,Yxxb);
      break;

      case JunctionIsW:
         if (Labelling->VENumber[S][0] == Nedge)  ChangeVertex(Labelling,S,Wbxx);
         else
         if (Labelling->VENumber[S][1] == Nedge)  ChangeVertex(Labelling,S,Wxbx);
         else
         if (Labelling->VENumber[S][2] == Nedge)  ChangeVertex(Labelling,S,Wxxb);
      break;

      case JunctionIsL:
         if (Labelling->VENumber[S][0] == Nedge)  ChangeVertex(Labelling,S,Lbx);
         else
         if (Labelling->VENumber[S][1] == Nedge)  ChangeVertex(Labelling,S,Lxb);
      break;

      case JunctionIsT:
         if (Labelling->VENumber[S][0] == Nedge)  ChangeVertex(Labelling,S,Tbab);
      break;

      case JunctionIsT4:
         if (Labelling->VENumber[S][0] == Nedge)  ChangeVertex(Labelling,S,Tbxx);
         else
         if (Labelling->VENumber[S][1] == Nedge)  ChangeVertex(Labelling,S,Txbx);
         else
         if (Labelling->VENumber[S][2] == Nedge)  ChangeVertex(Labelling,S,Txxb);
      break;

      case JunctionIsX:
         if (Labelling->VENumber[S][0] == Nedge)  ChangeVertex(Labelling,S,Xbxxx);
         else
         if (Labelling->VENumber[S][1] == Nedge)  ChangeVertex(Labelling,S,Xxbxx);
         else
         if (Labelling->VENumber[S][2] == Nedge)  ChangeVertex(Labelling,S,Xxxbx);
         else
         if (Labelling->VENumber[S][3] == Nedge)  ChangeVertex(Labelling,S,Xxxxb);
      break;

      case JunctionIsK:
         if (Labelling->VENumber[S][0] == Nedge)  ChangeVertex(Labelling,S,Kbxxx);
         else
         if (Labelling->VENumber[S][1] == Nedge)  ChangeVertex(Labelling,S,Kxbxx);
         else
         if (Labelling->VENumber[S][2] == Nedge)  ChangeVertex(Labelling,S,Kxxbx);
         else
         if (Labelling->VENumber[S][3] == Nedge)  ChangeVertex(Labelling,S,Kxxxb);
      break;

      case JunctionIsM:
         if (Labelling->VENumber[S][0] == Nedge)  ChangeVertex(Labelling,S,Mbxxx);
         else
         if (Labelling->VENumber[S][1] == Nedge)  ChangeVertex(Labelling,S,Mxbxx);
         else
         if (Labelling->VENumber[S][2] == Nedge)  ChangeVertex(Labelling,S,Mxxbx);
         else
         if (Labelling->VENumber[S][3] == Nedge)  ChangeVertex(Labelling,S,Mxxxb);
      break;
   }

   switch (Labelling->VertexLabel[E].Shape)
   {
      case JunctionIsY:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E, Yaxx);
         else
         if (Labelling->VENumber[E][1] == Nedge)  ChangeVertex(Labelling,E, Yxax);
         else
         if (Labelling->VENumber[E][2] == Nedge)  ChangeVertex(Labelling,E, Yxxa);
      break;

      case JunctionIsW:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E,Waxx);
         else
         if (Labelling->VENumber[E][1] == Nedge)  ChangeVertex(Labelling,E,Wxax);
         else
         if (Labelling->VENumber[E][2] == Nedge)  ChangeVertex(Labelling,E,Wxxa);
      break;

      case JunctionIsL:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E,Lax);
         else
         if (Labelling->VENumber[E][1] == Nedge)  ChangeVertex(Labelling,E,Lxa);
      break;

      case JunctionIsT:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E,Tbaa);
      break;

      case JunctionIsT4:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E,Taxx);
         else
         if (Labelling->VENumber[E][1] == Nedge)  ChangeVertex(Labelling,E,Txax);
         else
         if (Labelling->VENumber[E][2] == Nedge)  ChangeVertex(Labelling,E,Txxa);
      break;

      case JunctionIsX:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E,Xaxxx);
         else
         if (Labelling->VENumber[E][1] == Nedge)  ChangeVertex(Labelling,E,Xxaxx);
         else
         if (Labelling->VENumber[E][2] == Nedge)  ChangeVertex(Labelling,E,Xxxax);
         else
         if (Labelling->VENumber[E][3] == Nedge)  ChangeVertex(Labelling,E,Xxxxa);
      break;

      case JunctionIsK:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E,Kaxxx);
         else
         if (Labelling->VENumber[E][1] == Nedge)  ChangeVertex(Labelling,E,Kxaxx);
         else
         if (Labelling->VENumber[E][2] == Nedge)  ChangeVertex(Labelling,E,Kxxax);
         else
         if (Labelling->VENumber[E][3] == Nedge)  ChangeVertex(Labelling,E,Kxxxa);
      break;

      case JunctionIsM:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E,Maxxx);
         else
         if (Labelling->VENumber[E][1] == Nedge)  ChangeVertex(Labelling,E,Mxaxx);
         else
         if (Labelling->VENumber[E][2] == Nedge)  ChangeVertex(Labelling,E,Mxxax);
         else
         if (Labelling->VENumber[E][3] == Nedge)  ChangeVertex(Labelling,E,Mxxxa);
      break;

      case JunctionIs5way:
         if (Labelling->VENumber[E][0] == Nedge)  ChangeVertex(Labelling,E,Xaxxxx);
         else
         if (Labelling->VENumber[E][1] == Nedge)  ChangeVertex(Labelling,E,Xxaxxx);
         else
         if (Labelling->VENumber[E][2] == Nedge)  ChangeVertex(Labelling,E,Xxxaxx);
         else
         if (Labelling->VENumber[E][3] == Nedge)  ChangeVertex(Labelling,E,Xxxxax);
         else
         if (Labelling->VENumber[E][4] == Nedge)  ChangeVertex(Labelling,E,Xxxxxa);
      break;
   }

   Labelling->EdgeLabelIsFinalised[Nedge] = cTRUE;
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromConvexToJunction (LABELLING *Labelling, EDGE_INDEX EdgeNumber, VERTEX_INDEX Nvert)
{
   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFromConvexToJunction");

   switch (Labelling->VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Kcxxx);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Kxcxx);
         else
         if (Labelling->VENumber[Nvert][2] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Kxxcx);
         else
         if (Labelling->VENumber[Nvert][3] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Kxxxc);
      break;

      case JunctionIsL:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Lcx);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Lxc);
      break;

      case JunctionIsM:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Mcxxx);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Mxcxx);
         else
         if (Labelling->VENumber[Nvert][2] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Mxxcx);
         else
         if (Labelling->VENumber[Nvert][3] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Mxxxc);
      break;

      case JunctionIsT:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Tbac);
      break;

      case JunctionIsT4:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Tcxx);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Txcx);
         else
         if (Labelling->VENumber[Nvert][2] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Txxc);
      break;

      case JunctionIsW:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Wcxx);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Wxcx);
         else
         if (Labelling->VENumber[Nvert][2] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Wxxc);
      break;

      case JunctionIsX:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xcxxx);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xxcxx);
         else
         if (Labelling->VENumber[Nvert][2] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xxxcx);
         else
         if (Labelling->VENumber[Nvert][3] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xxxxc);
      break;

      case JunctionIsY:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Ycxx);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Yxcx);
         else
         if (Labelling->VENumber[Nvert][2] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Yxxc);
      break;

      case JunctionIs5way:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xcxxxx);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xxcxxx);
         else
         if (Labelling->VENumber[Nvert][2] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xxxcxx);
         else
         if (Labelling->VENumber[Nvert][3] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xxxxcx);
         else
         if (Labelling->VENumber[Nvert][4] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xxxxxc);
      break;

      case JunctionIsZ6:
      case JunctionIs6way:
         if (Labelling->VENumber[Nvert][0] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xcccccc|Xcdcdcd);
         else
         if (Labelling->VENumber[Nvert][1] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xcccccc|Xdcdcdc);
         else
         if (Labelling->VENumber[Nvert][2] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xcccccc|Xcdcdcd);
         else
         if (Labelling->VENumber[Nvert][3] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xcccccc|Xdcdcdc);
         else
         if (Labelling->VENumber[Nvert][4] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xcccccc|Xcdcdcd);
         else
         if (Labelling->VENumber[Nvert][5] == EdgeNumber)  ChangeVertex(Labelling,Nvert,Xcccccc|Xdcdcdc);
      break;
   }
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromConvex (LABELLING *Labelling, EDGE_INDEX Nedge)
{
   PropagateConstraintsFromConvexToJunction(Labelling,Nedge,Labelling->EdgeStartVertex[Nedge]);
   PropagateConstraintsFromConvexToJunction(Labelling,Nedge,Labelling->EdgeEndVertex[Nedge]);
   Labelling->EdgeLabelIsFinalised[Nedge] = cTRUE;
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromNonConvexToJunction (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
{
   switch (Labelling->VertexLabel[Nvert].Shape)
   {
      case JunctionIsK:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Kaxxx|Kbxxx|Kdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Kxaxx|Kxbxx|Kxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Kxxax|Kxxbx|Kxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Kxxxa|Kxxxb|Kxxxd);
      break;

      case JunctionIsL:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Lax|Lbx|Ldx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Lxa|Lxb|Lxd);
      break;

      case JunctionIsM:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Maxxx|Mbxxx|Mdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Mxaxx|Mxbxx|Mxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Mxxax|Mxxbx|Mxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Mxxxa|Mxxxb|Mxxxd);
      break;

      case JunctionIsT:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Tbaa|Tbab|Tbad);
      break;

      case JunctionIsT4:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Taxx|Tbxx|Tdxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Txax|Txbx|Txdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Txxa|Txxb|Txxd);
      break;

      case JunctionIsW:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Waxx|Wbxx|Wdxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Wxax|Wxbx|Wxdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Wxxa|Wxxb|Wxxd);
      break;

      case JunctionIsX:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xaxxx|Xbxxx|Xdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xxaxx|Xxbxx|Xxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxax|Xxxbx|Xxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxa|Xxxxb|Xxxxd);
      break;

      case JunctionIsY:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Yaxx|Ybxx|Ydxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Yxax|Yxbx|Yxdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Yxxa|Yxxb|Yxxd);
      break;

      case JunctionIs5way:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xaxxxx|Xbxxxx|Xdxxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xxaxxx|Xxbxxx|Xxdxxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxaxx|Xxxbxx|Xxxdxx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxax|Xxxxbx|Xxxxdx);
         else
         if (Labelling->VENumber[Nvert][4] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxxa|Xxxxxb|Xxxxxd);
      break;

      case JunctionIsZ6:
      case JunctionIs6way:
         ChangeVertex(Labelling,Nvert,Xdddddd|Xcdcdcd|Xdcdcdc);
      break;
   }
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromNonConvex (LABELLING *Labelling, EDGE_INDEX Nedge)
{
   PropagateConstraintsFromNonConvexToJunction(Labelling,Nedge,Labelling->EdgeStartVertex[Nedge]);
   PropagateConstraintsFromNonConvexToJunction(Labelling,Nedge,Labelling->EdgeEndVertex[Nedge]);
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromConcaveToJunction (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
{
   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFromConcaveToJunction");

   switch (Labelling->VertexLabel[Nvert].Shape)
   {
      case JunctionIsY:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Ydxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Yxdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Yxxd);
      break;

      case JunctionIsW:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Wdxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Wxdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Wxxd);
      break;

      case JunctionIsL:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Ldx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Lxd);
      break;

      case JunctionIsT:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Tbad);
      break;

      case JunctionIsT4:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Tdxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Txdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Txxd);
      break;

      case JunctionIsX:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxd);
      break;

      case JunctionIsK:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Kdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Kxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Kxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Kxxxd);
      break;

      case JunctionIsM:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Mdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Mxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Mxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Mxxxd);
      break;

      case JunctionIs5way:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xdxxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xxdxxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxdxx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxdx);
         else
         if (Labelling->VENumber[Nvert][4] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxxd);
      break;

      case JunctionIsZ6:
      case JunctionIs6way:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xdcdcdc|Xdddddd);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xcdcdcd|Xdddddd);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xdcdcdc|Xdddddd);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xcdcdcd|Xdddddd);
         else
         if (Labelling->VENumber[Nvert][4] == Nedge)  ChangeVertex(Labelling,Nvert,Xdcdcdc|Xdddddd);
         else
         if (Labelling->VENumber[Nvert][5] == Nedge)  ChangeVertex(Labelling,Nvert,Xcdcdcd|Xdddddd);
      break;
   }
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromConcave (LABELLING *Labelling, EDGE_INDEX Nedge)
{
   PropagateConstraintsFromConcaveToJunction(Labelling,Nedge,Labelling->EdgeStartVertex[Nedge]);
   PropagateConstraintsFromConcaveToJunction(Labelling,Nedge,Labelling->EdgeEndVertex[Nedge]);
   Labelling->EdgeLabelIsFinalised[Nedge] = cTRUE;
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromNonConcaveToJunction (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
{
   switch (Labelling->VertexLabel[Nvert].Shape)
   {
      case JunctionIsY:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Yaxx|Ybxx|Ycxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Yxax|Yxbx|Yxcx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Yxxa|Yxxb|Yxxc);
      break;

      case JunctionIsW:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Waxx|Wbxx|Wcxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Wxax|Wxbx|Wxcx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Wxxa|Wxxb|Wxxc);
      break;

      case JunctionIsL:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Lax|Lbx|Lcx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Lxa|Lxb|Lxc);
      break;

      case JunctionIsT:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Tbaa|Tbab|Tbac);
      break;

      case JunctionIsT4:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Taxx|Tbxx|Tcxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Txax|Txbx|Txcx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Txxa|Txxb|Txxc);
      break;

      case JunctionIsX:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xaxxx|Xbxxx|Xcxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xxaxx|Xxbxx|Xxcxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxax|Xxxbx|Xxxcx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxa|Xxxxb|Xxxxc);
      break;

      case JunctionIsK:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Kaxxx|Kbxxx|Kcxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Kxaxx|Kxbxx|Kxcxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Kxxax|Kxxbx|Kxxcx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Kxxxa|Kxxxb|Kxxxc);
      break;

      case JunctionIsM:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Maxxx|Mbxxx|Mcxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Mxaxx|Mxbxx|Mxcxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Mxxax|Mxxbx|Mxxcx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Mxxxa|Mxxxb|Mxxxc);
      break;

      case JunctionIs5way:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xaxxxx|Xbxxxx|Xcxxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xxaxxx|Xxbxxx|Xxcxxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxaxx|Xxxbxx|Xxxcxx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxax|Xxxxbx|Xxxxcx);
         else
         if (Labelling->VENumber[Nvert][4] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxxa|Xxxxxb|Xxxxxc);
      break;

      case JunctionIsZ6:
      case JunctionIs6way:
         ChangeVertex(Labelling,Nvert,Xcdcdcd|Xdcdcdc|Xcccccc);
   }
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromNonConcave (LABELLING *Labelling, EDGE_INDEX Nedge)
{
   PropagateConstraintsFromNonConcaveToJunction(Labelling,Nedge,Labelling->EdgeStartVertex[Nedge]);
   PropagateConstraintsFromNonConcaveToJunction(Labelling,Nedge,Labelling->EdgeEndVertex[Nedge]);
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromVisibleToJunction (LABELLING *Labelling, EDGE_INDEX Nedge, VERTEX_INDEX Nvert)
{
   CheckVertexNumber(Labelling,Nvert,"PropagateConstraintsFromVisibleToJunction");

   switch (Labelling->VertexLabel[Nvert].Shape)
   {
      case JunctionIsL:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Lcx|Ldx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Lxc|Lxd);
      break;

      case JunctionIsW:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Wcxx|Wdxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Wxcx|Wxdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Wxxc|Wxxd);
      break;

      case JunctionIsY:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Ycxx|Ydxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Yxcx|Yxdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Yxxc|Yxxd);
      break;

      case JunctionIsT:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Tbac|Tbad);
      break;

      case JunctionIsT4:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Tcxx|Tdxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Txcx|Txdx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Txxc|Txxd);
      break;

      case JunctionIsX:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xcxxx|Xdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xxcxx|Xxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxcx|Xxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxc|Xxxxd);
      break;

      case JunctionIsK:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Kcxxx|Kdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Kxcxx|Kxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Kxxcx|Kxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Kxxxc|Kxxxd);
      break;

      case JunctionIsM:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Mcxxx|Mdxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Mxcxx|Mxdxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Mxxcx|Mxxdx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Mxxxc|Mxxxd);
      break;

      case JunctionIs5way:
         if (Labelling->VENumber[Nvert][0] == Nedge)  ChangeVertex(Labelling,Nvert,Xcxxxx|Xdxxxx);
         else
         if (Labelling->VENumber[Nvert][1] == Nedge)  ChangeVertex(Labelling,Nvert,Xxcxxx|Xxdxxx);
         else
         if (Labelling->VENumber[Nvert][2] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxcxx|Xxxdxx);
         else
         if (Labelling->VENumber[Nvert][3] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxcx|Xxxxdx);
         else
         if (Labelling->VENumber[Nvert][4] == Nedge)  ChangeVertex(Labelling,Nvert,Xxxxxc|Xxxxxd);
      break;
   }
}


void CCueLabelsVarley_LineToJunctions::PropagateConstraintsFromVisible (LABELLING *Labelling, EDGE_INDEX Nedge)
{
   PropagateConstraintsFromVisibleToJunction(Labelling,Nedge,Labelling->EdgeStartVertex[Nedge]);
   PropagateConstraintsFromVisibleToJunction(Labelling,Nedge,Labelling->EdgeEndVertex[Nedge]);
}


void CCueLabelsVarley_LineToJunctions::PropagateLineConstraintsToJunctions (LABELLING *Labelling)
{
   for (int n=0; n<Labelling->EdgeCount; ++n)
   {
      if (!Labelling->EdgeLabelIsFinalised[n])
      {
         switch (Labelling->EdgeLabel[n])
         {
            case LineIsBackArrow:
               /* Don't bother. Change it to Arrow. */
               LabelLineAsBackArrow(Labelling,n);
            case LineIsArrow:
               PropagateConstraintsFromArrow(Labelling,n);
            break;

            case LineIsConvex:
               PropagateConstraintsFromConvex(Labelling,n);
            break;

            case LineIsConcave:
               PropagateConstraintsFromConcave(Labelling,n);
            break;

            case LineIsVisible:
               PropagateConstraintsFromVisible(Labelling,n);
            break;

            case LineIsUnknown:
               /* can't deduce anything from this */;
            break;

            case LineIsArrow|LineIsConvex:
            case LineIsBackArrow|LineIsConvex:
            case LineIsArrow|LineIsBackArrow|LineIsConvex:
               PropagateConstraintsFromNonConcave(Labelling,n);
            break;

            case LineIsArrow|LineIsConcave:
            case LineIsBackArrow|LineIsConcave:
            case LineIsArrow|LineIsBackArrow|LineIsConcave:
               PropagateConstraintsFromNonConvex(Labelling,n);
            break;
         }

      }
   }
}

