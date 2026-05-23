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

//#include <cstdio>

#include <stdafx.h>
#include <cassert>
#include <cmath>

//-------used to import/export data from/to the external database-----------
#include <vector>

#include "Tools_Geometry.h"
#include "DB_Model.h"
//#include "resource.h"
//#include "ReferDoc.h"
//#include "Refer.h"
////--------------------------------------------------------------------------
//
#include "DataLabels.h"
#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"
//#include "CueLabelsVarley_LabellingConstants.h"
//#include "CueLabelsVarley_LabellingParameters.h"

#include "CueLabelsVarley_LineEquations.h"


CCueLabelsVarley_LineEquations::CCueLabelsVarley_LineEquations()
{
}


CCueLabelsVarley_LineEquations::~CCueLabelsVarley_LineEquations()
{
}


lBOOLEAN CCueLabelsVarley_LineEquations::Close (lREAL p, lREAL q)
{
   lREAL d = p-q;
   return (d*d) < 0.01;
}


void CCueLabelsVarley_LineEquations::NormaliseImplicitLine (IMPLICIT_LINE2 *G)
{
   lREAL A = G->Normal.x;
   lREAL B = G->Normal.y;
   lREAL C = G->Distance;
   lREAL D = sqrt(A*A+B*B);
   if (C < 0.0)  D = -D;
   G->Normal.x = A/D;
   G->Normal.y = B/D;
   G->Distance = C/D;
}


void CCueLabelsVarley_LineEquations::FindImplicitLine (IMPLICIT_LINE2 *ImplicitLine, POINT2Dv *P0, POINT2Dv *P1)
{
   lREAL X0,Y0,X1,Y1;
   X0 = P0->x;  Y0 = P0->y;
   X1 = P1->x;  Y1 = P1->y;
   if (Close(X0,X1))
   {
      ImplicitLine->Normal.x = 1.0;
      ImplicitLine->Normal.y = 0.0;
      ImplicitLine->Distance = -X0;
   }
   else
   if (Close(Y0,Y1))
   {
      ImplicitLine->Normal.x = 0.0;
      ImplicitLine->Normal.y = 1.0;
      ImplicitLine->Distance = -Y0;
   }
   else
   {
      ImplicitLine->Normal.x = Y1 - Y0;
      ImplicitLine->Normal.y = X0 - X1;
      ImplicitLine->Distance = -(ImplicitLine->Normal.x*X0 + ImplicitLine->Normal.y*Y0);
      NormaliseImplicitLine(ImplicitLine);
   }
}


void CCueLabelsVarley_LineEquations::GetLineEquation (OBJECT *Object, EDGE_INDEX Nedge)
{
   VERTEX_INDEX Vs = Object->Labelling.EdgeStartVertex[Nedge];
   VERTEX_INDEX Vt = Object->Labelling.EdgeEndVertex[Nedge];

   if (Object->Vertices[Vs].XYKnown && Object->Vertices[Vt].XYKnown)
   {
      POINT2Dv P0,P1;
      P0.x = Object->Coordinates[Vs].x;
      P0.y = Object->Coordinates[Vs].y;
      P1.x = Object->Coordinates[Vt].x;
      P1.y = Object->Coordinates[Vt].y;

      FindImplicitLine(&Object->Edges[Nedge].LineEquation,&P0,&P1);
      Object->Edges[Nedge].EquationKnown = cTRUE;
   }
}


void CCueLabelsVarley_LineEquations::GetUnknownLineEquations (OBJECT *Object)
{
   for (EDGE_INDEX n=0; n<Object->Labelling.EdgeCount; ++n)  if (!Object->Edges[n].EquationKnown) GetLineEquation(Object,n);
}


void CCueLabelsVarley_LineEquations::GetAllLineEquations (OBJECT *Object)
{
   for (EDGE_INDEX n=0; n<Object->Labelling.EdgeCount; ++n)
	   GetLineEquation(Object,n);
}
